
/** Network Kanji Filter. (Version 1.4)
************************************************************************
************************************************************************
** Copyright (C) 1987, Fujitsu LTD. (Itaru ICHIKAWA)
** 連絡先： （株）富士通研究所　ソフト３研　市川　至 
** （E-Mail Address: ichikawa@flab.fujitsu.co.jp）
**	営利を目的としない限り、このソースのいかなる
**	複写，改変，修正も許諾します。その際には、この部分を残すこと。
**	このプログラムについては特に何の保証もしない、悪しからず。
**	Everyone is permitted to do anything on this program 
**	including copying, modifying, improving 
**	as long as you don't try to make money off it, 
**	or pretend that you wrote it.
**	i.e., the above copyright notice has to appear in all copies.
**	THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE.
***********************************************************************/
static char *CopyRight =
	"Copyright (C) 1987, FUJITSU LTD. (I.Ichikawa)";
/***********************************************************************
**	
**					 Written by I.Ichikawa
**					 Fujitsu Labs.,  JH2FKN/1
**
** USAGE:	nkf [flags] [file] 
**
** Flags:
** b    Output is bufferred 		(DEFAULT)
** u	Output is unbufferred
**
** t	no operation
**
** j,n	Outout code is JIS 7 bit 	(DEFAULT) 
** s,x	Output code is Shift JIS
** e,a	Output code is AT&T JIS
**
** i_	Output Kanji-in is ESC+'$'+ _	(DEFAULT_KI)
** o_   Output Kanji-out is ESC+'('+ _	(DEFAULT_KO)
**
** @J,J	= i@oJ				
** @H,H	= i@oH			
** BJ	= iBoJ			
** BB	= iBoB 	
**
**	New Option
**
** r	{de/en}crypt ROT13/47
**
**/

#include <stdio.h>
#include <sys/types.h>

#define		FALSE	0
#define		TRUE	1

#define		ESC	0x1b
#define		SP	0x20
#define		AT	0x40
#define		SSP	0xa0
#define		DEL	0x7f

#define		HOLD_SIZE	32

#define		DEFAULT_KI	'B'
#define		DEFAULT_KO	'B'
#define		DEFAULT_I	oconv		/* EUC, JIS7 */
/* #define	DEFAULT_I	s_oconv		/* Shift JIS */

#define		SJ0162	0x00e1		/* 01 - 62 ku offset */
#define		SJ6394	0x0161		/* 63 - 94 ku offset */

#ifdef SPEEDUP_HACK
#  define GETC(p)  (!unbuf_f||(p)->_cnt>=1?getc(p):(fflush(stdout),getc(p)))
#else
#  define GETC getc
#endif




/* buffers */
char            stdbuf[BUFSIZ];
short           hold_buf[HOLD_SIZE];
int             hold_count;

/* flags */
int             unbuf_f = FALSE,
                estab_f = FALSE,
                nop_f = FALSE;

/* options */
char            k_in = DEFAULT_KI,
                k_out = DEFAULT_KO;

/* converters */
int             s_iconv ();

int             e_oconv ();
int             j_oconv ();
int             s_oconv ();

unsigned int		rot13 ();
unsigned int		rot47 ();
unsigned int		nops ();

int             (*iconv) ();
unsigned int		(*r1conv) ();
unsigned int		(*r2conv) ();
int             (*oconv) ();

/* Global states */
int             in_kanji = FALSE,
                k_shift = FALSE;




main (argc, argv)
    int             argc;
    char          **argv;
{
    register FILE  *fin;
    register char  *cp;

    oconv = j_oconv;
    r1conv = nops;
    r2conv = nops;
    for (argc--, argv++; (argc > 0) && **argv == '-'; argc--, argv++) {
	cp = *argv;
	while (*cp)
	    switch (*cp++) {
	    case 'b':
		unbuf_f = FALSE;
		continue;
	    case 'u':
		unbuf_f = TRUE;
		continue;
	    case 't':
		nop_f = TRUE;
		continue;
	    case 'j':
	    case 'n':
		oconv = j_oconv;
		continue;
	    case 'e':
	    case 'a':
		oconv = e_oconv;
		continue;
	    case 's':
	    case 'x':
		oconv = s_oconv;
		continue;
	    case 'i':
		k_in = *cp++;
		continue;
	    case 'o':
		k_out = *cp++;
		continue;
	    case '@':
		k_in = '@';
		k_out = *cp++;
		continue;
	    case 'B':
		k_in = 'B';
		k_out = *cp++;
		continue;
	    case 'J':
		k_in = '@';
		k_out = 'J';
		continue;
	    case 'H':
		k_in = '@';
		k_out = 'H';
		continue;
	    case 'r':
		r1conv = rot13;
		r2conv = rot47;
		continue;
	    default:
		/* bogus option but ignored */
		continue;
	    }
    }

#ifndef SPEEDUP_HACK
    if (unbuf_f)
	setbuf (stdout, (char *) NULL);
    else
#endif
	setbuf (stdout, stdbuf);

    if (argc == 0) {
	if (nop_f)
	    noconvert (stdin);
	else
	    convert (stdin);
    }
    else {
	while (argc--) {
	    if ((fin = fopen (*argv++, "r")) == NULL) {
		perror (*--argv);
		return (-1);
	    } else {
		if (nop_f)
		    noconvert (fin);
		else
		    convert (fin);
		fclose (fin);
	    }
	}
    }

    return (0);
}


