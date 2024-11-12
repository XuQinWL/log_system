#include <iostream>
#include <memory>
#include <string>
using namespace std;

/*抽象电脑类*/
class Computer
{
public:
   void SetDisplay(const string& display)
   {
       display_ = display;
   }
   void setBoard(const std::string& board) { board_ = board; }

   virtual void SetOs() = 0;
   string toString()
   {
       string computer = "Computer:{\n";
       computer += "\tboard=" + board_ + ",\n";
       computer += "\tdisplay=" + display_ + ",\n";
       computer += "\tOs=" + os_ + ",\n";
       computer += "}\n";
       return computer;
   }

protected:
   string board_;
   string display_;
   string os_;
};

/*具体产品类*/
class MacBook : public Computer
{
public:
   virtual void SetOs() override{os_ = "Mac Os x12";}
};

/*抽象建造者类：包含创建一个产品对象的各个部件的抽象接口*/
class Build
{
public:
   virtual void buildBoard(const std::string& board) = 0;
   virtual void BuildDisplay(const std::string& display) = 0;
   virtual void BuildOs() = 0;
   virtual shared_ptr<Computer> build() = 0;
};

/*具体产品的具体建造者类：实现抽象接口，构建和组装各个部件*/
class MacBuild : public Build
{
public:
   MacBuild() : com_(new MacBook()) {}
   virtual void buildBoard(const std::string& board) {
       com_->setBoard(board);
   }
   void BuildDisplay(const std::string& display) override
   {
       com_->SetDisplay(display);
   }
   void BuildOs() override
   {
       com_->SetOs();
   }
   shared_ptr<Computer> build()
   {
       return com_;
   }
private:
   shared_ptr<Computer> com_;
};

/*指挥者类，提供给调用者使用，通过指挥者来构造复杂产品*/
class Director
{
public:
   Director(Build* build) : build_(build) {}
   void Construct(const std::string& board, const string& display)
   {
       build_->buildBoard(board);
       build_->BuildDisplay(display);
       build_->BuildOs();
   }
   shared_ptr<Build> build_;
};
int main()
{
   Build* build = new MacBuild();
   shared_ptr<Director> d(new Director(build));
   d->Construct("英特尔主板", "VOC显示器");
   cout << build->build()->toString();
   return 0;
}
