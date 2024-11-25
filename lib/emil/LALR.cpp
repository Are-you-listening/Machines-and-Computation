//
// Created by emill on 20/11/2023.
//

#include "LALR.h"
LALR::LALR(const CFG &cfg) : _cfg(cfg) {}

void LALR::createStates() {
    // manually create I0
    state* I0 = new state;
    this->I0 = I0;
    state_counter = 0;
    I0->_stateName = 0;
    I0->_connections = {};
    I0->_productions = createAugmented(make_tuple<string, vector<string>, set<string>>((_cfg.getStartSymbol() + "'"), {".", _cfg.getStartSymbol()}, {"$"}));
    I0->createConnections(*this);
    cout << "debug" << endl;
}

set<state *> LALR::findSimilar(const set<tuple<string, vector<string>, set<string>>> &rules) {
    queue<state*> remaining;
    set<state*> visited;
    remaining.push(I0);
    visited.emplace(I0);
    set<state*> similarstates;

    while(not remaining.empty()){
        state* currentstate = remaining.front();
        remaining.pop();

        bool similar = true;
        if (currentstate->_productions.size() == rules.size()){
            for (auto rule1 : currentstate->_productions){
                auto rule2 = rules.begin();
                if (get<0>(rule1) != get<0>(*rule2) || get<1>(rule1) != get<1>(*rule2)){
                    similar = false;
                }
            }
        } else {
            similar = false;
        }

        if (similar){
            similarstates.emplace(currentstate);
        }

        for (const auto& connection : currentstate->_connections){
            if (visited.find(connection.second) == visited.end()){
                remaining.push(connection.second);
                visited.emplace(connection.second);
            }
        }
    }
    return similarstates;
}

void LALR::mergeSimilar() {
    // find all the similar states
    set<set<state*>> similarsets;
    queue<state*> remaining;
    set<state*> visited;
    remaining.push(I0);
    visited.emplace(I0);

    while (not remaining.empty()){
        state* currentstate = remaining.front();
        remaining.pop();

        set<state*> similarstates = findSimilar(currentstate->_productions);
        if (similarstates.size() > 1){
            similarsets.emplace(similarstates);
            visited.insert(similarstates.begin(), similarstates.end());
        }

        for (const auto& connection : currentstate->_connections){
            if (visited.find(connection.second) == visited.end()){
                remaining.push(connection.second);
                visited.emplace(connection.second);
            }
        }
    }

    for (auto sstates: similarsets){
        int mainname = (*sstates.begin())->_stateName;
        set<int> othernames;
        auto stateiter = next(sstates.begin());
        while (stateiter != sstates.end()){
            othernames.emplace((*stateiter)->_stateName);
            stateiter++;
        }
        // go over whole table and replace othernames with mainname
        for (auto row : parseTable){
            for (auto element : row.second){
                for (auto othername : othernames){
                    string temp = to_string(othername);
                    replace(element.second.begin(), element.second.end(), to_string(othername)[0], to_string(mainname)[0]);
                }
            }
        }
        // merge other row contents into main row
        for (auto stateiter = next(sstates.begin()); stateiter != sstates.end(); stateiter++){
            for (auto row : parseTable){
                if (row.first == (*stateiter)->_stateName){
                    parseTable[mainname].insert(row.second.begin(), row.second.end());
                }
            }
        }
        // delete other rows
        for (auto stateiter = next(sstates.begin()); stateiter != sstates.end(); stateiter++){
            parseTable.erase((*stateiter)->_stateName);
        }
    }
}

