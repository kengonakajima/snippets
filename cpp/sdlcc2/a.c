# 5 "obj.y"
# 1 "objtypes.h" 1
typedef struct _vector3d {
    float x,y,z;
} vector3d;
typedef struct _vector2d {
    float x,y;
} vector2d;
typedef struct _vectors3d {
    int v_num;

    vector3d v[1024];
} vectors3d;
typedef struct _vectors2d {
    int v_num;

    vector2d v[1024];
} vectors2d;
typedef struct _rgbacolor {
    float r,g,b,a;
} rgbacolor;
typedef struct _rgbcolor {
    float r,g,b;
} rgbcolor;
typedef struct _faceinds {
    int inds_num;

    int inds[1024];
} faceinds;
typedef struct _face {
    int vinds_num;
    int vinds[4];
} face;
typedef struct _faces {
    int f_num;

    face f[1024];
} faces;

typedef struct _vertex {
    float x,y,z;
} vertex;
typedef struct _vertices {
    int v_num;

    vertex v[1024];
} vertices;

typedef struct _mesh
{

    char name[256];
    vertices v;
    faces f;
} mesh;
typedef struct _meshes
{

    mesh m[1024];
} meshes;
# 6 "obj.y" 2



void yyerror( char *);


extern char *obj_buf;
extern int obj_sz, obj_so_far;
extern int obj_num_lines;
# 21 "obj.y"
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
    mesh meshval;
    meshes meshesval;
} YYSTYPE;
# 1 "/usr/include/stdio.h" 1 3
# 27 "/usr/include/stdio.h" 3
# 1 "/usr/include/features.h" 1 3
# 283 "/usr/include/features.h" 3
# 1 "/usr/include/sys/cdefs.h" 1 3
# 284 "/usr/include/features.h" 2 3
# 311 "/usr/include/features.h" 3
# 1 "/usr/include/gnu/stubs.h" 1 3
# 312 "/usr/include/features.h" 2 3
# 28 "/usr/include/stdio.h" 2 3





# 1 "/usr/lib/gcc-lib/i586-redhat-linux/2.96/include/stddef.h" 1 3
# 199 "/usr/lib/gcc-lib/i586-redhat-linux/2.96/include/stddef.h" 3
typedef unsigned int size_t;
# 34 "/usr/include/stdio.h" 2 3

# 1 "/usr/include/bits/types.h" 1 3
# 26 "/usr/include/bits/types.h" 3
# 1 "/usr/include/features.h" 1 3
# 27 "/usr/include/bits/types.h" 2 3


# 1 "/usr/lib/gcc-lib/i586-redhat-linux/2.96/include/stddef.h" 1 3
# 30 "/usr/include/bits/types.h" 2 3


typedef unsigned char __u_char;
typedef unsigned short __u_short;
typedef unsigned int __u_int;
typedef unsigned long __u_long;

__extension__ typedef unsigned long long int __u_quad_t;
__extension__ typedef long long int __quad_t;
# 49 "/usr/include/bits/types.h" 3
typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
typedef signed int __int32_t;
typedef unsigned int __uint32_t;

__extension__ typedef signed long long int __int64_t;
__extension__ typedef unsigned long long int __uint64_t;

typedef __quad_t *__qaddr_t;

typedef __u_quad_t __dev_t;
typedef __u_int __uid_t;
typedef __u_int __gid_t;
typedef __u_long __ino_t;
typedef __u_int __mode_t;
typedef __u_int __nlink_t;
typedef long int __off_t;
typedef __quad_t __loff_t;
typedef int __pid_t;
typedef int __ssize_t;
typedef __u_long __rlim_t;
typedef __u_quad_t __rlim64_t;
typedef __u_int __id_t;

typedef struct
  {
    int __val[2];
  } __fsid_t;


typedef int __daddr_t;
typedef char *__caddr_t;
typedef long int __time_t;
typedef unsigned int __useconds_t;
typedef long int __suseconds_t;
typedef long int __swblk_t;

typedef long int __clock_t;


typedef int __clockid_t;


typedef int __timer_t;






typedef int __key_t;


typedef unsigned short int __ipc_pid_t;



typedef long int __blksize_t;




typedef long int __blkcnt_t;
typedef __quad_t __blkcnt64_t;


typedef __u_long __fsblkcnt_t;
typedef __u_quad_t __fsblkcnt64_t;


typedef __u_long __fsfilcnt_t;
typedef __u_quad_t __fsfilcnt64_t;


typedef __u_quad_t __ino64_t;


typedef __loff_t __off64_t;


typedef long int __t_scalar_t;
typedef unsigned long int __t_uscalar_t;


