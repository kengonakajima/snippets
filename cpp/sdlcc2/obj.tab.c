
/*  A Bison parser, made from obj.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	NAME	257
#define	FLOAT	258
#define	INTEGER	259
#define	MESH	260
#define	MATERIAL	261
#define	TEXTUREFILENAME	262
#define	MESHNORMALS	263
#define	MESHTEXTURECOORDS	264
#define	MESHMATERIALLIST	265

#line 1 "obj.y"




#include "objtypes.h"

#include <string.h>
#include <stdlib.h>

#define ERROR_RETURN return 1;
    
void yyerror( char *);    
int yylex(void);


meshes *obj_meshes;
char *obj_buf;
int obj_sz, obj_so_far;
int obj_num_lines = 0;
mesh *obj_cur_mesh;



#line 26 "obj.y"
typedef union {
    float fval;
    int ival;
    char sval[1024];
    rgbacolor rgbaval;
    rgbcolor rgbval;
    faceinds faceindsval;
    vector3d vec3val;
    vector2d vec2val;
    vectors3d vecs3val;
    vectors2d vecs2val;
    face faceval;
    faces facesval;
    vertex vertexval;
    vertices verticesval;
    texturefile texturefileval;
    material materialval;
    materials materialsval;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		129
#define	YYFLAG		-32768
#define	YYNTBASE	17

#define YYTRANSLATE(x) ((unsigned)(x) <= 265 ? yytranslate[x] : 38)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    16,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    15,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    13,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    12,     2,    14,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     3,     5,    19,    22,    24,    26,    28,    30,    34,
    36,    45,    56,    58,    62,    69,    72,    74,    87,    99,
   108,   115,   123,   134,   138,   140,   152,   156,   158,   165,
   173,   177,   179
};

static const short yyrhs[] = {    17,
    18,     0,    18,     0,     6,     3,    12,     5,    13,    23,
    13,     5,    13,    21,    13,    19,    14,     0,    19,    20,
     0,    20,     0,    30,     0,    35,     0,    32,     0,    21,
    15,    22,     0,    22,     0,     5,    13,     5,    15,     5,
    15,     5,    13,     0,     5,    13,     5,    15,     5,    15,
     5,    15,     5,    13,     0,    24,     0,    23,    15,    24,
     0,     4,    13,     4,    13,     4,    13,     0,    25,    26,
     0,    26,     0,     7,    12,    27,    13,     4,    13,    28,
    13,    28,    13,    29,    14,     0,     7,    12,    27,    13,
     4,    13,    28,    13,    28,    13,    14,     0,     4,    13,
     4,    13,     4,    13,     4,    13,     0,     4,    13,     4,
    13,     4,    13,     0,     8,    12,    16,     3,    16,    13,
    14,     0,    11,    12,     5,    13,     5,    13,    31,    13,
    25,    14,     0,    31,    15,     5,     0,     5,     0,     9,
    12,     5,    13,    33,    13,     5,    13,    21,    13,    14,
     0,    33,    15,    34,     0,    34,     0,     4,    13,     4,
    13,     4,    13,     0,    10,    12,     5,    13,    36,    13,
    14,     0,    36,    15,    37,     0,    37,     0,     4,    13,
     4,    13,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    70,    71,    78,    94,    95,    98,    99,   100,   104,   113,
   116,   123,   134,   135,   146,   158,   167,   170,   177,   187,
   195,   198,   204,   212,   221,   225,   242,   251,   255,   263,
   275,   284,   288
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","NAME","FLOAT",
"INTEGER","MESH","MATERIAL","TEXTUREFILENAME","MESHNORMALS","MESHTEXTURECOORDS",
"MESHMATERIALLIST","'{'","';'","'}'","','","'\\\"'","meshes","mesh","mesh_opts",
"mesh_opt","faces","face","vertices","vertex","materials","material","rgbacolor",
"rgbcolor","texturefile","meshmateriallist","faceinds","meshnormals","vectors3d",
"vector3d","meshtexturecoords","vectors2d","vector2d", NULL
};
#endif

static const short yyr1[] = {     0,
    17,    17,    18,    19,    19,    20,    20,    20,    21,    21,
    22,    22,    23,    23,    24,    25,    25,    26,    26,    27,
    28,    29,    30,    31,    31,    32,    33,    33,    34,    35,
    36,    36,    37
};

static const short yyr2[] = {     0,
     2,     1,    13,     2,     1,     1,     1,     1,     3,     1,
     8,    10,     1,     3,     6,     2,     1,    12,    11,     8,
     6,     7,    10,     3,     1,    11,     3,     1,     6,     7,
     3,     1,     4
};

static const short yydefact[] = {     0,
     0,     0,     2,     0,     1,     0,     0,     0,     0,     0,
    13,     0,     0,     0,     0,     0,    14,     0,     0,     0,
     0,     0,    10,    15,     0,     0,     0,     0,     0,     0,
     0,     0,     5,     6,     8,     7,     9,     0,     0,     0,
     0,     3,     4,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    28,     0,     0,    32,     0,    11,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    27,     0,    30,    31,    25,     0,    12,     0,     0,
    33,     0,     0,     0,     0,     0,     0,    17,    24,    29,
     0,     0,    23,    16,    26,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    20,     0,     0,     0,     0,    19,     0,    21,
     0,    18,     0,     0,     0,     0,    22,     0,     0
};

static const short yydefgoto[] = {     2,
     3,    32,    33,    22,    23,    10,    11,    87,    88,    97,
   106,   119,    34,    77,    35,    54,    55,    36,    57,    58
};

static const short yypact[] = {    -1,
    -2,     2,-32768,     8,-32768,    26,     0,    29,    21,    -9,
-32768,    31,    32,    29,    23,    25,-32768,    35,    36,    27,
    30,     6,-32768,-32768,    37,     7,    36,    33,    34,    38,
    39,     1,-32768,-32768,-32768,-32768,-32768,    40,    42,    44,
    47,-32768,-32768,    41,    45,    46,    48,    49,    51,    53,
    55,     9,    50,    10,-32768,    52,    13,-32768,    54,-32768,
    57,    60,    61,    51,    64,    56,    53,    66,    59,    62,
    63,-32768,    65,-32768,-32768,-32768,    14,-32768,    69,    36,
-32768,    67,    72,    68,    17,    70,    -7,-32768,-32768,-32768,
    71,    75,-32768,-32768,-32768,    73,    74,    76,    79,    77,
    78,    80,    84,    81,    82,    83,    85,    88,    84,    86,
    87,    89,-32768,    93,    -5,    90,    92,-32768,    91,-32768,
    28,-32768,    11,    94,    95,    97,-32768,    98,-32768
};

static const short yypgoto[] = {-32768,
    99,-32768,    96,   -27,   100,-32768,   101,-32768,   -18,-32768,
   -16,-32768,-32768,-32768,-32768,-32768,    43,-32768,-32768,    58
};


#define	YYLAST		128


static const short yytable[] = {    86,
     4,   128,   117,    13,     1,    14,    93,     1,   118,    29,
    30,    31,     8,   124,    42,    29,    30,    31,    26,     6,
    27,    60,    63,    61,    64,    66,    82,    67,    83,    91,
     7,    27,     9,    12,    15,    18,    16,    19,    20,    24,
    21,    28,    25,   123,    44,    39,    45,    38,    46,    40,
    41,    47,    85,    52,    53,    48,    56,    49,    50,    59,
    51,    69,    62,    70,    65,    71,    68,    73,    94,    74,
    76,    78,    84,    86,    79,    80,    89,    81,    96,   100,
    90,    92,   101,   104,    95,    98,    99,   105,   110,   102,
   103,   111,   112,   107,   108,   109,   116,   129,   113,   114,
     5,   115,   120,   121,   122,     0,    72,   126,     0,   125,
   127,     0,     0,     0,    17,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    75,     0,    37,    43
};

static const short yycheck[] = {     7,
     3,     0,     8,    13,     6,    15,    14,     6,    14,     9,
    10,    11,    13,     3,    14,     9,    10,    11,    13,    12,
    15,    13,    13,    15,    15,    13,    13,    15,    15,    13,
     5,    15,     4,    13,     4,    13,     5,    13,     4,    13,
     5,     5,    13,    16,     5,    12,     5,    15,     5,    12,
    12,     5,    80,     5,     4,    15,     4,    13,    13,     5,
    13,     5,    13,     4,    13,     5,    13,     4,    87,    14,
     5,    13,     4,     7,    13,    13,     5,    13,     4,     4,
    13,    12,     4,     4,    14,    13,    13,     4,     4,    13,
    13,     4,   109,    13,    13,    13,     4,     0,    13,    13,
     2,    13,    13,    12,    14,    -1,    64,    13,    -1,    16,
    14,    -1,    -1,    -1,    14,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    67,    -1,    27,    32
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/lib/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/lib/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 3:
#line 78 "obj.y"
{
    extern mesh *obj_cur_mesh;
    memcpy( (char*)&obj_cur_mesh->v, & yyvsp[-7].verticesval, sizeof( vertices ));
    memcpy( (char*)&obj_cur_mesh->f, & yyvsp[-3].facesval, sizeof( faces ));
    if( yyvsp[-7].verticesval.v_num != yyvsp[-9].ival ){
        yyerror( "bad v_num\n");
        ERROR_RETURN;
    }
    if( yyvsp[-3].facesval.f_num != yyvsp[-5].ival ){
        yyerror( "bad f_num\n");
        ERROR_RETURN;        
    }
    obj_meshes->m_num ++;
;
    break;}
case 6:
#line 98 "obj.y"
{;;
    break;}
case 7:
#line 99 "obj.y"
{;;
    break;}
case 8:
#line 100 "obj.y"
{;;
    break;}
case 9:
#line 104 "obj.y"
{
    if( yyval.facesval.f_num == MAX_FACES ){
        yyerror( "too many faces\n");
        ERROR_RETURN;                
    } else {
        yyval.facesval.f[yyval.facesval.f_num] = yyvsp[0].faceval;
        yyval.facesval.f_num ++;
    }
;
    break;}
case 10:
#line 113 "obj.y"
{ yyval.facesval.f[0] = yyvsp[0].faceval; yyval.facesval.f_num = 1; ;
    break;}
case 11:
#line 116 "obj.y"
{
/*    fprintf( stderr, "face3: %d,%d,%d\n",$3,$5,$7);*/
    yyval.faceval.vinds[0] = yyvsp[-5].ival;
    yyval.faceval.vinds[1] = yyvsp[-3].ival;
    yyval.faceval.vinds[2] = yyvsp[-1].ival;
    yyval.faceval.vinds_num = 3;
