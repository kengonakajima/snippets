
void hoge()
{
        printf( "%s\n", __func__  );        
}
int main(){
        hoge();
        printf( "%s\n", __func__  );
}
