
#include <iostream>
#include <list>
#include <random>

typedef std::pair<int, int> Item; // weight, value pairs
typedef std::list<Item> ItemList;

int knapsack(int capacity, const ItemList& items){
    int best_value=0;
    for(unsigned count=0; count<items.size()-1; count++){
        int value=0;
        int current_capacity=0;
        unsigned int count2=count;
        for(std::list<std::pair<int, int>>::const_iterator it=items.begin(); current_capacity<capacity&&it!=items.end(); it++, count2++){
            if(current_capacity+it->first<=capacity){
                value+=it->second;
                current_capacity+=it->first;
            }
        }
        if(best_value<value){
            best_value=value;
        }
    }
    return best_value;
}
