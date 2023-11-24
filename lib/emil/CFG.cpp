#include "CFG.h"

CFG::CFG(const string &filename) {

    ifstream input(filename);
    json j;
    input >> j;

    auto temp = j["Variables"];

    for (string symbol: temp) {
        _nonterminals.push_back(symbol);
    }

    temp = j["Terminals"];
    for (string symbol: temp) {
        _terminals.push_back(symbol);
    }

    _startSymbol = j["Start"].get<string>();
    pair<string, vector<string>> new_rule;
    for (auto rule: j["Productions"]) {
        new_rule.first = rule["head"].get<string>();
        for (const auto &symbol: rule["body"]) {
            new_rule.second.push_back(symbol);
        }
        _productions.insert(_productions.end(), new_rule);
        new_rule.second.clear();
    }
    sort(_nonterminals.begin(), _nonterminals.end());
    sort(_terminals.begin(), _terminals.end());
}

void CFG::print() {
    std::sort(_nonterminals.begin(), _nonterminals.end());
    string nonterminal_string = "V = {";
    for (int i = 0; i < _nonterminals.size(); i++) {
        nonterminal_string += _nonterminals[i];
        if (i != _nonterminals.size() - 1) {
            nonterminal_string += ", ";
        } else {
            nonterminal_string += "}";
        }
    }
    std::sort(_terminals.begin(), _terminals.end());
    string terminal_string = "T = {";
    for (int i = 0; i < _terminals.size(); i++) {
        terminal_string += _terminals[i];
        if (i != _terminals.size() - 1) {
            terminal_string += ", ";
        } else {
            terminal_string += "}";
        }
    }

    cout << nonterminal_string << endl;
    cout << terminal_string << endl;

    set<string> rules_str;
    string production_string;
    for (auto rule: _productions) {
        production_string += "  " + rule.first + " -> `";
        for (int i = 0; i < rule.second.size(); i++) {
            production_string += rule.second[i];
            if (i != rule.second.size() - 1) {
                production_string += " ";
            }
        }
        production_string += "`";
        rules_str.emplace(production_string);
        production_string.clear();
    }
    cout << "P = {\n";
    for (const string& rule : rules_str){
        cout << rule << "\n";
    }

    cout << "}" << endl;

    string start_symbol_string = "S = " + _startSymbol;

    cout << start_symbol_string << endl;
}

bool CFG::accepts(const string &input) const {
    vector<vector<set<string>>> table(input.size());
    vector<set<string>> row0(input.size());
    set<string> temp_element;
    // we construct the first row seperately because here we need to read terminals and not variable pairs
    for (int i = 0; i < input.size(); i++) {
        temp_element = this->findruleforterminal(input[i]);
        row0[i] = temp_element;
    }
    table[0] = (row0);

    // this part is going to be ugly
    for (int row_nr = 1; row_nr < input.size(); row_nr++) {// -1 because we already made the first row_nr
        vector<set<string>> newrow(input.size() - row_nr);
        for (int col_nr = 0; col_nr < newrow.size(); col_nr++) {
            int total_substr_length = row_nr + 1;
            set<string> v1;
            set<string> v2;
            set<string> newtableelement;
            for (int i = 1; i < total_substr_length; i++) {
                int remainder = total_substr_length - i;
                v1 = table[i - 1][col_nr];
                v2 = table[remainder - 1][col_nr + i];
                set<pair<string, string>> temp = cartesianproduct(v1, v2);
                for (const pair<string, string> &t: temp) {
                    set<string> newvars = findruleforvars(t);
                    newtableelement.insert(newvars.begin(), newvars.end());
                }
            }
            newrow[col_nr] = newtableelement;
        }
        table[row_nr] = newrow;
    }
    printTable(table);
    if (table[input.size() - 1][0].find(_startSymbol) != table[input.size() - 1][0].end()) {
        cout << "true" << endl;
        return true;
    }
    cout << "false" << endl;
    return false;
}

string encaseincurly(const set<string> &input) {
    /**
     * creates a string that lists the elements of the set between curly brackets
     * f.e. {A1,B,A2}
     */
    if (input.empty()) {
        return "{}";
    }

    string result = "{";
    for (const string &s: input) {
        result += s + ", ";
    }
    result = result.substr(0, result.size() - 2);
    result += "}";
    return result;
}

