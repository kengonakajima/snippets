// fastest encoding variation I've come up with (Not really, see comments after the article!)
// copied from
// http://www.experts-exchange.com/Programming/System/Windows__Programming/3_216-Fast-Base64-Encode-and-Decode.html
//

#include <stdio.h>
#include <string.h>
#include <assert.h>






static char Base64Digits[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";



unsigned short Base64Digits8192[ 4096 ];
void SetupTable8192()
{
    int j,k;
    for (j=0; j<64; j++ ) {
        for (k=0; k<64; k++ ) {
            unsigned short w;
            w  = Base64Digits[k] << 8;
            w |= Base64Digits[j]; 
            Base64Digits8192[(j*64)+k]= w;
        }
    }
}
//--------------------------
// or have a pre-built table, something like:
//BYTE Base64Digits8192[ ((64*64)*2)+1 ]=  // 8192+1 
//"AAABACADAEAFAGAHAIAJAKALAMANAOAPAQARASATA ... A6A7A8A9A+A/"
//"BABBBCBDBEBFBGBHBIBJBKBLBMBNBOBPBQBRBSBTB ... B6B7B8B9B+B/"
//...
//"9A9B9C9D9E9F9G9H9I9J9K9L9M9N9O9P9Q9R9S9T9...  5969798999+9/"
//"+A+B+C+D+E+F+G+H+I+J+K+L+M+N+O+P+Q+R+S+T+...  5+6+7+8+9+++/"
//"/A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R/S/T/...  5/6/7/8/9/+//"
//;




int ToBase64Fast( const unsigned char* pSrc, int nLenSrc, char* pDst, int nLenDst )
{
   int nLenOut= ((nLenSrc+2)/3)*4; // 4 out for every 3 in, rounded up
   if ( nLenOut+1 > nLenDst ) {
         return( 0 ); // fail!
   }


#if 1

//-------------- modify the inner loop of the ToBase64 fn to..   
   unsigned short* pwDst= (unsigned short*)pDst;
   while( nLenSrc > 2 ) {
      unsigned int n= pSrc[0];  // xxx1
      n <<= 8;           // xx1x
      n |= pSrc[1];      // xx12  
      n <<= 8;           // x12x
      n |= pSrc[2];      // x123  

      pwDst[0]= Base64Digits8192[ n >> 12 ]; 
      pwDst[1]= Base64Digits8192[ n & 0x00000fff ]; 

      nLenSrc -= 3;
      pwDst += 2;
      pSrc += 3;
   }
   // -------------- end of buffer special handling (see text)   
   pDst= (char*)pwDst;
#else
   
   unsigned int* pdwDst= (unsigned int*)pDst;
   while( nLenSrc > 2 ) {
      unsigned int n= pSrc[0];  // xxx1 // roll the data in...
      n <<= 8;           // xx1x
      n |= pSrc[1];      // xx12  
      n <<= 8;           // x12x
      n |= pSrc[2];      // x123  

      unsigned char m4= n & 0x3f;  n >>= 6;  // roll it out, 6 bits at a time
      unsigned char m3= n & 0x3f;  n >>= 6;
      unsigned char m2= n & 0x3f;  n >>= 6;
      unsigned char m1= n & 0x3f;  

      n  = Base64Digits[m4];  n <<= 8; 
      n |= Base64Digits[m3];  n <<= 8;
      n |= Base64Digits[m2];  n <<= 8;
      n |= Base64Digits[m1]; 

      *pdwDst++ = n;   // write four bytes in one operation
      nLenSrc -= 3;
      pSrc += 3;
   }
   // -------------- end of buffer special handling (see text)   
   pDst= (char*)pdwDst;
#endif
   

   
   if ( nLenSrc > 0 ) {  // some left after last triple
      int n1= (*pSrc & 0xfc) >> 2;
      int n2= (*pSrc & 0x03) << 4;
      if (nLenSrc > 1 ) {  // corrected.  Thanks to jprichey
            pSrc++;
            n2 |= (*pSrc & 0xf0) >> 4;
      }
      *pDst++ = Base64Digits[n1]; // encode at least 2 outputs
      *pDst++ = Base64Digits[n2];
      if (nLenSrc == 2) {  // 2 src bytes left to encode, output xxx=
         int n3= (*pSrc & 0x0f) << 2;
         pSrc++;
         n3 |= (*pSrc & 0xc0) >> 6;
         *pDst++ = Base64Digits[n3];
      }
      if (nLenSrc == 1) {  // 1 src byte left to encode, output xx==
         *pDst++ = '=';
      }
      *pDst++ = '=';
   }
   // *pDst= 0; nLenOut++ // could terminate with NULL, here
   return( nLenOut );
}

int main(int argc, char **argv ) {
    char *orig = "hello world!";
    char *expect = "aGVsbG8gd29ybGQh";
    char outbuf[1024];
    int outlen;
    int i;
    
    SetupTable8192();

    
    for(i=0;i<10000000;i++){
        outlen = ToBase64Fast( orig, strlen(orig), outbuf, sizeof(outbuf) - 1 );
        outbuf[outlen]=0;
        assert( strcmp( expect, outbuf ) == 0 );
    }
    
    printf("outlen:%d out:'%s'\n",outlen, outbuf );

    

    
}



