from dotenv import load_dotenv
from tqdm import tqdm
import os

from notion_client import Client
import requests
import pdftotext
import io
import json

from google import genai


def get_database(database_id: str, client: Client) -> dict:
    """
    Fetches the full database from Notion.
    Args:
        database_id (str): The ID of the Notion database to query.
        client (Client): An instance of the Notion Client to interact with the Notion API.
    Returns:
        dict: The response from the Notion API containing the database entries. 
              The structure of the response can be found in the Notion API documentation: https://developers.notion.com/reference/post-database-query.
    """
    response = client.databases.query(
        database_id=database_id,
        sorts=[
                {
                    "property": "Name",
                    "direction": "ascending"
                }
            ]
        )
    return response

def parse_rich_text(rich_text: list) -> str:
    """
    Extracts text from rich text objects. The rich text objects can be accessed for most blocks in Notion, such as paragraphs, headings, and lists.

    Args:
        rich_text (list): A list of rich text objects from Notion.
    Returns:
        str: A string containing the text extracted from the rich text objects, with appropriate formatting for markdown.
    """
    content = ""

    for text in rich_text:
        if text['type'] == 'text':
            modifiers = ""

            for modifier in text['text'].get('annotations', []):
                if modifier == 'bold':
                    modifiers += "**"
                elif modifier == 'italic':
                    modifiers += "*"
                elif modifier == 'strikethrough':
                    modifiers += "~~"
                elif modifier == 'underline':
                    modifiers += "__"
                elif modifier == 'code':
                    modifiers += "`"

            content += f"{modifiers}{text['plain_text']}{modifiers[::-1]}"
        elif text['type'] == 'equation':
            content += f"$ {text['equation']['expression']} $"
        else:
            print(f"Unsupported rich text type: {text['type']}")
            content += text['plain_text']
        
    return content

def parse_toggle_content(block_id: str, client: Client) -> str:
    """
    Recursively parses the content of a toggle block in Notion and its children blocks, formatting them as markdown.
    This function handles various block types such as paragraphs, lists, code blocks, equations, and nested toggles.

    Args:
        block_id (str): The ID of the toggle block to parse.
        client (Client): An instance of the Notion Client to interact with the Notion API.
    Returns:
        str: The content of the toggle block, formatted as markdown.
    """
    content = ""
    previous_block_type = None
    numbered_list_item_count = 0

    children = client.blocks.children.list(block_id=block_id)
    for child in children["results"]:
        if child["type"] == "paragraph":
            bg_color = child["paragraph"]["color"]

            if bg_color != "default":
                content += f"\n*Intuition: {parse_rich_text(child["paragraph"]["rich_text"])}*\n"
            else:
                content += f"{parse_rich_text(child["paragraph"]["rich_text"])}\n"
        elif child["type"] == "bulleted_list_item":
            # If the previous block was not a bulleted list, add a newline to ensure proper markdown formatting
            if previous_block_type != "bulleted_list_item":
                content += "\n"
            content += f"- {parse_rich_text(child["bulleted_list_item"]["rich_text"])}\n"
        elif child["type"] == "toggle":
            # Recursively parse toggles
            content += f"**{parse_rich_text(child["toggle"]["rich_text"])}**\n"
            content += parse_toggle_content(child["id"], client)
        elif child["type"] == "code":
            # Wrap code blocks in triple backticks with language
            content += f"```{child['code']['language']}\n{child['code']['rich_text'][0]['plain_text']}\n```\n"
        elif child["type"] == "equation":
            # Wrap equations in double dollar signs
            content += f"$$ {child['equation']['expression']} $$\n"
        elif child["type"] == "numbered_list_item":
            # If the previous block was not a numbered list, add a newline to ensure proper markdown formatting
            if previous_block_type != "numbered_list_item":
                content += "\n"
                numbered_list_item_count = 0
            else:
                numbered_list_item_count += 1
            content += f"{numbered_list_item_count + 1}. {parse_rich_text(child['numbered_list_item']['rich_text'])}\n"
        elif child["type"] == "heading_1":
            content += f"# {parse_rich_text(child['heading_1']['rich_text'])}\n"
        elif child["type"] == "heading_2":
            content += f"## {parse_rich_text(child['heading_2']['rich_text'])}\n"
        elif child["type"] == "heading_3":
            content += f"### {parse_rich_text(child['heading_3']['rich_text'])}\n"
        elif child["type"] == "image":
            continue  # Skip images for now
        else:
            print(f"Unsupported block type: {child['type']}")
            content += f"{child['plain_text']}\n"

        previous_block_type = child["type"]

    return content

def get_pdf_text(url: str) -> str:
    """
    Parses a PDF file from a given URL to extract its text content.

    Args:
        url (str): The URL of the PDF file to parse.
    Returns:
        str: The text content extracted from the PDF file.
    """
    response = requests.get(url)

    if response.status_code != 200:
        raise Exception(f"Failed to fetch PDF {url}. Error: {response.status_code}")
    pdf_content = response.content
    
    if not pdf_content:
        raise Exception(f"PDF content is empty for URL: {url}")

    pdf = pdftotext.PDF(io.BytesIO(pdf_content))

    return "\n\n".join(pdf)

