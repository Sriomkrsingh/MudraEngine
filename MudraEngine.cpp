#include "MudraEngine.h"

string MudraEngine::clean_string(string input) {
    string cleaned = "";
    for (char c : input) {
        if (!isspace(c)) cleaned += toupper(c);
    }
    return cleaned;
}

void MudraEngine::register_currency(string code, string history_name) {
    code = clean_string(code);
    historical_names[code] = history_name;
}

void MudraEngine::add_corridor(string from, string to, double mid_rate, double spread_percent) {
    from = clean_string(from);
    to = clean_string(to);
    adjList[from].push_back({to, mid_rate, spread_percent});
}

bool MudraEngine::load_rates_from_csv(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "[ERROR] Could not open " << filename << ".\n";
        return false;
    }

    string line;
    getline(file, line); // Skip CSV header row

    int count = 0;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string from, to, mid_rate_str, spread_str;

        getline(ss, from, ',');
        getline(ss, to, ',');
        getline(ss, mid_rate_str, ',');
        getline(ss, spread_str, ',');

        if (!from.empty() && !to.empty() && !mid_rate_str.empty() && !spread_str.empty()) {
            add_corridor(from, to, stod(mid_rate_str), stod(spread_str));
            count++;
        }
    }
    cout << "[SYSTEM] Successfully loaded " << count << " liquidity corridors from " << filename << ".\n";
    file.close();
    return true;
}

void MudraEngine::update_corridor(string from, string to, double new_mid_rate, double new_spread_percent) {
    from = clean_string(from);
    to = clean_string(to);
    bool updated = false;

    for (auto& edge : adjList[from]) {
        if (edge.target == to) {
            edge.mid_rate = new_mid_rate;
            edge.spread_percent = new_spread_percent;
            updated = true;
            break;
        }
    }

    if (!updated) {
        add_corridor(from, to, new_mid_rate, new_spread_percent);
    }

    // Invalidate memoization cache to prevent executing stale routes
    query_cache.clear();
    cout << "[SYSTEM] Rate updated for " << from << " -> " << to << ". Memoization cache cleared.\n";
}

void MudraEngine::display_available_currencies() {
    cout << "\n========================================================\n";
    cout << "             REGISTERED GLOBAL CURRENCIES               \n";
    cout << "========================================================\n";
    for (const auto& pair : historical_names) {
        cout << " [" << left << setw(4) << pair.first << "] " << pair.second << "\n";
    }
    cout << "========================================================\n";
}

RouteResult MudraEngine::find_optimal_route(string start, string target) {
    start = clean_string(start);
    target = clean_string(target);

    string cache_key = start + "->" + target;
    if (query_cache.find(cache_key) != query_cache.end()) {
        return query_cache[cache_key];
    }

    if (adjList.find(start) == adjList.end() || historical_names.find(target) == historical_names.end()) {
        return {false, {}, -1.0, 0.0};
    }

    // Min-heap for Dijkstra: pair<cumulative_cost, pair<current_node, path_history>>
    priority_queue<
        pair<double, pair<string, vector<string>>>,
        vector<pair<double, pair<string, vector<string>>>>,
        greater<pair<double, pair<string, vector<string>>>>> min_heap;

    unordered_map<string, double> min_costs;
    for (const auto& node : historical_names) {
        min_costs[node.first] = numeric_limits<double>::infinity();
    }

    min_costs[start] = 0.0;
    min_heap.push({0.0, {start, {start}}});

    RouteResult result = {false, {}, -1.0, 0.0};
    int safety_counter = 0;

    while (!min_heap.empty() && safety_counter++ < 20000) {
        auto current = min_heap.top();
        min_heap.pop();

        double current_cost = current.first;
        string current_curr = current.second.first;
        vector<string> path = current.second.second;

        if (current_cost > min_costs[current_curr]) continue;

        if (current_curr == target) {
            result.success = true;
            result.path = path;
            result.effective_rate = exp(-current_cost);
            break;
        }

        if (path.size() > historical_names.size()) continue;

        for (const auto& edge : adjList[current_curr]) {
            string next_curr = edge.target;
            double effective_rate = edge.get_effective_rate();

            // Acyclic check: avoid revisiting nodes already in current path
            if (find(path.begin(), path.end(), next_curr) != path.end()) continue;

            // Logarithmic transformation: turns multiplicative friction into additive distance
            double edge_weight = -log(effective_rate);
            double new_cost = current_cost + edge_weight;

            if (new_cost < min_costs[next_curr]) {
                min_costs[next_curr] = new_cost;
                vector<string> new_path = path;
                new_path.push_back(next_curr);
                min_heap.push({new_cost, {next_curr, new_path}});
            }
        }
    }

    if (result.success) {
        query_cache[cache_key] = result;
    }

    return result;
}

void MudraEngine::execute_remittance(string start, string target, double amount) {
    RouteResult res = find_optimal_route(start, target);

    if (!res.success) {
        cout << "\n[FAILED] No liquidity pathway exists between " << start << " and " << target << ".\n";
        return;
    }

    cout << "\n========================================================\n";
    cout << "           OPTIMAL REMITTANCE SETTLEMENT                \n";
    cout << "========================================================\n";
    cout << "Transfer: " << fixed << setprecision(2) << amount << " " << start << " --> " << target << "\n\n";

    cout << "Routing Pathway:\n";
    for (size_t i = 0; i < res.path.size(); ++i) {
        string code = res.path[i];
        string name = historical_names.count(code) ? historical_names[code] : code;
        cout<< "[" << code << "] " << name;
        if (i + 1 < res.path.size()) cout << " ----> ";
    }

    double final_payout = amount * res.effective_rate;
    cout << "\n\nEffective Exchange Rate : " << fixed << setprecision(6) << res.effective_rate << "\n";
    cout << "Final Payout (Net Fees) : " << fixed << setprecision(2) << final_payout << " " << target << "\n";
    cout << "========================================================\n";
}

void MudraEngine::start_interactive_mode() {
    string from_curr, to_curr;
    double amount;

    while (true) {
        cout << "\n[Commands: 'LIST' for currencies | 'EXIT' to quit]\n";
        cout << "Source Currency Code : ";
        cin >> from_curr;

        if (clean_string(from_curr) == "EXIT") break;
        if (clean_string(from_curr) == "LIST") {
            display_available_currencies();
            continue;
        }

        cout << "Target Currency Code : ";
        cin >> to_curr;

        cout << "Transfer Amount      : ";
        if (!(cin >> amount) || amount <= 0) {
            cout << "[ERROR] Invalid amount entered. Defaulting to 1000.00.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            amount = 1000.0;
        }

        execute_remittance(from_curr, to_curr, amount);
    }
    cout << "\n[MUDRA] Settlement engine shutdown complete.\n";
}