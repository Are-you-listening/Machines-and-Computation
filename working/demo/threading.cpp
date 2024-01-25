#include <vector>
int main(){
    std::vector<int> V1;
    std::vector<int> V2;
    std::vector<int> V3;
    for(std::vector<int>::iterator it=V1.begin(); it!=V1.end(); it++){
        for(std::vector<int>::iterator it1=V2.begin(); it1!=V2.end(); it1++){
            for(std::vector<int>::iterator it2=V3.begin(); it2!=V3.end(); it2++){
                for(std::vector<int>::iterator it3=V3.begin(); it3!=V3.end(); it3++){}
                it2++;
            }

            int a = 0;
            a += 1;
            it1++;
            V3[0]=0;
        }
    }
    return 0;
}