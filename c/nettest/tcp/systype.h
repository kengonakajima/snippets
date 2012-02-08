/*
   	システムの種類を決定する

*/

#ifdef unix

#define UNIX 1

#ifdef vax

#define VAX 1
#define BSD 1
#else
#ifdef pyr
#define PYRAMID 1
#define BSD 1
#else
#ifdef mc68k
#define UNIXPC 1
#define SYS5 1
#else
#ifdef i386
#define IBMPC 1
#define SYS5 1
#else
#ifdef accel

#define CELERITY 1
#define BSD 1
#else
unixのタイプが決定できません。
#endif
#endif
#endif
#endif
#endif

#endif	/* if unix */

#ifdef M_XENIX
#define UNIX 1
#define XENIX 1
#define SYS5 1
#define IBMPC 1
#endif

#ifdef MSDOS
#define IBMPC 1
#define MICROSOFT 1
#endif


#ifdef SYS5

#define rindex strrchr
#define index  strchr
#define u_char 	unchar
#define u_short ushort
#define u_int	uint
#define u_long	ulong
#endif

#ifdef MICROSOFT
#define rindex	strrchr
#define index	strchr
#define u_char 	unchar
#define u_short ushort
#define u_int	uint
#define u_long	ulong
#endif



