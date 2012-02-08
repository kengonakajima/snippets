/* TokenAdm.h */

#define StringLength 15
#define NumberOfToken 100

#ifdef TA
#define EXT
#else
#define EXT extern
#endif

struct TAdm{
  char TokenTable[NumberOfToken][StringLength];
  int ctn;
} tt;

EXT void InitTT(void);
EXT int AddToken(char *);
EXT int GEtNumber(char *);
EXT char *GetChar(int);

#undef EXT


