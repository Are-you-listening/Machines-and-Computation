//
// Created by watson on 10/26/23.
//

#ifndef TOI_STATE_H
#define TOI_STATE_H

#include <string>
#include <memory>
#include <map>
#include <vector>

using namespace std;

class State {
private:
    const string name;

    map<pair<const string,const string>, vector< pair<weak_ptr<State>,vector<string>>>> delta; // map[{1,Z0}] = { {q,{Z0}},{p,{Z0,X}} }

public:

    explicit State(const string &name);

    [[nodiscard]] const string &getName() const;

    vector<pair<weak_ptr<State>,vector<string>>> To(const string &input, const string &stacktop);

    void AddTransition(const string &input, const string &stacktop, weak_ptr<State> &State, vector<string> &replacement);

    const map<pair<const string, const string>, vector<pair<weak_ptr<State>, vector<string>>>> &getDelta() const;
};


#endif //TOI_STATE_H
