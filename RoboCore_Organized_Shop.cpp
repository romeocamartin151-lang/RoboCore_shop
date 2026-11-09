#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <limits>
#include <cctype>
using namespace std;

struct Product {
    int id;
    string name;
    string category;
    string brand;
    double price;
    int stockQuantity;
};

// Lab 2: Doubly linked list for the shopping cart.
struct CartNode {
    int productId, quantity;
    CartNode* prev;
    CartNode* next;
};

// Lab 3: Circular linked list for partner brands.
struct BrandNode {
    string brand;
    BrandNode* next;
};

// Lab 4: Stack records the previous stock value for Undo.
struct UndoNode {
    int productId, oldStock;
    UndoNode* next;
};

// Lab 5: Queue stores checkout orders in first-in, first-out order.
struct OrderNode {
    int orderNumber;
    double total;
    OrderNode* next;
};

// Lab 6: Binary Search Tree indexes product IDs.
struct TreeNode {
    int id;
    TreeNode* left;
    TreeNode* right;
};

const int MAX_PRODUCTS = 100;
Product products[MAX_PRODUCTS];
int productCount = 0;
int nextOrderNumber = 5001;

CartNode* cartHead = nullptr;
CartNode* cartTail = nullptr;
BrandNode* brandHead = nullptr;
UndoNode* undoTop = nullptr;
OrderNode* orderFront = nullptr;
OrderNode* orderRear = nullptr;
TreeNode* treeRoot = nullptr;

// -------------------- GENERAL HELPERS --------------------
string lowerText(string text) {
    transform(text.begin(), text.end(), text.begin(),
        [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return text;
}

int findProductIndex(int id) {
    for (int i = 0; i < productCount; ++i)
        if (products[i].id == id) return i;
    return -1;
}

// Lab 1: Dynamic allocation is demonstrated when adding linked nodes.
// The catalog uses a simple fixed-size array to keep the project easy to follow.
// -------------------- DATA STRUCTURE OPERATIONS --------------------
void addBrand(const string& brand) {
    if (!brandHead) {
        brandHead = new BrandNode{brand, nullptr};
        brandHead->next = brandHead;
        return;
    }
    BrandNode* current = brandHead;
    do {
        if (lowerText(current->brand) == lowerText(brand)) return;
        current = current->next;
    } while (current != brandHead);

    BrandNode* node = new BrandNode{brand, brandHead};
    current = brandHead;
    while (current->next != brandHead) current = current->next;
    current->next = node;
}

TreeNode* insertTree(TreeNode* root, int id) {
    if (!root) return new TreeNode{id, nullptr, nullptr};
    if (id < root->id) root->left = insertTree(root->left, id);
    else if (id > root->id) root->right = insertTree(root->right, id);
    return root;
}

bool searchTree(TreeNode* root, int id) {
    if (!root) return false;
    if (root->id == id) return true;
    return id < root->id ? searchTree(root->left, id) : searchTree(root->right, id);
}

// Remove a product ID from the BST while preserving the tree structure.
TreeNode* findMinNode(TreeNode* root) {
    while (root && root->left) root = root->left;
    return root;
}

TreeNode* removeTreeNode(TreeNode* root, int id) {
    if (!root) return nullptr;
    if (id < root->id) root->left = removeTreeNode(root->left, id);
    else if (id > root->id) root->right = removeTreeNode(root->right, id);
    else {
        if (!root->left) {
            TreeNode* rightChild = root->right;
            delete root;
            return rightChild;
        }
        if (!root->right) {
            TreeNode* leftChild = root->left;
            delete root;
            return leftChild;
        }
        TreeNode* successor = findMinNode(root->right);
        root->id = successor->id;
        root->right = removeTreeNode(root->right, successor->id);
    }
    return root;
}

void deleteTree(TreeNode* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

void addProduct(int id, string name, string category, string brand,
                double price, int stock) {
    if (productCount >= MAX_PRODUCTS) {
        cout << "Catalog is full.\n";
        return;
    }
    if (findProductIndex(id) != -1) {
        cout << "That product ID already exists.\n";
        return;
    }
    products[productCount++] = {id, name, category, brand, price, stock};
    addBrand(brand);
    treeRoot = insertTree(treeRoot, id);
}

// -------------------- INITIAL PRODUCT CATALOG --------------------
void loadProducts() {
    addProduct(1001, "Uno-Compatible Development Board", "Microcontroller", "Arduino", 650, 12);
    addProduct(1002, "Nano-Compatible Development Board", "Microcontroller", "Arduino", 350, 15);
    addProduct(1003, "Dual Motor Driver Module", "Motor Driver", "Pololu", 450, 8);
    addProduct(1004, "Micro Metal Gearmotor", "Motor", "Pololu", 380, 10);
    addProduct(1005, "Ultrasonic Distance Sensor", "Distance Sensor", "DFRobot", 180, 20);
    addProduct(1006, "IR Line Tracking Sensor", "Line Sensor", "DFRobot", 150, 16);
    addProduct(1007, "Distance Sensor Breakout Board", "Sensor Module", "SparkFun", 320, 9);
    addProduct(1008, "Electronic Prototyping Kit", "Prototyping Kit", "SparkFun", 550, 7);
    addProduct(1009, "Robot Chassis Kit", "Robot Chassis", "Pololu", 480, 6);
    addProduct(1010, "Jumper Wire Set", "Wiring", "SparkFun", 100, 30);
}

// -------------------- PRODUCT DISPLAY & INPUT --------------------
void showHeader() {
    cout << left << setw(7) << "ID" << setw(34) << "PRODUCT"
         << setw(18) << "CATEGORY" << setw(12) << "BRAND"
         << right << setw(10) << "PRICE" << setw(8) << "STOCK\n";
    cout << string(89, '-') << '\n';
}

void showProduct(const Product& p) {
    cout << left << setw(7) << p.id << setw(34) << p.name.substr(0, 32)
         << setw(18) << p.category.substr(0, 16) << setw(12) << p.brand
         << right << setw(10) << fixed << setprecision(2) << p.price
         << setw(8) << p.stockQuantity << '\n';
}

// Display partner brands first, followed by the complete product catalog.
void showBrands();

void showCatalog() {
    cout << "\n========== PARTNER BRANDS ==========\n";
    showBrands();
    cout << "\n========== ALL PRODUCTS ============\n";
    showHeader();
    for (int i = 0; i < productCount; ++i) showProduct(products[i]);
}

int readInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Please enter a valid whole number.\n";
    }
}

double readPrice(const string& prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter a valid non-negative price.\n";
    }
}

