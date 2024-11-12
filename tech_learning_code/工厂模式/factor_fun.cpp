// Created Time:    2023-08-13 16:06:37
// Modified Time:   2023-08-13 17:55:29

#include <iostream>
#include<memory>
#include<string>
using namespace std;

class Fruit{
public:
    virtual void show()=0;
};

class Apple:public Fruit{
public:
    virtual void show()
    {
        cout<<"apple"<<endl;
    }
};

class Banana:public Fruit{
public:
    virtual void show()
    {
        cout<<"banana"<<endl;
    }
};

class Factory{
public:
    virtual shared_ptr<Fruit> create() = 0;
};

class AppleFactory:public Factory {
public:
    shared_ptr<Fruit> create() override
    {
        return make_shared<Apple>();
    }
};

class BananaFactory:public Factory {
public:
    shared_ptr<Fruit> create() override
    {
        return make_shared<Banana>();
    }
};
int main(int argc, char *argv[]) {
    shared_ptr<Factory> a1(new AppleFactory());
    shared_ptr<Fruit> a2 = a1->create();
    a2->show();

    a1.reset(new BananaFactory());
    a2= a1->create();
    a2->show();
    return 0;
}

