//
// Created by emill on 20/11/2023.
//

#include "LALR.h"

class emptyElement : public exception{
    [[nodiscard]] const char* what() const noexcept override{
        return "it's 'that' error again (the parseTable is empty for this stack and inputsymbol)";
    }
};

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
    bool tableexists = false;
    if (std::filesystem::exists("parseTablefile.txt")){
        std::cout << "parseTablefile found!" << std::endl;
        tableexists = loadTable();
    }

    if (!tableexists) {
        std::cout << "No parseTablefile found for the current cfg." << std::endl;
        unordered_map<int, map<string, string>> createdTable;
        createStates();

        queue<State *> remaining;
        set<State *> visited;
        remaining.push(I0);
        visited.emplace(I0);

        while (not remaining.empty()) {
            State *currentState = remaining.front();
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


            for (const auto &connection: currentState->_connections) {
                if (visited.find(connection.second) == visited.end()) {
                    remaining.push(connection.second);
                    visited.emplace(connection.second);
                }
            }
        }
        parseTable = createdTable;
        mergeSimilar();
        saveTable();
    }
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

        //cout << "stacksymbol: " << stacksymbol << ", inputsymbol: " << inputsymbol << " --> " << operation << endl;
        if (operation.empty()){
            throw emptyElement();
        }

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
            //std::cout << operation << endl;
            //std::cout << rule->first << "\t-->\t";
            //for (auto c : rule->second){
            //    std::cout << " " << c << " ";
            //}
            //std::cout << "\n\n";
            if (inputsymbol != "$"){
                remaininginputvector.insert(remaininginputvector.begin(), inputtuple);
                remaininginputvector.insert(remaininginputvector.begin(), std::make_tuple(rule->first, "", S));
            } else {
                remaininginputvector.insert(remaininginputvector.begin(), std::make_tuple(rule->first, "", S));

            }

            auto* newparent = new ParseTree;
            newparent->symbol = rule->first;
            if (rule->first == _cfg.getS()){
                _root = newparent;
            }
            for (const string& symbol : rule->second){
                bool found = false;
                set<string> toRemove;
                for(auto it = treetops.begin(); it != treetops.end();it++){
                    if ((*it)->symbol == symbol){
                        ParseTree* temp = *it;
                        newparent->children.push_back(temp);
                        found = true;
                        toRemove.insert(symbol);
                        if (found){
                            break;
                        }
                    }
                }
                for (auto temp : toRemove){
                    auto it = treetops.begin();
                    while(it != treetops.end()){
                        if ((*it)->symbol == temp){
                            treetops.erase(it);
                            break;
                        }
                        it++;
                    }
                }
                if (not found) {
                    auto *newchild = new ParseTree;
                    newchild->symbol = symbol;
                    newparent->children.push_back(newchild);
                }
                s.pop();
            }
            treetops.insert(treetops.begin(), newparent);
        } else {
            s.push(stoi(operation));
        }
    }
    auto inputcopy = input;
    _root->addTokens(inputcopy);
    //vector<tuple<string, string, set<string>>> debugyield;
    //_root->getYield(debugyield);
    //for (auto elem : debugyield){
    //    cout << get<0>(elem) << " ";
    //}
}

void LALR::printTable() {
    for (const auto& rowpair : parseTable){
        cout << "State: " << to_string(rowpair.first) << endl;
        for (const auto& columnpair : rowpair.second){
            cout << "\tSymbol: " << columnpair.first << ", Table element: " << columnpair.second << endl;
        }
    }
}