noconvert (f)
    register FILE  *f;
{
    register int    c;

    while ((c = GETC (f)) != EOF)
	putchar (c);
}






convert (f)
    register FILE  *f;
{
    register int    c1,
                    c2;

    c2 = 0;
    iconv = oconv;

    while ((c1 = GETC (f)) != EOF) {
	if (c2) {
	    /* second byte */
	    if (c2 > DEL) {
		/* in case of 8th bit is on */
		if (!estab_f) {
		    /* in case of not established yet */
		    if (c1 > SSP) {
			/* It is still ambiguious */
			h_conv (f, c2, c1);
			c2 = 0;
			continue;
			/* goto next_word */
		    } else if (c1 < AT) {
			/* ignore bogus code */
			c2 = 0;
			continue;
			/* goto next_word */
		    } else {
			/* established */
			/* it seems to be Shift JIS */
			estab_f = TRUE;
			iconv = s_iconv;
			/* goto send */
		    }
		} else
		    /* in case of already established */
		if (c1 < AT) {
		    /* ignore bogus code */
		    c2 = 0;
		    continue;
		    /* goto next_word */
		} else;
		/* goto send */
	    } else
		/* 7 bit code */
		/* it might be kanji shitfted */
	    if ((c1 == DEL) || (c1 <= SP)) {
		/* ignore bogus code */
		c2 = 0;
		continue;
		/* goto next_word */
	    } else;
	    /* goto send */
	} else {
	    /* first byte */
	    if (c1 > DEL) {
		/* 8 bit code */
		if (!estab_f) {
		    /* not established yet */
		    if (c1 < SSP) {
			/* it seems to be Shift JIS */
			estab_f = TRUE;
			iconv = s_iconv;
		    } else if (c1 < 0xe0) {
			/* it seems to be EUC */
			estab_f = TRUE;
		    } else {
			/* still ambiguious */
		    }
		} else {
		    /* already established */
		}
		c2 = c1;
		continue;
		/* goto next_byte */
	    } else if ((c1 > SP) && (c1 != DEL)) {
		/* in case of Roman characters */
		if (k_shift) {
		    /* in case of Kanji shifted */
		    c2 = c1;
		    continue;
		    /* goto next_byte */
		} else;
		/* goto send */
	    } else if (c1 == ESC) {
		/* in case of ESC */
		if ((c1 = GETC (f)) == EOF) {
		    (*iconv) (0, ESC);
		    break;
		} else if (c1 == '$') {
		    if ((c1 = GETC (f)) == EOF) {
			(*iconv) (0, ESC);
			(*iconv) (0, '$');
			break;
		    } else {
			/* in case of Kanji in ESC sequence */
			k_shift = TRUE;
			continue;
		    }
		} else if (c1 == '(') {
		    if ((c1 = GETC (f)) == EOF) {
			(*iconv) (0, ESC);
			(*iconv) (0, '(');
			break;
		    } else {
			/* in case of Kanji out ESC sequence */
			k_shift = FALSE;
			continue;
		    }
		} else {
		    /* not kanji control */
		    (*iconv) (0, ESC);
		    /* goto send */
		}
	    } else;
	    /* goto send */
	}
	/* send: */
	(*iconv) (c2, c1);
	c2 = 0;
	continue;
	/* goto next_word */
    }

    /* epilogue */
    (*iconv) (EOF, 0);
    return;
}





