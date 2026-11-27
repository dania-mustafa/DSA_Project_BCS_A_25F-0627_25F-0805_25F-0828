// ============================================================================
// SECTION 0 : GLOBAL CONSTANTS, HELPERS, INPUT UTILITIES
// ============================================================================

// Global constants defined by Project Specification (Global Rule 3)
const double SAFETY_RESERVE = 10.0;
const double CHARGE_THRESHOLD = 20.0;
const int MAX_STOPS = 200;
const int MAX_PATH = 250;

// Validates that an identifier starts with 'prefix' followed exclusively by digits
bool validId(const string& s, const string& prefix)
{
    if (s.size() <= prefix.size() || s.substr(0, prefix.size()) != prefix)
        return false;
    for (size_t i = prefix.size(); i < s.size(); ++i)
    {
        if (s[i] < '0' || s[i] > '9')
            return false;
    }
    return true;
}

// Converts a string to uppercase for case-insensitive comparisons
string upperText(string s)
{
    for (size_t i = 0; i < s.size(); ++i)
    {
        if (s[i] >= 'a' && s[i] <= 'z')
            s[i] -= 32;
    }
    return s;
}

// Trims leading and trailing whitespace characters
string trimText(const string& s)
{
    size_t start = 0;
    while (start < s.size() && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r' || s[start] == '\n'))
        start++;
    size_t end = s.size();
    while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t' || s[end - 1] == '\r' || s[end - 1] == '\n'))
        end--;
    return s.substr(start, end - start);
}

// Standard battery consumption formula for one leg (Global Rule 3):
// consumption = distance * rate * (1 + 0.5 * carriedWeight / maxCapacity)
double calcLegBattery(int distance, double rate, double carriedWeight, double maxCapacity)
{
    if (distance <= 0 || maxCapacity <= 0.0)
        return 0.0;
    return distance * rate * (1.0 + 0.5 * (carriedWeight / maxCapacity));
}

// Validated integer input reader within [low, high]
int readInt(const string& prompt, int low, int high)
{
    string line;
    int x;
    while (true)
    {
        cout << prompt;
        if (!getline(cin, line))
            return low;
        stringstream ss(line);
        char extra;
        if ((ss >> x) && !(ss >> extra) && x >= low && x <= high)
            return x;
        cout << "  [Invalid Input] Enter an integer from " << low << " to " << high << ".\n";
    }
}

// Validated floating-point input reader within [low, high]
double readDouble(const string& prompt, double low, double high)
{
    string line;
    double x;
    while (true)
    {
        cout << prompt;
        if (!getline(cin, line))
            return low;
        stringstream ss(line);
        char extra;
        if ((ss >> x) && !(ss >> extra) && x >= low && x <= high)
            return x;
        cout << "  [Invalid Input] Enter a number from " << low << " to " << high << ".\n";
    }
}

// Trimmed line reader for string inputs
string readLine(const string& prompt)
{
    string s;
    cout << prompt;
    getline(cin, s);
    return trimText(s);
}