void LALR::generate() {
    const unsigned long split = Config::getConfig()->getSplitNesting();
    const unsigned long max = Config::getConfig()->getMaxNesting();
    unsigned long count = 0;
    unsigned long index;
    bool found = false;
    string functionName = "A";
    ParseTree* violator = nullptr;
    std::vector<ParseTree*> new_rootKids;

    //IGNORE If-Else nesting? Can the token therefore not be generated? ("{" and "}" ? )
    _root->cleanIncludeTypedefs(new_rootKids); //Collect includes

    _root->matchBrackets(_cfg.getT()); //Format first
    _root->findViolation(max,split,count,index,violator,_cfg.getT(),found); //Check for violations

    while(violator!=nullptr){
        //Find difference: vSet - dSet = result
        std::set<std::string> vSet; //Contains V,I,e
        std::set<std::string> dSet;
        violator->getTokenSet(vSet,dSet);
        std::set<std::string> result;
        std::set_difference(vSet.begin(), vSet.end(), dSet.begin(), dSet.end(), std::inserter(result, result.end()));
        std::set<string> result2;
        for(auto &k: result){
            if(!k.empty()){
                result2.insert(k);
            }
        }
        

        vector<ParseTree *> newKids;

        for (long unsigned int i = 0; i<=index; ++i) { //Pushback firsthalf of kids
            ParseTree *child = violator->children[i];
            newKids.push_back(child);
        }
        //if(newKids!= empty())
        //get<1>(newKids[newKids.size()-1]->token)+="{";

        vector<ParseTree *> tomove;
        for (long unsigned int i = index + 1; i < violator->children.size(); ++i) { //Skip the part for the functionCall
            ParseTree *child = violator->children[i];
            if (get<0>(child->token) == "}") {
                //newKids.push_back(child);
                //get<1>(newKids[newKids.size()-1]->token)+="}";
                index = i;
                break;
            }
            tomove.push_back(child);
        }
        auto* createFrom = new ParseTree(tomove,"", {"","",{}}); //Create a variable in between

        pair<bool,int> fDepth = {false,0};
        pair<bool,int> cbrDepth = {false,0};
        createFrom->checkBRC(fDepth,cbrDepth);
        if(cbrDepth.second>=fDepth.second){ //Replace brackets
            for(auto &node: newKids){
                if(get<0>(node->token)=="{"){ //
                    get<0>(node->token) = "\u1F600";
                    get<1>(node->token) = "\u1F600";
                }else if(get<0>(node->token)=="}" ){
                    get<0>(node->token) = "\u1F976";
                    get<1>(node->token) = "\u1F976";
                }
            }
            goto recheck;
        }

        newKids.push_back(functionCall(function(createFrom,result2,functionName))); //Create the new function in the root and add its functionCall()
        functionName+="A";
        newKids.push_back(violator->children[index]);

        for (long unsigned int i = index + 1; i < violator->children.size(); ++i) { //Pushback rest of the children
            ParseTree *child = violator->children[i];
            newKids.push_back(child);
        }
        violator->children = newKids; //Set children (now with functionCall)

        //Recheck everything
        recheck:
        violator= nullptr;
        count = 0;
        found = false;
        _root->findViolation(max,split,count,index,violator,_cfg.getT(), found); //Check for more violations
    }

    for(auto child: _root->children){ //Readd includes
        new_rootKids.push_back(child);
    }
    _root->children = new_rootKids;

    //Create File
    vector<tuple<string, string, set<string>>> yield;
    _root->getYield(yield);
    ofstream test("output/result.cpp");
    for(auto &k: yield){
        if(get<1>(k)=="\u1F600"){
            get<0>(k)="{";
            get<1>(k)="{";
        }else if(get<1>(k)=="\u1F976"){
            get<0>(k)="}";
            get<1>(k)="}";
        }

        auto str = get<1>(k);
        test << str;
        if(str[str.size()-1]==';' || str[str.size()-1]=='{' || str[str.size()-1]=='}' ||str[str.size()-1]=='>' ){
            test << "\n";
        }
    }
    test.close();
}

ParseTree::ParseTree(const vector<ParseTree *> &children, string symbol): children(children),symbol(std::move(symbol)) {}

ParseTree::~ParseTree() {
    for (const auto& child : children){
        delete child;
    }
}