;
    break;}
case 12:
#line 123 "obj.y"
{
/*    fprintf( stderr, "face4 %d,%d,%d,%d\n",$3,$5,$7,$9);*/
    yyval.faceval.vinds[0] = yyvsp[-7].ival;
    yyval.faceval.vinds[1] = yyvsp[-5].ival;
    yyval.faceval.vinds[2] = yyvsp[-3].ival;
    yyval.faceval.vinds[3] = yyvsp[-1].ival;
    yyval.faceval.vinds_num = 4;
;
    break;}
case 13:
#line 134 "obj.y"
{ yyval.verticesval.v[0] = yyvsp[0].vertexval;  yyval.verticesval.v_num = 1;;
    break;}
case 14:
#line 135 "obj.y"
{
    if( yyval.verticesval.v_num == MAX_VERTICES ){
        yyerror( "too many vertex\n");
        ERROR_RETURN;                        
    } else {
        yyval.verticesval.v[ yyval.verticesval.v_num ] = yyvsp[0].vertexval;
        yyval.verticesval.v_num ++;
    }
;
    break;}
case 15:
#line 146 "obj.y"
{
    yyval.vertexval.x = yyvsp[-5].fval;
    yyval.vertexval.y = yyvsp[-3].fval;
    yyval.vertexval.z = yyvsp[-1].fval;
/*    fprintf( stderr, "vertex:(%f,%f,%f)\n", $1, $3, $5 );*/
;
    break;}
