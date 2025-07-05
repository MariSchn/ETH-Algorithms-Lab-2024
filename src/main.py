from dotenv import load_dotenv
from tqdm import tqdm
import os
import argparse

from notion_client import Client
import requests
import pdftotext
import io
import json

from google import genai
from google.genai import types


def parse_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--model",
        type=str,
        default="gemma-3-27b-it",
        help="The model to use for generating content. See https://ai.google.dev/gemini-api/docs/models. Default is 'gemma-3-27b-it'.",
    )
    return parser.parse_args()

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
    # ===== PARSE ARGUMENTS =====
    args = parse_arguments()

    # ===== SETUP CLIENTS AND FETCH DATABASE =====
    load_dotenv()
    NOTION_CLIENT = Client(auth=os.getenv("NOTION_INTEGRATION_TOKEN"))
    DATABASE = get_database(os.getenv("NOTION_DATABASE_ID"), NOTION_CLIENT)

    GOOGLE_CLIENT = genai.Client(api_key=os.getenv("GOOGLE_GENAI_API_KEY"))
    MODEL = args.model

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
    INPUT_TEMPLATE = """
    # Problem Description
    <PROBLEM_DESCRIPTION>
    # End of Problem Description

    # Markdown Document Draft
    <DRAFT_MARKDOWN>
    # End of Draft
    """
    OUTPUT_TEMPLATE = """
    # <PROBLEM_NAME_HERE>

    ## 📝 Problem Description

    <PROBLEM_SUMMARY_HERE>

    ## 💡 Hints

    <ALL_HINTS_HERE>

    ## ✨ Solutions

    <ALL_POLISHED_SOLUTIONS_HERE>

    ## ⚡ Result

    ```plaintext

    ```
    """
    SYSTEM_PROMPT = f"""
    <PROMPT_SETUP>
    You are an expert technical writer for a university-level Algorithms Lab course. Your audience is students learning about competitive programming, algorithms, and data structures.

    Your task is to create a definitive problem walkthrough document. This document serves as the primary educational resource for students after they've attempted the problem. It must not only provide answers but also guide them through the thinking process, from basic observations to the optimal solution.
    </PROMPT_SETUP>

    <INSTRUCTIONS>
    You must follow these instructions meticulously.

    ### 1. General Rules
    - **Adhere to the Template:** Your final output must strictly follow the structure defined in the `<OUTPUT_TEMPLATE>`. You will populate the placeholders like `<ALL_HINTS_HERE>` by generating the necessary markdown blocks.
    - **Tone and Style:** Maintain a formal, educational, and encouraging tone. Write in clear, concise English.
    - **Completeness:** Ensure all required sections of the `<OUTPUT_TEMPLATE>` are populated, except for the "Result" section.
    - **Formatting:** Use proper markdown syntax for headings, code blocks, and lists. Use LaTeX notation with '$' for mathematical variables and expressions. Break down long texts into reasonably small paragraphs with (sub)headings where appropriate. Also use italic for emphasis and bold for important points.

    ### 2. Instructions for "Problem Description" Section
    - **Extract Problem Name:** Determine the problem's name from the input and place it in the `<PROBLEM_NAME_HERE>` placeholder.
    - **CRITICAL RULE:** Summarize the problem based *only* on the `<PROBLEM_DESCRIPTION>`. Do not state what algorithmic technique or which algorithm is used in the solution. The goal is to present the problem as it appears to a student *before* they have started trying to solve it.
    - **FORBIDDEN WORDS for this section include, but are not limited to:** `graph`, `node`, `edge`, `vertex`, `DP`, `dynamic programming`, `tree`, `delaunay triangulation`, `binary search tree`, `heap`, `priority queue`, `greedy`, `divide and conquer` `brute force`, `geometry`.
    - **Content Requirements:**
        1. Abstract away any narrative or story elements.
        2. Seamlessly integrate the definitions of the input and expected output, without havig dedicated paragraphs or sections only for the input and output.
        3. Use LaTeX notation with '$' for variables (e.g., an array $A$ of size $N$). However, **omit specific range constraints** (like $1 \\le N \\le 10^5$) from this summary. The goal is a clean, high-level problem statement.
    
    ### 3. Instructions for "Hints" Section
    - **Generate Hints:** For the `<ALL_HINTS_HERE>` placeholder, generate multiple, progressively helpful hints. Each hint must be wrapped in its own `<details>` block. Inside of each `<details>` block, use a `<summary>` tag to provide a brief title for the hint. E.g. <summary>Hint #1</summary>
    - **Hint Titles:** Use titles like "Hint #1", "Hint #2", etc., to indicate the order of hints. Do not include anything else in the title for the hints.
    - **Hint #1 (Subtle):** Should prompt initial thought, suggest a relevant concept, or point out a small observation.
    - **Hint #2 (More Direct):** Should suggest a possible technique, data structure, or a way to simplify the problem.
    - **Hint #3 (Key Insight):** Should reveal a critical step in the solution approach or warn against a common pitfall.
    - **Flexibility:** This is just a general guideline given for every problem in the course. Therefore, adjust the number of hints based on the problem's difficulty.

    ### 4. Instructions for "Solutions" Section
    - **CRITICAL RULE:** Your main goal is to improve the writing style, fix grammar, and ensure the logic is easy to follow. Preserve the author's original thought process and code structure.
    - **Generate Solution Blocks:** For the `<ALL_POLISHED_SOLUTIONS_HERE>` placeholder, polish each solution from the `<DRAFT_MARKDOWN>` and place it in a separate `<details>` block. Inside of each `<details>` block, use a `<summary>` tag to provide the brief title for the solution which is given in the <DRAFT_MARKDOWN>. E.g. <summary>First Solution (Test Set 1)</summary>
    - **Keep all Solutions:** Polish each solution from the draft, even non-optimal ones, as they are valuable for education.
    - **Focus on Core Logic:** Skip trivial steps like "read input" or "print output." Focus on the core algorithm and reasoning.
    - **Elaborate if Necessary:** If a draft explanation is brief, elaborate on it based on the provided code, maintaining the spirit of the original approach, thought process and solution.
    - **Code Formatting:**
        - Place the polished code at the end of its corresponding explanation. 
        - Use proper markdown code blocks with a language identifier (e.g., ```cpp).
        - Add comments to the code only if they significantly aid understanding.

    ### 5. Instructions for "Result" Section
    - **Leave Empty:** The "Result" section and its placeholder comment must be the absolute end of your output. Do not add any text there.
    </INSTRUCTIONS>

    <TEMPLATES>
    <INPUT_TEMPLATE>
    {INPUT_TEMPLATE}
    </INPUT_TEMPLATE>

    <OUTPUT_TEMPLATE>
    {OUTPUT_TEMPLATE}
    </OUTPUT_TEMPLATE>
    </TEMPLATES>

    Now, process the provided input and generate the complete, revised markdown document.
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
        solution_path = os.path.join(output_dir, "solution.cpp")

        # Check if the output file already exists
        if os.path.exists(output_path):
            tqdm.write(f"Output file already exists for {page['title']} (Week {page['week']}). Skipping generation.")
            continue

        tqdm.write(f"Generating README for {page['title']} (Week {page['week']}, Type {page['problem_type']})...")

        # Setup prompt
        user_prompt = USER_PROMPT_TEMPLATE.format(
            problem_pdf=page["pdf"] if page["pdf"] else "No PDF available.",
            draft_markdown=page["content"]
        )

        # Generate content using the model
        if "gemma" in MODEL.lower():
            # Gemma does not support system prompts, so we use the user prompt directly
            response = GOOGLE_CLIENT.models.generate_content(
                model=MODEL,
                contents=SYSTEM_PROMPT + "\n" + user_prompt,
            )
        elif "gemini" in MODEL.lower():
            response = GOOGLE_CLIENT.models.generate_content(
                model="gemini-2.5-pro",
                config=types.GenerateContentConfig(
                    system_instruction=SYSTEM_PROMPT,
                ),
                contents=user_prompt
            )

        # Post-process the response
        response_text = response.text
        if response_text.startswith("```markdown"):
            response_text = response_text[len("```markdown"):].lstrip()
    
        # Save the generated content to the output file
        os.makedirs(output_dir, exist_ok=True)
        with open(output_path, "w", encoding="utf-8") as f:
            f.write(response_text)

        # Save the final solution code to a separate file
        solution_code = response_text.split("```c++")[-1].split("```")[0].strip() if "```c++" in response_text else ""
        with open(solution_path, "w", encoding="utf-8") as f:
            f.write(solution_code)