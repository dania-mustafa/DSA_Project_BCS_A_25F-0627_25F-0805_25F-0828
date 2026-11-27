// ============================================================================
// SECTION 19 : WAREHOUSE CONTROLLER — MODULE 7 MENU (undoMenu)
// ============================================================================

// MODULE 7 MENU: Administrative Undo
void Warehouse::undoMenu()
{
    int c;
    do
    {
        cout << "\n--- MODULE 7: ADMIN UNDO ---\n"
            << "1. Show top of undo stack\n"
            << "2. Undo last operation\n"
            << "0. Back to main menu\n";
        c = readInt("Choice: ", 0, 2);

        if (c == 1)
        {
            if (history.empty()) cout << "Undo history empty.\n";
            else describeUndoNode(history.top());
        }
        else if (c == 2)
        {
            if (history.empty()) { cout << "Nothing to undo.\n"; continue; }
            Undo* u = history.top();

            if (u->type == 1)
            {
                Product* p = findProduct(u->a);
                RackStock* rs = findStock(p, u->b);
                if (rs) rs->total = u->oldQty;
                cout << "Restored total of " << u->a << " in " << u->b
                    << " to " << u->oldQty << ".\n";
            }
            else if (u->type == 2)
            {
                graph.removeEdge(u->a, u->b, true, u->oldBlocked);
                cout << "Restored pathway state " << u->a << "<->" << u->b << ".\n";
            }
            else if (u->type == 3)
            {
                Robot* r = findRobot(u->a);
                if (r) r->status = u->b;
                cout << "Restored robot " << u->a << " status to '" << u->b << "'.\n";
            }
            history.pop();
        }
    } while (c != 0);
}