void LALR::createTable() {
    unordered_map<int, map<string, string>> createdTable;
    createStates();

    queue<state*> remaining;
    set<state*> visited;
    remaining.push(I0);
    visited.emplace(I0);

    while(not remaining.empty()){
        state* currentstate = remaining.front();
        remaining.pop();

        map<string, string> rowcontent;

        // if otherstate is an endstate (1 rule with "." at the end of the body) place R{cfg rule number} in lookahead columns
        if (currentstate->isendingstate){
            for (string symbol : get<2>(*currentstate->_productions.begin())){
                if (currentstate->cfgrulenumber == -1){
                    rowcontent[symbol] = "accept";
                } else {
                    rowcontent[symbol] = "R" + to_string(currentstate->cfgrulenumber);
                }
            }
            createdTable[currentstate->_stateName] = rowcontent;
            continue;
        }

        for (auto connection : currentstate->_connections){
            string inputsymbol = connection.first;
            state* otherstate = connection.second;

            if (std::find(_cfg.getNonterminals().begin(), _cfg.getNonterminals().end(), inputsymbol) != _cfg.getNonterminals().end()){
                rowcontent[inputsymbol] = to_string(otherstate->_stateName);
            } else {
                rowcontent[inputsymbol] = "S" + to_string(otherstate->_stateName);
            }
        }
        createdTable[currentstate->_stateName] = rowcontent;

        for (const auto& connection : currentstate->_connections){
            if (visited.find(connection.second) == visited.end()){
                remaining.push(connection.second);
                visited.emplace(connection.second);
            }
        }
    }
    parseTable = createdTable;
    mergeSimilar();
}

augmentedrules LALR::createAugmented(const tuple<string, vector<string>, set<string>> &inputrule) {
    augmentedrules result;
    result.emplace(inputrule);
    set<tuple<string, vector<string>, set<string>>> newProductions;

    bool done = false;

    while (not done) {
        done = true;
        for (const auto &Arule: result) {
            string currentSymbol;
            auto dotPosition = std::find(get<1>(Arule).begin(), get<1>(Arule).end(), ".");

            if (dotPosition != std::get<1>(Arule).end() && std::next(dotPosition) != std::get<1>(Arule).end()) {
                currentSymbol = *next(dotPosition);
            } else {
                continue;// the "." is at the end of the rule body, so we don't need to add any new _productions
            }

            if (std::find(_cfg.getTerminals().begin(), _cfg.getTerminals().end(), currentSymbol) != _cfg.getTerminals().end()){
                // currentSymbol is a terminal --> there are no _productions with currentSymbol in head --> no _productions to add
                continue;
            }

            // currentSymbol is a non-terminal --> add the corresponding _productions
            for (const auto &rule: _cfg.getProductions()) {
                if (rule.first == currentSymbol) {
                    // add rule to newProductions
                    vector<string> tempvector = rule.second;
                    tempvector.insert(tempvector.begin(), ".");
                    // if currentSymbol + 1 exists --> first(nextsymbol) else --> copy lookahead from Arule
                    set<string> lookahead;
                    if (next(next(dotPosition)) < get<1>(Arule).end()){
                        lookahead = _cfg.First(*next(next(dotPosition)));
                    } else {
                        lookahead = get<2>(Arule);
                    }
                    if (result.find(make_tuple(currentSymbol, tempvector, lookahead)) == result.end()) {
                        newProductions.emplace(currentSymbol, tempvector, lookahead);
                    }
                }
            }
        }
        if (not newProductions.empty()) {
            result.insert(newProductions.begin(), newProductions.end());
            newProductions.clear();
            done = false;
        }
    }
    return result;
}