typedef int __intptr_t;


typedef unsigned int __socklen_t;




# 1 "/usr/include/bits/pthreadtypes.h" 1 3
# 23 "/usr/include/bits/pthreadtypes.h" 3
# 1 "/usr/include/bits/sched.h" 1 3
# 68 "/usr/include/bits/sched.h" 3
struct __sched_param
  {
    int __sched_priority;
  };
# 24 "/usr/include/bits/pthreadtypes.h" 2 3

typedef int _lt_spinlock_t;


struct _pthread_fastlock
{
  long int __status;
  _lt_spinlock_t __spinlock;

};



typedef struct _pthread_descr_struct *_pthread_descr;





typedef struct __pthread_attr_s
{
  int __detachstate;
  int __schedpolicy;
  struct __sched_param __schedparam;
  int __inheritsched;
  int __scope;
  size_t __guardsize;
  int __stackaddr_set;
  void *__stackaddr;
  size_t __stacksize;
} pthread_attr_t;



typedef struct
{
  struct _pthread_fastlock __c_lock;
  _pthread_descr __c_waiting;
} pthread_cond_t;



typedef struct
{
  int __dummy;
} pthread_condattr_t;


typedef unsigned int pthread_key_t;





typedef struct
{
  int __m_reserved;
  int __m_count;
  _pthread_descr __m_owner;
  int __m_kind;
  struct _pthread_fastlock __m_lock;
} pthread_mutex_t;



typedef struct
{
  int __mutexkind;
} pthread_mutexattr_t;



typedef int pthread_once_t;
# 142 "/usr/include/bits/pthreadtypes.h" 3
typedef unsigned long int pthread_t;
# 144 "/usr/include/bits/types.h" 2 3
# 36 "/usr/include/stdio.h" 2 3
# 44 "/usr/include/stdio.h" 3
typedef struct _IO_FILE FILE;
# 54 "/usr/include/stdio.h" 3
typedef struct _IO_FILE __FILE;
# 64 "/usr/include/stdio.h" 3
# 1 "/usr/include/libio.h" 1 3
# 30 "/usr/include/libio.h" 3
# 1 "/usr/include/_G_config.h" 1 3
# 9 "/usr/include/_G_config.h" 3
# 1 "/usr/include/bits/types.h" 1 3
# 10 "/usr/include/_G_config.h" 2 3




# 1 "/usr/lib/gcc-lib/i586-redhat-linux/2.96/include/stddef.h" 1 3
# 287 "/usr/lib/gcc-lib/i586-redhat-linux/2.96/include/stddef.h" 3
typedef long int wchar_t;
# 312 "/usr/lib/gcc-lib/i586-redhat-linux/2.96/include/stddef.h" 3
typedef unsigned int wint_t;
# 15 "/usr/include/_G_config.h" 2 3
# 24 "/usr/include/_G_config.h" 3
# 1 "/usr/include/wchar.h" 1 3
# 48 "/usr/include/wchar.h" 3
# 1 "/usr/lib/gcc-lib/i586-redhat-linux/2.96/include/stddef.h" 1 3
# 49 "/usr/include/wchar.h" 2 3

# 1 "/usr/include/bits/wchar.h" 1 3
# 51 "/usr/include/wchar.h" 2 3
# 67 "/usr/include/wchar.h" 3
typedef struct
{
  int __count;
  union
  {
    wint_t __wch;
    char __wchb[4];
  } __value;
} __mbstate_t;
# 25 "/usr/include/_G_config.h" 2 3

typedef struct
{
  __off_t __pos;
  __mbstate_t __state;
} _G_fpos_t;
typedef struct
{
  __off64_t __pos;
  __mbstate_t __state;
} _G_fpos64_t;
# 44 "/usr/include/_G_config.h" 3
# 1 "/usr/include/gconv.h" 1 3
# 26 "/usr/include/gconv.h" 3
# 1 "/usr/include/features.h" 1 3
# 27 "/usr/include/gconv.h" 2 3

# 1 "/usr/include/wchar.h" 1 3
# 48 "/usr/include/wchar.h" 3
# 1 "/usr/lib/gcc-lib/i586-redhat-linux/2.96/include/stddef.h" 1 3
# 49 "/usr/include/wchar.h" 2 3

# 1 "/usr/include/bits/wchar.h" 1 3
# 51 "/usr/include/wchar.h" 2 3
# 29 "/usr/include/gconv.h" 2 3

# 1 "/usr/lib/gcc-lib/i586-redhat-linux/2.96/include/stddef.h" 1 3
# 31 "/usr/include/gconv.h" 2 3





enum
{
  __GCONV_OK = 0,
  __GCONV_NOCONV,
  __GCONV_NODB,
  __GCONV_NOMEM,

