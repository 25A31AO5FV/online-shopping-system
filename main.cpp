#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

struct Product {
    int id{};
    string name;
    double price{};
    int stock{};
};

struct CartItem {
    int productId{};
    int quantity{};
};

class ShoppingSystem {
private:
    vector<Product> products;
    vector<CartItem> cart;
    const string productFile = "products.txt";
    const string orderFile = "orders.txt";

    Product* findProduct(int id) {
        for (auto& p : products) {
            if (p.id == id) return &p;
        }
        return nullptr;
    }

    const Product* findProduct(int id) const {
        for (const auto& p : products) {
            if (p.id == id) return &p;
        }
        return nullptr;
    }

    void saveProducts() const {
        ofstream out(productFile);
        for (const auto& p : products) {
            out << p.id << ' ' << quoted(p.name) << ' '
                << fixed << setprecision(2) << p.price << ' ' << p.stock << '\n';
        }
    }

    void loadProducts() {
        ifstream in(productFile);
        Product p;
        while (in >> p.id >> quoted(p.name) >> p.price >> p.stock) {
            products.push_back(p);
        }

        // First-run sample catalog.
        if (products.empty()) {
            products = {
                {101, "Wireless Mouse", 599.00, 20},
                {102, "Mechanical Keyboard", 1899.00, 12},
                {103, "USB-C Cable", 299.00, 35},
                {104, "Bluetooth Headphones", 1499.00, 15},
                {105, "Laptop Stand", 899.00, 10}
            };
            saveProducts();
        }
    }

    void showProducts() const {
        cout << "\n================ PRODUCT CATALOG ================\n";
        cout << left << setw(8) << "ID" << setw(28) << "PRODUCT"
             << right << setw(12) << "PRICE (Rs.)" << setw(10) << "STOCK\n";
        cout << string(58, '-') << '\n';

        for (const auto& p : products) {
            cout << left << setw(8) << p.id << setw(28) << p.name
                 << right << setw(12) << fixed << setprecision(2) << p.price
                 << setw(10) << p.stock << '\n';
        }
        cout << string(58, '-') << '\n';
    }

    void addToCart() {
        showProducts();
        int id, quantity;
        cout << "Enter product ID: ";
        if (!(cin >> id)) { invalidInput(); return; }
        cout << "Enter quantity: ";
        if (!(cin >> quantity)) { invalidInput(); return; }

        if (quantity <= 0) {
            cout << "Quantity must be greater than zero.\n";
            return;
        }

        Product* p = findProduct(id);
        if (!p) {
            cout << "Product not found.\n";
            return;
        }

        int alreadyInCart = 0;
        for (const auto& item : cart) {
            if (item.productId == id) alreadyInCart += item.quantity;
        }

        if (quantity + alreadyInCart > p->stock) {
            cout << "Only " << p->stock - alreadyInCart
                 << " more unit(s) can be added to your cart.\n";
            return;
        }

        auto it = find_if(cart.begin(), cart.end(),
                          [id](const CartItem& item) { return item.productId == id; });
        if (it == cart.end()) cart.push_back({id, quantity});
        else it->quantity += quantity;

        cout << "Product added to cart.\n";
    }

    double cartTotal() const {
        double total = 0.0;
        for (const auto& item : cart) {
            const Product* p = findProduct(item.productId);
            if (p) total += p->price * item.quantity;
        }
        return total;
    }

    void viewCart() const {
        cout << "\n==================== YOUR CART ====================\n";
        if (cart.empty()) {
            cout << "Your cart is empty.\n";
            return;
        }

        cout << left << setw(28) << "PRODUCT" << right
             << setw(8) << "QTY" << setw(14) << "UNIT PRICE"
             << setw(14) << "SUBTOTAL\n";
        cout << string(64, '-') << '\n';

        for (const auto& item : cart) {
            const Product* p = findProduct(item.productId);
            if (!p) continue;
            cout << left << setw(28) << p->name << right
                 << setw(8) << item.quantity
                 << setw(14) << fixed << setprecision(2) << p->price
                 << setw(14) << p->price * item.quantity << '\n';
        }
        cout << string(64, '-') << '\n';
        cout << "Cart total: Rs. " << fixed << setprecision(2) << cartTotal() << '\n';
    }

