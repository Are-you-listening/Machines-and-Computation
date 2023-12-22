bool sum(int a, int b){
    return a+b;
}

bool min(int a, int b){
    return a-b;
}

void assign(int& a, int b){
    a = b;
}

int main(int n){

    int a1 = 0;
    int a2 = 0;
    for (int i=0; i<n-1; i++){
        if (true){
            int temp;
            assign(temp, sum(a1, a2));
            assign(a1, a2);
            assign(a2, temp);

        }
    }

    return 0;
}