  __GCONV_EMPTY_INPUT,
  __GCONV_FULL_OUTPUT,
  __GCONV_ILLEGAL_INPUT,
  __GCONV_INCOMPLETE_INPUT,

  __GCONV_ILLEGAL_DESCRIPTOR,
  __GCONV_INTERNAL_ERROR
};



enum
{
  __GCONV_IS_LAST = 0x0001,
  __GCONV_IGNORE_ERRORS = 0x0002
};



struct __gconv_step;
struct __gconv_step_data;
struct __gconv_loaded_object;
struct __gconv_trans_data;



typedef int (*__gconv_fct) (struct __gconv_step *, struct __gconv_step_data *,
                            __const unsigned char **, __const unsigned char *,
                            unsigned char **, size_t *, int, int);


typedef int (*__gconv_init_fct) (struct __gconv_step *);
typedef void (*__gconv_end_fct) (struct __gconv_step *);



typedef int (*__gconv_trans_fct) (struct __gconv_step *,
                                  struct __gconv_step_data *, void *,
                                  __const unsigned char *,
                                  __const unsigned char **,
                                  __const unsigned char *, unsigned char **,
                                  size_t *);


typedef int (*__gconv_trans_context_fct) (void *, __const unsigned char *,
                                          __const unsigned char *,
                                          unsigned char *, unsigned char *);


typedef int (*__gconv_trans_query_fct) (__const char *, __const char ***,
                                        size_t *);


typedef int (*__gconv_trans_init_fct) (void **, const char *);
typedef void (*__gconv_trans_end_fct) (void *);

struct __gconv_trans_data
{

  __gconv_trans_fct __trans_fct;
  __gconv_trans_context_fct __trans_context_fct;
  __gconv_trans_end_fct __trans_end_fct;
  void *__data;
  struct __gconv_trans_data *__next;
};



struct __gconv_step
{
  struct __gconv_loaded_object *__shlib_handle;
  __const char *__modname;

  int __counter;

  char *__from_name;
  char *__to_name;

  __gconv_fct __fct;
  __gconv_init_fct __init_fct;
  __gconv_end_fct __end_fct;



  int __min_needed_from;
  int __max_needed_from;
  int __min_needed_to;
  int __max_needed_to;


  int __stateful;

  void *__data;
};



struct __gconv_step_data
{
  unsigned char *__outbuf;
  unsigned char *__outbufend;



  int __flags;



  int __invocation_counter;



  int __internal_use;

  __mbstate_t *__statep;
  __mbstate_t __state;



  struct __gconv_trans_data *__trans;
};



typedef struct __gconv_info
{
  size_t __nsteps;
  struct __gconv_step *__steps;
  __extension__ struct __gconv_step_data __data [0];
} *__gconv_t;
# 45 "/usr/include/_G_config.h" 2 3
typedef union
{
  struct __gconv_info __cd;
  struct
  {
    struct __gconv_info __cd;
    struct __gconv_step_data __data;
  } __combined;
} _G_iconv_t;

typedef int _G_int16_t __attribute__ ((__mode__ (__HI__)));
typedef int _G_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int _G_uint16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int _G_uint32_t __attribute__ ((__mode__ (__SI__)));
# 31 "/usr/include/libio.h" 2 3
# 51 "/usr/include/libio.h" 3
# 1 "/usr/lib/gcc-lib/i586-redhat-linux/2.96/include/stdarg.h" 1 3
# 43 "/usr/lib/gcc-lib/i586-redhat-linux/2.96/include/stdarg.h" 3
typedef __builtin_va_list __gnuc_va_list;
# 52 "/usr/include/libio.h" 2 3
# 158 "/usr/include/libio.h" 3
struct _IO_jump_t; struct _IO_FILE;
# 168 "/usr/include/libio.h" 3
typedef void _IO_lock_t;





struct _IO_marker {
  struct _IO_marker *_next;
  struct _IO_FILE *_sbuf;



  int _pos;
# 191 "/usr/include/libio.h" 3
};


enum __codecvt_result
{
  __codecvt_ok,
  __codecvt_partial,
  __codecvt_error,
  __codecvt_noconv
};
# 259 "/usr/include/libio.h" 3
struct _IO_FILE {
  int _flags;




  char* _IO_read_ptr;
  char* _IO_read_end;
  char* _IO_read_base;
  char* _IO_write_base;
  char* _IO_write_ptr;
  char* _IO_write_end;
  char* _IO_buf_base;
  char* _IO_buf_end;

  char *_IO_save_base;
  char *_IO_backup_base;
  char *_IO_save_end;

