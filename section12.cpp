// ============================================================================
// SECTION 12 : WAREHOUSE CONTROLLER — DATASET LOADER (loadAll)
// ============================================================================

// Loads and validates all initial warehouse data from external .txt files
void Warehouse::loadAll()
{
    graph.loadLocations("locations.txt");
    graph.loadEdges("pathways.txt");

    ifstream in("racks.txt");
    string id, loc;
    while (in >> id >> loc)
        appendRackMap(id, loc);
    in.close();

    in.open("products.txt");
    string name;
    double w;
    int reorder;
    while (in >> id >> w >> reorder)
    {
        getline(in, name);
        name = trimText(name);
        if (validId(id, "P") && w > 0 && reorder >= 0)
        {
            Product* p = new Product(id, name, w, reorder);
            bool ok = false;
            products = avlInsert(products, p, ok);
            if (!ok)
                delete p;
        }
    }
    in.close();

    in.open("inventory.txt");
    string rack;
    int qty;
    while (in >> id >> rack >> qty)
    {
        Product* p = findProduct(id);
        if (p && findRack(rack) && qty >= 0 && !findStock(p, rack))
        {
            RackStock* r = new RackStock(rack, qty);
            if (!p->racks)
                p->racks = r;
            else
            {
                RackStock* cur = p->racks;
                while (cur->next)
                    cur = cur->next;
                cur->next = r;
            }
        }
    }
    in.close();

    in.open("robots.txt");
    string type;
    double bat, cap;
    while (in >> id >> type >> bat >> cap >> loc)
    {
        type = upperText(type);
        Robot* r = NULL;
        if (!validId(id, "RB") || findRobot(id) || bat < 0 || bat > 100 || !graph.hasLocation(loc))
            continue;
        if (type == "PICKER" && cap > 0 && cap <= 15.0)
            r = new Picker(id, bat, cap, loc);
        else if (type == "CARRIER" && cap >= 15.0 && cap <= 30.0)
            r = new Carrier(id, bat, cap, loc);
        else if ((type == "HEAVY" || type == "HEAVY-DUTY") && cap > 0 && cap <= 60.0)
            r = new HeavyDuty(id, bat, cap, loc);

        if (r)
        {
            if (r->battery < CHARGE_THRESHOLD)
                r->status = "Waiting for Charging";
            appendRobot(r);
        }
    }
    in.close();

    in.open("chargers.txt");
    int slots;
    while (in >> id >> loc >> slots)
        appendCharger(id, loc, slots);
    in.close();

    in.open("orders.txt");
    string cust, pid;
    int q;
    while (in >> id >> cust)
    {
        int count;
        if (!(in >> count))
            break;
        if (!validId(id, "O") || !validId(cust, "CU") || findOrder(id))
        {
            for (int k = 0; k < count; k++)
                in >> pid >> q;
            continue;
        }
        Order* o = new Order(id, cust);
        for (int k = 0; k < count; k++)
        {
            in >> pid >> q;
            addItem(o, pid, q);
        }
        appendOrder(o);
        pending.enqueue(o);
    }
    in.close();

    cout << "======================================================================\n"
        << "   INTELLIGENT WAREHOUSE ROBOT & AUTOMATED ORDER FULFILLMENT SYSTEM\n"
        << "======================================================================\n"
        << "[OK] Loaded locations.txt, pathways.txt, racks.txt, products.txt,\n"
        << "     inventory.txt, robots.txt, chargers.txt, and orders.txt.\n";
}

