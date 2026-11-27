// ============================================================================
// SECTION 6 : MODULE 6 — TASK EXECUTION & CHARGING STATION MANAGEMENT
//             Data structures: ChargeWait, Charger (FIFO wait queues + slots)
// ============================================================================

// Node in a charging station's FIFO waiting queue
struct ChargeWait
{
    Robot* robot;
    ChargeWait* next;

    ChargeWait(Robot* r) : robot(r), next(NULL)
    {}
};

// Represents a charging station with fixed charging slots and a linked FIFO waiting queue
struct Charger
{
    string id, location;
    int slots;
    Robot** active;
    ChargeWait* front;
    ChargeWait* rear;
    Charger* next;

    Charger(string i = "", string l = "", int s = 1)
        : id(i), location(l), slots(s), front(NULL), rear(NULL), next(NULL)
    {
        active = new Robot * [s];
        for (int k = 0; k < s; k++)
            active[k] = NULL;
    }

    ~Charger()
    {
        delete[] active;
        while (front)
        {
            ChargeWait* p = front;
            front = front->next;
            delete p;
        }
    }

    // Returns the index of the first available charging slot, or -1 if all slots are full
    int freeSlot() const
    {
        for (int i = 0; i < slots; i++)
        {
            if (!active[i])
                return i;
        }
        return -1;
    }

    // Enqueues a robot at the rear of this station's FIFO waiting queue
    void enqueue(Robot* r)
    {
        ChargeWait* p = new ChargeWait(r);
        if (!rear)
        {
            front = rear = p;
        }
        else
        {
            rear->next = p;
            rear = p;
        }
    }

    // Dequeues and returns the front waiting robot when a slot becomes free
    Robot* dequeue()
    {
        if (!front)
            return NULL;
        ChargeWait* p = front;
        Robot* r = p->robot;
        front = front->next;
        if (!front)
            rear = NULL;
        delete p;
        return r;
    }
};