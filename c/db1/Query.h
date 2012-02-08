/* Query.h */

#define MaxAnswer 10

#ifdef Q
#define EXT
#else
#define EXT extern
#endif

EXT char * Qrandom();
EXT char *Query(char *);
EXT struct AnswerQueue{
	int ans[MaxAnswer];
	int num;
}aq;
#undef EXT
