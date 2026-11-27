// ============================================================================
// SECTION 7 : MODULE 7 — ADMINISTRATIVE UNDO SYSTEM
//             Data structure: Undo, UndoStack (linked LIFO stack)
// ============================================================================

// Node in the reversible administrative operation stack
struct Undo
{
    int type; // 1 = Manual quantity update, 2 = Block pathway, 3 = Robot maintenance change
    string a, b;
    int oldQty, newQty;
    bool oldBlocked;
    Undo* next;
};

// Manual linked LIFO stack storing reversible administrative operations
class UndoStack
{
    Undo* topNode;

public:
    UndoStack() : topNode(NULL)
    {
    }

    ~UndoStack()
    {
        while (topNode)
        {
            Undo* p = topNode;
            topNode = topNode->next;
            delete p;
        }
    }

    // Pushes a reversible operation record onto the top of the stack
    void push(int t, string a, string b, int oldq = 0, int newq = 0, bool blocked = false)
    {
        Undo* p = new Undo();
        p->type = t;
        p->a = a;
        p->b = b;
        p->oldQty = oldq;
        p->newQty = newq;
        p->oldBlocked = blocked;
        p->next = topNode;
        topNode = p;
    }

    // Returns pointer to the most recent operation without popping it
    Undo* top() const
    {
        return topNode;
    }

    // Pops and deletes the top operation record from the stack
    void pop()
    {
        if (topNode)
        {
            Undo* p = topNode;
            topNode = topNode->next;
            delete p;
        }
    }

    bool empty() const
    {
        return topNode == NULL;
    }
};