h_conv (f, c2, c1)
    register FILE  *f;
    register int    c1,
                    c2;
{
    register int    wc;


    /** it must NOT be in the kanji shifte sequence	 */
    /** it must NOT be written in JIS7 			 */
    /** and it must be after 2 byte 8bit code		 */

    hold_count = 0;
    push_hold_buf (c2, c1);
    c2 = 0;

    while ((c1 = GETC (f)) != EOF) {
	if (c2) {
	    /* second byte */
	    if (c1 > DEL) {
		/* then 8th bit is on */
		if (!estab_f) {
		    /* not established */
		    if (c1 > SSP) {
			/* it is still ambiguious yet */
			/* goto send */
		    } else if (c1 < AT) {
			/* ignore bogus code */
			c2 = 0;
			continue;
			/* goto next byte */
		    } else {
			/* now established */
			/* it seems to be Shift JIS */
			estab_f = TRUE;
			iconv = s_iconv;
			/* goto send */
		    }
		} else;
		/* just established */
		/* goto send without checking */
	    } else {
		/* 7 bit code */
		/* it might be bogus code  then ignore */
		c2 = 0;
		continue;
		/* goto next byte */
	    }
	} else {
	    /* First byte */
	    if (c1 > DEL) {
		/* 8th bit is on */
		if (c1 < SSP) {
		    /* it seems to be Shift JIS */
		    estab_f = TRUE;
		    iconv = s_iconv;
		} else if (c1 < 0xe0) {
		    /* it seems to be EUC */
		    estab_f = TRUE;
		} else {
		    /* still ambiguious */
		}
		c2 = c1;
		continue;
		/* goto next_byte */
	    } else;
	    /* 7 bit code , then send without any process */
	    /* goto send */
	}
	/* send: */
	if ((push_hold_buf (c2, c1) == EOF) || estab_f)
	    break;
	c2 = 0;
	continue;
    }

    /** now,
     ** 1) EOF is detected, or
     ** 2) Code is established, or
     ** 3) Buffer is FULL (but last word is pushed)
     **
     ** in 1) and 3) cases, we must use default
     ** Kanji codes, that is DEFAULT_I
     **/

    if (estab_f == FALSE) {
	iconv = DEFAULT_I;
	estab_f = TRUE;
    }
    for (wc = 0; wc < hold_count; wc++) {
	c1 = hold_buf[wc];
	c2 = 0x0ff & (c1 >> 8);
	c1 = c1 & 0x0ff;
	(*iconv) (c2, c1);
    }
    return;
}




push_hold_buf (c2, c1)
    int             c2,
                    c1;
{
    if (hold_count >= HOLD_SIZE)
	return (EOF);
    hold_buf[hold_count++] = ((c2 & 0x0ff) << 8) | (c1 & 0x0ff);
    return ((hold_count >= HOLD_SIZE) ? EOF : hold_count);
}



s_iconv (c2, c1)
    register int    c2,
                    c1;
{
    if ((c2 == EOF) || (c2 == 0)) {
	/* NOP */
    } else {
	c2 &= 0x0ff;
	c1 &= 0x0ff;

	c2 = c2 + c2 - ((c2 <= 0x9f) ? SJ0162 : SJ6394);
	if (c1 < 0x9f)
	    c1 = c1 - ((c1 > DEL) ? SP : 0x1f);
	else {
	    c1 = c1 - 0x7e;
	    c2++;
	}
    }
    (*oconv) (c2, c1);
}


e_oconv (c2, c1)
    register int    c2,
                    c1;
{
    if (c2 == EOF)
	return;
    else if (c2 == 0)
	putchar ((*r1conv) (c1));
    else {
	putchar (((*r2conv) (c2)) | 0x080);
	putchar (((*r2conv) (c1)) | 0x080);
    }
}





s_oconv (c2, c1)
    register int    c2,
                    c1;
{
    if (c2 == EOF)
	return;
    else if (c2 == 0)
	putchar ((*r1conv) (c1));
    else {
	c2 = (*r2conv) (c2);
	c1 = (*r2conv) (c1);
	putchar ((((c2 - 1) >> 1) + ((c2 <= 0x5e) ? 0x71 : 0xb1)));
	putchar ((c1 + ((c2 & 1) ? ((c1 < 0x60) ? 0x1f : 0x20) : 0x7e)));
    }
}

j_oconv (c2, c1)
    register int    c2,
                    c1;
{
    if (c2 == EOF) {
	if (in_kanji) {
	    putchar (ESC);
	    putchar ('(');
	    putchar (k_out);
	}
    } else if (c2 == 0) {
	if (in_kanji) {
	    putchar (ESC);
	    putchar ('(');
	    putchar (k_out);
	    in_kanji = FALSE;
	}
	putchar ((*r1conv)(c1));
    } else {
	if (in_kanji == FALSE) {
	    putchar (ESC);
	    putchar ('$');
	    putchar (k_in);
	    in_kanji = TRUE;
	}
	putchar ((*r2conv)(c2));
	putchar ((*r2conv)(c1));
    }
}


unsigned int	nops (c)
	char	c;
{
	return (c & 0x07f);
}


unsigned int	rot13 (c)
	char	c;
{
	c &= 0x07f;
	if ( c < 'A' ) return (c);
	else if (c <= 'M') return (c + 13);
	else if (c <= 'Z') return (c - 13);
	else if (c < 'a') return (c);
	else if (c <= 'm') return (c + 13);
	else if (c <= 'z') return (c - 13);
	else return (c);
}

unsigned int	rot47 (c)
	char	c;
{
	c &= 0x07f;
	if ( c < '!' ) return (c);
	else if ( c <= 'O' ) return (c + 47);
	else if ( c <= '~' ) return (c - 47);
	else return (c);
}




