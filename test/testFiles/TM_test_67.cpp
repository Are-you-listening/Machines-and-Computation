class Base{
public:
    Base(int a = 0);
private:
    int a;
    void t();
};

Base::Base(int a): a{a} {
    Base::a += 1;
}

void Base::t(){
    for (int i =0; i<10; i++){
        if (a > 0){
            for (int j =0; j<10; j++){
                for (int k =0; k<10; k++){
                    a += 1;
                }
            }
        }else{
            for (int j =0; j<5; j++){
                for (int k =0; k<5; k++){
                    a -= 1;
                }
            }
        }


    }
}