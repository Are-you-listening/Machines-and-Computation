#include <iostream>
#include <list>

typedef std::pair<int, int> Item; // weight, value pairs
typedef std::list<Item> ItemList;
using namespace std;
int knapsack(int capacity, const ItemList& items){
    if (capacity == 0 || items.empty()){
        return 0;
    }

    int highest = 0;

    ItemList lst_2 = items;
    auto l2 = lst_2.front();
    lst_2.pop_front();
    int val2 = knapsack(capacity-l2.first, lst_2);
    if (capacity-l2.first >= 0){
        val2 += l2.second;
    }

    int val3 = knapsack(capacity, lst_2);

    highest = max(val2, val3);




    /*
    int highest = 0;
    for (Item i: items){
        if (capacity-i.first < 0){
            continue;
        }
        ItemList lst_2 = items;
        lst_2.remove(i);
        int val = knapsack(capacity-i.first, lst_2);
        val += i.second;
        if (val > highest){
            highest = val;
        }
    }
     */
    return highest;
}
int main() {
    ItemList il = {{12, 4}, {2, 2}, {1, 1}, {1, 2}, {4, 10}};
    cout << knapsack(15, il) << endl;
    return 0;
}