void ParseTree::findBracket(bool left, std::tuple<ParseTree *, unsigned long, unsigned long, bool> &data,const std::vector<std::string> &Terminals) { // { _root, bracket , depth, found, rootstack }
    long unsigned int i;
    int adjust;
    long unsigned int extreme;
    std::string bracket;

    if(left){
        i = 0;
        extreme = children.size();
        adjust = 1;
        bracket="{";
    }else{
        i = children.size()-1;
        extreme = -1;
        adjust = -1;
        bracket="}";
        if(children.empty()){
            i = 0;
        }
    }

    for(long unsigned int j=i ; j!=extreme; j+=adjust){
        if(std::get<3>(data)){ //In case found
            return;
        }

        if(get<0>(children[j]->token)==bracket){ //If bracket found
            std::get<0>(data) = this; //Set root
            std::get<1>(data) = j; //Set child
            std::get<3>(data) = true; //Set found
            return;
        }else if( std::find(Terminals.begin(), Terminals.end(),get<0>(children[j]->token))==Terminals.end() ){ //If Variable: search the left most "{" in this tree
            std::get<2>(data) = ++std::get<2>(data); //Increase depth
            children[j]->findBracket(left,data,Terminals);
        }
    }

    if(std::get<3>(data)){ //In case found
        return;
    }

    //Nothing found in this subtree; return
    --std::get<2>(data); //Decrease depth
}

ParseTree* LALR::functionCall(const string& code) {
    auto k = new ParseTree({},"D");
    //auto t = code.substr(5,code.size()); //remove "void" from name
    k->token = {"D",code,{}};
    return k;
}

string LALR::function(ParseTree *violator, std::set<std::string> &tokenSet, const string &functionName) const {
    std::vector<ParseTree*> newKids;
    long unsigned int i;
    long unsigned int index;
    std::vector<string> variableNamesFunctionCall;

    //Create The Function
    for(i = 0; i<_root->children.size(); ++i){ //Pushback firsthalf of kids
        ParseTree* child = _root->children[i];

        // here we could also check if the first character is "#" and not just "#include"
        if (get<1>(child->token).substr(0, 8) != "#include" || get<1>(child->token).substr(0, 7) != "typedef" ) { //Create after includes
            index = i;
            break;
        }
        newKids.push_back(child);
    }

    set<string> newvariables;
    for (const string& variable : tokenSet){
        auto lastSpace = variable.find_last_of(' ');
        if (lastSpace == variable.npos){
            // there is no space in the variable --> only name or only type
            cout << "missing variable type or name" << endl;
            break;
        }
        string variableType = variable.substr(0, lastSpace);
        string variableName = variable.substr(lastSpace+1);
        if (variableType.back() == '&' || variableType.back() == '*' || variableName.front() == '&' || variableName.front() == '*'){
            // we don't need to add a & because the variable is already passed by reference, or it is a pointer
            newvariables.emplace(variable);
        } else {
            variableType.push_back('&');
            string newvariable = variableType + " " + variableName;
            newvariables.emplace(newvariable);
        }

        //KARS: Use this for later in functionCall
        variableNamesFunctionCall.push_back(variableName);
    }

    string functiondefinition = "void " + functionName + "(";
    for (const string& variable : newvariables){
        functiondefinition += variable + ",";
    }
    if(functiondefinition.back()!='('){
        functiondefinition.pop_back();  // remove the unnecessary "," we added in the previous for loop
    }
    functiondefinition += ")";

    set<string> emptyset = {};
    tuple<string, string, set<string>> newfunctiontoken;
    get<0>(newfunctiontoken)  = "D";
    get<1>(newfunctiontoken) = functiondefinition;
    get<2>(newfunctiontoken) = emptyset;
    auto functionTree = new ParseTree;
    functionTree->symbol = "D";
    functionTree->token = newfunctiontoken;

    tuple<string, string, set<string>> openbracket;
    get<0>(openbracket) = "{";
    get<1>(openbracket) = "{";
    get<2>(openbracket) = emptyset;
    auto openbracketTree = new ParseTree;
    openbracketTree->symbol = "{";
    openbracketTree->token = openbracket;

    tuple<string, string, set<string>> closingbracket;
    get<0>(closingbracket) = "}";
    get<1>(closingbracket) = "}";
    get<2>(closingbracket) = emptyset;
    auto closingbracketTree = new ParseTree;
    closingbracketTree->symbol = "}";
    closingbracketTree->token = closingbracket;

    newKids.push_back(functionTree);
    newKids.push_back(openbracketTree);
    newKids.push_back(violator);
    newKids.push_back(closingbracketTree);

    for(i = index; i<_root->children.size(); ++i){
        ParseTree* child = _root->children[i];
        newKids.push_back(child);
    }
    _root->children = newKids;

    std::string temp;
    for(auto &k: variableNamesFunctionCall){
        temp+=k+",";
    }
    return functionName+"(" + temp.substr(0,temp.size()-1) + ");"; //Cut of the last comma
}

