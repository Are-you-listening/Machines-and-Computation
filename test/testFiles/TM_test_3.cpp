
class Base{
public:
    int kikker();
    int test(){int b = 0;}
private:
    unsigned int a = 0;
};

int Base::kikker(){
    a = 5;
}

int main(){
    Base* b = nullptr;
}