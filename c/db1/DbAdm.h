/* Database AdminiStrator */

#define ITEMS 16
#define DATAS 100

#ifdef DA
#define EXT
#else
#define EXT extern
#endif

EXT int ReadDataBase(FILE *);

EXT struct DataBase{
	int d[ITEMS];
	int c;
}dd[DATAS];

#undef EXT