set<string> CFG::findruleforterminal(const char &terminal) const {
    /**
     * this function assumes the CFG is in CNF
     * finds the rule heads for a given terminals
     * --> collects all non-terminals that can produce the given terminal
     */
    string input = string(1, terminal);
    set<string> result;
    for (auto rule: _productions) {
        if (rule.second.size() != 1) { continue; }
        if (*rule.second.begin() == input) {
            result.emplace(rule.first);
        }
    }
    return result;
}

set<string> CFG::findruleforvars(const pair<string, string> &vars) const {
    /**
     * this function assumes the CFG is in CNF
     * finds all rule heads that produce the given non-terminal pair
     */
    set<string> result;
    for (auto rule: _productions) {

        if (rule.second.size() == 1) { continue; }     // we don't need _productions that produce a terminal
        if (rule.second[0] != vars.first) { continue; }// if the first var is wrong it is not a rule we want

        if (rule.second[1] == vars.second) {
            result.emplace(rule.first);
        }
    }
    return result;
}

set<pair<string, string>> cartesianproduct(const set<string> &v1, const set<string> &v2) {
    set<pair<string, string>> result;
    if (v1.empty() || v2.empty()) {
        return result;
    }

    for (const auto &i: v1) {
        for (const auto &j: v2) {
            pair<string, string> new_pair = make_pair(i, j);
            result.emplace(new_pair);
        }
    }
    return result;
}

void printTable(vector<vector<set<string>>> &table) {
    string output;

    int amount_of_cols = table[0].size();
    vector<int> longest_per_column(amount_of_cols);
    int col_counter = 0;
    for (const auto &row: table) {
        col_counter = 0;
        for (const auto &element: row) {
            int set_length = 0;
            for (const auto &symbol: element) {
                set_length += symbol.size();
            }
            set_length += 2 + 2 * set_length;
            if (set_length > longest_per_column[col_counter]) {
                longest_per_column[col_counter] = set_length;
            }
            col_counter++;
        }
    }

    for (auto row = table.end() - 1; row >= table.begin(); row--) {
        string rowstring = "|";
        col_counter = 0;
        for (auto &element: *row) {
            string temp = encaseincurly(element);
            string extension;
            if (temp.size() <= longest_per_column[col_counter]) {
                extension = string(longest_per_column[col_counter] - temp.size(), ' ');
            }
            rowstring += " " + temp.append(extension) + "|";
            col_counter++;
        }
        output += rowstring + "\n";
    }
    cout << output;
}

void CFG::toCNF() {
    /**
     * steps:
     * 1) eliminate epsilon productions
     * 2) eliminate unit pairs
     * 3) eliminate useless symbols
     * 4) replace terminals in bad bodies  (sentential forms, bv. A --> aBa)
     * 5) breaking _productions with more than 2 vars in body
     */
    cout << "Original CFG: \n" << endl;
    print();
    cout << "\n-------------------------------------" << endl;
    cout << "\n >> Eliminating epsilon productions" << endl;
    set<string> nullables = findnullables();
    int prev_productions_size = _productions.size();
    removeepsilons(nullables);
    cout << "  Nullables are " << encaseincurly(nullables) << endl;
    printf("  Created %i productions, original had %i\n\n", _productions.size(), prev_productions_size);
    print();

    int unitproduction_count = 0;
    map<string, set<string>> unitpairs = findunitpairs(unitproduction_count);
    prev_productions_size = _productions.size();
    cout << "\n >> Eliminating unit pairs" << endl;
    removeunitpairs(unitpairs);
    printf("  Found %i unit productions\n", unitproduction_count);
    cout << "  Unit pairs: " << convertunitpairstostring(unitpairs) << endl;
    printf("  Created %i new productions, original had %i\n\n", _productions.size(), prev_productions_size);
    print();

    prev_productions_size = _productions.size();
    int prev_nonterminals_size = _nonterminals.size();
    int prev_terminals_size = _terminals.size();
    set<string> generating = findgenerating();
    set<string> reachable = findreachable();

    cout << "\n >> Eliminating useless symbols" << endl;
    removeuseless(generating, reachable);
    cout << "  Generating symbols: " << encaseincurly(generating) << endl;
    cout << "  Reachable symbols: " << encaseincurly(reachable) << endl;
    set<string> useful;
    set_intersection(generating.begin(), generating.end(), reachable.begin(), reachable.end(), inserter(useful, useful.begin()));
    cout << "  Useful symbols: " << encaseincurly(useful) << endl;
    printf("  Removed %i variables, %i terminals and %i productions\n\n", prev_nonterminals_size-_nonterminals.size(), prev_terminals_size-_terminals.size(), prev_productions_size-_productions.size());
    print();

    prev_nonterminals_size = _nonterminals.size();
    prev_productions_size = _productions.size();
    cout << "\n >> Replacing terminals in bad bodies" << endl;
    set<string> newvars = replaceterminals();
    printf("  Added %i new variables: %s\n", _nonterminals.size()-prev_nonterminals_size, encaseincurly(newvars).c_str());
    printf("  Created %i new productions, original had %i\n\n", _productions.size(), prev_productions_size);
    print();

    prev_nonterminals_size = _nonterminals.size();
    int bodiesbroken = breakrules();
    printf("\n >> Broke %i bodies, added %i new variables\n", bodiesbroken, _nonterminals.size()-prev_nonterminals_size);
    cout << ">>> Result CFG:\n" << endl;
    print();
}

