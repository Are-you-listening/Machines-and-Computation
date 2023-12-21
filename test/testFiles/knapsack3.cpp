#include <list>
#include <iostream>
typedef std::pair<int, int> Item;
typedef std::list<Item> ItemList;

int knapsack(int capacity, const ItemList &items){
    if (items.empty()){
        return 0;
    } else {
        auto currentItem = items.back();
        ItemList reducedItems;
        reducedItems.assign(items.begin(), --items.end());
        if (capacity < currentItem.first){
            return knapsack(capacity, reducedItems);
        }
        return std::max(
                knapsack(capacity - currentItem.first, reducedItems) + currentItem.second,
                knapsack(capacity, reducedItems)
        );

    }
}

int main(){
    //item: {weight, value}
    ItemList myitems{{12,4}, {2,2}, {1,1}, {1,2}, {4,10}};
    std::cout << knapsack(15, myitems) << std::endl;
    return 0;
}