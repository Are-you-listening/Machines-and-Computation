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
    std::cout << "start createAugmented" << std::endl;
    I0->_productions = createAugmented(make_tuple<string, vector<string>, set<string>>((_cfg.getS() + "'"), {".", _cfg.getS()}, {"$"}));
    std::cout << "end createAugmented" << std::endl;
    I0->createConnections(*this);
    std::cout << "end createConnections" << std::endl;
    //cout << "debug" << endl;
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

        // if currentstate is an endstate (1 rule with "." at the end of the body) place R{cfg rule number} in lookahead columns
        for (const auto &endingpair: currentstate->endings) {
            for (const string &lookahead: endingpair.second) {
                if (endingpair.first == -1) {
                    rowcontent[lookahead] = "accept";
                } else {
                    rowcontent[lookahead] = "R" + to_string(endingpair.first);
                }
            }
        }

        for (const auto &connection: currentstate->_connections) {
            string inputsymbol = connection.first;
            state *otherstate = connection.second;

            if (std::find(_cfg.getV().begin(), _cfg.getV().end(), inputsymbol) != _cfg.getV().end()) {
                rowcontent[inputsymbol] = to_string(otherstate->_stateName);
            } else {
                rowcontent[inputsymbol] = "S" + to_string(otherstate->_stateName);
            }
        }

        // this if statement and the code in it might be unnecessary and could be removed without consequences
        if (createdTable.find(currentstate->_stateName) != createdTable.end()) {
            for (auto &element: createdTable[currentstate->_stateName]) {
                if (rowcontent.find(element.first) != rowcontent.end()) {
                    if (element.second.substr(0, 1) == "R" && rowcontent[element.first].substr(0, 1) == "S") {
                        element.second = rowcontent[element.first];// the new row has a shift in the corresponding position while the original has a reduce
                    }
                }
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


    for (const auto &Arule: result) {

        string currentSymbol;
        auto dotPosition = std::find(get<1>(Arule).begin(), get<1>(Arule).end(), ".");

        if (dotPosition != std::get<1>(Arule).end() && std::next(dotPosition) != std::get<1>(Arule).end()) {
            currentSymbol = *next(dotPosition);
        } else {
            continue;// the "." is at the end of the rule body, so we don't need to add any new _productions
        }

        if (std::find(_cfg.getT().begin(), _cfg.getT().end(), currentSymbol) != _cfg.getT().end()){
            // currentSymbol is a terminal --> there are no _productions with currentSymbol in head --> no _productions to add
            continue;
        }


        // currentSymbol is a non-terminal --> add the corresponding _productions
        for (const auto &rule: _cfg.getP()) {
            if (rule.first != currentSymbol) {
                continue;
            }

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
    result.insert(newProductions.begin(), newProductions.end());
    newProductions.clear();

    return result;
}

void state::createConnections(LALR &lalr) {
    map<string, augmentedrules > transitionmap;
    for (const auto& rule : _productions) {
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
        transitionmap[readSymbol].emplace(changedrule);
    }
    for (auto newstate_data: transitionmap) {
        string readSymbol = newstate_data.first;
        augmentedrules newrules;

        for (auto changedrule: newstate_data.second) {
            augmentedrules temp = lalr.createAugmented(changedrule);
            newrules.insert(temp.begin(), temp.end());
        }

        state *otherstate;
        state *tempstate = lalr.findstate(newrules);
        if (tempstate != nullptr) {
            _connections.emplace_back(readSymbol, tempstate);
            otherstate = tempstate;
        } else {
            state *newstate = new state;
            lalr.state_counter++;
            newstate->_stateName = lalr.state_counter;
            newstate->_productions = newrules;
            _connections.emplace_back(readSymbol, newstate);
            otherstate = newstate;
            newstate->createConnections(lalr);
        }

        for (const auto &state_rule: otherstate->_productions) {
            if (get<1>(state_rule).back() ==
                ".") {// if the state_rule has a dot at the end of the body it is an ending state_rule
                if (get<0>(state_rule) == lalr._cfg.getS() + "'") {
                    otherstate->endings.emplace(-1, get<2>(state_rule));
                } else {
                    pair<string, vector<string>> temprule = {get<0>(state_rule),
                                                             get<1>(state_rule)};// with this temprule we will find the cfg state_rule number
                    temprule.second.pop_back();
                    int index = 0;
                    for (const auto &cfg_rule: lalr._cfg.getP()) {
                        if (cfg_rule == temprule) {
                            break;
                        }
                        index++;
                    }
                    otherstate->endings.emplace(index, get<2>(state_rule));
                }
            }
        }
    }
    //std::cout << "end createConections" <<std::endl;
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
        for (auto rule: current->_productions) {
            cout << "\t" << get<0>(rule) << " --> ";
            for (auto element: get<1>(rule)) {
                cout << " " << element;
            }
            cout << " ,";
            for (auto LA: get<2>(rule)) {
                cout << " " << LA;
            }
            cout << endl;
        }

        for (const auto& connection : current->_connections){
            if (visited.find(connection.second) == visited.end()){
                remaining.push(connection.second);
                visited.emplace(connection.second);
            }
        }
    }
}

void LALR::parse(std::vector<std::pair<std::string, std::string>> &input) {
    stack<int> s;
    s.push(0);

    std::vector<std::pair<std::string, std::string>> remaininginputvector = input;
    remaininginputvector.push_back({"$","$"});

    vector<parseTree*> treetops;
    while (true){
        if (remaininginputvector.empty()){
            break;
        }
        int stacksymbol = s.top();
        string inputsymbol = remaininginputvector.begin()->first;
        if (inputsymbol != "$") {
            remaininginputvector.erase(remaininginputvector.begin());
        }
        string operation = parseTable[stacksymbol][inputsymbol];

        //cout << "stacksymbol: " << stacksymbol << ", inputsymbol: " << inputsymbol << " --> " << operation << endl;

        if (operation == "accept"){
            break;
        } else if (operation.substr(0, 1) == "S"){
            s.push(stoi(operation.substr(1, operation.length()-1)));
        } else if (operation.substr(0, 1) == "R"){
            int count = 0;
            auto rule = _cfg.getP().begin();
            while (count != stoi(operation.substr(1, operation.length()-1))){
                rule++;
                count++;
            }
            remaininginputvector.insert(remaininginputvector.begin(), make_pair(rule->first, ""));
            parseTree* newparent = new parseTree;
            treetops.push_back(newparent);
            newparent->symbol = rule->first;
            if (rule->first == _cfg.getS()){
                _root = newparent;
            }
            for (string symbol : rule->second){
                bool found = false;
                auto it = treetops.begin();
                set<vector<parseTree*>::iterator> toRemove;
                while(it != treetops.end()){
                    if ((*it)->symbol == symbol){
                        parseTree* temp = *it;
                        newparent->children.push_back(temp);
                        found = true;
                        toRemove.insert(it);
                    }
                    it++;
                }
                for (auto temp : toRemove){
                    treetops.erase(temp);
                }
                if (not found) {
                    parseTree *newchild = new parseTree;
                    newchild->symbol = symbol;
                    newparent->children.push_back(newchild);
                }
                s.pop();
            }
            treetops.push_back(newparent);
        } else {
            s.push(stoi(operation));
        }
    }
}

void LALR::printTable() {
    for (const auto& rowpair : parseTable){
        cout << "State: " << to_string(rowpair.first) << endl;
        for (const auto& columnpair : rowpair.second){
            cout << "\tSymbol: " << columnpair.first << ", Table element: " << columnpair.second << endl;
        }
    }
}

void LALR::cleanUp() {
    bool V = false;
    _root->traverse(_cfg.getT(),_root,V);
}

parseTree::~parseTree() {
    for (const auto& child : children){
        delete child;
    }
}

void parseTree::traverse(const std::vector<std::string> &T, parseTree* _root, bool &V_root) {
    bool V = false;

    if(this->symbol=="{" || this->symbol=="}"){
        return;
    }

    for(long unsigned int i = 0;  i<children.size(); ++i){
        auto child = children[i];
        if(child->symbol=="{" || child->symbol=="}"){ //Stop cleanup if bracket reach; we may not modify this
            V=true;
        }else if( std::find(T.begin(), T.end(),child->symbol)==T.end() ){ //We found a variable
            V = true;
            child->traverse(T,this,V); //Traverse the child
            //Reloop
            if(!V){
                i = -1;
            }
        }
    }

    std::vector<parseTree*> temp;
    if(!V){ //No Variables found: Cleanup this one
        for(auto &it: _root->children){
            if(it==this){
                for(auto &k: this->children){ //Add children of this
                    temp.push_back(k);
                }
            }else{ //Add the already added child to keep the correct order
                temp.push_back(it);
            }
        }
        V_root = false;
        _root->children=temp;
        this->children.clear();
        delete this;
    }
}

parseTree::parseTree( vector<parseTree *> children,  string symbol) : children(children), symbol(symbol) {}

parseTree::parseTree() {

}
