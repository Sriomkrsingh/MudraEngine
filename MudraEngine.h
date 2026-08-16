#ifndef MUDRA_ENGINE_H
#define MUDRA_ENGINE_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <limits>

using namespace std;

// Represents a directed payment corridor with per-connection spread fee
struct CorridorEdge {
    string target;
    double mid_rate;
    double spread_percent; // Human-readable percentage (e.g., 0.3 for 0.3%)

    // Calculates the effective rate received on this individual leg
    double get_effective_rate() const {
        return mid_rate * (1.0 - (spread_percent / 100.0));
    }
};

// Result structure for routing query
struct RouteResult {
    bool success;
    vector<string> path;
    double effective_rate;
    double total_retained_ratio;
};

class MudraEngine {
private:
    unordered_map<string, vector<CorridorEdge>> adjList;
    unordered_map<string, string> historical_names;
    unordered_map<string, RouteResult> query_cache;

    string clean_string(string input);

public:
    void register_currency(string code, string history_name);
    void add_corridor(string from, string to, double mid_rate, double spread_percent);
    bool load_rates_from_csv(const string& filename);
    void update_corridor(string from, string to, double new_mid_rate, double new_spread_percent);

    RouteResult find_optimal_route(string start, string target);
    void execute_remittance(string start, string target, double amount);

    void display_available_currencies();
    void start_interactive_mode();
};

#endif