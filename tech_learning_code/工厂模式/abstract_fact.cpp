#include <iostream>
#include <memory>
#include <string>

using namespace std;

class Fruit
{
public:
    virtual void show() = 0;
};
class Apple : public Fruit
{
public:
    virtual void show()
    {
        std::cout << "我是一个苹果" << std::endl;
    }

private:
    std::string _color;
};
class Banana : public Fruit
{
public:
    virtual void show()
    {
        std::cout << "我是一个香蕉" << std::endl;
    }
};
class Animal
{
public:
    virtual void voice() = 0;
};
class Lamp : public Animal
{
public:
    void voice() { std::cout << "咩咩咩\n"; }
};
class Dog : public Animal
{
public:
    void voice() { std::cout << "汪汪汪\n"; }
};

class Factory
{
public:
    virtual std::shared_ptr<Fruit> getFruit(const std::string &name) = 0;
    virtual std::shared_ptr<Animal> getAnimal(const std::string &name) = 0;
};
class FruitFactory : public Factory
{
public:
    virtual std::shared_ptr<Animal> getAnimal(const std::string &name)
    {
        return std::shared_ptr<Animal>();
    }
    virtual std::shared_ptr<Fruit> getFruit(const std::string &name)
    {
        if (name == "苹果")
        {
            return std::make_shared<Apple>();
        }
        else if (name == "香蕉")
        {
            return std::make_shared<Banana>();
        }
        return std::shared_ptr<Fruit>();
    }
};
class AnimalFactory : public Factory
{
public:
    virtual std::shared_ptr<Fruit> getFruit(const std::string &name)
    {
        return std::shared_ptr<Fruit>();
    }
    virtual std::shared_ptr<Animal> getAnimal(const std::string &name)
    {
        if (name == "小羊")
        {
            return std::make_shared<Lamp>();
        }
        else if (name == "小狗")
        {
            return std::make_shared<Dog>();
        }
        return std::shared_ptr<Animal>();
    }
};

class FactoryProducer
{
public:
    static shared_ptr<Factory> getFactory(const string &name)
    {
        if (name == "动物")
        {
            return make_shared<AnimalFactory>();
        }
        else
        {
            return make_shared<FruitFactory>();
        }
    }
};
int main()
{
    shared_ptr<Factory> fruit_factory = FactoryProducer::getFactory("水果");
    shared_ptr<Fruit> fruit = fruit_factory->getFruit("苹果");
    fruit->show();
    fruit = fruit_factory->getFruit("香蕉");
    fruit->show();
    std::shared_ptr<Factory> animal_factory = FactoryProducer::getFactory("动物");
    std::shared_ptr<Animal> animal = animal_factory->getAnimal("小羊");
    animal->voice();
    animal = animal_factory->getAnimal("小狗");
    animal->voice();
    return 0;
}