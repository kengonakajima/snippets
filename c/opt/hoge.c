#include <stdio.h>
static inline unsigned int get_u32(const char *buf){ return *((unsigned int*)(buf)); }
static inline unsigned short get_u16(const char *buf){ return *((unsigned short*)(buf)); }
static inline unsigned char get_u8(const char *buf){ return *((unsigned char*)(buf)); }
static inline void set_u32(char *buf, unsigned int v){ (*((unsigned int*)(buf))) = (unsigned int)(v) ; }
static inline void set_u16(char *buf, unsigned short v){ (*((unsigned short*)(buf))) = (unsigned short)(v); }
static inline void set_u8( char *buf, unsigned char v){ (*((unsigned char*)(buf))) = (unsigned char)(v); }
static inline float get_f32(const char *buf) { return *((float*)(buf)); }
int main() {
    char buf[100];
    set_u32(buf,12345);
    set_u16(buf,12345);
    for(int i=0;i<100;i++) set_u32(buf+i,i);
    int tot=0;
    for(int i=0;i<100;i++) tot+=get_u8(buf+i);
    printf("%d\n",tot);
}