case 16:
#line 158 "obj.y"
{
    if( yyval.materialsval.mat_num == MAX_MATERIAL ){
        yyerror( "too many material\n");
        ERROR_RETURN;                                
    } else {
        yyval.materialsval.mat[ yyval.materialsval.mat_num ] = yyvsp[0].materialval;
        yyval.materialsval.mat_num ++;
    }
;
    break;}
case 17:
#line 167 "obj.y"
{ yyval.materialsval.mat[0] = yyvsp[0].materialval; yyval.materialsval.mat_num = 1; ;
    break;}
case 18:
#line 170 "obj.y"
{
    yyval.materialval.facecol = yyvsp[-9].rgbaval;
    yyval.materialval.power = yyvsp[-7].fval;
    yyval.materialval.specular = yyvsp[-5].rgbval;
    yyval.materialval.emissive = yyvsp[-3].rgbval;
    yyval.materialval.texf = yyvsp[-1].texturefileval;
;
    break;}
case 19:
#line 178 "obj.y"
{
    yyval.materialval.facecol = yyvsp[-8].rgbaval;
    yyval.materialval.power = yyvsp[-6].fval;
    yyval.materialval.specular = yyvsp[-4].rgbval;
    yyval.materialval.emissive = yyvsp[-2].rgbval;
    yyval.materialval.texf.filename[0] = 0;
;
    break;}
