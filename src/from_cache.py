from dotenv import load_dotenv
import os
import argparse
import json

from google import genai
from google.genai import types

from prompts import *


def parse_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--cached-page",
        type=str,
        required=True,
        help="Path to the cached page JSON file containing the problem description and draft markdown.",
    )
    parser.add_argument(
        "--model",
        type=str,
        default="gemma-3-27b-it",
        help="The model to use for generating content. See https://ai.google.dev/gemini-api/docs/models. Default is 'gemma-3-27b-it'.",
    )
    return parser.parse_args()

if __name__ == "__main__":
    # ===== PARSE ARGUMENTS =====
    args = parse_arguments()

    # ===== SETUP CLIENTS AND FETCH DATABASE =====
    load_dotenv()

    GOOGLE_CLIENT = genai.Client(api_key=os.getenv("GOOGLE_GENAI_API_KEY"))
    MODEL = args.model

    # ===== LOAD CACHED PAGE =====
    if not os.path.exists(args.cached_page):
        raise FileNotFoundError(f"Cached page file '{args.cached_page}' does not exist.")
    
    page = json.load(open(args.cached_page, "r", encoding="utf-8"))

    # ===== LLM REVISION =====
    # Setup output paths
    output_dir = f"../Week_{page['week']:02d}/{page['title'].replace(' ', '_')}"
    output_path = os.path.join(output_dir, "README.md")
    solution_path = os.path.join(output_dir, "solution.cpp")

    # Check if the output file already exists
    if os.path.exists(output_path):
        overwrite = input(f"File '{output_path}' already exists. Overwrite? (y/n): ").strip().lower()
        if overwrite != "y":
            exit(0)
        else:
            os.remove(output_path)
            os.remove(solution_path)

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