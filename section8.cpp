/ ============================================================================
// SECTION 8 : WAREHOUSE CONTROLLER — FIELDS & CROSS-MODULE LOOKUPS
// ============================================================================

class Warehouse
{
    Graph graph;            // Module 1
    AVLNode* products;      // Module 2
    RackMap* rackMaps;      // Module 1↔2 bridge
    Order* orders;          // Module 3
    Robot* robots;          // Module 4
    Charger* chargers;      // Module 6
    OrderQueue pending;     // Module 3
    UndoStack history;      // Module 7
    int path[MAX_PATH], pathLen, pathDistance;

    // Temporary structure used during Module 5 heuristic rack allocation
    struct PlannedAlloc
    {
        string rackId;
        string locationId;
        string productId;
        int quantity;
        double unitWeight;
        bool visited;
    };

    // ----- Cross-module lookups -----
    Product* findProduct(string id) const
    {
        return avlFind(products, id);
    }

    Order* findOrder(string id) const
    {
        for (Order* p = orders; p; p = p->next)
            if (p->id == id) return p;
        return NULL;
    }

    Robot* findRobot(string id) const
    {
        for (Robot* p = robots; p; p = p->next)
            if (p->id == id) return p;
        return NULL;
    }

    RackMap* findRack(string id) const
    {
        for (RackMap* p = rackMaps; p; p = p->next)
            if (p->id == id) return p;
        return NULL;
    }

    RackStock* findStock(Product* p, string rack) const
    {
        if (!p) return NULL;
        for (RackStock* r = p->racks; r; r = r->next)
            if (r->rackId == rack) return r;
        return NULL;
    }

    Charger* findCharger(string id) const
    {
        for (Charger* p = chargers; p; p = p->next)
            if (p->id == id) return p;
        return NULL;
    }

    // ----- Cross-module appenders -----
    void appendOrder(Order* o)
    {
        if (!orders) orders = o;
        else {
            Order* p = orders;
            while (p->next) p = p->next;
            p->next = o;
        }
    }

    void appendRobot(Robot* r)
    {
        if (!robots) robots = r;
        else {
            Robot* p = robots;
            while (p->next) p = p->next;
            p->next = r;
        }
    }

    // Validates and maps a unique rack (RK##) to a unique RACK location (Global Rule 2)
    bool appendRackMap(string id, string loc)
    {
        if (!validId(id, "RK") || findRack(id) || graph.getType(loc) != "RACK")
            return false;
        for (RackMap* p = rackMaps; p; p = p->next)
            if (p->location == loc) return false;
        RackMap* p = new RackMap(id, loc);
        if (!rackMaps) rackMaps = p;
        else {
            RackMap* cur = rackMaps;
            while (cur->next) cur = cur->next;
            cur->next = p;
        }
        return true;
    }

    // Validates and installs a unique charging station (C##) at a unique CHARGING location (Global Rule 2)
    bool appendCharger(string id, string loc, int slots)
    {
        if (!validId(id, "C") || findCharger(id) || slots <= 0 || graph.getType(loc) != "CHARGING")
            return false;
        for (Charger* p = chargers; p; p = p->next)
            if (p->location == loc) return false;
        Charger* c = new Charger(id, loc, slots);
        if (!chargers) chargers = c;
        else {
            Charger* p = chargers;
            while (p->next) p = p->next;
            p->next = c;
        }
        return true;
    }

    // Adds a product quantity to a Pending order (merging quantities if already present)
    bool addItem(Order* o, string pid, int qty)
    {
        if (!o || o->status != "Pending" || o->hasStartedAnyTrip() || !findProduct(pid) || qty <= 0)
            return false;
        for (Item* i = o->items; i; i = i->next)
        {
            if (i->productId == pid)
            {
                i->quantity += qty;
                return true;
            }
        }
        Item* p = new Item(pid, qty);
        if (!o->items) o->items = p;
        else {
            Item* i = o->items;
            while (i->next) i = i->next;
            i->next = p;
        }
        return true;
    }

    // Releases all active rack reservations across the AVL tree for order 'o'
    void releaseReservations(Order* o)
    {
        if (o) releaseAllOrderReservations(products, o->id);
    }

