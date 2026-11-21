// SECTION 1 : MODULE 2 — PRODUCT INVENTORY & STORAGE RACK MANAGEMENT
// Reservation, RackStock, Product, AVL Tree

// Represents a single order's reservation record inside a specific storage rack
struct Reservation
{
    string orderId;
    int quantity;
    Reservation* next;

    Reservation(string o = "", int q = 0) : orderId(o), quantity(q), next(NULL)
    {
    }
};

// Node in a product's linked list of storage racks
struct RackStock
{
    string rackId;
    int total, reserved;
    Reservation* reservations;
    RackStock* next;

    RackStock(string r = "", int t = 0)
        : rackId(r), total(t), reserved(0), reservations(NULL), next(NULL)
    {
    }

    // Destructor frees all reservation nodes for this rack entry
    ~RackStock()
    {
        while (reservations)
        {
            Reservation* p = reservations;
            reservations = reservations->next;
            delete p;
        }
    }

    // Available stock is total quantity minus reserved quantity
    int available() const
    {
        return total - reserved;
    }

    // Adds or merges a reservation record and maintains reserved == sum(reservations)
    void addReservation(const string& orderId, int qty)
    {
        if (qty <= 0)
            return;
        for (Reservation* res = reservations; res; res = res->next)
        {
            if (res->orderId == orderId)
            {
                res->quantity += qty;
                reserved += qty;
                return;
            }
        }
        Reservation* n = new Reservation(orderId, qty);
        if (!reservations)
        {
            reservations = n;
        }
        else
        {
            Reservation* cur = reservations;
            while (cur->next)
                cur = cur->next;
            cur->next = n;
        }
        reserved += qty;
    }

    // Releases all units reserved by 'orderId' in this rack when an order/assignment is cancelled
    int releaseOrder(const string& orderId)
    {
        int released = 0;
        Reservation** cur = &reservations;
        while (*cur)
        {
            if ((*cur)->orderId == orderId)
            {
                Reservation* target = *cur;
                released += target->quantity;
                reserved -= target->quantity;
                if (reserved < 0)
                    reserved = 0;
                *cur = target->next;
                delete target;
            }
            else
            {
                cur = &((*cur)->next);
            }
        }
        return released;
    }
};

// Product entity stored at each AVL tree node
struct Product
{
    string id, name;
    double unitWeight;
    int reorderLevel;
    RackStock* racks;

    Product(string i = "", string n = "", double w = 0.0, int r = 0)
        : id(i), name(n), unitWeight(w), reorderLevel(r), racks(NULL)
    {
    }

    // Destructor frees the linked list of RackStock nodes
    ~Product()
    {
        while (racks)
        {
            RackStock* p = racks;
            racks = racks->next;
            delete p;
        }
    }

    // Calculates total physical stock across all racks for this product
    int totalStock() const
    {
        int sum = 0;
        for (RackStock* r = racks; r; r = r->next)
            sum += r->total;
        return sum;
    }

    // Calculates total reserved stock across all racks for this product
    int reservedStock() const
    {
        int sum = 0;
        for (RackStock* r = racks; r; r = r->next)
            sum += r->reserved;
        return sum;
    }

    // Calculates total available stock (total - reserved) across all racks
    int availableStock() const
    {
        int sum = 0;
        for (RackStock* r = racks; r; r = r->next)
            sum += r->available();
        return sum;
    }
};

// AVL Tree Node indexing Product pointers by ProductID
struct AVLNode
{
    Product* data;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(Product* p) : data(p), left(NULL), right(NULL), height(1)
    {
    }
};

// Returns height of an AVL node (0 if NULL)
int height(AVLNode* n)
{
    return n ? n->height : 0;
}

// Returns the larger of two integers
int maxInt(int a, int b)
{
    return a > b ? a : b;
}

// Recalculates an AVL node's height from its children
void fixHeight(AVLNode* n)
{
    if (n)
        n->height = 1 + maxInt(height(n->left), height(n->right));
}

// Right rotation for Left-Left (LL) imbalance in the AVL tree
AVLNode* rotateRight(AVLNode* y)
{
    AVLNode* x = y->left;
    AVLNode* t = x->right;
    x->right = y;
    y->left = t;
    fixHeight(y);
    fixHeight(x);
    return x;
}

// Left rotation for Right-Right (RR) imbalance in the AVL tree
AVLNode* rotateLeft(AVLNode* x)
{
    AVLNode* y = x->right;
    AVLNode* t = y->left;
    y->left = x;
    x->right = t;
    fixHeight(x);
    fixHeight(y);
    return y;
}

// Computes the balance factor (left subtree height - right subtree height)
int balanceFactor(AVLNode* n)
{
    return n ? height(n->left) - height(n->right) : 0;
}

// Rebalances an AVL subtree using LL, LR, RR, or RL rotations
AVLNode* balance(AVLNode* n)
{
    if (!n)
        return n;
    fixHeight(n);
    if (balanceFactor(n) > 1)
    {
        if (balanceFactor(n->left) < 0)
            n->left = rotateLeft(n->left);
        return rotateRight(n);
    }
    if (balanceFactor(n) < -1)
    {
        if (balanceFactor(n->right) > 0)
            n->right = rotateRight(n->right);
        return rotateLeft(n);
    }
    return n;
}

