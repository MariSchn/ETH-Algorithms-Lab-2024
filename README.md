# ETH Zurich – Algorithms Lab FS24 – Solutions

## Overview

This repo contains my solutions to the problems from the Algorithms Lab course at ETH Zurich from the FS24 semester.

For each problem, you'll find:

- 🧠 **Problem Description**: Clear explanation and summary of the problem statement.
- 💡 **Hints**: Multiple hints that guide you step-by-step. Each hint reveals progressively more information, designed to help you come up with the solution yourself.
- 🔄 **(Intermediate Solutions)**: Intermediate solutions that pass only some test sets, showing the evolution and the thought process of coming up with the final solution.
- ✅ **Solutions**: A solution that passes all test sets with comprehensive explanation of the approach, and detailed code comments.

All hints and solutions are collapsed in toggles by default to prevent spoilers. You can expand them to view the content. Initially, only the problem description is visible.

In addition to the problems there are also some [general tips](#-general-tips) and [algorithm specific tips](#-algorithm-specific-tips) that can help you approach the problems more effectively.

---

## 🗺️ Problem Index

| Week  | Problem 1 | Problem 2 | Problem 3 | Problem 4 | Problem of the Week |
|-------|-----------|-----------|-----------|-----------|---------------------|
| 01    |           |           |           |           |                     |
| 02    |           |           |           |           |                     |
| 03    |           |           |           |           |                     |
| 04    |           |           |           |           |                     |
| 05    |           |           |           |           |                     |
| 06    |           |           |           |           |                     |
| 07    |           |           |           |           |                     |
| 08    |           |           |           |           |                     |
| 09    |           |           |           |           |                     |
| 10    |           |           |           |           |                     |
| 11    |           |           |           |           |                     |
| 12    |           |           |           |           |                     |
| 13    |           |           |           |           |                     |
| 14    |           |           |           |           |                     |

---

## 🛠️ General Tips

Many of these tips might seem obvious or trivial, but having an overview of all of them can help you remember them when you're stuck on a problem.

- **Think before coding**: Take some time before starting to code. I would recommend to only start coding once you have an idea that you believe should work. Nothing is more annoying than coding up a solution and have it fail on you. If you dive right in, it makes it harder to distinugish between a coding error and a conceptual error.

- **Solve examples**: Start out by first solving the example inputs given in the problem statement by hand. This gives you a better understanding of the problem and can help you find patterns or edge cases that need to be handled.

- **Take input constraints into account**: The size of the input already tells you what complexity you should aim for. This can greatly reduce the amount of algorithms you need to consider.

- **Go test-set-by-test-set**: Often it is helpful to first consider the first test set. This will often be much easier than the entire problem and lead you on the right track. This is not always the case, but it is a good heuristic.
---

## 📚 Library Specific Tips

<details>

<summary>Boost Graph Library (BGL)</summary>

- Boost can be very rough at first. Don't feel bad if you need to look at the solution for the actual code. Finding the correct algorithm/approach will be more important in the long run.

</details>

<details>

<summary>Computational Geometry Algorithms Library (CGAL)</summary>

- 

</details>

---

## 🧩 Algorithm Specific Tips

<details>

<summary>Sliding Window</summary>

- 

</details>

<details>

<summary>Dynamic Programming</summary>

- Begin by trying to formulate the recurrence relation. If you don't have it, there is no point in coding.

</details>

<details>

<summary>Split & List</summary>

</details>

<details>

<summary>Greedy</summary>

</details>

<details>

<summary>Geometry</summary>

</details>

<details>

<summary>Delaunay Triangulation</summary>

</details>

<details>

<summary>Linear Programming</summary>

- If the problem asks you to "*round to the nearest integer*", it is probably a linear programming problem. This is a very ad-hoc tip, but you would be surprised how often this is the case.

</details>

<details>

<summary>Graphs</summary>

</details>

<details>

<summary>Max Flow/Min Cut</summary>

</details>

<details>

<summary>Max Flow Min Cost</summary>

</details>

---

## 📝 Notes

- This repo is intended as a study and reference resource. If you're currently taking the course, I recommend attempting the problems yourself before looking at the solutions.
- Feel free to open issues or pull requests if you spot any errors or have suggestions.

---

## ⚠️ Disclaimer

All explanations and writeups are based on personal notes I took throughout the semester while working on each problem. These notes were later revised using [Gemma 3 27B Instruct](https://huggingface.co/google/gemma-3-27b-it) and [Gemini 2.5 Pro](https://deepmind.google/models/gemini/pro/), and then revised one more time by me. This cycle was done twice to ensure that the solutions and hints are accurate and (hopefully) helpful. The script that was used for the LLM revision can be found in the `src` directory.