set<string> CFG::findnullables() {
    bool done = false;
    set<string> nullables;

    while (not done) {
        done = true;
        for (auto rule: _productions) {
            if (nullables.find(rule.first) != nullables.end()) { continue; }

            if (rule.second.size() == 0) {
                nullables.emplace(rule.first);
                done = false;
                continue;
            }
            bool non_nullable_found = false;// if we find a non nullable in body then the head is not nullable (in this rule)
            for (auto symbol: rule.second) {
                if (nullables.find(symbol) == nullables.end()) {
                    non_nullable_found = true;
                    break;
                }
            }
            if (not non_nullable_found) {
                nullables.emplace(rule.first);
                done = false;
            }
        }
    }
    return nullables;
}

map<string, set<string>> CFG::findunitpairs(int &unitproduction_count) {
    map<string, set<string>> unitpairs;

    for (auto var : _nonterminals){
        unitpairs[var].emplace(var);
    }

    for (auto rule: _productions) {
        if (rule.second.size() != 1) { continue; }
        if (std::find(_nonterminals.begin(), _nonterminals.end(), rule.second[0]) == _nonterminals.end()) { continue; }
        // the rule has a body of size 1 and that body is a non terminal
        unitproduction_count++;
        unitpairs[rule.first].emplace(rule.first);
        unitpairs[rule.first].emplace(rule.second[0]);
    }
    for (auto &pair: unitpairs) {
        for (string var: pair.second) {
            if (unitpairs[var].empty()) {
                unitpairs.erase(var);
                continue;
            }

            for (string element: unitpairs[var]) {
                pair.second.emplace(element);
            }
        }
    }
    return unitpairs;
}

set<string> CFG::findreachable() {
    set<string> reached_symbols = {_startSymbol};
    bool done = false;
    while (not done) {
        done = true;
        for (const string &temp: reached_symbols) {
            for (const auto &rule: _productions) {
                if (rule.first != temp) { continue; }
                for (const auto &symbol: rule.second) {
                    if (reached_symbols.find(symbol) != reached_symbols.end()) { continue; }
                    reached_symbols.emplace(symbol);
                    done = false;
                }
            }
        }
    }
    return reached_symbols;
}

set<string> CFG::findgenerating() {
    set<string> generating_symbols;
    // all terminal symbols are always generating
    for (const string &terminal: _terminals) {
        generating_symbols.emplace(terminal);
    }

    // if all symbols in body are generating, then the rule head is also generating
    bool done = false;
    while (not done) {
        done = true;
        for (const auto &rule: _productions) {
            if (generating_symbols.find(rule.first) != generating_symbols.end()) { continue; }// if the head is already generating, we don't need to check the body
            bool non_generating_found = false;
            for (const string &symbol: rule.second) {
                if (generating_symbols.find(symbol) == generating_symbols.end()) {
                    non_generating_found = true;
                    break;
                }
            }
            if (not non_generating_found) {
                generating_symbols.emplace(rule.first);
                done = false;
            }
        }
    }
    return generating_symbols;
}

