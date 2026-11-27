/*
============================================================================
   MODULE 3 — CUSTOMER ORDER MANAGEMENT
   Data structures: Item, Order, OrderQ, OrderQueue
============================================================================
*/

// Node in an order's linked list of requested product items
struct Item
{
    string productId;
    int quantity, delivered;
    Item* next;

    Item(string p = "", int q = 0) : productId(p), quantity(q), delivered(0), next(NULL)
    {}
};

// Forward declaration: full definition lives in SECTION 5 (Module 5)
struct Trip;

// Represents a customer order record in the main order linked list
struct Order
{
    string id, customer, status;
    Item* items;
    Order* next;
    string assignedRobot;
    Trip* trips;

    Order(string i = "", string c = "")
        : id(i), customer(c), status("Pending"), items(NULL), next(NULL),
        assignedRobot(""), trips(NULL)
    {}

    ~Order();

    // Frees all planned Trip nodes associated with this order
    void clearPlan();

    // Returns true if at least one trip or delivery has already been recorded
    bool hasStartedAnyTrip() const;

    // Computes total remaining undelivered weight of the order
    double totalWeight(AVLNode* root) const
    {
        double w = 0.0;
        for (Item* i = items; i; i = i->next)
        {
            Product* p = avlFind(root, i->productId);
            if (p && i->quantity > i->delivered)
                w += (i->quantity - i->delivered) * p->unitWeight;
        }
        return w;
    }

    // Returns the maximum single-unit weight among remaining items in the order
    double maxUnitWeight(AVLNode* root) const
    {
        double heaviest = 0.0;
        for (Item* i = items; i; i = i->next)
        {
            if (i->quantity > i->delivered)
            {
                Product* p = avlFind(root, i->productId);
                if (p && p->unitWeight > heaviest)
                    heaviest = p->unitWeight;
            }
        }
        return heaviest;
    }
};

// Node in the FIFO pending order queue
struct OrderQ
{
    Order* order;
    OrderQ* next;

    OrderQ(Order* o) : order(o), next(NULL)
    {}
};

// Manual linked FIFO queue managing Pending customer orders
class OrderQueue
{
    OrderQ* front;
    OrderQ* rear;
    int n;

public:
    OrderQueue() : front(NULL), rear(NULL), n(0)
    {}

    ~OrderQueue()
    {
        while (front)
        {
            OrderQ* p = front;
            front = front->next;
            delete p;
        }
    }

    // Enqueues a Pending order at the rear (or at the front when returning a cancelled assignment)
    void enqueue(Order* o, bool atFront = false)
    {
        if (!o || contains(o->id))
            return;
        OrderQ* p = new OrderQ(o);
        if (!front)
        {
            front = rear = p;
        }
        else if (atFront)
        {
            p->next = front;
            front = p;
        }
        else
        {
            rear->next = p;
            rear = p;
        }
        n++;
    }

    // Checks if an order ID is already present in the queue
    bool contains(const string& id) const
    {
        for (OrderQ* p = front; p; p = p->next)
        {
            if (p->order->id == id)
                return true;
        }
        return false;
    }

    // Returns the oldest pending order at the front without removing it
    Order* peek() const
    {
        return front ? front->order : NULL;
    }

    // Returns head pointer for non-destructive traversal in Module 5
    OrderQ* getFrontNode() const
    {
        return front;
    }

    // Removes and returns the front order from the queue
    Order* dequeue()
    {
        if (!front)
            return NULL;
        OrderQ* p = front;
        Order* o = p->order;
        front = front->next;
        if (!front)
            rear = NULL;
        delete p;
        n--;
        return o;
    }

    // Removes an order by ID from anywhere in the queue using an O(n) linear scan
    bool remove(string id)
    {
        OrderQ** p = &front;
        while (*p)
        {
            if ((*p)->order->id == id)
            {
                OrderQ* t = *p;
                *p = t->next;
                if (t == rear)
                    rear = NULL;
                delete t;
                n--;
                if (!front)
                {
                    rear = NULL;
                }
                else if (!rear)
                {
                    rear = front;
                    while (rear->next)
                        rear = rear->next;
                }
                return true;
            }
            p = &(*p)->next;
        }
        return false;
    }

    int size() const
    {
        return n;
    }

    // Displays both the processing chain (O101 -> O102 -> ...) and a formatted table
    void display() const
    {
        if (!front)
        {
            cout << "Pending order queue is empty.\n";
            return;
        }
        cout << "Processing Order: Front -> ";
        for (OrderQ* p = front; p; p = p->next)
            cout << p->order->id << " -> ";
        cout << "Rear\n\n";

        cout << left << setw(6) << "Pos"
            << setw(11) << "Order ID"
            << setw(11) << "Customer"
            << setw(18) << "Status"
            << "Products" << right << "\n";
        cout << string(72, '-') << "\n";
        int pos = 1;
        for (OrderQ* p = front; p; p = p->next, pos++)
        {
            cout << left << setw(6) << pos
                << setw(11) << p->order->id
                << setw(11) << p->order->customer
                << setw(18) << p->order->status;
            for (Item* it = p->order->items; it; it = it->next)
            {
                cout << it->productId << " x " << (it->quantity - it->delivered);
                if (it->next)
                    cout << ", ";
            }
            cout << right << "\n";
        }
        cout << string(72, '-') << "\n";
    }
};

