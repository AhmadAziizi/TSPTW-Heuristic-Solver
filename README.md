# TSPTW Heuristic Solver

This project is a heuristic solver written in C for the **Traveling Salesman Problem with Time Windows (TSPTW)**.

The goal is to construct a valid route that visits as many cities as possible while respecting each city's allowed time window. The solver then tries to improve the route using local search techniques.

---

## Project Overview

The Traveling Salesman Problem (TSP) is a classical optimization problem where the objective is to find an efficient route between multiple locations.

This project focuses on a more realistic variation called **TSP with Time Windows**, where each city has:

- City ID
- X and Y coordinates
- Opening time
- Closing time

A city can only be visited within its allowed time window.

The program attempts to find a route that:

1. Visits the maximum possible number of cities
2. Minimizes total travel distance
3. Minimizes total route time when distances are equal

---

## Why This Project Matters

Problems like TSPTW appear in many real-world scheduling and routing systems, such as:

- Delivery route planning
- Cargo and logistics optimization
- Courier service scheduling
- Public transportation planning
- Maintenance team routing
- Warehouse and supply chain operations
- Vehicle routing with customer time constraints

Because exact solutions can become very expensive for large inputs, heuristic methods are often used to find good solutions within a limited time.

---

## How It Works

The solver uses a heuristic approach with multiple stages:

### 1. Greedy Route Construction

The algorithm starts from different cities and builds a route by repeatedly selecting the next feasible city based on arrival time and distance.

### 2. Feasibility Check

Each route is checked to make sure all visited cities respect their time windows.

### 3. Local Search Improvement

The route is improved using local search operations such as:

- Unvisited city insertion
- Swap operation
- Single city relocation
- Block relocation

### 4. Best Route Selection

The best solution is selected using this priority:

1. Highest number of visited cities
2. Shortest total route length
3. Shortest total route time

---

## Features

- Reads city data from an input file
- Calculates Euclidean distances between cities
- Builds feasible routes under time window constraints
- Uses greedy construction
- Applies adaptive local search
- Supports large input sizes
- Uses a time limit to control execution
- Outputs the best route found

---

## Input Format

Each line in the input file should contain:

```text
city_id x_coordinate y_coordinate opening_time closing_time
```

Example:

```text
1 10 20 0 100
2 30 50 20 150
3 70 40 50 200
```

---

## Output Format

The output file starts with:

```text
number_of_visited_cities total_route_length total_route_time
```

Then it prints the IDs of the cities in the selected route.

Example:

```text
71 11819 12969
154
153
155
160
...
```

---



## Project Structure

```text
TSPTW-Heuristic-Solver/
│
├── tsptw_solver.c
├── README.md
├── .gitignore
│
└── outputs/
    ├── output-1.txt
    ├── output-2.txt
    ├── output-3.txt
    └── output-4.txt
```

---

## Technologies Used

- C
- Algorithms
- Heuristic Optimization
- Local Search
- File Handling
- Time Complexity Optimization

---

## Future Improvements

- Add more local search operators
- Add 2-opt route optimization
- Compare results with other heuristic methods
- Add visualization for routes
- Export route statistics in CSV format
- Add command-line options for time limit and random seed

---