if __name__ == "__main__":
    # ===== SETUP CLIENTS AND FETCH DATABASE =====
    load_dotenv(".env.local")
    NOTION_CLIENT = Client(auth=os.getenv("NOTION_INTEGRATION_TOKEN"))
    DATABASE = get_database(os.getenv("NOTION_DATABASE_ID"), NOTION_CLIENT)

    GOOGLE_CLIENT = genai.Client(api_key=os.getenv("GOOGLE_GENAI_API_KEY"))
    MODEL = "gemma-3-27b-it"

    # ===== PARSE PAGES FROM DATABASE ======
    # * This is highly dependent on the structure of the Notion database.
    pages = []
    for result in tqdm(DATABASE["results"], desc="Parsing Notion Pages"):
        # Fetch the properties of the page
        page_properties = NOTION_CLIENT.pages.retrieve(page_id=result["id"])["properties"]

        title = page_properties["Name"]["title"][0]["text"]["content"]
        week = page_properties["Week"]["number"]
        difficulty = page_properties["Difficulty"]["select"]["name"] if page_properties["Difficulty"]["select"] else None
        problem_type = page_properties["Type"]["select"]["name"]
        tags = [tag["name"] for tag in page_properties["Tags"]["multi_select"]]
        pdf_url = page_properties["Problem PDF"]["files"][0]["file"]["url"] if page_properties["Problem PDF"]["files"] else None

        # Check if the page is already cached
        sanitized_title = title.replace(" ", "_").replace("/", "_").replace("\\", "_")
        cache_path = f"cache/{sanitized_title}.json"
        if os.path.exists(cache_path):
            tqdm.write(f"Loading cached page for {title} (Week {week}, Type: {problem_type})")

            with open(cache_path, "r") as f:
                cached_page = json.load(f)
                pages.append(cached_page)

            continue

        tqdm.write(f"Processing page: {title} (Week {week}, Type: {problem_type})")

        # Fetch the content of the page
        page_content = NOTION_CLIENT.blocks.children.list(block_id=result["id"])
        current_section = None


        # Parse the content of the page
        parsed_page = ""
        for block in page_content["results"]:
            if block["type"] == "heading_2":
                heading = parse_rich_text(block["heading_2"]["rich_text"])

                if heading == "📝 Notes":
                    continue

                parsed_page += f"## {heading}\n"
            if block["type"] == "heading_3":
                parsed_page += f"<details>\n<summary>{parse_rich_text(block["heading_3"]["rich_text"])}</summary>\n"
                parsed_page += parse_toggle_content(block["id"], NOTION_CLIENT)
                parsed_page += "</details>\n\n"

        pages.append({
            "title": title,
            "week": week,
            "difficulty": difficulty,
            "problem_type": problem_type,
            "tags": tags,
            "pdf_url": pdf_url,
            "pdf": get_pdf_text(pdf_url) if pdf_url else None,
            "content": parsed_page
        })

        # Save the parsed page to cache
        with open(cache_path, "w") as f:
            json.dump(pages[-1], f, indent=2)

    # ===== LLM REVISION =====
    # Prompt setup
    INPUT_FORMAT = """
    # Problem Description
    <problem_description>
    # End of Problem Description

    # Markdown Document Draft
    <draft_markdown>
    # End of Draft
    """
    OUTPUT_FORMAT = """
    # <Problem Name>

    ## 📝 Problem Description

    **CRITICAL RULE: You must describe the problem using only the terms and entities mentioned in the description. DO NOT classify the problem or use abstract algorithmic terminology.** The goal is to present the problem as it appears to a student *before* they have started analyzing it.

    **FORBIDDEN WORDS for this section include, but are not limited to: `graph`, `node`, `edge`, `vertex`, `DP`, `dynamic programming`, `tree`, `delaunay triangulation`, `binary search tree`, `heap`, `priority queue`, `greedy`, `divide and conquer` `brute force`, `geometry`.**
    S
    Based *only* on the <problem_description>, summarize the problem. Abstract away any narrative or story elements. Focus on clearly and concisely defining:
    1. The input (what is given in the problem).
    2. The output (what needs to be computed or produced).
    3. Use mathematical notation with "$" for variables and constraints where appropriate.

    Ensure this description is faithful to the formal problem definition in the problem description and easy to understand.

    ## 💡 Hints

    *Provide 2-3 progressively helpful hints that guide a student toward the solution(s). Hints should not give away the full solution but empower the student to discover it. Make sure to place the hints in the <details> tags so they can be expanded by the student.
    * Hint #1: Subtle, prompting initial thought, a relevant concept, or a small observation.
    * Hint #2: More direct, suggesting a possible technique, data structure, or a way to simplify the problem.
    * Hint #3: Reveals a key insight, a critical step in the solution approach, or a common pitfall to avoid.*

    <details><summary>Hint #1</summary>

    </details>
    <details><summary>Hint #2</summary>

    </details>
    <details><summary>Hint #3</summary>

    </details>

    ## ✨ Solutions

    * Polish and clarify the draft solution(s) provided in `<draft_markdown>`. Improve the writing style, fix grammar or formatting issues, and ensure the logic is easy to follow. Preserve the author's original thought process and code structure as much as reasonably possible. Do not rewrite entire solutions from scratch unless the draft is exceptionally minimal or completely off-topic.
    * All of the solutions given here are correct, you should mainly focus on polishing the writing style, fixing grammar or formatting issues, and ensuring the logic and explanation is easy to follow for students who have not solved the problem.
    * When explaining the solutions, skip trivial steps like "Read input", "Output", or "Initialize variables". Focus on the core logic and reasoning behind the solution.
    * The code should be placed at the end of the solution explanation.

    <details><summary>First Solution (Test Set 1)</summary>

    <!-- Solution draft will be here — polish it but keep the structure -->

    </details>

    <!-- Repeat the above block for each distinct solution approach present in the draft -->

    <details><summary>Final Solution</summary>

    <!-- Solution draft will be here — polish it but keep the structure -->

    </details>

    ## ⚡ Result

    *Leave this section entirely empty. The student will fill in the result after submitting their solution.*

    ```plaintext

    ```
    """
    SYSTEM_PROMPT = f"""You are an expert technical writer specializing in competitive programming, algorithms, and data structures. Your audience is university students in an Algorithms Lab course (e.g., at ETH Zurich), so your explanations must be educational, clear, precise, and accessible, even to those unfamiliar with the specific problem.

    Your primary task is to revise and complete a markdown document that presents a problem and its solution(s). You will be given a problem description and a draft markdown document.

    **Key Responsibilities:**

    1.  **Adhere Strictly to Output Format:** The final output *must* follow the `OUTPUT_FORMAT` precisely.
    2.  **Problem Description:** Extract the problem name from the problem description or draft. Populate the "Problem Description" section using *only* the information from the `<problem_description>`, as detailed in the `OUTPUT_FORMAT`.
    3.  **Hints:** Develop "Hints" as specified in the `OUTPUT_FORMAT`. These should be original and derived from your understanding of the problem and common solution paths.
    4.  **Solutions:**
        *   Carefully review and polish the solution explanations and code provided in the `<draft_markdown>`.
        *   Focus on improving clarity, correctness, grammar, and formatting.
        *   Preserve the original author's approach and thought process.
        *   Correct minor logical in the draft.
        *   If a draft solution has significant flaws, explain these flaws clearly while still presenting the polished (but flawed) original attempt, as per `OUTPUT_FORMAT` guidelines.
        *   Ensure code is well-commented if the draft lacks comments or if comments would significantly aid understanding.
    5.  **Tone and Style:** Maintain a formal, educational, and encouraging tone. Write in clear, concise English.
    6.  **Completeness:** Ensure all sections of the `OUTPUT_FORMAT` are addressed. If the draft is missing content for a section you are asked to polish (e.g., a solution explanation is very brief), elaborate on it based on the provided code and problem, maintaining the spirit of the draft's approach.

    # BEGIN INPUT FORMAT
    {INPUT_FORMAT}
    # END INPUT FORMAT

    # BEGIN OUTPUT FORMAT
    {OUTPUT_FORMAT}
    # END OUTPUT FORMAT

    Process the provided input and generate the complete, revised markdown document.
    """
    USER_PROMPT_TEMPLATE = """
    # Problem Description
    {problem_pdf}
    # End of Problem Description

    # Markdown Document Draft
    {draft_markdown}
    # End of Draft
    """

    # ===== GENERATE READMEs =====
    for page in tqdm(pages, desc="Generating READMEs"):
        # Setup output paths
        output_dir = f"../Week_{page['week']:02d}/{page['title'].replace(' ', '_')}"
        output_path = os.path.join(output_dir, "README.md")

        # Check if the output file already exists
        if os.path.exists(output_path):
            tqdm.write(f"Output file already exists for {page['title']} (Week {page['week']}). Skipping generation.")
            continue

        tqdm.write(f"Generating README for {page['title']} (Week {page['week']}, Type {page['problem_type']})...")

        # Setup prompt for the model
        prompt = SYSTEM_PROMPT
        prompt += "\n" 
        prompt += USER_PROMPT_TEMPLATE.format(
            problem_pdf=page["pdf"] if page["pdf"] else "No PDF available.",
            draft_markdown=page["content"]
        )

        # Generate content using the model
        response = GOOGLE_CLIENT.models.generate_content(
            model=MODEL,
            contents=prompt,
        )

        response_text = response.text
        if response_text.startswith("```markdown"):
            response_text = response_text[len("```markdown"):].lstrip()
        if response_text.endswith("```"):
            response_text = response_text[:-3].rstrip()

        os.makedirs(output_dir, exist_ok=True)
        with open(output_path, "w", encoding="utf-8") as f:
            f.write(response_text)