// Recursively inserts a new product into the AVL tree and rebalances
AVLNode* avlInsert(AVLNode* n, Product* p, bool& inserted)
{
    if (!n)
    {
        inserted = true;
        return new AVLNode(p);
    }
    if (p->id < n->data->id)
        n->left = avlInsert(n->left, p, inserted);
    else if (p->id > n->data->id)
        n->right = avlInsert(n->right, p, inserted);
    else
        return n;
    return balance(n);
}

// Finds the node with the smallest ProductID in a non-empty subtree
AVLNode* minNode(AVLNode* n)
{
    return n->left ? minNode(n->left) : n;
}

// Recursively deletes a product by ID from the AVL tree and rebalances
AVLNode* avlDelete(AVLNode* n, const string& id, bool& deleted, bool deleteProductData = true)
{
    if (!n)
        return NULL;
    if (id < n->data->id)
    {
        n->left = avlDelete(n->left, id, deleted, deleteProductData);
    }
    else if (id > n->data->id)
    {
        n->right = avlDelete(n->right, id, deleted, deleteProductData);
    }
    else
    {
        deleted = true;
        if (!n->left || !n->right)
        {
            AVLNode* child = n->left ? n->left : n->right;
            if (deleteProductData)
                delete n->data;
            delete n;
            return child;
        }
        else
        {
            AVLNode* s = minNode(n->right);
            if (deleteProductData)
                delete n->data;
            n->data = s->data;
            bool dummy = false;
            n->right = avlDelete(n->right, s->data->id, dummy, false);
        }
    }
    return balance(n);
}

// Searches for a product by ID in O(log n) time
Product* avlFind(AVLNode* n, const string& id)
{
    if (!n)
        return NULL;
    if (id == n->data->id)
        return n->data;
    return id < n->data->id ? avlFind(n->left, id) : avlFind(n->right, id);
}

// Post-order traversal to free all nodes and Product objects in the AVL tree
void destroyAVL(AVLNode* n)
{
    if (!n)
        return;
    destroyAVL(n->left);
    destroyAVL(n->right);
    delete n->data;
    delete n;
}

// Prints formatted table rows for all products in the AVL tree (in-order traversal)
void printProducts(AVLNode* n)
{
    if (!n)
        return;
    printProducts(n->left);
    Product* p = n->data;
    cout << left << setw(8) << p->id
        << setw(22) << p->name
        << right << fixed << setprecision(1) << setw(8) << p->unitWeight << " kg   "
        << left << setw(10) << p->reorderLevel
        << setw(8) << p->totalStock()
        << setw(10) << p->reservedStock()
        << setw(10) << p->availableStock() << "  ";
    if (!p->racks)
        cout << "None";
    for (RackStock* r = p->racks; r; r = r->next)
    {
        cout << r->rackId << ":" << r->total;
        if (r->next)
            cout << " -> ";
    }
    cout << right << "\n";
    printProducts(n->right);
}

// Accumulates and prints stock totals across all racks for Module 2 Operations 8 & 9
void accumulateStockTotals(AVLNode* n, int& grandTotal, int& grandReserved, int& grandAvailable)
{
    if (!n)
        return;
    accumulateStockTotals(n->left, grandTotal, grandReserved, grandAvailable);
    Product* p = n->data;
    int t = p->totalStock(), r = p->reservedStock(), a = p->availableStock();
    grandTotal += t;
    grandReserved += r;
    grandAvailable += a;
    cout << left << setw(8) << p->id
        << setw(22) << p->name
        << setw(10) << t
        << setw(10) << r
        << setw(10) << a << right << "\n";
    accumulateStockTotals(n->right, grandTotal, grandReserved, grandAvailable);
}

// Prints products whose available stock is strictly below their reorder threshold
void lowStock(AVLNode* n, bool& anyFound)
{
    if (!n)
        return;
    lowStock(n->left, anyFound);
    int avail = n->data->availableStock();
    if (avail < n->data->reorderLevel)
    {
        anyFound = true;
        cout << left << setw(8) << n->data->id
            << setw(22) << n->data->name
            << setw(12) << avail
            << setw(14) << n->data->reorderLevel
            << setw(12) << (n->data->reorderLevel - avail) << right << "\n";
    }
    lowStock(n->right, anyFound);
}

// Returns true if any rack for product 'p' currently holds a reservation
bool productHasReservations(Product* p)
{
    for (RackStock* r = p->racks; r; r = r->next)
    {
        if (r->reserved > 0)
            return true;
    }
    return false;
}

// Recursively releases all reservations belonging to 'orderId' across the entire AVL tree
void releaseAllOrderReservations(AVLNode* n, const string& orderId)
{
    if (!n)
        return;
    releaseAllOrderReservations(n->left, orderId);
    for (RackStock* r = n->data->racks; r; r = r->next)
        r->releaseOrder(orderId);
    releaseAllOrderReservations(n->right, orderId);
}