string readLine(const string& prompt) {
    string value;
    cout << prompt;
    getline(cin, value);
    return value;
}

// -------------------- CATALOG SEARCH & FILTER --------------------
void filterBrand() {
    string brand = lowerText(readLine("Enter brand: "));
    bool found = false;
    showHeader();
    for (int i = 0; i < productCount; ++i) {
        if (lowerText(products[i].brand) == brand) {
            showProduct(products[i]);
            found = true;
        }
    }
    if (!found) cout << "No products found for that brand.\n";
}

// Lab 7: Linear search through product names, categories, and brands.
void searchKeyword() {
    string keyword = lowerText(readLine("Search keyword: "));
    if (keyword.empty()) {
        cout << "Please enter a keyword.\n";
        return;
    }

    bool found = false;
    showHeader();
    for (int i = 0; i < productCount; ++i) {
        string text = lowerText(products[i].name + " " + products[i].category + " " + products[i].brand);
        if (text.find(keyword) != string::npos) {
            showProduct(products[i]);
            found = true;
        }
    }
    if (!found) cout << "No matching products found.\n";
}

// Lab 7: Sort a copy by price, then use binary search for price boundaries.
void filterPrice() {
    double low = readPrice("Minimum price (PHP): ");
    double high = readPrice("Maximum price (PHP): ");
    if (low > high) {
        cout << "Minimum price must not exceed maximum price.\n";
        return;
    }

    Product sorted[MAX_PRODUCTS];
    copy(products, products + productCount, sorted);
    sort(sorted, sorted + productCount,
        [](const Product& a, const Product& b) { return a.price < b.price; });

    int left = 0, right = productCount;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (sorted[mid].price < low) left = mid + 1;
        else right = mid;
    }
    int start = left;
    left = 0; right = productCount;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (sorted[mid].price <= high) left = mid + 1;
        else right = mid;
    }
    int end = left;

    if (start == end) {
        cout << "No products found in that price range.\n";
        return;
    }
    showHeader();
    for (int i = start; i < end; ++i) showProduct(sorted[i]);
}

