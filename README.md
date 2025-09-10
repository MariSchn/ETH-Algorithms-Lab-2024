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

| Week | Problem 1                                                            | Problem 2                                                  | Problem 3                                                    | Problem 4                                           | Problem of the Week                                          |
|------|----------------------------------------------------------------------|------------------------------------------------------------|------------------------------------------------------------- |-----------------------------------------------------|--------------------------------------------------------------|
| 01   | [Dominoes](Week_01/Dominoes)                                         | [Even Pairs](Week_01/Even_Pairs)                           | [Even Matrices](Week_01/Even_Matrices)                       | [Build the Sum](Week_01/Build_the_Sum)              |                                                              |
| 02   | [Beach Bars](Week_02/Beach_Bars)                                     | [Burning Coins](Week_02/Burning_Coins)                     | [Defensive Line](Week_02/Defensive_Line)                     | [The Great Game](Week_02/The_Great_Game)            | [Deck of Cards](Week_02/Deck_of_Cards)                       |
| 03   | [First Steps with BGL](Week_03/First_Steps_with_BGL)                 | [Buddy Selection](Week_03/Buddy_Selection)                 | [Ant Challenge](Week_03/Ant_Challenge)                       | [Important Bridges](Week_03/Important_Bridges)      | [James Bond's Sovereign](Week_03/James_Bond’s_Sovereign)     |
| 04   | [Hit](Week_04/Hit)                                                   | [First Hit](Week_04/First_Hit)                             | [Antenna](Week_04/Antenna)                                   | [Hiking Maps](Week_04/Hiking_Maps)                  | [The Iron Islands](Week_04/The_Iron_Islands)                 |
| 05   | [Moving Books](Week_05/Moving_Books)                                 | [Asterix the Gaul](Week_05/Asterix_the_Gaul)               | [Severus Snape](Week_05/Severus_Snape)                       | [Boats](Week_05/Boats)                              | [Tracking](Week_05/Tracking)                                 |
| 06   | [Tiles](Week_06/Tiles)                                               | [Coin Tossing Tournament](Week_06/Coin_Tossing_Tournament) | [Knights](Week_06/Knights)                                   | [Kingdom Defense](Week_06/Kingdom_Defense)          | [Motorcycles](Week_06/Motorcycles)                           |
| 07   | [Bistro](Week_07/Bistro)                                             | [H1N1](Week_07/H1N1)                                       | [Germs](Week_07/Germs)                                       | [Clues](Week_07/Clues)                              | [Octopussy](Week_07/Octopussy)                               |
| 08   | [Maxmize it!](Week_08/Maximize_it!)                                  | [Diet](Week_08/Diet)                                       | [Inball](Week_08/Inball)                                     | [Casterly Rock](Week_08/Casterly_Rock)              | [Attack on King's Landing](Week_08/Attack_on_King's_Landing) |
| 09   | [Placing Knights](Week_09/Placing_Knights)                           | [Real Estate Market](Week_09/Real_Estate_Market)           | [Canteen](Week_09/Canteen)                                   | [Algocoon](Week_09/Algocoon)                        | [Idefix](Week_09/Idefix)                                     |
| 10   | [Asterix and the Tour of Gaul](Week_10/Asterix_and_the_Tour_of_Gaul) | [Rubeus Hagrid](Week_10/Rubeus_Hagrid)                     | [San Francisco](Week_10/San_Francisco)                       | [Than Hand's Tourney](Week_10/The_Hand's_Tourney)   | [Hermione Granger](Week_10/Hermione_Granger)                 |
| 11   | [Asterix in Switzerland](Week_11/Asterix_in_Switzerland)             | [Lernaean Hydra](Week_11/Lernaean_Hydra)                  | [Sith](Week_11/Revenge_of_the_Sith)                          | [Worldcup](Week_11/World_Cup)                       | [Ceryneian Hind](Week_11/Ceryneian_Hind)                     |
| 12   | [Alastor Moody](Week_12/Alastor_Moody)                               | [Nemean Lion](Week_12/Nemean_Lion)                         | [Rapunzel](Week_12/Rapunzel)                                 | [Return of the Jedi](Week_12/Return_of_the_Jedi)    | [Pied Piper](Week_12/Pied_Piper)                             |
| 13   | [Asterix and the Chariot Race](Week_13/Asterix_and_the_Chariot_Race) | [Car Sharing](Week_13/Car_Sharing)                         | [Fighting Pits of Meereen](Week_13/Fighting_Pits_of_Meereen) | [Suez](Week_13/Suez)                                | [Schneewittchen](Week_13/Schneewittchen)                     |
| 14   |                                                                      |                                                            |                                                              |                                                     | [Ludo Bagman](Week_14/Ludo_Bagman)                           |

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

- Whenever you need to maximize a contiguous segment you probably need to use a sliding window.

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

- If you want to try a greedy approach you will probably need to sort (part of) your input first to then choose greedily.

</details>

<details>

<summary>Geometry</summary>

</details>

<details>

<summary>Delaunay Triangulation</summary>

- As soon as the problem asks for some notion of distance or proximity, you will probably need to usea Delaunay Triangulation, as it is just super cheap to do.
- You often will want to store information at each vertex in the Delaunay Triangulation. Most elementary one would be its index to reference later. For this you can use the following setup (see code example provided in AlgoLab docs for more details).
```cpp
typedef std::size_t                                            Index;
typedef CGAL::Triangulation_vertex_base_with_info_2<Index,K>   Vb;
typedef CGAL::Triangulation_face_base_2<K>                     Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>            Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>                  Delaunay;

typedef std::tuple<Index,Index,K::FT> Edge;

typedef K::Point_2 Point;
typedef std::pair<Point,Index> IPoint;

...

Index idx = vertex->info();
```

</details>

<details>

<summary>Linear Programming</summary>

- If the problem asks you to "*round to the nearest integer*", it is probably a linear programming problem. This is a very ad-hoc tip, but you would be surprised how often this is the case.

- CGAL creates all intermediate variables, e.g. if you create variable 1000 but the previous highest you had was 100, all 900 variables in between are also created, greaetly impacting run time.

</details>

<details>

<summary>Graphs</summary>

</details>

<details>

<summary>Max Flow/Min Cut</summary>

</details>

<details>

<summary>Max Flow Min Cost</summary>

- In Max Flow Min Cut problems the intuitive way of modeling it often involves negative costs. This then works for the first few test sets, but for the last one you need to rescale it to be non-negative to use the faster, 

</details>

---

## 📝 Notes

- This repo is intended as a study and reference resource. If you're currently taking the course, I recommend attempting the problems yourself before looking at the solutions.
- Feel free to open issues or pull requests if you spot any errors or have suggestions.

---

## ⚠️ Disclaimer

All explanations and writeups are based on personal notes I took throughout the semester while working on each problem. These notes were later revised using [Gemma 3 27B Instruct](https://huggingface.co/google/gemma-3-27b-it) and [Gemini 2.5 Pro](https://deepmind.google/models/gemini/pro/), and then revised one more time by me. This cycle was done twice to ensure that the solutions and hints are accurate and (hopefully) helpful. The script that was used for the LLM revision can be found in the `src` directory.