void CFG::removeepsilons(const set<string> &nullables) {
    auto rule = _productions.begin();
    while (rule != _productions.end()) {
        if (rule->second.empty()) {
            _productions.erase(rule);
            rule = _productions.begin();
        }
        rule++;
    }
    set<pair<string, vector<string>>> newrules;
    for (auto &rule: _productions) {
        bool nullables_found = false;
        int counter = -1;// initialize at -1 because we have to increment the counter before non-nullables are filtered out
        vector<int> nullables_positions;
        for (const string &symbol: rule.second) {
            counter++;
            if (nullables.find(symbol) == nullables.end()) { continue; }
            nullables_found = true;
            nullables_positions.push_back(counter);// keep track of nullable positions to make the removal easier
        }
        if (nullables_found && rule.second.size() > 1) {// we have to create new _productions

            for (int i = 0; i < pow(2, nullables_positions.size()) - 1; i++) {// -1 because the situation where no nullables are removed is the original rule
                int nullablecounter = 0;
                vector<string> new_body = rule.second;
                bitset<32> mask(i);// we make a binary representation of i to see which nullables are included and which are not
                auto remover = new_body.begin();
                while (remover != new_body.end()) {
                    if (nullables.find(*remover) == nullables.end()) {
                        remover++;
                    } else if (mask[nullablecounter] == 1) {// we found a nullable but aren't removing it
                        nullablecounter++;
                        remover++;
                    } else {// we found a nullable and are removing it
                        nullablecounter++;
                        new_body.erase(remover);
                    }
                }
                pair<string, vector<string>> new_rule = make_pair(rule.first, new_body);
                newrules.emplace(new_rule);
            }
        }
    }
    _productions.insert(newrules.begin(), newrules.end());
}

void CFG::removeunitpairs(const map<string, set<string>> &unitpairs) {
    auto rule = _productions.begin();
    while (rule != _productions.end()) {
        if (unitpairs.find(rule->first) == unitpairs.end()) {
            rule++;
            continue;
        }
        if (rule->second.size() != 1) {
            rule++;
            continue;
        }
        if (std::find(_terminals.begin(), _terminals.end(), rule->second[0]) != _terminals.end()) {
            rule++;
            continue;
        }
        _productions.erase(rule);
        rule = _productions.begin();
    }
    for (auto unitpair: unitpairs) {
        set<vector<string>> newbodies;
        for (auto temp: unitpair.second) {// should rename this variable, but am not creative right now :(
            for (auto rule: _productions) {
                if (rule.first == temp) {
                    newbodies.insert(rule.second);
                }
            }
        }
        for (auto body: newbodies) {
            pair<string, vector<string>> new_rule = make_pair(unitpair.first, body);
            _productions.emplace(new_rule);
        }
    }
}

void CFG::removeuseless(set<string> &generating, set<string> &reachable) {
    // remove symbols that are not generating
    generating = findgenerating();
    auto rule = _productions.begin();
    while (rule != _productions.end()) {
        bool rule_deleted = false;
        if (generating.find(rule->first) != generating.end()) {
            for (auto symbol: rule->second) {
                if (generating.find(symbol) != generating.end()) { continue; }

                _productions.erase(rule);
                rule = _productions.begin();
                rule_deleted = true;
                break;
            }
            if (not rule_deleted) {
                rule++;
                continue;
            }
        }
        if (not rule_deleted) {
            _productions.erase(rule);
            rule = _productions.begin();
        }
    }

    // remove symbols that are not reachable
    reachable = findreachable();
    rule = _productions.begin();
    while (rule != _productions.end()) {
        bool rule_deleted = false;
        if (reachable.find(rule->first) != reachable.end()) {
            for (auto symbol: rule->second) {
                if (reachable.find(symbol) != reachable.end()) { continue; }

                _productions.erase(rule);
                rule = _productions.begin();
                rule_deleted = true;
                break;
            }
            if (not rule_deleted) {
                rule++;
                continue;
            }
        }
        if (not rule_deleted) {
            _productions.erase(rule);
            rule = _productions.begin();
        }
    }

    auto var = _nonterminals.begin();
    while (var != _nonterminals.end()){
        if (generating.find(*var) == generating.end() || reachable.find(*var) == reachable.end()){
            // the variable is not generating or not reachable --> delete it
            _nonterminals.erase(var);
            var = _nonterminals.begin();
            continue;
        }
        var++;
    }
    auto terminal = _terminals.begin();
    while (terminal != _terminals.end()){
        if (reachable.find(*terminal) == reachable.end()){ // terminals are always generating
            // the terminal is not reachable --> delete it
            _terminals.erase(terminal);
            terminal = _terminals.begin();
            continue;
        }
        terminal++;
    }
}

