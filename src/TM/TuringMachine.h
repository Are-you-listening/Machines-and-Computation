//
// Created by tibov on 02/11/23.
//

#ifndef TOG_TURINGMACHINE_H
#define TOG_TURINGMACHINE_H

#include "Tape.h"
#include "TuringProduction.h"
#include "lib/json.hpp"
#include "TuringTools.h"

#include <vector>
#include <map>
#include <fstream>
#include <set>

using json = nlohmann::json;

class TuringMachine {
public:
    TuringMachine();
    explicit TuringMachine(const string &path);
    ~TuringMachine();

    void load(const string &path);
    void load(json &data);
    void load(const vector<string>& states, const string& start_state, const string& input, int tape_size, const vector<Transition>& productions);
    void load_input(const string& input, int index);
    [[nodiscard]] string getTapeData(unsigned int index) const;
    [[nodiscard]] string exportTapeData(unsigned int index, bool full=false) const;
    void move();

    [[nodiscard]] bool isHalted() const;
    [[nodiscard]] unsigned int getTapeAmount() const;

    [[nodiscard]] const string &getCurrentState() const;
    int getTuringIndex(int i);
    void clear(bool full=false);
    TuringMachine* toSingleTape();
    void makeStorage(int size);
    string getControlStorage();
    map<string, vector<Transition>> getProductions() const;

    void setSingleTape(bool singleTape);

    bool isSingleTape() const;

private:
    vector<Tape*> tapes;
    map<string, TuringProduction*> production_trees;
    set<string> states;
    string start_state;
    string current_state;
    char* storage_in_state;
    int storage_in_state_size;

    bool halted;

    set<int> getUsefullIndexes(const Transition& t);
    set<int> getUsefullIndexesParent(const vector<Transition>& t);

    bool single_tape = false;

    void singleTapeProd(vector<vector<IncompleteTransition>>& new_transitions, const string& k, const vector<Transition>& v, int& counter, const int& mark_track, const int& new_control, const vector<int>& storage_in_state_indexes, const vector<int>& none_moving);

};


#endif //TOG_TURINGMACHINE_H
