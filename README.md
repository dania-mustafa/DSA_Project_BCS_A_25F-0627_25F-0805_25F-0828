# DSA_Project_BCS_A_25F-0627_25F-0805_25F-0828

# Intelligent Warehouse Robot and Automated Order Fulfillment System

A menu-driven C++ console application that automates warehouse operations for e-commerce businesses. The system handles warehouse navigation, inventory management, order processing, robot allocation, picking plan generation, order execution, battery management, charging, and operation history with undo functionality.

All data structures are implemented manually without using STL containers. Dynamic memory is managed through `new` and `delete`, and initial warehouse data is loaded from external text files.

---

## Table of Contents

- [Features](#features)
- [Modules Overview](#modules-overview)
- [Data Structures Used](#data-structures-used)
- [Project Structure](#project-structure)
- [How to Compile and Run](#how-to-compile-and-run)
- [Input File Format](#input-file-format)
- [Usage Guide](#usage-guide)
- [Rules and Constraints](#rules-and-constraints)
- [Authors](#authors)

---

## Features

- Warehouse layout represented as a weighted undirected graph with RACK, PACKING, and CHARGING locations
- Reachability checking using BFS and shortest path finding using Dijkstra's algorithm
- Product inventory stored in a self-balancing AVL tree for O(log n) search and update
- Per-rack stock tracking with reservation management for customer orders
- FIFO order queue for processing orders in sequence
- Robot fleet with inheritance and polymorphism (Picker, Carrier, HeavyDuty)
- Intelligent robot assignment based on trip count, distance, and Robot ID
- Picking plan generation with battery safety reserve
- Trip validation before execution (pathways, battery, capacity, reservations)
- Battery drain calculation with weight-based consumption formula
- Automatic charging station assignment with FIFO waiting queue
- Undo functionality for reversible administrative operations using a LIFO stack
- Low stock alerts based on reorder levels

---

## Modules Overview

### Module 1: Warehouse Layout and Pathways
Represents the warehouse using a custom adjacency list graph. Locations are typed as RACK, PACKING, or CHARGING. Pathways can be OPEN or BLOCKED. BFS checks reachability, while Dijkstra's algorithm finds shortest routes with deterministic tie-breaking (distance → fewer locations → Location ID).

### Module 2: Product Inventory and Racks
Products are stored in an AVL tree indexed by ProductID. Each product connects to a RackStock linked list tracking quantities across racks. A nested Reservation linked list manages stock reserved for orders. Tracks total, reserved, and available quantities and generates low stock alerts.

### Module 3: Customer Order Management
Orders are handled using manually implemented linked lists. Each order contains product items and quantities. Pending orders are maintained in a FIFO queue. Order cancellation runs in O(n) time and releases reserved stock and assigned robots.

### Module 4: Robot Fleet Management
Robot fleet uses inheritance and polymorphism. A base `Robot` class defines common properties, while `Picker`, `Carrier`, and `HeavyDuty` classes inherit from it. Weight capacities are 15 kg, 30 kg, and 60 kg respectively. Battery drain rates are 0.20, 0.25, and 0.35.

### Module 5: Intelligent Assignment and Picking Plan
Generates picking plans by finding the nearest reachable racks containing required products. Dijkstra's algorithm arranges rack visits efficiently. Large orders are split into multiple trips based on robot capacity. Robot selection is based on fewest trips, shortest total distance, and smallest Robot ID. A 10% battery safety reserve is maintained.

### Module 6: Order Picking and Delivery Execution
Executes generated picking plans one trip at a time. Validates pathways, battery, load capacity, and reservations before making changes. Supports route recalculation if a pathway becomes blocked during execution.

### Module 7: Robot Battery and Charging Management
Manages battery levels and charging station availability. Robots below 20% battery are directed to the nearest reachable charging station with an available slot. If all slots are occupied, the robot enters a FIFO waiting queue. Charging restores battery to 100%.

### Module 8: Operation History and Undo Management
A manually implemented LIFO stack maintains history of reversible administrative operations: manual stock updates, pathway blocking/unblocking, and robot maintenance status changes. Each operation stores its previous state for restoration via undo.

---

## Data Structures Used

| Data Structure | Purpose |
|---|---|
| Self-Balancing AVL Tree | Product indexing by ProductID (O(log n) lookup) |
| Singly Linked Lists | Product storage racks, reservation records, customer orders, order items, planned trips/stops, robot fleet, charging stations |
| Adjacency-List Graph | Undirected weighted warehouse layout |
| Breadth-First Search (BFS) | Unweighted reachability across OPEN pathways |
| Dijkstra's Algorithm | Shortest weighted route with 3-level deterministic tie-breaking |
| Linked FIFO Queues | Pending customer orders and station waiting queues |
| Linked Stack | Reversible administrative operation undo history |

---

### Requirements
- C++ compiler (g++, clang++, or MSVC)
- C++11 or later

## Usage Guide

- On launch, the application presents a main menu. Navigate by entering the number corresponding to the desired operation. Key sections include:
- Warehouse Layout — add locations, add pathways, block/unblock pathways, check reachability
- Inventory — add products, update stock, view stock totals, check low-stock alerts
- Orders — create orders, view pending queue, cancel orders
- Robots — view fleet, change maintenance status
- Assignment & Picking — generate picking plans, view trip breakdowns
- Execution — execute trips, validate routes
- Charging — dispatch robots to charging, view waiting queue
- History & Undo — view operation history, undo last reversible action
- All inputs are validated (IDs must follow L##, P###, R### patterns; numbers must fall within expected ranges). Invalid input is rejected with a helpful message rather than crashing.

## Design Decisions

- No STL containers — All lists, queues, stacks, trees, and graphs are implemented from scratch using pointers and dynamic allocation, as required by the project specification.
- Deterministic tie-breaking — When multiple shortest paths exist, Dijkstra selects by distance, then fewer locations, then smallest Location ID. This guarantees reproducible results.
- Battery safety reserve — A 10% reserve is enforced so robots never strand mid-route.
- Validation before mutation — Order execution validates the entire trip before deducting stock or updating status, preventing partial or inconsistent state changes.
- Modular single-file design — All modules live in one main.cpp for easy submission and review, organized by clear section headers.
- File-based initialization — Warehouse data loads from text files, keeping the binary independent of hardcoded layouts and making the system easy to reset or       reconfigure.

### Course Information

Course: Data Structures (CL2001)
Project: Intelligent Warehouse Robot and Automated Order Fulfillment System
Semester: Fall 2026
Institution: FAST-NUCES, CFD Campus