  struct _IO_marker *_markers;

  struct _IO_FILE *_chain;

  int _fileno;
  int _blksize;
  __off_t _old_offset;



  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];



  _IO_lock_t *_lock;
# 303 "/usr/include/libio.h" 3
  __off64_t _offset;





  void *__pad1;
  void *__pad2;

  int _mode;

  char _unused2[15 * sizeof (int) - 2 * sizeof (void *)];

};


typedef struct _IO_FILE _IO_FILE;


struct _IO_FILE_plus;

extern struct _IO_FILE_plus _IO_2_1_stdin_;
extern struct _IO_FILE_plus _IO_2_1_stdout_;
extern struct _IO_FILE_plus _IO_2_1_stderr_;
# 342 "/usr/include/libio.h" 3
typedef __ssize_t __io_read_fn (void *__cookie, char *__buf, size_t __nbytes);







typedef __ssize_t __io_write_fn (void *__cookie, __const char *__buf,
                                 size_t __n);







typedef int __io_seek_fn (void *__cookie, __off64_t *__pos, int __w);


typedef int __io_close_fn (void *__cookie);
# 394 "/usr/include/libio.h" 3
extern int __underflow (_IO_FILE *) ;
extern int __uflow (_IO_FILE *) ;
extern int __overflow (_IO_FILE *, int) ;
extern wint_t __wunderflow (_IO_FILE *) ;
extern wint_t __wuflow (_IO_FILE *) ;
extern wint_t __woverflow (_IO_FILE *, wint_t) ;
# 424 "/usr/include/libio.h" 3
extern int _IO_getc (_IO_FILE *__fp) ;
extern int _IO_putc (int __c, _IO_FILE *__fp) ;
extern int _IO_feof (_IO_FILE *__fp) ;
extern int _IO_ferror (_IO_FILE *__fp) ;

extern int _IO_peekc_locked (_IO_FILE *__fp) ;





extern void _IO_flockfile (_IO_FILE *) ;
extern void _IO_funlockfile (_IO_FILE *) ;
extern int _IO_ftrylockfile (_IO_FILE *) ;
# 454 "/usr/include/libio.h" 3
extern int _IO_vfscanf (_IO_FILE * __restrict, const char * __restrict,
                        __gnuc_va_list, int *__restrict) ;
extern int _IO_vfprintf (_IO_FILE *__restrict, const char *__restrict,
                         __gnuc_va_list) ;
extern __ssize_t _IO_padn (_IO_FILE *, int, __ssize_t) ;
extern size_t _IO_sgetn (_IO_FILE *, void *, size_t) ;

extern __off64_t _IO_seekoff (_IO_FILE *, __off64_t, int, int) ;
extern __off64_t _IO_seekpos (_IO_FILE *, __off64_t, int) ;

extern void _IO_free_backup_area (_IO_FILE *) ;
# 65 "/usr/include/stdio.h" 2 3
# 79 "/usr/include/stdio.h" 3
typedef _G_fpos_t fpos_t;
# 128 "/usr/include/stdio.h" 3
# 1 "/usr/include/bits/stdio_lim.h" 1 3
# 129 "/usr/include/stdio.h" 2 3



extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;






extern int remove (__const char *__filename) ;

extern int rename (__const char *__old, __const char *__new) ;




extern FILE *tmpfile (void) ;
# 160 "/usr/include/stdio.h" 3
extern char *tmpnam (char *__s) ;




extern char *tmpnam_r (char *__s) ;
# 177 "/usr/include/stdio.h" 3
extern char *tempnam (__const char *__dir, __const char *__pfx)
             __attribute__ ((__malloc__));




extern int fclose (FILE *__stream) ;

extern int fflush (FILE *__stream) ;



extern int fflush_unlocked (FILE *__stream) ;
# 200 "/usr/include/stdio.h" 3
extern FILE *fopen (__const char *__restrict __filename,
                    __const char *__restrict __modes) ;

extern FILE *freopen (__const char *__restrict __filename,
                      __const char *__restrict __modes,
                      FILE *__restrict __stream) ;
# 230 "/usr/include/stdio.h" 3
extern FILE *fdopen (int __fd, __const char *__modes) ;
# 253 "/usr/include/stdio.h" 3
extern void setbuf (FILE *__restrict __stream, char *__restrict __buf) ;



extern int setvbuf (FILE *__restrict __stream, char *__restrict __buf,
                    int __modes, size_t __n) ;




extern void setbuffer (FILE *__restrict __stream, char *__restrict __buf,
                       size_t __size) ;


extern void setlinebuf (FILE *__stream) ;




extern int fprintf (FILE *__restrict __stream,
                    __const char *__restrict __format, ...) ;

