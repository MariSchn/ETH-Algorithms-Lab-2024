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

from prompts import *


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