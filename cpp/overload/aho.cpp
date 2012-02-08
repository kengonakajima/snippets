class x
{
 public:
    int f1(){
        return 2;
    }
    char f1(){
        return 1;
    }
};


int main()
{
    char c;
    int i;
    x xx;
    c = xx.f1();
    i = xx.f1();
}