extern int printf (__const char *__restrict __format, ...) ;

extern int sprintf (char *__restrict __s,
                    __const char *__restrict __format, ...) ;


extern int vfprintf (FILE *__restrict __s, __const char *__restrict __format,
                     __gnuc_va_list __arg) ;

extern int vprintf (__const char *__restrict __format, __gnuc_va_list __arg)
            ;

extern int vsprintf (char *__restrict __s, __const char *__restrict __format,
                     __gnuc_va_list __arg) ;



extern int snprintf (char *__restrict __s, size_t __maxlen,
                     __const char *__restrict __format, ...)
             __attribute__ ((__format__ (__printf__, 3, 4)));

extern int vsnprintf (char *__restrict __s, size_t __maxlen,
                      __const char *__restrict __format, __gnuc_va_list __arg)
             __attribute__ ((__format__ (__printf__, 3, 0)));
# 324 "/usr/include/stdio.h" 3
extern int fscanf (FILE *__restrict __stream,
                   __const char *__restrict __format, ...) ;

extern int scanf (__const char *__restrict __format, ...) ;

extern int sscanf (__const char *__restrict __s,
                   __const char *__restrict __format, ...) ;
# 350 "/usr/include/stdio.h" 3
extern int fgetc (FILE *__stream) ;
extern int getc (FILE *__stream) ;


extern int getchar (void) ;







extern int getc_unlocked (FILE *__stream) ;
extern int getchar_unlocked (void) ;




extern int fgetc_unlocked (FILE *__stream) ;




extern int fputc (int __c, FILE *__stream) ;
extern int putc (int __c, FILE *__stream) ;


extern int putchar (int __c) ;







extern int fputc_unlocked (int __c, FILE *__stream) ;




extern int putc_unlocked (int __c, FILE *__stream) ;
extern int putchar_unlocked (int __c) ;





extern int getw (FILE *__stream) ;


extern int putw (int __w, FILE *__stream) ;




extern char *fgets (char *__restrict __s, int __n, FILE *__restrict __stream)
            ;
# 416 "/usr/include/stdio.h" 3
extern char *gets (char *__s) ;
# 440 "/usr/include/stdio.h" 3
extern int fputs (__const char *__restrict __s, FILE *__restrict __stream)
            ;
# 450 "/usr/include/stdio.h" 3
extern int puts (__const char *__s) ;



extern int ungetc (int __c, FILE *__stream) ;



extern size_t fread (void *__restrict __ptr, size_t __size,
                     size_t __n, FILE *__restrict __stream) ;

extern size_t fwrite (__const void *__restrict __ptr, size_t __size,
                      size_t __n, FILE *__restrict __s) ;



extern size_t fread_unlocked (void *__restrict __ptr, size_t __size,
                              size_t __n, FILE *__restrict __stream) ;
extern size_t fwrite_unlocked (__const void *__restrict __ptr, size_t __size,
                               size_t __n, FILE *__restrict __stream) ;




extern int fseek (FILE *__stream, long int __off, int __whence) ;

extern long int ftell (FILE *__stream) ;

extern void rewind (FILE *__stream) ;
# 494 "/usr/include/stdio.h" 3
extern int fgetpos (FILE *__restrict __stream, fpos_t *__restrict __pos)
            ;

extern int fsetpos (FILE *__stream, __const fpos_t *__pos) ;
# 530 "/usr/include/stdio.h" 3
extern void clearerr (FILE *__stream) ;

extern int feof (FILE *__stream) ;

extern int ferror (FILE *__stream) ;



extern void clearerr_unlocked (FILE *__stream) ;
extern int feof_unlocked (FILE *__stream) ;
extern int ferror_unlocked (FILE *__stream) ;




extern void perror (__const char *__s) ;




extern int sys_nerr;
extern __const char *__const sys_errlist[];
# 561 "/usr/include/stdio.h" 3
extern int fileno (FILE *__stream) ;




extern int fileno_unlocked (FILE *__stream) ;






extern FILE *popen (__const char *__command, __const char *__modes) ;


extern int pclose (FILE *__stream) ;





extern char *ctermid (char *__s) ;
# 608 "/usr/include/stdio.h" 3
extern void flockfile (FILE *__stream) ;



extern int ftrylockfile (FILE *__stream) ;


extern void funlockfile (FILE *__stream) ;
# 40 "obj.y" 2
# 55 "obj.y"
static const char yytranslate[] = { 0,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 16, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2, 14, 2,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 13, 2, 15, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 1, 3, 4, 5, 6,
     7, 8, 9, 10, 11, 12
};
# 138 "obj.y"
static const short yyr1[] = { 0,
    17, 17, 18, 19, 19, 20, 20, 21, 21, 22,
    23, 23, 24, 24, 24, 25, 25, 26, 26, 27,
    28, 29, 29, 30, 31, 32, 33, 33, 34, 35,
    35, 36, 37, 38, 38, 39
};