// -------------------- CART & CHECKOUT --------------------
void addToCart() {
    int id = readInt("Product ID: ");
    int qty = readInt("Quantity: ");
    int index = findProductIndex(id);
    if (index == -1 || qty <= 0) {
        cout << "Invalid product ID or quantity.\n";
        return;
    }
    if (products[index].stockQuantity < qty) {
        cout << "Not enough stock. Available: " << products[index].stockQuantity << '\n';
        return;
    }

    CartNode* current = cartHead;
    while (current) {
        if (current->productId == id) {
            if (current->quantity + qty > products[index].stockQuantity) {
                cout << "Cart quantity exceeds available stock.\n";
                return;
            }
            current->quantity += qty;
            cout << "Cart quantity updated.\n";
            return;
        }
        current = current->next;
    }

    CartNode* node = new CartNode{id, qty, cartTail, nullptr};
    if (!cartHead) cartHead = cartTail = node;
    else {
        cartTail->next = node;
        cartTail = node;
    }
    cout << "Added to cart.\n";
}

void viewCart() {
    if (!cartHead) {
        cout << "Cart is empty.\n";
        return;
    }
    double total = 0;
    for (CartNode* node = cartHead; node; node = node->next) {
        int index = findProductIndex(node->productId);
        if (index == -1) continue;
        double subtotal = products[index].price * node->quantity;
        cout << products[index].name << " | Qty: " << node->quantity
             << " | Subtotal: PHP " << fixed << setprecision(2) << subtotal << '\n';
        total += subtotal;
    }
    cout << "Total: PHP " << fixed << setprecision(2) << total << '\n';
}


// Remove one product entry from the shopping cart.
void removeFromCart() {
    if (!cartHead) {
        cout << "Shopping cart is empty.\n";
        return;
    }

    int id = readInt("Enter the product ID to remove from cart: ");
    CartNode* current = cartHead;
    while (current && current->productId != id) current = current->next;

    if (!current) {
        cout << "That product is not in your cart.\n";
        return;
    }

    if (current->prev) current->prev->next = current->next;
    else cartHead = current->next;

    if (current->next) current->next->prev = current->prev;
    else cartTail = current->prev;

    delete current;
    cout << "Product removed from shopping cart.\n";
}

// Lab 5: Add a checkout order to the FIFO queue; deduct stock at submission.
void checkout() {
    if (!cartHead) {
        cout << "Cart is empty.\n";
        return;
    }

    for (CartNode* node = cartHead; node; node = node->next) {
        int index = findProductIndex(node->productId);
        if (index == -1 || products[index].stockQuantity < node->quantity) {
            cout << "Checkout stopped: stock is no longer sufficient.\n";
            return;
        }
    }

    double total = 0;
    for (CartNode* node = cartHead; node; node = node->next) {
        int index = findProductIndex(node->productId);
        UndoNode* undo = new UndoNode{node->productId, products[index].stockQuantity, undoTop};
        undoTop = undo;
        products[index].stockQuantity -= node->quantity;
        total += products[index].price * node->quantity;
    }

    OrderNode* order = new OrderNode{nextOrderNumber++, total, nullptr};
    if (!orderRear) orderFront = orderRear = order;
    else {
        orderRear->next = order;
        orderRear = order;
    }
    cout << "Order submitted. Total: PHP " << fixed << setprecision(2) << total << '\n';

    while (cartHead) {
        CartNode* next = cartHead->next;
        delete cartHead;
        cartHead = next;
    }
    cartTail = nullptr;
}

// -------------------- ORDER QUEUE & STOCK UNDO --------------------
void processOrder() {
    if (!orderFront) {
        cout << "No orders waiting.\n";
        return;
    }
    OrderNode* order = orderFront;
    orderFront = orderFront->next;
    if (!orderFront) orderRear = nullptr;
    cout << "Processed order #" << order->orderNumber << " | Total: PHP "
         << fixed << setprecision(2) << order->total << '\n';
    delete order;
}

void showOrders() {
    if (!orderFront) {
        cout << "No orders waiting.\n";
        return;
    }
    for (OrderNode* node = orderFront; node; node = node->next)
        cout << "Order #" << node->orderNumber << " | PHP "
             << fixed << setprecision(2) << node->total << '\n';
}

// Lab 4: Pop the most recent saved stock value.
void undoStock() {
    if (!undoTop) {
        cout << "Nothing to undo.\n";
        return;
    }
    UndoNode* node = undoTop;
    undoTop = undoTop->next;
    int index = findProductIndex(node->productId);
    if (index != -1) {
        products[index].stockQuantity = node->oldStock;
        cout << "Stock restored to " << node->oldStock << ".\n";
    }
    delete node;
}

