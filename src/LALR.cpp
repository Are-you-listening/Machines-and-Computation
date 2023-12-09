//
// Created by emill on 20/11/2023.
//

#include "LALR.h"

LALR::LALR(const CFG &cfg) : _cfg(cfg) {}

void LALR::createStates() {
    // manually create I0
    auto* I0 = new State;
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

set<State *> LALR::findSimilar(const set<tuple<string, vector<string>, set<string>>> &rules) {
    queue<State*> remaining;
    set<State*> visited;
    remaining.push(I0);
    visited.emplace(I0);
    set<State*> similarStates;

    while(not remaining.empty()){
        State* currentState = remaining.front();
        remaining.pop();

        bool similar = true;
        if (currentState->_productions.size() == rules.size()){
            for (auto rule1 : currentState->_productions){
                auto rule2 = rules.begin();
                if (get<0>(rule1) != get<0>(*rule2) || get<1>(rule1) != get<1>(*rule2)){
                    similar = false;
                }
            }
        } else {
            similar = false;
        }

        if (similar){
            similarStates.emplace(currentState);
        }

        for (const auto& connection : currentState->_connections){
            if (visited.find(connection.second) == visited.end()){
                remaining.push(connection.second);
                visited.emplace(connection.second);
            }
        }
    }
    return similarStates;
}

void LALR::mergeSimilar() {
    // find all the similar States
    set<set<State*>> similarsets;
    queue<State*> remaining;
    set<State*> visited;
    remaining.push(I0);
    visited.emplace(I0);

    while (not remaining.empty()){
        State* currentState = remaining.front();
        remaining.pop();

        set<State*> similarStates = findSimilar(currentState->_productions);
        if (similarStates.size() > 1){
            similarsets.emplace(similarStates);
            visited.insert(similarStates.begin(), similarStates.end());
        }

        for (const auto& connection : currentState->_connections){
            if (visited.find(connection.second) == visited.end()){
                remaining.push(connection.second);
                visited.emplace(connection.second);
            }
        }
    }

    for (const auto& sStates: similarsets){
        int mainname = (*sStates.begin())->_stateName;
        set<int> othernames;
        auto Stateiter = next(sStates.begin());
        while (Stateiter != sStates.end()){
            othernames.emplace((*Stateiter)->_stateName);
            Stateiter++;
        }
        // go over the whole table and replace othernames with mainname
        for (auto &row : parseTable){
            for (auto& element : row.second){
                for (auto othername : othernames){

                    string prefix = element.second.substr(0,1);
                    string remainder;
                    if (prefix != "S" && prefix != "R"){
                        prefix = "e";
                    } else {
                        remainder = element.second.substr(1, element.second.length()-1);
                    }

                    if (prefix == "e" && remainder == to_string(othername)){
                        element.second = to_string(mainname);
                    } else if (prefix != "e" && remainder == to_string(othername)) {
                        element.second = prefix + to_string(mainname);
                    }
                }
            }
        }
        // merge other row contents into the main row
        for (auto Stateiter = next(sStates.begin()); Stateiter != sStates.end(); Stateiter++){
            for (auto row : parseTable){
                if (row.first == (*Stateiter)->_stateName){
                    parseTable[mainname].insert(row.second.begin(), row.second.end());
                }
            }
        }
        // delete other rows
        for (auto Stateiter = next(sStates.begin()); Stateiter != sStates.end(); Stateiter++){
            parseTable.erase((*Stateiter)->_stateName);
        }
    }
}

void LALR::createTable() {
    unordered_map<int, map<string, string>> createdTable;
    createStates();

    queue<State*> remaining;
    set<State*> visited;
    remaining.push(I0);
    visited.emplace(I0);

    while(not remaining.empty()){
        State* currentState = remaining.front();
        remaining.pop();

        map<string, string> rowcontent;

        // if currentState is an endState (1 rule with "." at the end of the body) place R{cfg rule number} in lookahead columns
        for (const auto &endingpair: currentState->endings) {
            for (const string &lookahead: endingpair.second) {
                if (endingpair.first == -1) {
                    rowcontent[lookahead] = "accept";
                } else {
                    rowcontent[lookahead] = "R" + to_string(endingpair.first);
                }
            }
        }

        for (const auto &connection: currentState->_connections) {
            string inputsymbol = connection.first;
            State *otherState = connection.second;

            if (std::find(_cfg.getV().begin(), _cfg.getV().end(), inputsymbol) != _cfg.getV().end()) {
                rowcontent[inputsymbol] = to_string(otherState->_stateName);
            } else {
                rowcontent[inputsymbol] = "S" + to_string(otherState->_stateName);
            }
        }

        // this if Statement and the code in it might be unnecessary and could be removed without consequences
        if (createdTable.find(currentState->_stateName) != createdTable.end()) {
            for (auto &element: createdTable[currentState->_stateName]) {
                if (rowcontent.find(element.first) != rowcontent.end()) {
                    if (element.second.substr(0, 1) == "R" && rowcontent[element.first].substr(0, 1) == "S") {
                        element.second = rowcontent[element.first];// the new row has a shift in the corresponding position while the original has a reduction
                    }
                }
            }
        }

        createdTable[currentState->_stateName] = rowcontent;


        for (const auto& connection : currentState->_connections){
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

void State::createConnections(LALR &lalr) {
    map<string, augmentedrules > transitionmap;
    for (const auto& rule : _productions) {
        int dotIndex = 0;
        // create a new State where we move the dot one space to the right if this State doesn't exist already
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
    for (const auto& newState_data: transitionmap) {
        string readSymbol = newState_data.first;
        augmentedrules newrules;

        for (const auto& changedrule: newState_data.second) {
            augmentedrules temp = lalr.createAugmented(changedrule);
            newrules.insert(temp.begin(), temp.end());
        }

        State *otherState;
        State *tempState = lalr.findState(newrules);
        if (tempState != nullptr) {
            _connections.emplace_back(readSymbol, tempState);
            otherState = tempState;
        } else {
            auto *newState = new State;
            lalr.state_counter++;
            newState->_stateName = lalr.state_counter;
            newState->_productions = newrules;
            _connections.emplace_back(readSymbol, newState);
            otherState = newState;
            newState->createConnections(lalr);
        }

        for (const auto &State_rule: otherState->_productions) {
            if (get<1>(State_rule).back() ==
                ".") {// if the State_rule has a dot at the end of the body, it is an ending State_rule
                if (get<0>(State_rule) == lalr._cfg.getS() + "'") {
                    otherState->endings.emplace(-1, get<2>(State_rule));
                } else {
                    pair<string, vector<string>> temprule = {get<0>(State_rule),
                                                             get<1>(State_rule)};// with this temprule we will find the cfg State_rule number
                    temprule.second.pop_back();
                    int index = 0;
                    for (const auto &cfg_rule: lalr._cfg.getP()) {
                        if (cfg_rule == temprule) {
                            break;
                        }
                        index++;
                    }
                    otherState->endings.emplace(index, get<2>(State_rule));
                }
            }
        }
    }
    //std::cout << "end createConections" <<std::endl;
}

State::~State() {
    for (auto& connection : _connections){
        delete connection.second;
    }
}

State *LALR::findState(const set<tuple<string, vector<string>, set<string>>>& rules) {
    // search for a State that has the given rules using a breadth-first search pattern
    queue<State*> remaining;
    set<State*> visited;
    remaining.push(I0);
    visited.emplace(I0);

    while(not remaining.empty()){
        State* current = remaining.front();
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

void LALR::printStates() {
    queue<State*> remaining;
    set<State*> visited;
    remaining.push(I0);

    while (not remaining.empty()){
        State* current = remaining.front();
        remaining.pop();

        cout << current->_stateName << endl;
        for (auto rule: current->_productions) {
            cout << "\t" << get<0>(rule) << " --> ";
            for (const auto& element: get<1>(rule)) {
                cout << " " << element;
            }
            cout << " ,";
            for (const auto& LA: get<2>(rule)) {
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

void LALR::parse(std::vector<std::tuple<std::string, std::string, std::set<std::string>>> &input) {
    stack<int> s;
    s.push(0);
    std::set<std::string> S={};
    auto remaininginputvector = input;
    remaininginputvector.emplace_back("$","$", S);

    vector<ParseTree*> treetops;
    while (true){
        if (remaininginputvector.empty()){
            break;
        }
        int stacksymbol = s.top();
        string inputsymbol = get<0>(*remaininginputvector.begin());
        auto inputtuple = *remaininginputvector.begin();
        if (inputsymbol != "$") {
            remaininginputvector.erase(remaininginputvector.begin());
        }
        string operation = parseTable[stacksymbol][inputsymbol];

        cout << "stacksymbol: " << stacksymbol << ", inputsymbol: " << inputsymbol << " --> " << operation << endl;

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

            if (inputsymbol != "$"){
                remaininginputvector.insert(remaininginputvector.begin(), inputtuple);
                remaininginputvector.insert(remaininginputvector.begin(), std::make_tuple(rule->first, "", S));
            } else {
                remaininginputvector.insert(remaininginputvector.begin(), std::make_tuple(rule->first, "", S));

            }

            auto* newparent = new ParseTree;
            treetops.push_back(newparent);
            newparent->symbol = rule->first;
            if (rule->first == _cfg.getS()){
                _root = newparent;
            }
            for (const string& symbol : rule->second){
                bool found = false;
                set<vector<ParseTree*>::iterator> toRemove;
                for(auto it = treetops.begin(); it != treetops.end();it++){
                    if ((*it)->symbol == symbol){
                        ParseTree* temp = *it;
                        newparent->children.push_back(temp);
                        found = true;
                        toRemove.insert(it);
                    }
                }
                for (auto temp : toRemove){
                    treetops.erase(temp);
                }
                if (not found) {
                    auto *newchild = new ParseTree;
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

void LALR::printTable() {
    for (const auto& rowpair : parseTable){
        cout << "State: " << to_string(rowpair.first) << endl;
        for (const auto& columnpair : rowpair.second){
            cout << "\tSymbol: " << columnpair.first << ", Table element: " << columnpair.second << endl;
        }
    }
}

void LALR::cleanUp() const {
    bool V = false;
    _root->clean(_cfg.getT(), _root, V);

    /**
     * Make sure every parenthese is matched at the same level
     **/
    std::tuple<ParseTree *, ParseTree *, unsigned long,bool> lb = {nullptr, nullptr,0,false};
    std::tuple<ParseTree *, ParseTree *, unsigned long,bool> rb = {nullptr, nullptr,0,false};
    vector<ParseTree*> tempchilds;
    bool found;
    _root->findBracket(true,lb,_cfg.getT());
    _root->findBracket(false,rb,_cfg.getT());

    //Bracket with highest depth needs to be placed on the equal level
    if( get<2>(lb) > get<2>(rb) ) { //Left is deeper and needs to be replaced
        get<0>(rb)->sameUpperRoot(get<1>(lb),found); //Check if the root of RB contains LB
        if(found){ //sameUpperRoot: respect index/child order;
            // index after which the bracket needs to be inserted:
            // for(child: children){ IF(child.sameUpperRoot) } === index
            found = false; //Reset found
            long unsigned int i;

            for(i = 0; i<get<0>(rb)->children.size(); ++i ){
                get<0>(rb)->children[i]->sameUpperRoot(get<1>(lb),found);

                if(found){
                    tempchilds.push_back(get<0>(rb)->children[i]);
                    tempchilds.push_back(get<1>(lb));
                }else{
                    tempchilds.push_back(get<0>(rb)->children[i]);
                }
            }

            for(i = i; i<get<0>(rb)->children.size(); ++i ){ //Add the rest of the children
                tempchilds.push_back(get<0>(rb)->children[i]);
            }

            get<0>(rb)->children = tempchilds; //Replace
            tempchilds.clear();

        }else{ //Just regular move

            //Make 1 new vector with all the children including LeftBracket
            tempchilds = {get<1>(lb)};
            for(auto &child: get<0>(rb)->children ){
                tempchilds.push_back(child);
            }
            get<0>(rb)->children = tempchilds; //Replace
            tempchilds.clear();
        }

        //Erase LB from lb_root
        for(auto &child: get<0>(lb)->children ){
            if(child!=get<1>(lb)){ //Add everything except the bracket itsself
                tempchilds.push_back(child);
            }
        }
        get<0>(lb)->children = tempchilds; //Replace lb _root
        tempchilds.clear(); //Free useless used memory


    }else if( get<2>(lb) == get<2>(rb) ){
        //Might be equal; don't change a thing
    }else{ //Right is deeper, add right bracket to leftbracket-parents and remove rightbracket from its own parent
        get<0>(lb)->sameUpperRoot(get<1>(rb),found); //Check if the root of RB contains LB
        if(found){
            found = false; //Reset found
            long unsigned int i;

            for(i = 0; i<get<0>(lb)->children.size(); ++i ){ //For every child of LB Root
                get<0>(lb)->children[i]->sameUpperRoot(get<1>(rb),found); //Check if this is the upperRoot of the node we want to replace

                if(found){ //If yes
                    tempchilds.push_back(get<1>(rb)); //Add the LB
                    tempchilds.push_back(get<0>(lb)->children[i]); //Add the child
                }else{
                    tempchilds.push_back(get<0>(lb)->children[i]); //Just add the child to keep the order
                }
            }

            //Add the rest of the children
            for(i = i; i<get<0>(lb)->children.size(); ++i ){
                tempchilds.push_back(get<0>(lb)->children[i]);
            }
            get<0>(lb)->children = tempchilds; //Replace
            tempchilds.clear();
        }else{ //Nothing special required
            get<0>(lb)->children.push_back(get<1>(rb)); //Add rightbracket at the end
        }

        //Erase from other one
        for(auto &child: get<0>(rb)->children ){
            if(child!=get<1>(rb)){
                tempchilds.push_back(child);
            }
        }
        get<0>(rb)->children = tempchilds; //Replace rb _root
        tempchilds.clear(); //Free useless memory
    }
}

void LALR::move() {
    unsigned long max = Config::getConfig()->getMaxNesting();
    std::cout << std::endl;
}

ParseTree::~ParseTree() {
    for (const auto& child : children){
        delete child;
    }
}

void ParseTree::clean(const std::vector<std::string> &T, ParseTree* _root, bool &V_root) {
    bool V = false;

    for(long unsigned int i = 0;  i<children.size(); ++i){
        auto child = children[i];
        if(child->symbol=="{" || child->symbol=="}"){ //Stop cleanup if a bracket is reached; we may not modify this
            V=true;
        }else if( std::find(T.begin(), T.end(),child->symbol)==T.end() ){ //We found a variable
            V = true;
            child->clean(T, this, V); //Traverse the child
            //Reloop
            if(!V){ //Reloop in case the child made a change
                i = -1;
            }
        }
    }

    std::vector<ParseTree*> temp;
    if(!V){ //No Variables found: Cleanup this one
        for(auto &it: _root->children){ //Collect the "new children"
            if(it==this){
                for(auto &k: this->children){ //Add children of this
                    temp.push_back(k);
                }
            }else{ //Add the already added child to keep the correct order
                temp.push_back(it);
            }
        }
        V_root = false; //Signal a change
        _root->children=temp; //Replace new children
        this->children.clear(); //Clear old one
        delete this; //Delete old one
    }
}

ParseTree::ParseTree(const vector<ParseTree *> &children, string symbol): children(children),symbol(std::move(symbol)) {}

void ParseTree::findBracket(bool left, std::tuple<ParseTree *, ParseTree *, unsigned long, bool> &data,const std::vector<std::string> &T) { // { _root, bracket , depth }
    long unsigned int i;
    int adjust;
    long unsigned int extreme;
    long unsigned int reset;
    std::string bracket;

    if(left){
        i = 0;
        extreme = children.size();
        adjust = 1;
        reset = -1;
        bracket="{";
    }else{
        i = children.size()-1;
        extreme = 0;
        adjust = -1;
        reset = children.size()-1;
        bracket="}";
    }

    for(long unsigned int j=i ; j!=extreme; j+=adjust){
        if(std::get<3>(data)){ //In case found
            return;
        }

        if(children[j]->symbol==bracket){ //If bracket found
            std::get<0>(data) = this; //Set root
            std::get<1>(data) = children[j]; //Set child
            std::get<3>(data) = true; //Set found
            j = reset;
            return;
        }else if( std::find(T.begin(), T.end(),children[j]->symbol)==T.end() ){ //If Variable: search the left most "{" in this tree
            std::get<2>(data) = ++std::get<2>(data); //Increase depth
            children[j]->findBracket(left,data,T);
        }
    }

    //Nothing found in this subtree; return
}

void ParseTree::sameUpperRoot(ParseTree* lostChild, bool &found) {
    for(long unsigned int i = 0; i<children.size(); ++i){
        if(children[i]==lostChild){
            found = true;
            return;
        }else{
            children[i]->sameUpperRoot(lostChild,found);
            if(found){return;}
        }
    }
}
