#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <iconv.h>

void convert(char *pin, char *pout, size_t olen, const char *from, const char *to)
{
    iconv_t cd;
    size_t ilen, rlen;

    cd = iconv_open(to, from);
    ilen = strlen(pin);

    rlen = iconv(cd, &pin, &ilen, &pout, &olen);
    if (rlen == -1) {
        printf("%u(%s)\n", (unsigned int)rlen, strerror(errno));
        exit (-1);
    }

    *pout = '\0';
    iconv_close(cd);
}

int main(void)
{
    const int bufsize = 65535;
    char text[bufsize], text2[bufsize];
  
    strcpy(text, "あいうえお");
    convert(text, text2, bufsize, "EUC-JP", "UTF-8");
    printf("%s\n", text2);
}
