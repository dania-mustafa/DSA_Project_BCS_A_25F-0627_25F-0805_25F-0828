// ============================================================================
// SECTION 2 : MODULE 1 — WAREHOUSE LAYOUT & PATHWAY MANAGEMENT
//             Data structures: Location, Edge, Vertex, Graph, RackMap
//             Algorithms: BFS reachability, Dijkstra shortest path
// ============================================================================

// Represents a warehouse location node (RACK, PACKING, or CHARGING)
struct Location
{
    string id, type, name;
};

// Directed edge node in a vertex's adjacency linked list
struct Edge
{
    int to, distance;
    bool blocked;
    Edge* next;

    Edge(int t, int d, bool b = false) : to(t), distance(d), blocked(b), next(NULL)
    {
    }
};

// Graph vertex containing location metadata and head of its adjacency list
struct Vertex
{
    Location data;
    Edge* edges;

    Vertex() : edges(NULL)
    {
    }

    ~Vertex()
    {
        while (edges)
        {
            Edge* p = edges;
            edges = edges->next;
            delete p;
        }
    }
};

// Custom undirected weighted graph implemented with an adjacency list
class Graph
{
    Vertex* v;
    int count, capacity;

    // Finds the array index of a location by its ID, or -1 if absent
    int indexOf(const string& id) const
    {
        for (int i = 0; i < count; i++)
        {
            if (v[i].data.id == id)
                return i;
        }
        return -1;
    }

    // Returns pointer to the edge from vertex index 'a' to vertex index 'b'
    Edge* findEdge(int a, int b) const
    {
        for (Edge* e = v[a].edges; e; e = e->next)
        {
            if (e->to == b)
                return e;
        }
        return NULL;
    }

public:
    Graph(int cap = 250) : count(0), capacity(cap)
    {
        v = new Vertex[capacity];
    }

    ~Graph()
    {
        delete[] v;
    }

    int size() const
    {
        return count;
    }

    // Adds a new location node with unique L## ID and valid location type
    bool addLocation(string id, string type, string name)
    {
        type = upperText(type);
        name = trimText(name);
        if (!validId(id, "L") || name.empty() || indexOf(id) >= 0 || count >= capacity ||
            (type != "RACK" && type != "PACKING" && type != "CHARGING"))
            return false;

        v[count].data.id = id;
        v[count].data.type = type;
        v[count].data.name = name;
        count++;
        return true;
    }

    bool hasLocation(string id) const
    {
        return indexOf(id) >= 0;
    }

    int getIndex(string id) const
    {
        return indexOf(id);
    }

    string idAt(int i) const
    {
        return (i >= 0 && i < count) ? v[i].data.id : "";
    }

    string typeAt(int i) const
    {
        return (i >= 0 && i < count) ? v[i].data.type : "";
    }

    string nameAt(int i) const
    {
        return (i >= 0 && i < count) ? v[i].data.name : "";
    }

    string getType(const string& id) const
    {
        int idx = indexOf(id);
        return idx >= 0 ? v[idx].data.type : "";
    }

    // Adds an undirected weighted pathway between two existing locations
    bool addEdge(string a, string b, int d)
    {
        int x = indexOf(a), y = indexOf(b);
        if (x < 0 || y < 0 || x == y || d <= 0 || findEdge(x, y))
            return false;

        Edge* e1 = new Edge(y, d, false);
        if (!v[x].edges)
            v[x].edges = e1;
        else
        {
            Edge* cur = v[x].edges;
            while (cur->next)
                cur = cur->next;
            cur->next = e1;
        }

        Edge* e2 = new Edge(x, d, false);
        if (!v[y].edges)
            v[y].edges = e2;
        else
        {
            Edge* cur = v[y].edges;
            while (cur->next)
                cur = cur->next;
            cur->next = e2;
        }
        return true;
    }

    // Checks whether an edge exists between 'a' and 'b' and outputs its blocked state
    bool getEdgeState(const string& a, const string& b, bool& isBlocked) const
    {
        int x = indexOf(a), y = indexOf(b);
        if (x < 0 || y < 0)
            return false;
        Edge* e = findEdge(x, y);
        if (!e)
            return false;
        isBlocked = e->blocked;
        return true;
    }