static const short yyr2[] = { 0,
     2, 1, 13, 1, 3, 8, 10, 1, 3, 6,
     2, 1, 1, 1, 1, 2, 1, 12, 11, 8,
     6, 2, 1, 1, 5, 10, 3, 1, 11, 3,
     1, 6, 7, 3, 1, 4
};

static const short yydefact[] = { 0,
     0, 0, 2, 0, 1, 0, 0, 0, 0, 0,
     8, 0, 0, 0, 0, 0, 9, 0, 0, 0,
     0, 0, 4, 10, 0, 0, 0, 0, 0, 0,
     0, 0, 12, 13, 14, 15, 5, 0, 0, 0,
     0, 3, 11, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 31, 0, 0, 35, 0, 6,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 30, 0, 33, 34, 28, 0, 7, 0, 0,
    36, 0, 0, 0, 0, 0, 0, 17, 27, 32,
     0, 0, 26, 16, 29, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 20, 0, 0, 0, 0, 19, 0, 23,
    24, 21, 0, 18, 22, 0, 0, 25, 0, 0
};

static const short yydefgoto[] = { 2,
     3, 22, 23, 10, 11, 32, 33, 87, 88, 97,
   106, 119, 120, 121, 34, 77, 35, 54, 55, 36,
    57, 58
};

static const short yypact[] = { -2,
    -1, 0,-32768, 2,-32768, 8, -8, 16, 21, 6,
-32768, 31, 32, 16, 23, 25,-32768, 35, 36, 27,
    29, 9,-32768,-32768, 38, 7, 36, 30, 34, 37,
    39, 1,-32768,-32768,-32768,-32768,-32768, 42, 43, 45,
    47,-32768,-32768, 40, 41, 44, 46, 48, 52, 54,
    55, 10, 49, 13,-32768, 50, 14,-32768, 51,-32768,
    56, 61, 62, 52, 64, 57, 54, 65, 53, 59,
    60,-32768, 63,-32768,-32768,-32768, 17,-32768, 70, 36,
-32768, 68, 72, 66, 18, 69, -7,-32768,-32768,-32768,
    71, 74,-32768,-32768,-32768, 67, 73, 78, 79, 75,
    76, 80, 83, 77, 81, 82, 87, 88, 83, 84,
    85, 86,-32768, 89, -6, 90, 92,-32768, -5,-32768,
-32768,-32768, 93,-32768,-32768, 94, 91,-32768, 101,-32768
};

static const short yypgoto[] = {-32768,
   100, -35, 95,-32768, 96,-32768, 97,-32768, -17,-32768,
     3,-32768, -16,-32768,-32768,-32768,-32768,-32768, 98,-32768,
-32768, 58
};





static const short yytable[] = { 129,
    86, 4, 117, 117, 1, 8, 1, 93, 118, 124,
    29, 30, 31, 7, 6, 42, 29, 30, 31, 13,
     9, 14, 26, 60, 27, 61, 63, 66, 64, 67,
    82, 91, 83, 27, 12, 15, 18, 16, 19, 20,
    24, 21, 25, 28, 85, 38, 39, 44, 45, 40,
    46, 41, 47, 52, 49, 48, 53, 50, 56, 51,
    59, 69, 62, 65, 68, 70, 78, 71, 73, 94,
    76, 74, 79, 80, 84, 86, 81, 89, 96, 90,
    98, 92, 100, 101, 104, 95, 99, 105, 102, 103,
   107, 110, 111, 116, 108, 109, 126, 113, 114, 115,
   130, 5, 125, 122, 123, 128, 0, 127, 0, 17,
     0, 112, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 37, 0, 0, 75, 0, 0, 0, 43, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 72
};

static const short yycheck[] = { 0,
     8, 3, 9, 9, 7, 14, 7, 15, 15, 15,
    10, 11, 12, 6, 13, 15, 10, 11, 12, 14,
     5, 16, 14, 14, 16, 16, 14, 14, 16, 16,
    14, 14, 16, 16, 14, 5, 14, 6, 14, 5,
    14, 6, 14, 6, 80, 16, 13, 6, 6, 13,
     6, 13, 6, 6, 14, 16, 5, 14, 5, 14,
     6, 6, 14, 14, 14, 5, 14, 6, 5, 87,
     6, 15, 14, 14, 5, 8, 14, 6, 5, 14,
    14, 13, 5, 5, 5, 15, 14, 5, 14, 14,
    14, 5, 5, 5, 14, 14, 4, 14, 14, 14,
     0, 2, 119, 14, 13, 15, -1, 14, -1, 14,
    -1, 109, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 27, -1, -1, 67, -1, -1, -1, 32, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 64
};
# 3 "/usr/lib/bison.simple"
# 137 "/usr/lib/bison.simple"
int yychar;
YYSTYPE yylval;







