// ============================================================================
// SECTION 5 : MODULE 5 — ORDER ASSIGNMENT & PICKING PLAN GENERATION
//             Data structures: PlanStop, Trip (heuristic planning artefacts)
// ============================================================================

// Represents one planned rack pickup stop inside a trip
struct PlanStop
{
    string rackId, locationId, productId;
    int quantity;
    double unitWeight;
    bool collected;
    int legPath[MAX_PATH];
    int legPathLen;
    int legDistance;
    double legBattery;
    PlanStop* next;

    PlanStop(string r = "", string loc = "", string pid = "", int q = 0, double w = 0.0)
        : rackId(r), locationId(loc), productId(pid), quantity(q), unitWeight(w),
        collected(false), legPathLen(0), legDistance(0), legBattery(0.0), next(NULL)
    {}
};

// Represents one complete trip (start -> rack stops -> packing station)
struct Trip
{
    int tripNumber;
    string startLocation;
    string packingLocation;
    PlanStop* stops;
    int toPackPath[MAX_PATH];
    int toPackPathLen;
    int toPackDistance;
    double toPackBattery;
    int totalDistance;
    double totalWeight;
    double totalBattery;
    bool executed;
    Trip* next;

    Trip(int num = 1, string start = "")
        : tripNumber(num), startLocation(start), packingLocation(""), stops(NULL),
        toPackPathLen(0), toPackDistance(0), toPackBattery(0.0),
        totalDistance(0), totalWeight(0.0), totalBattery(0.0), executed(false), next(NULL)
    {}

    ~Trip()
    {
        while (stops)
        {
            PlanStop* p = stops;
            stops = stops->next;
            delete p;
        }
    }
};

// --- Deferred Order methods (need full Trip definition from this section) ---

Order::~Order()
{
    while (items)
    {
        Item* p = items;
        items = items->next;
        delete p;
    }
    clearPlan();
}

void Order::clearPlan()
{
    while (trips)
    {
        Trip* t = trips;
        trips = trips->next;
        delete t;
    }
}

bool Order::hasStartedAnyTrip() const
{
    for (Item* i = items; i; i = i->next)
    {
        if (i->delivered > 0)
            return true;
    }
    for (Trip* t = trips; t; t = t->next)
    {
        if (t->executed)
            return true;
    }
    return false;
}