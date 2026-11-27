// ============================================================================
// SECTION 13 : WAREHOUSE CONTROLLER — MODULE 1 MENU (layoutMenu)
// ============================================================================

// MODULE 1 MENU: Warehouse Layout and Pathway Management
void Warehouse::layoutMenu()
{
    int c;
    do
    {
        cout << "\n--- MODULE 1: WAREHOUSE LAYOUT & PATHWAY MANAGEMENT ---\n"
            << "1. Display all locations and pathways\n"
            << "2. Add a new location\n"
            << "3. Add a pathway between two locations\n"
            << "4. Remove a pathway\n"
            << "5. Block a pathway\n"
            << "6. Reopen a blocked pathway\n"
            << "7. Search for a location by ID\n"
            << "8. Check connectivity between two locations (BFS)\n"
            << "9. Find shortest route between two locations (Dijkstra)\n"
            << "0. Back to main menu\n";
        c = readInt("Choice: ", 0, 9);

        string a, b, t, n;
        int d;
        if (c == 1)
        {
            graph.display();
        }
        else if (c == 2)
        {
            a = readLine("Location ID (L##): ");
            t = upperText(readLine("Type (RACK / PACKING / CHARGING): "));
            n = readLine("Location Name: ");

            if (!validId(a, "L"))
            {
                cout << "Invalid ID format. Must be L followed by digits (e.g., L16).\n";
            }
            else if (t == "RACK")
            {
                string newRackId = readLine("Rack ID (RK##): ");
                bool locationAlreadyMapped = false;
                for (RackMap* p = rackMaps; p; p = p->next)
                {
                    if (p->location == a)
                    {
                        locationAlreadyMapped = true;
                        break;
                    }
                }

                if (!validId(newRackId, "RK"))
                    cout << "Invalid Rack ID format. Must be RK followed by digits.\n";
                else if (findRack(newRackId))
                    cout << "This Rack ID already exists.\n";
                else if (locationAlreadyMapped)
                    cout << "This location is already mapped to another rack.\n";
                else if (graph.addLocation(a, t, n))
                {
                    if (appendRackMap(newRackId, a))
                        cout << "Rack location " << a
                        << " added and mapped to Rack " << newRackId << ".\n";
                }
                else
                {
                    cout << "Could not add location (duplicate ID or empty name).\n";
                }
            }
            else if (graph.addLocation(a, t, n))
            {
                cout << "Location " << a << " added successfully.\n";
            }
            else
            {
                cout << "Could not add location (duplicate ID, invalid type, or empty name).\n";
            }
        }
        else if (c == 3)
        {
            a = readLine("First location ID: ");
            b = readLine("Second location ID: ");
            d = readInt("Positive distance: ", 1, 1000000);
            if (!graph.hasLocation(a) || !graph.hasLocation(b))
                cout << "Error: One or both locations do not exist.\n";
            else if (graph.addEdge(a, b, d))
                cout << "Pathway added between " << a << " and " << b << " (distance " << d << ").\n";
            else
                cout << "Could not add pathway (self-loop or pathway already exists).\n";
        }
        else if (c == 4)
        {
            a = readLine("First location ID: ");
            b = readLine("Second location ID: ");
            if (graph.removeEdge(a, b, false))
                cout << "Pathway between " << a << " and " << b << " removed.\n";
            else
                cout << "Pathway not found.\n";
        }
        else if (c == 5)
        {
            a = readLine("First location ID: ");
            b = readLine("Second location ID: ");
            bool wasBlocked = false;
            if (!graph.getEdgeState(a, b, wasBlocked))
                cout << "Pathway not found.\n";
            else if (wasBlocked)
                cout << "Pathway between " << a << " and " << b << " is already BLOCKED.\n";
            else
            {
                graph.removeEdge(a, b, true, true);
                history.push(2, a, b, 0, 0, wasBlocked);
                cout << "Pathway between " << a << " and " << b << " is now BLOCKED (recorded in Undo stack).\n";
            }
        }
        else if (c == 6)
        {
            a = readLine("First location ID: ");
            b = readLine("Second location ID: ");
            bool wasBlocked = false;
            if (!graph.getEdgeState(a, b, wasBlocked))
                cout << "Pathway not found.\n";
            else if (!wasBlocked)
                cout << "Pathway between " << a << " and " << b << " is already OPEN.\n";
            else
            {
                graph.removeEdge(a, b, true, false);
                cout << "Pathway between " << a << " and " << b << " reopened (OPEN).\n";
            }
        }
        else if (c == 7)
        {
            a = readLine("Location ID: ");
            if (!graph.displayLocation(a))
                cout << "Location " << a << " not found.\n";
        }
        else if (c == 8)
        {
            a = readLine("Start location ID: ");
            b = readLine("End location ID: ");
            if (!graph.hasLocation(a) || !graph.hasLocation(b))
                cout << "Error: Start or end location does not exist.\n";
            else
                cout << (graph.connected(a, b)
                    ? "Result (BFS): Locations " + a + " and " + b + " are CONNECTED via open pathways.\n"
                    : "Result (BFS): No open route connects " + a + " and " + b + ".\n");
        }
        else if (c == 9)
        {
            a = readLine("Start location ID: ");
            b = readLine("End location ID: ");
            if (!graph.hasLocation(a) || !graph.hasLocation(b))
                cout << "Error: Start or end location does not exist.\n";
            else if (graph.shortest(a, b, path, pathLen, pathDistance))
                graph.displayPath(path, pathLen, pathDistance);
            else
                cout << "No open route exists between " << a << " and " << b << ".\n";
        }
    } while (c != 0);
}