int yynerrs;
# 217 "/usr/lib/bison.simple"
# 242 "/usr/lib/bison.simple"
int yyparse (void);



int
yyparse()

{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;
  int yychar1 = 0;

  short yyssa[200];
  YYSTYPE yyvsa[200];

  short *yyss = yyssa;
  YYSTYPE *yyvs = yyvsa;
# 273 "/usr/lib/bison.simple"
  int yystacksize = 200;
  int yyfree_stacks = 0;
# 285 "/usr/lib/bison.simple"
  YYSTYPE yyval;



  int yylen;






  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = -2;






  yyssp = yyss - 1;
  yyvsp = yyvs;







yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {


      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;





      int size = yyssp - yyss + 1;
# 356 "/usr/lib/bison.simple"
      if (yystacksize >= 10000)
        {
          yyerror("parser stack overflow");
          if (yyfree_stacks)
            {
              free (yyss);
              free (yyvs);



            }
          return 2;
        }
      yystacksize *= 2;
      if (yystacksize > 10000)
        yystacksize = 10000;



      yyss = (short *) __builtin_alloca (yystacksize * sizeof (*yyssp));
      __builtin_memcpy((char *)yyss,(char *)yyss1,size * (unsigned int) sizeof (*yyssp));

      yyvs = (YYSTYPE *) __builtin_alloca (yystacksize * sizeof (*yyvsp));
      __builtin_memcpy((char *)yyvs,(char *)yyvs1,size * (unsigned int) sizeof (*yyvsp));
# 388 "/usr/lib/bison.simple"
      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
# 399 "/usr/lib/bison.simple"
      if (yyssp >= yyss + yystacksize - 1)
        goto yyabortlab;
    }






  goto yybackup;
 yybackup:







  yyn = yypact[yystate];
  if (yyn == -32768)
    goto yydefault;






  if (yychar == -2)
    {




      yychar = yylex();
    }



  if (yychar <= 0)
    {
      yychar1 = 0;
      yychar = 0;





    }
  else
    {
      yychar1 = ((unsigned)(yychar) <= 266 ? yytranslate[yychar] : 40);
# 463 "/usr/lib/bison.simple"
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > 162 || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];
# 478 "/usr/lib/bison.simple"
  if (yyn < 0)
    {
      if (yyn == -32768)
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == 130)
    goto yyacceptlab;
# 499 "/usr/lib/bison.simple"
  if (yychar != 0)
    yychar = -2;

  *++yyvsp = yylval;





  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;


yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen];
# 542 "/usr/lib/bison.simple"
  switch (yyn) {

case 1:
# 63 "obj.y"
{ yyval.meshesval.m[0] = yyvsp[0].meshval; ;
    break;}
case 2:
# 64 "obj.y"
{ yyval.meshesval.m[0] = yyvsp[0].meshval; ;
    break;}
case 3:
# 69 "obj.y"
{
# 82 "obj.y"
;
    break;}
case 4:
# 87 "obj.y"
{ yyval.facesval.f[0] = yyvsp[0].faceval; yyval.facesval.f_num = 1; ;
    break;}
case 5:
# 88 "obj.y"
{
    if( yyval.facesval.f_num == 1024 ){
        yyerror( "too many faces\n");
    } else {
        yyval.facesval.f[0] = yyvsp[0].faceval;
        yyval.facesval.f_num ++;
    }
;
    break;}
case 6:
# 98 "obj.y"
{
    fprintf( stderr, "face3: %d,%d,%d\n",yyvsp[-5].ival,yyvsp[-3].ival,yyvsp[-1].ival);
    yyval.faceval.vinds[0] = yyvsp[-5].ival;
    yyval.faceval.vinds[1] = yyvsp[-3].ival;
    yyval.faceval.vinds[2] = yyvsp[-1].ival;
    yyval.faceval.vinds_num = 3;
;
    break;}
case 7:
# 105 "obj.y"
{
    fprintf( stderr, "face4 %d,%d,%d,%d\n",yyvsp[-7].ival,yyvsp[-5].ival,yyvsp[-3].ival,yyvsp[-1].ival);
    yyval.faceval.vinds[0] = yyvsp[-7].ival;
    yyval.faceval.vinds[1] = yyvsp[-5].ival;
    yyval.faceval.vinds[2] = yyvsp[-3].ival;
    yyval.faceval.vinds[3] = yyvsp[-1].ival;
    yyval.faceval.vinds_num = 4;
;
    break;}
case 8:
# 117 "obj.y"
{ yyval.verticesval.v[0] = yyvsp[0].vertexval; yyval.verticesval.v_num = 1;;
    break;}
case 9:
# 118 "obj.y"
{
    if( yyval.verticesval.v_num == 1024 ){
        yyerror( "too many vertex\n");
    } else {
        yyval.verticesval.v[ yyval.verticesval.v_num ] = yyvsp[0].vertexval;
        yyval.verticesval.v_num ++;
    }
;
    break;}
case 10:
# 128 "obj.y"
{
    yyval.vertexval.x = yyvsp[-5].fval;
    yyval.vertexval.y = yyvsp[-3].fval;
    yyval.vertexval.z = yyvsp[-1].fval;
    fprintf( stderr, "vertex:(%f,%f,%f)\n", yyvsp[-5].fval, yyvsp[-3].fval, yyvsp[-1].fval );
;
    break;}
case 20:
# 157 "obj.y"
{
    yyval.rgbaval.r = yyvsp[-7].fval;
    yyval.rgbaval.g = yyvsp[-5].fval;
    yyval.rgbaval.b = yyvsp[-3].fval;
    yyval.rgbaval.a = yyvsp[-1].fval;
;
    break;}
case 21:
# 165 "obj.y"
{ yyval.rgbval.r = yyvsp[-5].fval; yyval.rgbval.g = yyvsp[-3].fval; yyval.rgbval.b = yyvsp[-1].fval; ;
    break;}
case 25:
# 175 "obj.y"
{ fprintf(stderr,"texturefilename_t! name:'%s'\n", yyvsp[-2].sval ); ;
    break;}
case 27:
# 182 "obj.y"
{ yyval.faceindsval.inds[0] = yyvsp[0].ival; ;
    break;}
case 28:
# 183 "obj.y"
{ yyval.faceindsval.inds[0] = yyvsp[0].ival; ;
    break;}
case 30:
# 191 "obj.y"
{ yyval.vecs3val.v[0] = yyvsp[0].vec3val; ;
    break;}
case 31:
# 192 "obj.y"
{ yyval.vecs3val.v[0] = yyvsp[0].vec3val; ;
    break;}
case 32:
# 196 "obj.y"
{ yyval.vec3val.x = yyvsp[-5].fval; yyval.vec3val.y = yyvsp[-3].fval; yyval.vec3val.z = yyvsp[-1].fval; ;
    break;}
case 34:
# 204 "obj.y"
{ yyval.vecs2val.v[0] = yyvsp[0].vec2val; ;
    break;}
case 35:
# 205 "obj.y"
{ yyval.vecs2val.v[0] = yyvsp[0].vec2val; ;
    break;}
case 36:
# 209 "obj.y"
{ yyval.vec2val.x = yyvsp[-3].fval; yyval.vec2val.y = yyvsp[-1].fval; ;
    break;}
}
# 543 "/usr/lib/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
# 561 "/usr/lib/bison.simple"
  *++yyvsp = yyval;
