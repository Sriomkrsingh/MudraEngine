# Mudra Engine: Smart Currency Exchange & Routing System

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![Algorithms](https://img.shields.io/badge/Algorithms-Graph_Theory-orange.svg)
![Maths](https://img.shields.io/badge/Maths-Logarithmic_Optimization-purple.svg)
![Build](https://img.shields.io/badge/Build-Zero_Dependencies-success.svg)

**Mudra Engine** is a fast, C++ based routing system I **developed** to find the absolute cheapest way to convert money across borders. It uses graph **algorithms** to calculate whether a direct currency exchange is the best option, or if taking a multi-step "detour" through major currencies (like USD or EUR) will save you money on bank fees.

## 🚀 Why I Built This

When you send money directly between two smaller economies (for example, Brazilian Real to Indian Rupee), banks charge massive hidden fees (spreads) because those trades are rare. 

However, trading Brazilian Real to US Dollars, and then US Dollars to Indian Rupees is very common, so the fees are tiny. Mudra Engine acts like a "Google Maps for Money." It instantly scans global exchange rates and bank fees to find the most cost-effective path for your money to travel.

## 🧠 How the Algorithm and Maths Work

Standard pathfinding **algorithms** (like Dijkstra's) are designed to *add* distances together. But in finance, exchange rates are *multiplied*. You cannot just plug currency rates into a standard algorithm. 

To solve this, I **engineered** a mathematical trick using Logarithms:

1. **Calculate the Real Rate:** First, the engine subtracts the bank's fee from the market rate.
   `Effective Rate = Mid-Market Rate * (1 - Bank Fee %)`

2. **The Maths Transformation:** I used a negative natural logarithm to turn the multiplication problem into an addition problem.
   `Edge Weight = -ln(Effective Rate)`

By feeding these negative logarithms into the algorithm, Mudra Engine mathematically guarantees it will find the route that leaves the most money in the user's pocket.

## ⚙️ Key Features

* **Engineered Routing:** **Developed** a custom version of Dijkstra’s Algorithm in C++ to instantly explore thousands of possible currency routes.
* **Smart Memory:** Built an instant memory cache so if a user asks for the same route twice, the engine remembers the answer in zero seconds.
* **Real-World Fees:** The network understands that trading Currency A to Currency B has a different fee than trading B to A (just like real banks).
* **Dynamic Data:** Reads market rates easily from a `rates.csv` file, meaning you can update the world's economy without rewriting a single line of C++ code.

## 🛠️ How to Run on Your Machine

This project was **engineered** using standard C++17 with no messy external libraries to install. 

Run the following commands in your terminal to download, compile, and execute the engine:


### 1. Download the code and open the folder
### 2. Compile the C++ code
### 3. Run the program (Use the command for your OS)
```bash
 git clone [https://github.com/YourUsername/Mudra-Engine.git](https://github.com/YourUsername/Mudra-Engine.git)
    cd Mudra-Engine

  g++ -std=c++17 main.cpp MudraEngine.cpp -o mudra_engine

  ./mudra_engine       # For Mac or Linux
  .\mudra_engine.exe   # For Windows (PowerShell)
