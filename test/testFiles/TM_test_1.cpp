
class Base{
public:
    Base();
private:
    unsigned int a = 0;
};

Base::Base() {
    a = 5;
}

int main(){
    Base* b = new Base;

    delete b;

    return 0;
}