# 585 "/usr/lib/bison.simple"
  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - 17] + *yyssp;
  if (yystate >= 0 && yystate <= 162 && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - 17];

  goto yynewstate;

yyerrlab:

  if (! yyerrstatus)

    {
      ++yynerrs;
# 643 "/usr/lib/bison.simple"
        yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:

  if (yyerrstatus == 3)
    {



      if (yychar == 0)
        goto yyabortlab;






      yychar = -2;
    }




  yyerrstatus = 3;

  goto yyerrhandle;

yyerrdefault:
# 681 "/usr/lib/bison.simple"
yyerrpop:

  if (yyssp == yyss) goto yyabortlab;
  yyvsp--;
  yystate = *--yyssp;
# 701 "/usr/lib/bison.simple"
yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == -32768)
    goto yyerrdefault;

  yyn += 1;
  if (yyn < 0 || yyn > 162 || yycheck[yyn] != 1)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == -32768)
        goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == 130)
    goto yyacceptlab;






  *++yyvsp = yylval;




  yystate = yyn;
  goto yynewstate;

 yyacceptlab:

  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);



    }
  return 0;

 yyabortlab:

  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);



    }
  return 1;
}
# 214 "obj.y"


void yyerror( char *m )
{
    extern int obj_num_lines;
    fprintf(stderr, "YYERROR in %d: %s\n", obj_num_lines, m);
}