void showBrands() {
    if (!brandHead) return;
    cout << "Partner brands: ";
    BrandNode* current = brandHead;
    do {
        cout << current->brand;
        current = current->next;
        if (current != brandHead) cout << " -> ";
    } while (current != brandHead);
    cout << " -> back to start\n";
}

// Lab 8: Insertion sort by price; quicksort by stock or brand.
// -------------------- SORTING ALGORITHMS --------------------
int partition(Product arr[], int low, int high, int mode) {
    Product pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; ++j) {
        bool before = mode == 2 ? arr[j].stockQuantity < pivot.stockQuantity
                                : lowerText(arr[j].brand) < lowerText(pivot.brand);
        if (before) {
            ++i;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSort(Product arr[], int low, int high, int mode) {
    if (low >= high) return;
    int pivot = partition(arr, low, high, mode);
    quickSort(arr, low, pivot - 1, mode);
    quickSort(arr, pivot + 1, high, mode);
}

void sortProducts() {
    cout << "1. Price (insertion sort)\n2. Stock (quicksort)\n3. Brand (quicksort)\n";
    int choice = readInt("Choose: ");
    if (choice < 1 || choice > 3) {
        cout << "Invalid choice.\n";
        return;
    }
    Product sorted[MAX_PRODUCTS];
    copy(products, products + productCount, sorted);

    if (choice == 1) {
        for (int i = 1; i < productCount; ++i) {
            Product key = sorted[i];
            int j = i - 1;
            while (j >= 0 && sorted[j].price > key.price) {
                sorted[j + 1] = sorted[j];
                --j;
            }
            sorted[j + 1] = key;
        }
    } else quickSort(sorted, 0, productCount - 1, choice);

    showHeader();
    for (int i = 0; i < productCount; ++i) showProduct(sorted[i]);
}

// -------------------- BST SEARCH & INVENTORY MANAGEMENT --------------------
void searchById() {
    int id = readInt("Product ID: ");
    if (searchTree(treeRoot, id)) {
        int index = findProductIndex(id);
        if (index != -1) {
            showHeader();
            showProduct(products[index]);
        }
    } else cout << "Product not found.\n";
}

void addNewProduct() {
    if (productCount >= MAX_PRODUCTS) {
        cout << "Catalog is full.\n";
        return;
    }
    int id = readInt("New product ID: ");
    if (id <= 0 || findProductIndex(id) != -1) {
        cout << "ID must be positive and unique.\n";
        return;
    }
    string name = readLine("Name: ");
    string category = readLine("Category: ");
    string brand = readLine("Brand: ");
    double price = readPrice("Price (PHP): ");
    int stock = readInt("Stock (0 or higher): ");
    if (name.empty() || category.empty() || brand.empty() || stock < 0) {
        cout << "Name, category, and brand are required; stock cannot be negative.\n";
        return;
    }
    addProduct(id, name, category, brand, price, stock);
    cout << "Product added.\n";
}

// Remove a brand from the circular list when no products use it anymore.
void removeBrandIfUnused(const string& brand) {
    for (int i = 0; i < productCount; ++i)
        if (lowerText(products[i].brand) == lowerText(brand)) return;

    if (!brandHead) return;
    BrandNode* current = brandHead;
    BrandNode* previous = nullptr;
    do {
        if (lowerText(current->brand) == lowerText(brand)) {
            if (current->next == current) brandHead = nullptr;
            else {
                if (!previous) {
                    BrandNode* tail = brandHead;
                    while (tail->next != brandHead) tail = tail->next;
                    brandHead = brandHead->next;
                    tail->next = brandHead;
                } else previous->next = current->next;
            }
            delete current;
            return;
        }
        previous = current;
        current = current->next;
    } while (current != brandHead);
}

void removeProduct() {
    int id = readInt("Enter product ID to remove: ");
    int index = findProductIndex(id);
    if (index == -1) {
        cout << "Product not found. Nothing was removed.\n";
        return;
    }

    showHeader();
    showProduct(products[index]);
    string confirm = lowerText(readLine("Remove this product? (yes/no): "));
    if (confirm != "yes" && confirm != "y") {
        cout << "Removal cancelled.\n";
        return;
    }

    string removedBrand = products[index].brand;

    // Remove this product from the doubly linked shopping cart, if present.
    CartNode* cart = cartHead;
    while (cart) {
        CartNode* next = cart->next;
        if (cart->productId == id) {
            if (cart->prev) cart->prev->next = cart->next;
            else cartHead = cart->next;
            if (cart->next) cart->next->prev = cart->prev;
            else cartTail = cart->prev;
            delete cart;
        }
        cart = next;
    }

    // Remove old undo records for this product to avoid stale stock references.
    UndoNode* previousUndo = nullptr;
    UndoNode* undo = undoTop;
    while (undo) {
        UndoNode* next = undo->next;
        if (undo->productId == id) {
            if (previousUndo) previousUndo->next = next;
            else undoTop = next;
            delete undo;
        } else previousUndo = undo;
        undo = next;
    }

    for (int i = index; i < productCount - 1; ++i)
        products[i] = products[i + 1];
    --productCount;

    treeRoot = removeTreeNode(treeRoot, id);
    removeBrandIfUnused(removedBrand);

    cout << "Product " << id << " was successfully removed from the catalog.\n";
    cout << "Any matching cart item and undo record were also removed.\n";
}

void updateStock() {
    int id = readInt("Product ID: ");
    int index = findProductIndex(id);
    if (index == -1) {
        cout << "Product not found.\n";
        return;
    }
    int stock = readInt("New stock (0 or higher): ");
    if (stock < 0) {
        cout << "Stock cannot be negative.\n";
        return;
    }
    undoTop = new UndoNode{id, products[index].stockQuantity, undoTop};
    products[index].stockQuantity = stock;
    cout << "Stock updated.\n";
}

// -------------------- MEMORY CLEANUP --------------------
void cleanup() {
    while (cartHead) {
        CartNode* next = cartHead->next;
        delete cartHead;
        cartHead = next;
    }
    while (undoTop) {
        UndoNode* next = undoTop->next;
        delete undoTop;
        undoTop = next;
    }
    while (orderFront) {
        OrderNode* next = orderFront->next;
        delete orderFront;
        orderFront = next;
    }
    if (brandHead) {
        BrandNode* current = brandHead->next;
        while (current != brandHead) {
            BrandNode* next = current->next;
            delete current;
            current = next;
        }
        delete brandHead;
        brandHead = nullptr;
    }
    deleteTree(treeRoot);
}

// -------------------- MAIN MENU & PROGRAM ENTRY --------------------
void menu() {
    cout << "\n==============================================\n"
         << "                 ROBOCORE\n"
         << "       Robotics & Automation Store\n"
         << "==============================================\n"
         << " PRODUCT CATALOG\n"
         << "  1. View partner brands & all products\n"
         << "  2. Filter products by brand\n"
         << "  3. Search products by keyword\n"
         << "  4. Filter products by price\n"
         << "  5. Sort products\n"
         << "  6. Search product by ID (BST)\n"
         << "----------------------------------------------\n"
         << " SHOPPING & CHECKOUT\n"
         << "  7. Add product to cart\n"
         << "  8. View shopping cart\n"
         << "  9. Remove product from cart\n"
         << " 10. Checkout\n"
         << " 11. Process next order\n"
         << " 12. View waiting orders\n"
         << "----------------------------------------------\n"
         << " INVENTORY & BRANDS\n"
         << " 13. Add a new product\n"
         << " 14. Remove a product from inventory\n"
         << " 15. Update product stock\n"
         << " 16. Undo last stock change\n"
         << "----------------------------------------------\n"
         << "  0. Exit\n"
         << "==============================================\n";
}

int main() {
    loadProducts();
    int choice;

    do {
        menu();
        choice = readInt("Choose an option: ");

        switch (choice) {
            // Product catalog
            case 1: showCatalog(); break;
            case 2: filterBrand(); break;
            case 3: searchKeyword(); break;
            case 4: filterPrice(); break;
            case 5: sortProducts(); break;
            case 6: searchById(); break;

            // Shopping and checkout
            case 7: addToCart(); break;
            case 8: viewCart(); break;
            case 9: removeFromCart(); break;
            case 10: checkout(); break;
            case 11: processOrder(); break;
            case 12: showOrders(); break;

            // Inventory and partner brands
            case 13: addNewProduct(); break;
            case 14: removeProduct(); break;
            case 15: updateStock(); break;
            case 16: undoStock(); break;

            case 0: cout << "Thank you for using RoboCore.\n"; break;
            default: cout << "Choose a valid menu number.\n";
        }
    } while (choice != 0);

    cleanup();
    return 0;
}