void ParseTree::findViolation(const unsigned long &max,const unsigned long &split, unsigned long &count, unsigned long &index,ParseTree* &Rviolator,const std::vector<std::string> &Terminals, bool &found) {
    if(found){
        return;
    }

    for(long unsigned int i = 0; i<children.size();++i){
        ParseTree* child = children[i];
        if(get<0>(child->token)=="{") { //Found nesting
            ++count;
            if (count == split) {
                Rviolator = this;
                index = i;
            }

            if( count == max){
                found = true;
                return;
            }

            if(found){
                return;
            }

        }else if(std::find(Terminals.begin(), Terminals.end(),get<0>(child->token))==Terminals.end()){ //Found some V
            child->findViolation(max,split,count,index,Rviolator,Terminals,found); //Search further in the Variable nodes

            if(found){
                return;
            }
            
        }else if(get<0>(child->token)=="}"){ //Didn't reach max but did found matching; should now decrease?
            --count; //Is this right?
            if(!found){
                if(Rviolator!= nullptr && count<split){
                    Rviolator = nullptr;
                    found = true;
                }
            }
        }
    }
    //--count;
    // Reset the count how?
    //Nothing more to handle
}

void ParseTree::matchBrackets(const std::vector<std::string> &Terminals) {
    std::tuple<ParseTree *, unsigned long, unsigned long,bool> lb = {nullptr, NULL, 0, false}; //{ root, child, depth,  }
    std::tuple<ParseTree *, unsigned long, unsigned long,bool> rb = {nullptr, NULL, 0, false}; //{ root, child, depth,  }

    //Find left bracket
    this->findBracket(true,lb,Terminals);

    //In case not found: return
    if(!get<3>(lb)){
        return;
    }

    ParseTree* LBroot = std::get<0>(lb); //Initialise root
    ParseTree* V;

    //Find RB
    for(unsigned long i = std::get<1>(lb)+1; i<LBroot->children.size(); ++i){
        ParseTree* child = LBroot->children[i];
        if(std::find(Terminals.begin(), Terminals.end(),get<0>(child->token))==Terminals.end()){ //Found a first variable
            V = child;
            child->findBracket(false,rb,Terminals); //Find in here for the right bracket
            break;
        }
    }
    ParseTree* RBroot = std::get<0>(rb); //Initialise root
    ParseTree* RB = RBroot->children[std::get<1>(rb)];

    //Set new children
    vector<ParseTree*> newKids;
    for(auto child: LBroot->children){ //Set new children of LBroot
        newKids.push_back(child);
        if(child==V){
            newKids.push_back(RB);
        }
    }
    LBroot->children=newKids;
    newKids.clear();

    for(auto child: RBroot->children){ //Set new children of RBroot
        if(child!=RB){
            newKids.push_back(child);
        }
    }
    RBroot->children = newKids;
    newKids.clear();

    //Go recursively in other kiddos
    for(unsigned long i = std::get<1>(lb)+1; i<LBroot->children.size(); ++i){ //Start from child after LB
        ParseTree* child = LBroot->children[i];
        if(child!=RB){ //Go recursively
            child->matchBrackets(Terminals);
        }
    }
}

ParseTree* ParseTree::findRoot(ParseTree *&child,const std::vector<std::string> &Terminals) {
    for(auto kid : children){
        if(child==kid){
            return this;
        }else if( std::find(Terminals.begin(), Terminals.end(),get<0>(kid->token))==Terminals.end() ){ //Found a variable
            auto result = kid->findRoot(child,Terminals);
            if(result!= nullptr){
                return result; //Found!
            }
        }
    }
    //Nothing found
    return nullptr;
}

void LALR::saveTable() {
    std::ofstream outFile("parseTablefile.txt");
    if (outFile.is_open()){
        // first output the cfg to the file, so we can check what parseTable is in the file
        outFile << _cfg.createString();
        outFile << "CFGend" << endl;
        for (const auto& row : parseTable) {
            outFile << row.first << std::endl;
            for (const auto& valuepair : row.second){
                outFile << valuepair.first << " " << valuepair.second << std::endl;
            }
            outFile << "rowend" << std::endl;
        }
        outFile.close();
    }
}