case 20:
#line 187 "obj.y"
{
    yyval.rgbaval.r = yyvsp[-7].fval;
    yyval.rgbaval.g = yyvsp[-5].fval;
    yyval.rgbaval.b = yyvsp[-3].fval;
    yyval.rgbaval.a = yyvsp[-1].fval;
;
    break;}
case 21:
#line 195 "obj.y"
{ yyval.rgbval.r = yyvsp[-5].fval; yyval.rgbval.g = yyvsp[-3].fval; yyval.rgbval.b = yyvsp[-1].fval; ;
    break;}
case 22:
#line 198 "obj.y"
{
    snprintf( yyval.texturefileval.filename,sizeof( yyval.texturefileval.filename),"%s", yyvsp[-3].sval );
;
    break;}
case 23:
#line 204 "obj.y"
{
    extern mesh *obj_cur_mesh;
    obj_cur_mesh->ml.matlist = yyvsp[-1].materialsval;
    obj_cur_mesh->ml.finds = yyvsp[-3].faceindsval;
;
    break;}
case 24:
#line 212 "obj.y"
{
    if( yyval.faceindsval.inds_num == MAX_FACEINDS ){
        yyerror( "too many faceind\n");
        ERROR_RETURN;                                
    } else {
        yyval.faceindsval.inds[ yyval.faceindsval.inds_num ] = yyvsp[0].ival;
        yyval.faceindsval.inds_num ++;
    }
;
    break;}
case 25:
#line 221 "obj.y"
{ yyval.faceindsval.inds[0] = yyvsp[0].ival; yyval.faceindsval.inds_num = 1; ;
    break;}
case 26:
#line 226 "obj.y"
{
    extern mesh *obj_cur_mesh;
    if( yyvsp[-6].vecs3val.v_num != yyvsp[-8].ival ){
        yyerror( "bad v_num\n");
        ERROR_RETURN;                                
    }
    if( yyvsp[-2].facesval.f_num != yyvsp[-4].ival ){
        yyerror( "bad f_num\n");
        ERROR_RETURN;                                
    }
    obj_cur_mesh->mn.v = yyvsp[-6].vecs3val;
    obj_cur_mesh->mn.f = yyvsp[-2].facesval;
;
    break;}
case 27:
#line 242 "obj.y"
{
    if( yyval.vecs3val.v_num == MAX_VECTORS3D ){
        yyerror( "too many vector3d\n");
        ERROR_RETURN;                                
    } else {
        yyval.vecs3val.v[ yyval.vecs3val.v_num ] = yyvsp[0].vec3val;
        yyval.vecs3val.v_num ++;
    }
;
    break;}
case 28:
#line 251 "obj.y"
{ yyval.vecs3val.v[0] = yyvsp[0].vec3val; yyval.vecs3val.v_num = 1; ;
    break;}
case 29:
#line 255 "obj.y"
{
    yyval.vec3val.x = yyvsp[-5].fval;
    yyval.vec3val.y = yyvsp[-3].fval;
    yyval.vec3val.z = yyvsp[-1].fval;
;
    break;}
case 30:
#line 263 "obj.y"
{
    extern mesh *obj_cur_mesh;
    obj_cur_mesh->mc.coords = yyvsp[-2].vecs2val;
    
    if( yyvsp[-2].vecs2val.v_num != yyvsp[-4].ival ){
        yyerror( "bad coords num\n");
        ERROR_RETURN;                                
    }
;
    break;}
case 31:
#line 275 "obj.y"
{
    if( yyval.vecs2val.v_num == MAX_VECTORS2D ){
        yyerror( "too many vector2d\n");
        ERROR_RETURN;                                
    } else {
        yyval.vecs2val.v[yyval.vecs2val.v_num] = yyvsp[0].vec2val;
        yyval.vecs2val.v_num++;
    }
;
    break;}
case 32:
#line 284 "obj.y"
{ yyval.vecs2val.v[0] = yyvsp[0].vec2val; yyval.vecs2val.v_num = 1; ;
    break;}
