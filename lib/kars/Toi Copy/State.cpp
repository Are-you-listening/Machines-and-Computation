//
// Created by watson on 10/26/23.
//

#include "State.h"

State::State(const string &name) : name(name) {}

const string &State::getName() const {
    return name;
}

vector<pair<weak_ptr<State>,vector<string>>> State::To(const string &input, const string &stacktop) {
    auto ptr = delta.find({input,stacktop});
    if(ptr!=delta.end()){
        return ptr->second;
    }
    return {};
}

void State::AddTransition(const string &input, const string &stacktop, weak_ptr<State> &State,vector<string> &replacement) {
    auto ptr = delta.find({input,stacktop});
    if(ptr!=delta.end()){
        ptr->second.push_back(make_pair(State,replacement));
    }else{
        delta[{input,stacktop}]={make_pair(State,replacement)};
    }
}

const map<pair<const string, const string>, vector<pair<weak_ptr<State>, vector<string>>>> &State::getDelta() const {
    return delta;
}
