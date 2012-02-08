/* NIST Secure Hash Algorithm */
/* heavily modified by Uwe Hollerbach <uh@alumni.caltech edu> */
/* from Peter C. Gutmann's implementation as found in */
/* Applied Cryptography by Bruce Schneier */
/* Further modifications to include the "UNRAVEL" stuff, below */

/* This code is in the public domain */

/* Modifications to dynamically determine endianness by Neil Winton */
/* $Id: sha_func.c,v 1.2 1999/11/19 21:30:07 nwinton Exp nwinton $ */

#include <string.h>
#include "sha.h"

/* UNRAVEL should be fastest & biggest */
/* UNROLL_LOOPS should be just as big, but slightly slower */
/* both undefined should be smallest and slowest */

#define UNRAVEL
/* #define UNROLL_LOOPS */

/* NIST's proposed modification to SHA of 7/11/94 may be */
/* activated by defining USE_MODIFIED_SHA; leave it off for now */
#undef USE_MODIFIED_SHA

/* SHA f()-functions */

#define f1(x,y,z)	((x & y) | (~x & z))
#define f2(x,y,z)	(x ^ y ^ z)
#define f3(x,y,z)	((x & y) | (x & z) | (y & z))
#define f4(x,y,z)	(x ^ y ^ z)

/* SHA constants */

#define CONST1		0x5a827999L
#define CONST2		0x6ed9eba1L
#define CONST3		0x8f1bbcdcL
#define CONST4		0xca62c1d6L

/* 32-bit rotate */

#define ROT32(x,n)	((x << n) | (x >> (32 - n)))

/* the generic case, for when the overall rotation is not unraveled */

#define FG(n)	\
    T = ROT32(A,5) + f##n(B,C,D) + E + *WP++ + CONST##n;	\
    E = D; D = C; C = ROT32(B,30); B = A; A = T

/* specific cases, for when the overall rotation is unraveled */

#define FA(n)	\
    T = ROT32(A,5) + f##n(B,C,D) + E + *WP++ + CONST##n; B = ROT32(B,30)

#define FB(n)	\
    E = ROT32(T,5) + f##n(A,B,C) + D + *WP++ + CONST##n; A = ROT32(A,30)

#define FC(n)	\
    D = ROT32(E,5) + f##n(T,A,B) + C + *WP++ + CONST##n; T = ROT32(T,30)

#define FD(n)	\
    C = ROT32(D,5) + f##n(E,T,A) + B + *WP++ + CONST##n; E = ROT32(E,30)

#define FE(n)	\
    B = ROT32(C,5) + f##n(D,E,T) + A + *WP++ + CONST##n; D = ROT32(D,30)

#define FT(n)	\
    A = ROT32(B,5) + f##n(C,D,E) + T + *WP++ + CONST##n; C = ROT32(C,30)

/* do SHA transformation */

static void sha_transform(SHA_INFO *sha_info)
{
    int i;
    SHA_LONG T, A, B, C, D, E, W[80], *WP;

    for (i = 0; i < 16; ++i) {
	W[i] = sha_info->data[i];
    }
    for (i = 16; i < 80; ++i) {
	W[i] = W[i-3] ^ W[i-8] ^ W[i-14] ^ W[i-16];
#ifdef USE_MODIFIED_SHA
	W[i] = ROT32(W[i], 1);
#endif /* USE_MODIFIED_SHA */
    }
    A = sha_info->digest[0];
    B = sha_info->digest[1];
    C = sha_info->digest[2];
    D = sha_info->digest[3];
    E = sha_info->digest[4];
    WP = W;
#ifdef UNRAVEL
    FA(1); FB(1); FC(1); FD(1); FE(1); FT(1); FA(1); FB(1); FC(1); FD(1);
    FE(1); FT(1); FA(1); FB(1); FC(1); FD(1); FE(1); FT(1); FA(1); FB(1);
    FC(2); FD(2); FE(2); FT(2); FA(2); FB(2); FC(2); FD(2); FE(2); FT(2);
    FA(2); FB(2); FC(2); FD(2); FE(2); FT(2); FA(2); FB(2); FC(2); FD(2);
    FE(3); FT(3); FA(3); FB(3); FC(3); FD(3); FE(3); FT(3); FA(3); FB(3);
    FC(3); FD(3); FE(3); FT(3); FA(3); FB(3); FC(3); FD(3); FE(3); FT(3);
    FA(4); FB(4); FC(4); FD(4); FE(4); FT(4); FA(4); FB(4); FC(4); FD(4);
    FE(4); FT(4); FA(4); FB(4); FC(4); FD(4); FE(4); FT(4); FA(4); FB(4);
    sha_info->digest[0] += E;
    sha_info->digest[1] += T;
    sha_info->digest[2] += A;
    sha_info->digest[3] += B;
    sha_info->digest[4] += C;
#else /* !UNRAVEL */
#ifdef UNROLL_LOOPS
    FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1);
    FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1);
    FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2);
    FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2);
    FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3);
    FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3);
    FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4);
    FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4);
