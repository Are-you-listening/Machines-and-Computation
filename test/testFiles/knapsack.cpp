//
// Created by anass on 10-12-2023.
//
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
        unsigned int count2=0;
        for(auto it=items.begin(); current_capacity<capacity&&it!=items.end(); it++, count2++){
            if(count2<count){
                continue;
            }
            if(current_capacity+it->first<=capacity){
                value+=it->second;
                current_capacity+=it->first;
            }
        }
        if(best_value<value){
            best_value=value;
        }
    }
    if(items.size()>1){
        ItemList copy=items;
        copy.pop_front();
        int value2=knapsack(capacity,copy);
        if(best_value<value2){
            best_value=value2;
        }
    }
    return best_value;
}

int main(){
    ItemList items;
    for(unsigned int i=0; i<100; i++){
        items.emplace_back(rand()%1000,rand()%1000);
    }
    std::cout <<knapsack(rand()%1000,items)<<std::endl;
    return 0;
}