    // Returns the positive edge distance if an OPEN pathway exists between 'a' and 'b', else -1
    int getOpenEdgeDistance(const string& a, const string& b) const
    {
        int x = indexOf(a), y = indexOf(b);
        if (x < 0 || y < 0)
            return -1;
        Edge* e = findEdge(x, y);
        if (!e || e->blocked)
            return -1;
        return e->distance;
    }

    // Deletes a pathway from both adjacency lists, or updates its blocked status
    bool removeEdge(string a, string b, bool blockOnly = false, bool blocked = true)
    {
        int x = indexOf(a), y = indexOf(b);
        if (x < 0 || y < 0)
            return false;
        Edge* e = findEdge(x, y);
        if (!e)
            return false;

        if (blockOnly)
        {
            e->blocked = blocked;
            Edge* f = findEdge(y, x);
            if (f)
                f->blocked = blocked;
            return true;
        }

        Edge** p = &v[x].edges;
        while (*p && (*p)->to != y)
            p = &(*p)->next;
        if (*p)
        {
            Edge* t = *p;
            *p = t->next;
            delete t;
        }

        p = &v[y].edges;
        while (*p && (*p)->to != x)
            p = &(*p)->next;
        if (*p)
        {
            Edge* t = *p;
            *p = t->next;
            delete t;
        }
        return true;
    }

    // Breadth-First Search (BFS) reachability check ignoring BLOCKED pathways
    bool connected(string start, string end)
    {
        int s = indexOf(start), t = indexOf(end);
        if (s < 0 || t < 0)
            return false;

        bool* seen = new bool[count];
        int* q = new int[count];
        int f = 0, r = 0;

        for (int i = 0; i < count; i++)
            seen[i] = false;
        seen[s] = true;
        q[r++] = s;

        while (f < r)
        {
            int u = q[f++];
            if (u == t)
            {
                delete[] seen;
                delete[] q;
                return true;
            }
            for (Edge* e = v[u].edges; e; e = e->next)
            {
                if (!e->blocked && !seen[e->to])
                {
                    seen[e->to] = true;
                    q[r++] = e->to;
                }
            }
        }
        delete[] seen;
        delete[] q;
        return false;
    }

    // Dijkstra's shortest path algorithm ignoring BLOCKED pathways.
    // Tie-breaks: 1) Smaller distance, 2) Fewer locations, 3) Smaller Location ID.
    bool shortest(string start, string end, int path[], int& pathLen, int& distance)
    {
        int s = indexOf(start), t = indexOf(end);
        pathLen = 0;
        distance = 0;
        if (s < 0 || t < 0)
            return false;

        int* dist = new int[count];
        int* steps = new int[count];
        int* prev = new int[count];
        bool* used = new bool[count];

        for (int i = 0; i < count; i++)
        {
            dist[i] = INT_MAX;
            steps[i] = INT_MAX;
            prev[i] = -1;
            used[i] = false;
        }
        dist[s] = 0;
        steps[s] = 1;

        for (int k = 0; k < count; k++)
        {
            int u = -1;
            for (int i = 0; i < count; i++)
            {
                if (!used[i] && dist[i] != INT_MAX)
                {
                    if (u < 0 ||
                        dist[i] < dist[u] ||
                        (dist[i] == dist[u] && steps[i] < steps[u]) ||
                        (dist[i] == dist[u] && steps[i] == steps[u] && v[i].data.id < v[u].data.id))
                    {
                        u = i;
                    }
                }
            }
            if (u < 0 || dist[u] == INT_MAX)
                break;
            used[u] = true;

            for (Edge* e = v[u].edges; e; e = e->next)
            {
                if (!e->blocked)
                {
                    int nd = dist[u] + e->distance;
                    int ns = steps[u] + 1;
                    bool better = false;

                    if (nd < dist[e->to])
                        better = true;
                    else if (nd == dist[e->to] && ns < steps[e->to])
                        better = true;
                    else if (nd == dist[e->to] && ns == steps[e->to] && prev[e->to] >= 0 &&
                        v[u].data.id < v[prev[e->to]].data.id)
                        better = true;

                    if (better)
                    {
                        dist[e->to] = nd;
                        steps[e->to] = ns;
                        prev[e->to] = u;
                    }
                }
            }
        }

        bool ok = (dist[t] != INT_MAX);
        if (ok)
        {
            int rev[MAX_PATH], n = 0;
            for (int x = t; x != -1 && n < MAX_PATH; x = prev[x])
                rev[n++] = x;
            for (int i = n - 1; i >= 0; i--)
                path[pathLen++] = rev[i];
            distance = dist[t];
        }

        delete[] dist;
        delete[] steps;
        delete[] prev;
        delete[] used;
        return ok;
    }