void state::createConnections(LALR &lalr) {
    for (const auto& rule : _productions){
        int dotIndex = 0;
        // create a new state where we move the dot one space to the right if this state doesn't exist already
        for (const string& c : get<1>(rule)){
            if (c == "."){
                break;
            }
            dotIndex++;
        }
        if (dotIndex >= get<1>(rule).size()-1){     // check if the dot is the last char in the body
            continue;
        }
        vector<string> newvector = get<1>(rule);
        string readSymbol = newvector[dotIndex+1];
        newvector[dotIndex] = readSymbol;
        newvector[dotIndex+1] = ".";
        tuple<string, vector<string>, set<string>> changedrule = make_tuple(get<0>(rule), newvector, get<2>(rule));

        augmentedrules newrules = lalr.createAugmented(changedrule);
        state* tempstate = lalr.findstate(newrules);
        if (tempstate != nullptr){
            _connections.emplace_back(readSymbol, tempstate);
        } else {
            state *newstate = new state;
            lalr.state_counter++;
            newstate->_stateName = lalr.state_counter;
            newstate->_productions = newrules;
            _connections.emplace_back(readSymbol, newstate);

            if (newstate->_productions.size() == 1 && *(get<1>(*newstate->_productions.begin()).end() - 1) == ".") {
                newstate->isendingstate = true;
                tuple<string, vector<string>, set<string>> newstaterule = *newstate->_productions.begin();
                if (get<0>(newstaterule) == lalr._cfg.getStartSymbol() + "'"){
                    newstate->cfgrulenumber = -1;   // in this case we have the rule S' -> S. , $   and this will result in accept in the parse table. This is the only situation where we write accept in the parse table
                } else {
                    get<1>(newstaterule).pop_back();// remove the "."
                    int count = 0;
                    for (auto rule: lalr._cfg.getProductions()) {
                        if (rule.first == get<0>(newstaterule) && rule.second == get<1>(newstaterule)) {
                            newstate->cfgrulenumber = count;// we need the rule number in the parse Table
                        }
                        count++;
                    }
                }
            }

            newstate->createConnections(lalr);
        }
    }
}
state::~state() {
    for (auto& connection : _connections){
        delete connection.second;
    }
}

state *LALR::findstate(const set<tuple<string, vector<string>, set<string>>>& rules) {
    // search for a state that has the given rules using breadth-first search pattern
    queue<state*> remaining;
    set<state*> visited;
    remaining.push(I0);
    visited.emplace(I0);

    while(not remaining.empty()){
        state* current = remaining.front();
        remaining.pop();

        if (current->_productions == rules){
            return current;
        }
        for (const auto& connection : current->_connections){
            if (visited.find(connection.second) == visited.end()){
                remaining.push(connection.second);
                visited.emplace(connection.second);
            }
        }
    }
    return nullptr;
}

void LALR::printstates() {
    queue<state*> remaining;
    set<state*> visited;
    remaining.push(I0);

    while (not remaining.empty()){
        state* current = remaining.front();
        remaining.pop();

        cout << current->_stateName << endl;

        for (const auto& connection : current->_connections){
            if (visited.find(connection.second) == visited.end()){
                remaining.push(connection.second);
                visited.emplace(connection.second);
            }
        }
    }
}

void LALR::parse(string inputstring) {
    stack<int> s;
    s.push(0);
    string remaininginput = inputstring + "$";
    vector<parseTree*> treetops;
    bool done = false;
    while (not done){
        if (remaininginput.empty()){
            break;
        }
        int stacksymbol = s.top();
        string inputsymbol = remaininginput.substr(0, 1);
        if (inputsymbol != "$") {
            remaininginput = remaininginput.substr(1, remaininginput.length() - 1);
        }
        string operation = parseTable[stacksymbol][inputsymbol];

        if (operation == "accept"){
            break;
        } else if (operation.substr(0, 1) == "S"){
            s.push(stoi(operation.substr(1, operation.length()-1)));
        } else if (operation.substr(0, 1) == "R"){
            int count = 0;
            auto rule = _cfg.getProductions().begin();
            while (count != stoi(operation.substr(1, operation.length()-1))){
                rule++;
                count++;
            }
            remaininginput = rule->first + remaininginput;
            parseTree* newparent = new parseTree;
            treetops.push_back(newparent);
            if (rule->first == _cfg.getStartSymbol()){
                _root = newparent;
            }
            newparent->symbol = rule->first;
            for (string symbol : rule->second){
                bool found = false;
                for (int i = 0; i < treetops.size(); i++){
                    if (symbol == treetops[i]->symbol){
                        newparent->children.push_back(treetops[i]);
                        found = true;
                        break;
                    }
                }
                if (not found) {
                    parseTree *newchild = new parseTree;
                    newchild->symbol = symbol;
                    newparent->children.push_back(newchild);
                }
                s.pop();
            }
        } else {
            s.push(stoi(operation));
        }
    }
}
parseTree::~parseTree() {
    for (const auto& child : children){
        delete child;
    }
}
