#include <iostream>

int main(){
    bool a = true;
    bool b = false;
    if(a){
        bool c = a;
        b = true;
        if (c){
            bool d = false;
            for (int i = 0; i<5; ++i){
                d = i % 2 == 0;
                if (d){
                    std::cout << "Never Nestifier" << std::endl;
                }
            }
        }
    }else{
        return b;
    }

}