    void removeFromCart() {
        if (cart.empty()) {
            cout << "Your cart is empty.\n";
            return;
        }
        viewCart();
        int id;
        cout << "Enter product ID to remove: ";
        if (!(cin >> id)) { invalidInput(); return; }

        auto oldSize = cart.size();
        cart.erase(remove_if(cart.begin(), cart.end(),
                             [id](const CartItem& item) { return item.productId == id; }),
                   cart.end());

        cout << (cart.size() < oldSize ? "Item removed from cart.\n"
                                      : "That product is not in your cart.\n");
    }

    void checkout() {
        if (cart.empty()) {
            cout << "Your cart is empty. Add products before checkout.\n";
            return;
        }

        viewCart();
        cout << "\nProceed with checkout? (y/n): ";
        char confirm;
        cin >> confirm;
        if (confirm != 'y' && confirm != 'Y') {
            cout << "Checkout cancelled.\n";
            return;
        }

        // Validate all stock before changing any inventory.
        for (const auto& item : cart) {
            const Product* p = findProduct(item.productId);
            if (!p || p->stock < item.quantity) {
                cout << "Checkout failed: stock changed for a cart item.\n";
                return;
            }
        }

        for (const auto& item : cart) {
            Product* p = findProduct(item.productId);
            p->stock -= item.quantity;
        }
        saveProducts();

        ofstream orders(orderFile, ios::app);
        orders << "ORDER | " << quoted("Customer")
               << " | Total Rs. " << fixed << setprecision(2) << cartTotal() << '\n';
        for (const auto& item : cart) {
            const Product* p = findProduct(item.productId);
            if (p) orders << "  " << quoted(p->name) << " x "
                          << item.quantity << '\n';
        }
        orders << "----------------------------------------\n";

        cout << "\nOrder placed successfully!\n";
        cout << "Amount paid (demo): Rs. " << fixed << setprecision(2)
             << cartTotal() << '\n';
        cart.clear();
    }

    void viewOrderHistory() const {
        ifstream in(orderFile);
        if (!in) {
            cout << "No order history found yet.\n";
            return;
        }
        cout << "\n================ ORDER HISTORY ================\n";
        string line;
        while (getline(in, line)) cout << line << '\n';
    }

    void adminAddProduct() {
        Product p;
        cout << "Enter new product ID: ";
        if (!(cin >> p.id)) { invalidInput(); return; }
        if (findProduct(p.id)) {
            cout << "That product ID already exists.\n";
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter product name: ";
        getline(cin, p.name);
        if (p.name.empty()) {
            cout << "Product name cannot be empty.\n";
            return;
        }
        cout << "Enter price (Rs.): ";
        if (!(cin >> p.price)) { invalidInput(); return; }
        cout << "Enter stock quantity: ";
        if (!(cin >> p.stock)) { invalidInput(); return; }

        if (p.price < 0 || p.stock < 0) {
            cout << "Price and stock cannot be negative.\n";
            return;
        }
        products.push_back(p);
        saveProducts();
        cout << "Product added to catalog.\n";
    }

    void invalidInput() {
        cout << "Invalid input. Please enter the requested value.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

public:
    ShoppingSystem() { loadProducts(); }

    void run() {
        int choice = 0;
        do {
            cout << "\n\n========== ONLINE SHOPPING SYSTEM ==========\n"
                 << "1. View products\n"
                 << "2. Add product to cart\n"
                 << "3. View cart\n"
                 << "4. Remove item from cart\n"
                 << "5. Checkout\n"
                 << "6. View order history\n"
                 << "7. Admin: add product\n"
                 << "8. Exit\n"
                 << "Choose an option: ";

            if (!(cin >> choice)) {
                invalidInput();
                continue;
            }

            switch (choice) {
                case 1: showProducts(); break;
                case 2: addToCart(); break;
                case 3: viewCart(); break;
                case 4: removeFromCart(); break;
                case 5: checkout(); break;
                case 6: viewOrderHistory(); break;
                case 7: adminAddProduct(); break;
                case 8: cout << "Thank you for shopping with us!\n"; break;
                default: cout << "Invalid option. Choose 1-8.\n";
            }
        } while (choice != 8);
    }
};

int main() {
    ShoppingSystem app;
    app.run();
    return 0;
}