    // Tabular display of all warehouse locations and their adjacency lists
    void display() const
    {
        if (count == 0)
        {
            cout << "No locations in warehouse layout.\n";
            return;
        }
        cout << left << setw(10) << "Location"
            << setw(11) << "Type"
            << setw(23) << "Name"
            << "Connected Pathways" << right << "\n";
        cout << string(78, '-') << "\n";
        for (int i = 0; i < count; i++)
        {
            cout << left << setw(10) << v[i].data.id
                << setw(11) << v[i].data.type
                << setw(23) << v[i].data.name;
            if (!v[i].edges)
                cout << "None";
            for (Edge* e = v[i].edges; e; e = e->next)
            {
                cout << "-> " << v[e->to].data.id << "(" << e->distance << ","
                    << (e->blocked ? "BLOCKED" : "OPEN") << ") ";
            }
            cout << right << "\n";
        }
        cout << string(78, '-') << "\n";
    }

    // Displays full details and connected pathways for a single location ID
    bool displayLocation(const string& id) const
    {
        int i = indexOf(id);
        if (i < 0)
            return false;
        cout << "Location ID : " << v[i].data.id << "\n"
            << "Type        : " << v[i].data.type << "\n"
            << "Name        : " << v[i].data.name << "\n"
            << "Pathways    : ";
        if (!v[i].edges)
            cout << "None";
        for (Edge* e = v[i].edges; e; e = e->next)
        {
            cout << v[e->to].data.id << " [dist=" << e->distance << ", "
                << (e->blocked ? "BLOCKED" : "OPEN") << "]  ";
        }
        cout << "\n";
        return true;
    }

    // Prints a route sequence of location IDs along with total distance
    void displayPath(int p[], int n, int d) const
    {
        cout << "Shortest Route: ";
        for (int i = 0; i < n; i++)
        {
            if (i)
                cout << " -> ";
            cout << idAt(p[i]);
        }
        cout << " | Total Distance: " << d << " units\n";
    }

    // Finds the nearest reachable location of a specified type (e.g., PACKING)
    int nearestType(string from, string type, string& found, int& distance)
    {
        int best = INT_MAX, bi = -1, path[MAX_PATH], len, dist;
        for (int i = 0; i < count; i++)
        {
            if (v[i].data.type == type && shortest(from, v[i].data.id, path, len, dist))
            {
                if (dist < best || (dist == best && (bi < 0 || v[i].data.id < v[bi].data.id)))
                {
                    best = dist;
                    bi = i;
                }
            }
        }
        if (bi < 0)
            return -1;
        found = v[bi].data.id;
        distance = best;
        return bi;
    }

    // Loads location nodes from an external text file
    bool loadLocations(const char* file)
    {
        ifstream in(file);
        if (!in)
            return false;
        string id, type, name;
        while (in >> id >> type)
        {
            getline(in, name);
            name = trimText(name);
            addLocation(id, type, name);
        }
        return true;
    }

    // Loads undirected pathways and their OPEN/BLOCKED states from an external file
    bool loadEdges(const char* file)
    {
        ifstream in(file);
        if (!in)
            return false;
        string a, b, status;
        int d;
        while (in >> a >> b >> d >> status)
        {
            if (addEdge(a, b, d) && upperText(status) == "BLOCKED")
                removeEdge(a, b, true, true);
        }
        return true;
    }
};

// Maps a storage rack ID (RK##) to its installed graph location ID (L##)
struct RackMap
{
    string id, location;
    RackMap* next;

    RackMap(string a = "", string b = "") : id(a), location(b), next(NULL)
    {
    }
};
