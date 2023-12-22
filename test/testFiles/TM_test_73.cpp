#include <vector>

int main() {
    if (true){
        if (true){
            if (true){
                std::vector<int> V = {1, 3, 4, 5, 6};
                for (int &i: V) {
                    i++;
                }
            }

        }

    }

}