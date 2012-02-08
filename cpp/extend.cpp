class A
{
 public:
    int a;
    
 private:
    int aa;
};

class B : public A
{
    
 public:
    B(){
        a = 5;
    }
    int b;
};


int main()
{
    B *x = new B();

    x->b = 10;
    x->a = 20;
    
}
