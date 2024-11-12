/*通过生产不同水果来学习简单工厂模式*/
#include<iostream>
#include<memory>
#include<string>
using namespace std;

class Fruit{
public:
    virtual void show()=0;
};

class Apple:public Fruit
{
public:
    virtual void show() {
        cout<<"appel"<<endl;
    }
};
class Banana:public Fruit
{
 public:
    virtual void show() {
        cout<<"banana"<<endl;
    }
};

class Build{
public:
    static shared_ptr<Fruit> create(const string& fruit)
    {
        if(fruit == "苹果")
            return  make_shared<Apple>();
        else if(fruit == "香蕉")
            return  make_shared<Banana>();
        else
            return shared_ptr<Fruit>();
    }    
};
		
int main()
{
	shared_ptr<Fruit> fruit = Build::create("苹果");
	fruit->show();
	fruit = Build::create("香蕉");
	fruit->show();
	return 0;
}
