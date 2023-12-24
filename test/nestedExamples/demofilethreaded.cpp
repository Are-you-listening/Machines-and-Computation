#include <iostream>
#include <thread>
void A(){
    bool d =false;
    for (int i =0;i<5;++i){
        d =i % 2 ==0;
        if (d){
            std::cout << "Never Nestifier" << std::endl;
        }
    }
}

int main(){
    bool a =true;
    bool b =false;
    if(a){
        bool c =a;
        b =true;
        if (c){
            std::thread a0000000(A);
            a0000000.join();
        }
    }
    else{
        return b;
    }

}