// ============================================================================
// SECTION 15 : WAREHOUSE CONTROLLER — MODULE 3 MENU (orderMenu)
// ============================================================================

// MODULE 3 MENU: Customer Order Management
void Warehouse::orderMenu()
{
    int c;
    do
    {
        cout << "\n--- MODULE 3: CUSTOMER ORDER MANAGEMENT ---\n"
            << "1. Add a new customer order\n"
            << "2. Add product(s) to a Pending order\n"
            << "3. Search for an order by ID\n"
            << "4. Display pending order queue (Front to Rear)\n"
            << "5. View front order of queue (Peek) and queue size\n"
            << "6. Cancel an order\n"
            << "7. Display orders (Pending / Completed / All)\n"
            << "0. Back to main menu\n";
        c = readInt("Choice: ", 0, 7);

        string id, cust, pid;
        int q;

        if (c == 1)
        {
            id = readLine("Order ID (O##): ");
            cust = readLine("Customer ID (CU##): ");
            if (!validId(id, "O") || !validId(cust, "CU"))
            {
                cout << "Invalid ID format. Order must be O## and Customer must be CU##.\n";
            }
            else if (findOrder(id))
            {
                cout << "Duplicate Order ID rejected.\n";
            }
            else
            {
                Order* o = new Order(id, cust);
                int count = readInt("How many products to add now? (0 to add later): ", 0, 100);
                for (int k = 0; k < count; k++)
                {
                    pid = readLine("  Product ID (P##): ");
                    q = readInt("  Quantity: ", 1, 1000000);
                    if (addItem(o, pid, q))
                        cout << "  -> Added/merged " << q << " units of " << pid << ".\n";
                    else
                        cout << "  -> Product " << pid << " not found in AVL tree; skipped.\n";
                }
                appendOrder(o);
                pending.enqueue(o);
                cout << "Order " << id << " created and enqueued at rear of pending queue.\n";
            }
        }
        else if (c == 2)
        {
            id = readLine("Order ID (O##): ");
            Order* o = findOrder(id);
            if (!o)
                cout << "Order not found.\n";
            else if (o->status != "Pending" || o->hasStartedAnyTrip())
                cout << "Items can only be added to a Pending order that has not started any delivery.\n";
            else
            {
                int count = readInt("How many product entries to add? ", 1, 100);
                for (int k = 0; k < count; k++)
                {
                    pid = readLine("  Product ID (P##): ");
                    q = readInt("  Quantity: ", 1, 1000000);
                    if (addItem(o, pid, q))
                        cout << "  -> Item " << pid << " (qty " << q << ") added/merged.\n";
                    else
                        cout << "  -> Could not add item (Product " << pid << " does not exist).\n";
                }
            }
        }
        else if (c == 3)
        {
            id = readLine("Order ID: ");
            Order* o = findOrder(id);
            if (!o)
                cout << "Order not found.\n";
            else
                showOrder(o);
        }
        else if (c == 4)
        {
            cout << "\n--- PENDING ORDERS QUEUE (Front to Rear) ---\n";
            pending.display();
        }
        else if (c == 5)
        {
            Order* o = pending.peek();
            if (o)
                cout << "Front of Queue (Peek): " << o->id << " (Customer: " << o->customer << ")\n";
            else
                cout << "Pending queue is empty.\n";
            cout << "Total Queue Size     : " << pending.size() << "\n";
        }
        else if (c == 6)
        {
            id = readLine("Order ID to cancel: ");
            Order* o = findOrder(id);
            if (!o)
            {
                cout << "Order not found.\n";
            }
            else if (o->status == "Partially Fulfilled" || o->status == "Completed" || o->status == "Cancelled")
            {
                cout << "Cannot cancel order with status '" << o->status << "'.\n";
            }
            else if (o->hasStartedAnyTrip())
            {
                cout << "Cannot cancel order: at least one delivery trip has already been executed.\n";
            }
            else
            {
                pending.remove(o->id);
                releaseReservations(o);

                if (!o->assignedRobot.empty())
                {
                    Robot* r = findRobot(o->assignedRobot);
                    if (r)
                    {
                        r->task = "";
                        r->status = (r->battery < CHARGE_THRESHOLD) ? "Waiting for Charging" : "Available";
                    }
                    o->assignedRobot = "";
                }

                o->clearPlan();
                o->status = "Cancelled";
                cout << "Order " << o->id << " cancelled. Reserved stock and assigned robot (if any) released.\n";
            }
        }
        else if (c == 7)
        {
            int filter = readInt("Filter (1 = Pending, 2 = Completed, 3 = All orders): ", 1, 3);
            bool any = false;
            cout << string(78, '-') << "\n";
            for (Order* o = orders; o; o = o->next)
            {
                if (filter == 3 ||
                    (filter == 1 && o->status == "Pending") ||
                    (filter == 2 && o->status == "Completed"))
                {
                    showOrder(o);
                    cout << string(78, '-') << "\n";
                    any = true;
                }
            }
            if (!any)
                cout << "No matching orders found.\n";
        }
    } while (c != 0);
}