#else /* !UNROLL_LOOPS */
    for (i =  0; i < 20; ++i) { FG(1); }
    for (i = 20; i < 40; ++i) { FG(2); }
    for (i = 40; i < 60; ++i) { FG(3); }
    for (i = 60; i < 80; ++i) { FG(4); }
#endif /* !UNROLL_LOOPS */
    sha_info->digest[0] += A;
    sha_info->digest[1] += B;
    sha_info->digest[2] += C;
    sha_info->digest[3] += D;
    sha_info->digest[4] += E;
#endif /* !UNRAVEL */
}

/* change endianness of data if necessary */

static void maybe_byte_reverse(SHA_LONG *buffer, int count)
{
    static int initialized = 0;
    static int is_little_endian = 0;
    int i;
    SHA_LONG in;

    if (!initialized)
    {
	union {
	    unsigned char bytes[4];
	    SHA_LONG integer;
	} u;

	/*
	** First call -- figure out endianness.
	**
	** In theory we ought to worry about thread safety but in practice
	** even if two threads come in here simultaneously the worst
	** that will happen is that they will both end up figuring out
	** the endianness and will come to the same answer!
	*/

	initialized++;

	u.integer = 0x12345678;

	is_little_endian = (u.bytes[0] == 0x78);
    }

    if (is_little_endian) {
	count /= sizeof(SHA_LONG);
	for (i = 0; i < count; ++i) {
	    in = *buffer;
	    *buffer++ = ((in << 24) & 0xff000000) | ((in <<  8) & 0x00ff0000) |
			((in >>  8) & 0x0000ff00) | ((in >> 24) & 0x000000ff);
	}
    }
}

/* initialize the SHA digest */

void sha_init(SHA_INFO *sha_info)
{
    sha_info->digest[0] = 0x67452301L;
    sha_info->digest[1] = 0xefcdab89L;
    sha_info->digest[2] = 0x98badcfeL;
    sha_info->digest[3] = 0x10325476L;
    sha_info->digest[4] = 0xc3d2e1f0L;
    sha_info->count_lo = 0L;
    sha_info->count_hi = 0L;
    sha_info->local = 0;
}

/* update the SHA digest */

void sha_update(SHA_INFO *sha_info, SHA_BYTE *buffer, int count)
{
    int i;

    if ((sha_info->count_lo + ((SHA_LONG) count << 3)) < sha_info->count_lo) {
	++sha_info->count_hi;
    }
    sha_info->count_lo += (SHA_LONG) count << 3;
    sha_info->count_hi += (SHA_LONG) count >> 29;
    if (sha_info->local) {
	i = SHA_BLOCKSIZE - sha_info->local;
	if (i > count) {
	    i = count;
	}
	memcpy(((SHA_BYTE *) sha_info->data) + sha_info->local, buffer, i);
	count -= i;
	buffer += i;
	sha_info->local += i;
	if (sha_info->local == SHA_BLOCKSIZE) {
	    maybe_byte_reverse(sha_info->data, SHA_BLOCKSIZE);
	    sha_transform(sha_info);
	} else {
	    return;
	}
    }
    while (count >= SHA_BLOCKSIZE) {
	memcpy(sha_info->data, buffer, SHA_BLOCKSIZE);
	buffer += SHA_BLOCKSIZE;
	count -= SHA_BLOCKSIZE;
	maybe_byte_reverse(sha_info->data, SHA_BLOCKSIZE);
	sha_transform(sha_info);
    }
    memcpy(sha_info->data, buffer, count);
    sha_info->local = count;
}

/* finish computing the SHA digest */

void sha_final(SHA_INFO *sha_info)
{
    int count;
    SHA_LONG lo_bit_count, hi_bit_count;

    lo_bit_count = sha_info->count_lo;
    hi_bit_count = sha_info->count_hi;
    count = (int) ((lo_bit_count >> 3) & 0x3f);
    ((SHA_BYTE *) sha_info->data)[count++] = 0x80;
    if (count > SHA_BLOCKSIZE - 8) {
	memset(((SHA_BYTE *) sha_info->data) + count, 0, SHA_BLOCKSIZE - count);
	maybe_byte_reverse(sha_info->data, SHA_BLOCKSIZE);
	sha_transform(sha_info);
	memset((SHA_BYTE *) sha_info->data, 0, SHA_BLOCKSIZE - 8);
    } else {
	memset(((SHA_BYTE *) sha_info->data) + count, 0,
	    SHA_BLOCKSIZE - 8 - count);
    }
    maybe_byte_reverse(sha_info->data, SHA_BLOCKSIZE);
    sha_info->data[14] = hi_bit_count;
    sha_info->data[15] = lo_bit_count;
    sha_transform(sha_info);
}