case 33:
#line 288 "obj.y"
{ yyval.vec2val.x = yyvsp[-3].fval; yyval.vec2val.y = yyvsp[-1].fval; ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/lib/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 293 "obj.y"



void yyerror( char *m )
{
    extern int obj_num_lines;
    fprintf(stderr, "YYERROR in %d: %s\n", obj_num_lines, m);
}


void obj_meshes_dump( meshes *m )
{
    int j,i;
    fprintf(stderr,"Dumping meshes. m_num=%d\n", m->m_num );
    for(i=0;i<m->m_num;i++){
        for(j=0;j<m->mp[i]->v.v_num;j++){
            fprintf(stderr,"M[%d]V[%d]=%f,%f,%f\n", i,j,
                    m->mp[i]->v.v[j].x,
                    m->mp[i]->v.v[j].y,
                    m->mp[i]->v.v[j].z );
        }
        for(j=0;j<m->mp[i]->f.f_num;j++){
            fprintf(stderr,"M[%d]F[%d]=%d,%d,%d,%d\n", i,j,
                    m->mp[i]->f.f[j].vinds[0],
                    m->mp[i]->f.f[j].vinds[1],
                    m->mp[i]->f.f[j].vinds[2],
                    m->mp[i]->f.f[j].vinds[3]
                    );
        }
        for(j=0;j<m->mp[i]->ml.matlist.mat_num;j++){
            fprintf(stderr,"M[%d]ML.MAT[%d]=(%f,%f,%f,%f),%f,(%f,%f,%f),(%f,%f,%f),%s\n",
                    i,j,
                    m->mp[i]->ml.matlist.mat[j].facecol.r,
                    m->mp[i]->ml.matlist.mat[j].facecol.g,
                    m->mp[i]->ml.matlist.mat[j].facecol.b,
                    m->mp[i]->ml.matlist.mat[j].facecol.a,                    
                    m->mp[i]->ml.matlist.mat[j].power,
                    m->mp[i]->ml.matlist.mat[j].specular.r,
                    m->mp[i]->ml.matlist.mat[j].specular.g,
                    m->mp[i]->ml.matlist.mat[j].specular.b,
                    m->mp[i]->ml.matlist.mat[j].emissive.r,
                    m->mp[i]->ml.matlist.mat[j].emissive.g,
                    m->mp[i]->ml.matlist.mat[j].emissive.b,
                    m->mp[i]->ml.matlist.mat[j].texf.filename );
        }
        for(j=0;j<m->mp[i]->ml.finds.inds_num;j++){
            fprintf(stderr,"M[%d]ML.FINDS[%d]=%d\n",
                    i,j,
                    m->mp[i]->ml.finds.inds[j] );
        }
        for(j=0;j<m->mp[i]->mc.coords.v_num;j++){
            fprintf(stderr,"M[%d]MC.COORDS[%d]=(%f,%f)\n",
                    i,j,
                    m->mp[i]->mc.coords.v[j].x,
                    m->mp[i]->mc.coords.v[j].y );
        }
        for(j=0;j<m->mp[i]->mn.v.v_num;j++){
            fprintf(stderr,"M[%d]MN.V[%d]=(%f,%f,%f)\n",
                    i,j,
                    m->mp[i]->mn.v.v[j].x,
                    m->mp[i]->mn.v.v[j].y,
                    m->mp[i]->mn.v.v[j].z );
                    
        }
        for(j=0;j<m->mp[i]->mn.f.f_num;j++){
            fprintf(stderr,"M[%d]MN.F[%d]=%d(%d,%d,%d,%d)\n",
                    i,j,
                    m->mp[i]->mn.f.f[j].vinds_num,
                    m->mp[i]->mn.f.f[j].vinds[0],
                    m->mp[i]->mn.f.f[j].vinds[1],
                    m->mp[i]->mn.f.f[j].vinds[2],
                    m->mp[i]->mn.f.f[j].vinds[3] );
        }
    }
}


int obj_parse_meshes_in_buf( char *buf, int sz, meshes *m )
{
    int ret;
    
    /* use global vars */
    obj_buf = buf;
    obj_sz = sz;
    obj_so_far = 0;
    obj_meshes = m;
    ret = yyparse();

    if( m->m_num == 0 ){
        return -1;
    } else {
        return 0;
    }
    return 0;
}

