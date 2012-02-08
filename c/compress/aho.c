/*



  lifestorm てきすとぷろとこる専用圧縮

  もじれつ -> みじかいばいなり  と
  みじかいばいなり -> もじれつ

  を用意する。


  heher :

  

 */

struct st_node
{
    char ch;
};

#define MAXST 8192
struct st_node stn[MAXST];



void initstn(void)
{

    int i;
    for(i=0;i<256;i++){
        stn[i].ch = i;
    }

    // 256 : end of stream (eof)
           
        
}




int main()
{
    initstn();
    
}