bool LALR::loadTable() {
    std::ifstream inFile("parseTablefile.txt");
    if (inFile.is_open()){
        string currentcfg = _cfg.createString();
        stringstream filecfg;
        while(!inFile.eof()){
            string line;
            std::getline(inFile, line);
            if (line.empty() || line == "CFGend"){
                break;
            }
            filecfg << line << std::endl;
        }
        string result = filecfg.str();
        if (currentcfg != filecfg.str()){
            std::filesystem::remove("parseTablefile.txt");
            return false;
        }
        while(!inFile.eof()){
            string line;
            std::getline(inFile, line);

            if (line.empty()){
                break;
            }

            int key = stoi(line);       // oh boy here we go again

            while(true){
                std::getline(inFile, line);

                if (line.empty() || line == "rowend"){
                    break;
                }

                string inputsymbol;
                string operation;
                std::istringstream linestream(line);
                linestream >> inputsymbol >> operation;
                parseTable[key][inputsymbol] = operation;
            }
        }
        inFile.close();
        return true;
    } else {
        std::cout << "couldn't open parseTable file" << std::endl;
        return false;
    }
}

void ParseTree::addTokens(vector<tuple<string, string, set<string>>> &tokens) {
    if (children.empty()){
        token = tokens.front();
        if (not tokens.empty()) {
            tokens.erase(tokens.begin());
        }
    } else {
        for (auto child : children){
            child->addTokens(tokens);
        }
    }
}

void ParseTree::getYield(vector<tuple<string, string, set<string>>> &yield) {
    if (children.empty()){
        yield.push_back(token);
    } else {
        for (auto child : children){
            child->getYield(yield);
        }
    }
}

void ParseTree::getTokenSet(set<std::string> &vSet, std::set<std::string> &dSet) const {
    for(auto &child: children){
        if(get<0>(child->token)=="V" || get<0>(child->token)=="I" || get<0>(child->token)=="e" ){
            vSet.insert( get<2>(child->token).begin(), get<2>(child->token).end() );
        }else if(get<0>(child->token)=="D" || get<0>(child->token)=="F"){
            dSet.insert(get<2>(child->token).begin(), get<2>(child->token).end() );
        }
        child->getTokenSet(vSet,dSet); //Go recursively for every child
    }
}

void ParseTree::cleanIncludeTypedefs(std::vector<ParseTree*> &newKids) {
    std::vector<unsigned long> indices;
    std::vector<ParseTree*> tempKids;

    for(unsigned long i = 0; i<this->children.size(); ++i){
        auto child = children[i];
        if(get<1>(child->token).find("#include")!=std::string::npos || get<1>(child->token).find("typedef")!=std::string::npos){
            newKids.push_back(child);
        }else{
            tempKids.push_back(child);
            child->cleanIncludeTypedefs(newKids); //Go recursively
        }
    }
    children=tempKids;
}

void ParseTree::checkBRC(pair<bool,int> &fDepth , pair<bool,int> &cbrDepth) {
    for(auto &child: children){
        if(cbrDepth.first && fDepth.first){ //Both found, return, else go to through every child
            return; //This statement can make the proces shorter
        }

        if(get<1>(child->token).find("continue")!=std::string::npos || get<1>(child->token).find("break")!=std::string::npos || get<1>(child->token).find("return")!=std::string::npos  ){
            cbrDepth.first=true;
        }else if(get<0>(child->token)=="F" ){ //Don't count I or e, we only want to check if the cbr is in a forloop!
            fDepth.first=true;
        }else{
            //if(!get<0>(child->token).empty()){ //Don't count empty nodes
                if(!cbrDepth.first){ //In case not yet found
                    --cbrDepth.second;
                }
                if(!fDepth.first){
                    --fDepth.second;
                }
            //}
            child->checkBRC(fDepth,cbrDepth);
        }
    }
}

ParseTree::ParseTree(const vector<ParseTree *> &children, const string &symbol,
                     const tuple<string, string, set<string>> &token): children(children),symbol(symbol),token(token) {

}


