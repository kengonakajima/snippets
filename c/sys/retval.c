int main()
{
    int r;

    while(1){
        sleep(1);
        r = system("./rand.rb");
        printf("%d\n",r);
    }
    return 0;
}
