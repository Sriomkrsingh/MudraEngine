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

class MudraEngine {
private:
    // currency code -> list of {target currency, effective rate after spread}
    unordered_map<string, vector<pair<string, double>>> adjList;
    // currency code -> country/region name
    unordered_map<string, string> names;
    // "FROM->TO" -> {path, effective_rate}  (empty path = no route)
    unordered_map<string, pair<vector<string>, double>> cache;

    string clean_string(string input) {
        string cleaned = "";
        for (char c : input) {
            if (!isspace(c)) cleaned += toupper(c);
        }
        return cleaned;
    }

public:
    void register_currency(string code, string country) {
        names[clean_string(code)] = country;
    }

    void add_corridor(string from, string to, double mid_rate, double spread_percent) {
        from = clean_string(from);
        to = clean_string(to);
        double effective_rate = mid_rate * (1.0 - (spread_percent / 100.0));
        adjList[from].push_back({to, effective_rate});
    }

    bool load_rates_from_csv(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "[ERROR] Could not open " << filename << ".\n";
            return false;
        }

        string line;
        getline(file, line); 

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

    void display_available_currencies() {
        cout << "\n========================================================\n";
        cout << "             REGISTERED GLOBAL CURRENCIES               \n";
        cout << "========================================================\n";
        for (const auto& n : names) {
            cout << " [" << left << setw(4) << n.first << "] " << n.second << "\n";
        }
        cout << "========================================================\n";
    }

    // Returns {path, effective_rate}. Empty path means no route found.
    pair<vector<string>, double> optimal_route(string start, string target) {
        start = clean_string(start);
        target = clean_string(target);

        string cache_key = start + "->" + target;
        if (cache.count(cache_key)) return cache[cache_key];

        if (!adjList.count(start) || !names.count(target)) {
            return {{}, -1.0};
        }

        
        priority_queue<
            pair<double, pair<string, vector<string>>>,
            vector<pair<double, pair<string, vector<string>>>>,
            greater<pair<double, pair<string, vector<string>>>>> min_heap;

        unordered_map<string, double> min_costs;
        for (const auto& n : names) min_costs[n.first] = numeric_limits<double>::infinity();
        min_costs[start] = 0.0;
        min_heap.push({0.0, {start, {start}}});

        pair<vector<string>, double> result = {{}, -1.0};
        int safety_counter = 0;

        while (!min_heap.empty() && safety_counter++ < 20000) {
            auto current = min_heap.top();
            min_heap.pop();

            double current_cost = current.first;
            string current_curr = current.second.first;
            vector<string> path = current.second.second;

            if (current_cost > min_costs[current_curr]) continue;

            if (current_curr == target) {
                result = {path, exp(-current_cost)};
                break;
            }

            if (path.size() > names.size()) continue;

            for (const auto& edge : adjList[current_curr]) {
                string next_curr = edge.first;
                double effective_rate = edge.second;

                if (find(path.begin(), path.end(), next_curr) != path.end()) continue;

                double new_cost = current_cost - log(effective_rate);

                if (new_cost < min_costs[next_curr]) {
                    min_costs[next_curr] = new_cost;
                    vector<string> new_path = path;
                    new_path.push_back(next_curr);
                    min_heap.push({new_cost, {next_curr, new_path}});
                }
            }
        }

        if (!result.first.empty()) cache[cache_key] = result;
        return result;
    }

    void execute_remittance(string start, string target, double amount) {
        auto res = optimal_route(start, target);
        vector<string>& path = res.first;
        double rate = res.second;

        if (path.empty()) {
            cout << "\n[FAILED] No liquidity pathway exists between " << start << " and " << target << ".\n";
            return;
        }

        cout << "\n========================================================\n";
        cout << "           OPTIMAL REMITTANCE SETTLEMENT                \n";
        cout << "========================================================\n";
        cout << "Transfer: " << fixed << setprecision(2) << amount << " " << start << " --> " << target << "\n\n";

        cout << "Optimal Routing Pathway:\n";
        for (size_t i = 0; i < path.size(); ++i) {
            string code = path[i];
            string name = names.count(code) ? names[code] : code;
            cout << "[" << code << "] " << name;
            if (i + 1 < path.size()) cout << " ----> ";
        }

        double final_payout = amount * rate;
        cout << "\n\nEffective Exchange Rate : " << fixed << setprecision(6) << rate << "\n";
        cout << "Final Payout (Net Fees) : " << fixed << setprecision(2) << final_payout << " " << target << "\n";
        cout << "========================================================\n";
    }

    void start_interactive_mode() {
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
};
