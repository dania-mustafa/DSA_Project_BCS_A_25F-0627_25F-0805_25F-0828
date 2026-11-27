// ============================================================================
// SECTION 20 : MAIN ENTRY POINT
// ============================================================================

int main()
{
    Warehouse w;
    w.loadAll();

    int c;
    do
    {
        cout << "\n=========== MAIN MENU ===========\n"
            << "1. Warehouse Layout & Pathway Management\n"
            << "2. Product Inventory & Storage Rack Management\n"
            << "3. Customer Order Management\n"
            << "4. Robot Fleet Management\n"
            << "5. Order Assignment & Picking Plan\n"
            << "6. Task Execution & Charging Stations\n"
            << "7. Administrative Undo\n"
            << "0. Exit\n";
        c = readInt("Choice: ", 0, 7);

        switch (c)
        {
        case 1: w.layoutMenu();     break;
        case 2: w.inventoryMenu();  break;
        case 3: w.orderMenu();      break;
        case 4: w.robotMenu();      break;
        case 5: w.assignmentMenu(); break;
        case 6: w.executionMenu();  break;
        case 7: w.undoMenu();       break;
        }
    } while (c != 0);

    cout << "Exiting. Goodbye!\n";
    return 0;
}