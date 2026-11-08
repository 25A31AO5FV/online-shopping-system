# Online Shopping System (C++)

An intermediate-level, menu-driven console application demonstrating object-oriented programming, STL containers, input validation, and file handling.

## Features
- Display a product catalog with ID, name, price, and stock.
- Add products to a shopping cart.
- View cart quantities, subtotals, and total.
- Remove a product from the cart.
- Checkout with a stock validation step.
- Reduce inventory after a successful checkout.
- Save and display order history.
- Add products through a simple admin menu.
- Persist product inventory between program runs.

> This is an academic/demo project. It does not process real payments, authenticate administrators, or store customer personal/payment information.

## Requirements
- C++ compiler supporting C++17 (GCC 8+, Clang 7+, or a recent Visual Studio).
- Windows, Linux, or macOS terminal.

## Files
```text
OnlineShoppingSystem_CPP/
├── main.cpp
├── README.md
├── docs/
│   └── PROJECT_REPORT.md
└── data/
    └── README.txt
```

At first launch, the program creates `products.txt` in the current working directory and inserts five sample products. After checkout, it creates/appends `orders.txt`.

## Build and run

### Windows (MinGW-w64 / GCC)
Open Command Prompt or the VS Code terminal in this folder:
```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o shopping.exe
shopping.exe
```

### Linux / macOS
```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o shopping
./shopping
```

### Visual Studio
Create a Console App, replace the generated source with `main.cpp`, select the C++17 language standard, then Build and Run.

**Important:** Run the executable with this project folder as its working directory so the data files are created in a predictable location.

## Quick test
1. Choose `1` to view the sample catalog.
2. Choose `2`, enter product ID `101`, and quantity `2`.
3. Choose `3` to confirm the cart total.
4. Choose `5`, then enter `y` to place the demo order.
5. Choose `6` to see the saved order history.
6. Exit and run the program again; check that the updated stock remains saved.

## Data format
- `products.txt`: one product per line, stored as `id "product name" price stock`.
- `orders.txt`: readable order summary appended after each successful checkout.

Both files are generated at runtime and are intentionally excluded from version control.
