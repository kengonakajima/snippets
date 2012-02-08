/*
  やりたいこと：　便利関数を serverクラスに追加する helperみたいに。
  
 */
#include <iostream>


class sv 
{
 public:

    void send_ping(){
    }
    
    void recv_ping(){
        send_ping();
    }
    
};


class helper : public sv
{
 public:

    void send_ping_helper(){
        send_ping();
    }
    
};


int main()
{
    sv *s = new sv();
    (

}
