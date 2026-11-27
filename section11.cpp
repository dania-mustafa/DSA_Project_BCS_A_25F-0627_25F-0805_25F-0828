// ============================================================================
// SECTION 11 : WAREHOUSE CONTROLLER — MODULE 7 UNDO DISPLAY
// ============================================================================

// Prints a summary of a reversible administrative operation node on the Undo stack
void Warehouse::describeUndoNode(Undo* u) const
{
    if (!u)
        return;
    if (u->type == 1)
    {
        cout << "[Manual Quantity Update] Product: " << u->a << " | Rack: " << u->b
            << " | Old Qty: " << u->oldQty << " -> New Qty: " << u->newQty << "\n";
    }
    else if (u->type == 2)
    {
        cout << "[Block Pathway] Pathway: " << u->a << " <-> " << u->b
            << " | Previous Status: " << (u->oldBlocked ? "BLOCKED" : "OPEN") << "\n";
    }
    else if (u->type == 3)
    {
        cout << "[Robot Maintenance Change] Robot: " << u->a
            << " | Previous Status: " << u->b << "\n";
    }
}