    // Displays formatted details and item breakdown for a single order
    void showOrder(Order* o) const
    {
        if (!o) return;
        cout << "Order ID : " << left << setw(8) << o->id
            << "| Customer: " << setw(8) << o->customer
            << "| Status: " << setw(20) << o->status
            << "| Robot: " << setw(6) << (o->assignedRobot.empty() ? "None" : o->assignedRobot)
            << "| Rem. Weight: " << right << fixed << setprecision(1) << o->totalWeight(products) << " kg\n";
        if (!o->items)
            cout << "  (No items added yet)\n";
        for (Item* i = o->items; i; i = i->next)
        {
            Product* p = findProduct(i->productId);
            cout << "  -> " << left << setw(6) << i->productId
                << " (" << setw(20) << (p ? p->name : "Unknown") << ") | "
                << "Ordered: " << right << setw(3) << i->quantity
                << " | Delivered: " << setw(3) << i->delivered
                << " | Remaining: " << setw(3) << (i->quantity - i->delivered) << "\n";
        }
    }

    // Prints a formatted table of all robots (or only Available robots)
    void printRobotTable(bool availableOnly) const
    {
        bool any = false;
        cout << left << setw(8) << "Robot"
            << setw(13) << "Type"
            << setw(11) << "Battery"
            << setw(11) << "Capacity"
            << setw(7) << "Rate"
            << setw(10) << "Location"
            << setw(22) << "Status"
            << "Task" << right << "\n";
        cout << string(90, '-') << "\n";
        for (Robot* r = robots; r; r = r->next)
        {
            if (!availableOnly || r->status == "Available")
            {
                any = true;
                ostringstream batSS, capSS;
                batSS << fixed << setprecision(1) << r->battery << "%";
                capSS << fixed << setprecision(1) << r->capacity << " kg";

                cout << left << setw(8) << r->id
                    << setw(13) << r->type
                    << setw(11) << batSS.str()
                    << setw(11) << capSS.str()
                    << fixed << setprecision(2) << setw(7) << r->rate
                    << setw(10) << r->location
                    << setw(22) << r->status
                    << (r->task.empty() ? "None" : r->task) << right << "\n";
            }
        }
        if (!any) cout << "No matching robots found.\n";
        cout << string(90, '-') << "\n";
    }

    // ----- Module 5 internal declarations -----
    bool checkOrderShortage(Order* o, string& shortageProduct, int& totalAvailableUnits) const;
    bool buildRobotPlan(Order* o, Robot* r, bool allowPartial,
        Trip*& outTrips, int& outTripCount,
        int& outTotalDist, double& outTotalBattery, string& failReason);

    // ----- Module 6 internal declarations -----
    void displayTripDetails(Trip* t) const;
    bool validatePathOpen(int p[], int len, string& blockedPair) const;
    bool recalculateTripRoutes(Trip* t, Robot* r, string& failMsg, const string& startLocation);
    void cancelOrderAssignment(Order* o, Robot* r);
    void executeOrderTask(Order* o);
    Charger* findWaitingCharger(const string& robotId) const;
    bool removeRobotFromWaitingQueue(const string& id);
    Charger* selectBestCharger(Robot* r, int& outDist, bool& outHasFreeSlot);

    // ----- Module 7 internal declaration -----
    void describeUndoNode(Undo* u) const;

public:
    Warehouse() : graph(250), products(NULL), rackMaps(NULL), orders(NULL), robots(NULL), chargers(NULL)
    {
    }

    // Destructor frees all dynamically allocated structures across the warehouse system
    ~Warehouse()
    {
        destroyAVL(products);
        while (rackMaps) { RackMap* p = rackMaps; rackMaps = rackMaps->next; delete p; }
        while (orders) { Order* p = orders;   orders = orders->next;   delete p; }
        while (robots) { Robot* p = robots;   robots = robots->next;   delete p; }
        while (chargers) { Charger* p = chargers; chargers = chargers->next; delete p; }
    }

    // ----- Section 12 -----
    void loadAll();

    // ----- Menu entry points -----
    void layoutMenu();      // Module 1
    void inventoryMenu();   // Module 2
    void orderMenu();       // Module 3
    void robotMenu();       // Module 4
    void assignmentMenu();  // Module 5
    void executionMenu();   // Module 6
    void undoMenu();        // Module 7
};