set<string> CFG::replaceterminals() {
    // replace terminals in bad bodies
    set<string> newvars;
    map<string, string> terminalcreators;// <terminal, rule_that_creates_terminal>
    for (const auto &rule: _productions) {
        if (rule.second.size() != 1) { continue; }
        if (std::find(_terminals.begin(), _terminals.end(), rule.second[0]) == _terminals.end()) { continue; }

        // the rule body has 1 terminal
        terminalcreators[rule.second[0]] = rule.first;// this will overwrite when we have multiple variables that produce the same terminal, but this shouldn't matter
    }

    auto rule = _productions.begin();
    while (rule != _productions.end()) {
        if (rule->second.size() <= 1) {
            rule++;
            continue;
        }
        int symbol_counter = 0;
        bool replacement_done = false;
        while (symbol_counter < rule->second.size()) {
            if (std::find(_nonterminals.begin(), _nonterminals.end(), rule->second[symbol_counter]) != _nonterminals.end()) {
                symbol_counter++;
                continue;
            }

            // we have a terminal in a rule body of size > 1
            vector<string> newbody = rule->second;
            if (terminalcreators.find(rule->second[symbol_counter]) != terminalcreators.end()) {
                // there already is a rule that creates this terminal --> replace terminal and continue
                newbody[symbol_counter] = terminalcreators[rule->second[symbol_counter]];
                _productions.emplace(make_pair(rule->first, newbody));
                _productions.erase(rule);
                replacement_done = true;
                break;
            } else {
                // there is no rule that creates this terminal --> create rule and replace
                string newvar = "_" + rule->second[symbol_counter];
                newvars.emplace(newvar);
                _nonterminals.push_back(newvar);
                terminalcreators[rule->second[symbol_counter]] = newvar;
                newbody[symbol_counter] = newvar;
                _productions.emplace(make_pair(rule->first, newbody));
                vector<string> terminalvector = {rule->second[symbol_counter]};
                _productions.emplace(make_pair(newvar, terminalvector));
                _productions.erase(rule);
                replacement_done = true;
                break;
            }
        }
        if (replacement_done) {
            rule = _productions.begin();
        } else {
            rule++;
        }
    }
    return newvars;
}

int CFG::breakrules() {
    // breaking _productions with more than 2 vars in body
    int bodiesbroken = 0;
    map<string, int> amountused;

    for (const auto &var: _nonterminals) {
        amountused[var] = 1;
    }

    auto rule = _productions.begin();
    while (rule != _productions.end()) {
        if (rule->second.size() <= 2) {
            rule++;
            continue;
        }

        // break of the last 2 vars and move them to a new rule
        bodiesbroken++;
        amountused[rule->first]++;
        string newvar = rule->first + "_" + to_string(amountused[rule->first]);
        _nonterminals.push_back(newvar);

        vector<string> changed_body = rule->second;
        vector<string> new_body(rule->second.end() - 2, rule->second.end());

        changed_body.erase(changed_body.end() - 2, changed_body.end());// remove the last 2 symbols and add the new replacement variable
        changed_body.push_back(newvar);

        pair<string, vector<string>> changed_rule = make_pair(rule->first, changed_body);
        pair<string, vector<string>> new_rule = make_pair(newvar, new_body);
        _productions.emplace(changed_rule);
        _productions.emplace(new_rule);
        _productions.erase(rule);
        rule = _productions.begin();
    }
    return bodiesbroken;
}

string CFG::convertunitpairstostring(map<string, set<string>> &unitpairs) {
    set<string> pairstrings;
    string tempstring;
    for (auto pair : unitpairs){
        for (auto rhs : pair.second){
            tempstring = "(" + pair.first + ", " + rhs + ")";
            pairstrings.emplace(tempstring);
        }
    }
    return encaseincurly(pairstrings);
}
const vector<string> &CFG::getNonterminals() const {
    return _nonterminals;
}
const vector<string> &CFG::getTerminals() const {
    return _terminals;
}
const set<pair<string, vector<string>>> &CFG::getProductions() const {
    return _productions;
}
const string &CFG::getStartSymbol() const {
    return _startSymbol;
}

set<string> CFG::First(const string &input) {
    set<string> result;
    for (const auto& rule : _productions){
        if (rule.second.size() == 0){   // the empty string has no useful information for the parser, so we ignore it
            continue;
        }
        if (rule.first == input){
            result.emplace(rule.second[0]);
        }
    }
    return result;
}
