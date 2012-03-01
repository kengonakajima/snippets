#include <stdio.h>
#include <time.h>

int main (void) {
    time_t t = time(NULL);
    struct tm tm;
    char str[81];
    
    localtime_r(&t,&tm);
    
    strftime(str,sizeof(str),"%Y-%m-%d %H:%I:%S",&tm);
    
    printf("%s",str);
    
    return 0;
}
