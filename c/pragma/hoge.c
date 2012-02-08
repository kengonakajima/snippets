/* #pragma pack(1) */

#pragma pack( push, aaaa )

#pragma pack(1)
struct aho
{
    int a;
    char b;
    char c;
};

#pragma pack( pop, aaaa )

int main() {
    printf( "%d\n" , sizeof( struct aho ));
}
