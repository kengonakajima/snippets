#include <stdio.h>

typedef enum  : unsigned char {
      
} EN;

#pragma pack(1)
struct hoge {
    EN a;
    EN b;
    EN c;
};
int main() {
    EN a;
    struct hoge b;
    printf("a:%d b:%d\n",sizeof(a), sizeof(b));
}
