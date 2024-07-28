# 0 "/home/kalen/src/A/src/A.c"
# 1 "/home/kalen/src/A/build//"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "/home/kalen/src/A/src/A.c"
# 1 "/home/kalen/src/A/./src/A.h" 1



# 1 "/usr/include/assert.h" 1 3 4
# 35 "/usr/include/assert.h" 3 4
# 1 "/usr/include/features.h" 1 3 4
# 392 "/usr/include/features.h" 3 4
# 1 "/usr/include/features-time64.h" 1 3 4
# 20 "/usr/include/features-time64.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 21 "/usr/include/features-time64.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/timesize.h" 1 3 4
# 19 "/usr/include/x86_64-linux-gnu/bits/timesize.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 20 "/usr/include/x86_64-linux-gnu/bits/timesize.h" 2 3 4
# 22 "/usr/include/features-time64.h" 2 3 4
# 393 "/usr/include/features.h" 2 3 4
# 486 "/usr/include/features.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 1 3 4
# 559 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 560 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/long-double.h" 1 3 4
# 561 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 2 3 4
# 487 "/usr/include/features.h" 2 3 4
# 510 "/usr/include/features.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 1 3 4
# 10 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/gnu/stubs-64.h" 1 3 4
# 11 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 2 3 4
# 511 "/usr/include/features.h" 2 3 4
# 36 "/usr/include/assert.h" 2 3 4
# 66 "/usr/include/assert.h" 3 4




# 69 "/usr/include/assert.h" 3 4
extern void __assert_fail (const char *__assertion, const char *__file,
      unsigned int __line, const char *__function)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));


extern void __assert_perror_fail (int __errnum, const char *__file,
      unsigned int __line, const char *__function)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));




extern void __assert (const char *__assertion, const char *__file, int __line)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));



# 5 "/home/kalen/src/A/./src/A.h" 2
# 1 "/usr/lib/gcc/x86_64-linux-gnu/11/include/stddef.h" 1 3 4
# 143 "/usr/lib/gcc/x86_64-linux-gnu/11/include/stddef.h" 3 4
typedef long int ptrdiff_t;
# 209 "/usr/lib/gcc/x86_64-linux-gnu/11/include/stddef.h" 3 4
typedef long unsigned int size_t;
# 321 "/usr/lib/gcc/x86_64-linux-gnu/11/include/stddef.h" 3 4
typedef int wchar_t;
# 415 "/usr/lib/gcc/x86_64-linux-gnu/11/include/stddef.h" 3 4
typedef struct {
  long long __max_align_ll __attribute__((__aligned__(__alignof__(long long))));
  long double __max_align_ld __attribute__((__aligned__(__alignof__(long double))));
# 426 "/usr/lib/gcc/x86_64-linux-gnu/11/include/stddef.h" 3 4
} max_align_t;
# 6 "/home/kalen/src/A/./src/A.h" 2
# 1 "/usr/include/stdio.h" 1 3 4
# 27 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/libc-header-start.h" 1 3 4
# 28 "/usr/include/stdio.h" 2 3 4





# 1 "/usr/lib/gcc/x86_64-linux-gnu/11/include/stddef.h" 1 3 4
# 34 "/usr/include/stdio.h" 2 3 4


# 1 "/usr/lib/gcc/x86_64-linux-gnu/11/include/stdarg.h" 1 3 4
# 40 "/usr/lib/gcc/x86_64-linux-gnu/11/include/stdarg.h" 3 4
typedef __builtin_va_list __gnuc_va_list;
# 37 "/usr/include/stdio.h" 2 3 4

# 1 "/usr/include/x86_64-linux-gnu/bits/types.h" 1 3 4
# 27 "/usr/include/x86_64-linux-gnu/bits/types.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 28 "/usr/include/x86_64-linux-gnu/bits/types.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/timesize.h" 1 3 4
# 19 "/usr/include/x86_64-linux-gnu/bits/timesize.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 20 "/usr/include/x86_64-linux-gnu/bits/timesize.h" 2 3 4
# 29 "/usr/include/x86_64-linux-gnu/bits/types.h" 2 3 4


typedef unsigned char __u_char;
typedef unsigned short int __u_short;
typedef unsigned int __u_int;
typedef unsigned long int __u_long;


typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
typedef signed int __int32_t;
typedef unsigned int __uint32_t;

typedef signed long int __int64_t;
typedef unsigned long int __uint64_t;






typedef __int8_t __int_least8_t;
typedef __uint8_t __uint_least8_t;
typedef __int16_t __int_least16_t;
typedef __uint16_t __uint_least16_t;
typedef __int32_t __int_least32_t;
typedef __uint32_t __uint_least32_t;
typedef __int64_t __int_least64_t;
typedef __uint64_t __uint_least64_t;



typedef long int __quad_t;
typedef unsigned long int __u_quad_t;







typedef long int __intmax_t;
typedef unsigned long int __uintmax_t;
# 141 "/usr/include/x86_64-linux-gnu/bits/types.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/typesizes.h" 1 3 4
# 142 "/usr/include/x86_64-linux-gnu/bits/types.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/time64.h" 1 3 4
# 143 "/usr/include/x86_64-linux-gnu/bits/types.h" 2 3 4


typedef unsigned long int __dev_t;
typedef unsigned int __uid_t;
typedef unsigned int __gid_t;
typedef unsigned long int __ino_t;
typedef unsigned long int __ino64_t;
typedef unsigned int __mode_t;
typedef unsigned long int __nlink_t;
typedef long int __off_t;
typedef long int __off64_t;
typedef int __pid_t;
typedef struct { int __val[2]; } __fsid_t;
typedef long int __clock_t;
typedef unsigned long int __rlim_t;
typedef unsigned long int __rlim64_t;
typedef unsigned int __id_t;
typedef long int __time_t;
typedef unsigned int __useconds_t;
typedef long int __suseconds_t;
typedef long int __suseconds64_t;

typedef int __daddr_t;
typedef int __key_t;


typedef int __clockid_t;


typedef void * __timer_t;


typedef long int __blksize_t;




typedef long int __blkcnt_t;
typedef long int __blkcnt64_t;


typedef unsigned long int __fsblkcnt_t;
typedef unsigned long int __fsblkcnt64_t;


typedef unsigned long int __fsfilcnt_t;
typedef unsigned long int __fsfilcnt64_t;


typedef long int __fsword_t;

typedef long int __ssize_t;


typedef long int __syscall_slong_t;

typedef unsigned long int __syscall_ulong_t;



typedef __off64_t __loff_t;
typedef char *__caddr_t;


typedef long int __intptr_t;


typedef unsigned int __socklen_t;




typedef int __sig_atomic_t;
# 39 "/usr/include/stdio.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/types/__fpos_t.h" 1 3 4




# 1 "/usr/include/x86_64-linux-gnu/bits/types/__mbstate_t.h" 1 3 4
# 13 "/usr/include/x86_64-linux-gnu/bits/types/__mbstate_t.h" 3 4
typedef struct
{
  int __count;
  union
  {
    unsigned int __wch;
    char __wchb[4];
  } __value;
} __mbstate_t;
# 6 "/usr/include/x86_64-linux-gnu/bits/types/__fpos_t.h" 2 3 4




typedef struct _G_fpos_t
{
  __off_t __pos;
  __mbstate_t __state;
} __fpos_t;
# 40 "/usr/include/stdio.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/types/__fpos64_t.h" 1 3 4
# 10 "/usr/include/x86_64-linux-gnu/bits/types/__fpos64_t.h" 3 4
typedef struct _G_fpos64_t
{
  __off64_t __pos;
  __mbstate_t __state;
} __fpos64_t;
# 41 "/usr/include/stdio.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/types/__FILE.h" 1 3 4



struct _IO_FILE;
typedef struct _IO_FILE __FILE;
# 42 "/usr/include/stdio.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/types/FILE.h" 1 3 4



struct _IO_FILE;


typedef struct _IO_FILE FILE;
# 43 "/usr/include/stdio.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/types/struct_FILE.h" 1 3 4
# 35 "/usr/include/x86_64-linux-gnu/bits/types/struct_FILE.h" 3 4
struct _IO_FILE;
struct _IO_marker;
struct _IO_codecvt;
struct _IO_wide_data;




typedef void _IO_lock_t;





struct _IO_FILE
{
  int _flags;


  char *_IO_read_ptr;
  char *_IO_read_end;
  char *_IO_read_base;
  char *_IO_write_base;
  char *_IO_write_ptr;
  char *_IO_write_end;
  char *_IO_buf_base;
  char *_IO_buf_end;


  char *_IO_save_base;
  char *_IO_backup_base;
  char *_IO_save_end;

  struct _IO_marker *_markers;

  struct _IO_FILE *_chain;

  int _fileno;
  int _flags2;
  __off_t _old_offset;


  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];

  _IO_lock_t *_lock;







  __off64_t _offset;

  struct _IO_codecvt *_codecvt;
  struct _IO_wide_data *_wide_data;
  struct _IO_FILE *_freeres_list;
  void *_freeres_buf;
  size_t __pad5;
  int _mode;

  char _unused2[15 * sizeof (int) - 4 * sizeof (void *) - sizeof (size_t)];
};
# 44 "/usr/include/stdio.h" 2 3 4
# 52 "/usr/include/stdio.h" 3 4
typedef __gnuc_va_list va_list;
# 63 "/usr/include/stdio.h" 3 4
typedef __off_t off_t;
# 77 "/usr/include/stdio.h" 3 4
typedef __ssize_t ssize_t;






typedef __fpos_t fpos_t;
# 133 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/stdio_lim.h" 1 3 4
# 134 "/usr/include/stdio.h" 2 3 4
# 143 "/usr/include/stdio.h" 3 4
extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;






extern int remove (const char *__filename) __attribute__ ((__nothrow__ , __leaf__));

extern int rename (const char *__old, const char *__new) __attribute__ ((__nothrow__ , __leaf__));



extern int renameat (int __oldfd, const char *__old, int __newfd,
       const char *__new) __attribute__ ((__nothrow__ , __leaf__));
# 178 "/usr/include/stdio.h" 3 4
extern int fclose (FILE *__stream);
# 188 "/usr/include/stdio.h" 3 4
extern FILE *tmpfile (void)
  __attribute__ ((__malloc__)) __attribute__ ((__malloc__ (fclose, 1))) ;
# 205 "/usr/include/stdio.h" 3 4
extern char *tmpnam (char[20]) __attribute__ ((__nothrow__ , __leaf__)) ;




extern char *tmpnam_r (char __s[20]) __attribute__ ((__nothrow__ , __leaf__)) ;
# 222 "/usr/include/stdio.h" 3 4
extern char *tempnam (const char *__dir, const char *__pfx)
   __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) __attribute__ ((__malloc__ (__builtin_free, 1)));






extern int fflush (FILE *__stream);
# 239 "/usr/include/stdio.h" 3 4
extern int fflush_unlocked (FILE *__stream);
# 258 "/usr/include/stdio.h" 3 4
extern FILE *fopen (const char *__restrict __filename,
      const char *__restrict __modes)
  __attribute__ ((__malloc__)) __attribute__ ((__malloc__ (fclose, 1))) ;




extern FILE *freopen (const char *__restrict __filename,
        const char *__restrict __modes,
        FILE *__restrict __stream) ;
# 293 "/usr/include/stdio.h" 3 4
extern FILE *fdopen (int __fd, const char *__modes) __attribute__ ((__nothrow__ , __leaf__))
  __attribute__ ((__malloc__)) __attribute__ ((__malloc__ (fclose, 1))) ;
# 308 "/usr/include/stdio.h" 3 4
extern FILE *fmemopen (void *__s, size_t __len, const char *__modes)
  __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) __attribute__ ((__malloc__ (fclose, 1))) ;




extern FILE *open_memstream (char **__bufloc, size_t *__sizeloc) __attribute__ ((__nothrow__ , __leaf__))
  __attribute__ ((__malloc__)) __attribute__ ((__malloc__ (fclose, 1))) ;
# 328 "/usr/include/stdio.h" 3 4
extern void setbuf (FILE *__restrict __stream, char *__restrict __buf) __attribute__ ((__nothrow__ , __leaf__));



extern int setvbuf (FILE *__restrict __stream, char *__restrict __buf,
      int __modes, size_t __n) __attribute__ ((__nothrow__ , __leaf__));




extern void setbuffer (FILE *__restrict __stream, char *__restrict __buf,
         size_t __size) __attribute__ ((__nothrow__ , __leaf__));


extern void setlinebuf (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__));







extern int fprintf (FILE *__restrict __stream,
      const char *__restrict __format, ...);




extern int printf (const char *__restrict __format, ...);

extern int sprintf (char *__restrict __s,
      const char *__restrict __format, ...) __attribute__ ((__nothrow__));





extern int vfprintf (FILE *__restrict __s, const char *__restrict __format,
       __gnuc_va_list __arg);




extern int vprintf (const char *__restrict __format, __gnuc_va_list __arg);

extern int vsprintf (char *__restrict __s, const char *__restrict __format,
       __gnuc_va_list __arg) __attribute__ ((__nothrow__));



extern int snprintf (char *__restrict __s, size_t __maxlen,
       const char *__restrict __format, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 3, 4)));

extern int vsnprintf (char *__restrict __s, size_t __maxlen,
        const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 3, 0)));
# 403 "/usr/include/stdio.h" 3 4
extern int vdprintf (int __fd, const char *__restrict __fmt,
       __gnuc_va_list __arg)
     __attribute__ ((__format__ (__printf__, 2, 0)));
extern int dprintf (int __fd, const char *__restrict __fmt, ...)
     __attribute__ ((__format__ (__printf__, 2, 3)));







extern int fscanf (FILE *__restrict __stream,
     const char *__restrict __format, ...) ;




extern int scanf (const char *__restrict __format, ...) ;

extern int sscanf (const char *__restrict __s,
     const char *__restrict __format, ...) __attribute__ ((__nothrow__ , __leaf__));





# 1 "/usr/include/x86_64-linux-gnu/bits/floatn.h" 1 3 4
# 119 "/usr/include/x86_64-linux-gnu/bits/floatn.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/floatn-common.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/bits/floatn-common.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/long-double.h" 1 3 4
# 25 "/usr/include/x86_64-linux-gnu/bits/floatn-common.h" 2 3 4
# 120 "/usr/include/x86_64-linux-gnu/bits/floatn.h" 2 3 4
# 431 "/usr/include/stdio.h" 2 3 4



extern int fscanf (FILE *__restrict __stream, const char *__restrict __format, ...) __asm__ ("" "__isoc99_fscanf")

                               ;
extern int scanf (const char *__restrict __format, ...) __asm__ ("" "__isoc99_scanf")
                              ;
extern int sscanf (const char *__restrict __s, const char *__restrict __format, ...) __asm__ ("" "__isoc99_sscanf") __attribute__ ((__nothrow__ , __leaf__))

                      ;
# 459 "/usr/include/stdio.h" 3 4
extern int vfscanf (FILE *__restrict __s, const char *__restrict __format,
      __gnuc_va_list __arg)
     __attribute__ ((__format__ (__scanf__, 2, 0))) ;





extern int vscanf (const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__format__ (__scanf__, 1, 0))) ;


extern int vsscanf (const char *__restrict __s,
      const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__format__ (__scanf__, 2, 0)));





extern int vfscanf (FILE *__restrict __s, const char *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vfscanf")



     __attribute__ ((__format__ (__scanf__, 2, 0))) ;
extern int vscanf (const char *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vscanf")

     __attribute__ ((__format__ (__scanf__, 1, 0))) ;
extern int vsscanf (const char *__restrict __s, const char *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vsscanf") __attribute__ ((__nothrow__ , __leaf__))



     __attribute__ ((__format__ (__scanf__, 2, 0)));
# 513 "/usr/include/stdio.h" 3 4
extern int fgetc (FILE *__stream);
extern int getc (FILE *__stream);





extern int getchar (void);






extern int getc_unlocked (FILE *__stream);
extern int getchar_unlocked (void);
# 538 "/usr/include/stdio.h" 3 4
extern int fgetc_unlocked (FILE *__stream);
# 549 "/usr/include/stdio.h" 3 4
extern int fputc (int __c, FILE *__stream);
extern int putc (int __c, FILE *__stream);





extern int putchar (int __c);
# 565 "/usr/include/stdio.h" 3 4
extern int fputc_unlocked (int __c, FILE *__stream);







extern int putc_unlocked (int __c, FILE *__stream);
extern int putchar_unlocked (int __c);






extern int getw (FILE *__stream);


extern int putw (int __w, FILE *__stream);







extern char *fgets (char *__restrict __s, int __n, FILE *__restrict __stream)
     __attribute__ ((__access__ (__write_only__, 1, 2)));
# 632 "/usr/include/stdio.h" 3 4
extern __ssize_t __getdelim (char **__restrict __lineptr,
                             size_t *__restrict __n, int __delimiter,
                             FILE *__restrict __stream) ;
extern __ssize_t getdelim (char **__restrict __lineptr,
                           size_t *__restrict __n, int __delimiter,
                           FILE *__restrict __stream) ;







extern __ssize_t getline (char **__restrict __lineptr,
                          size_t *__restrict __n,
                          FILE *__restrict __stream) ;







extern int fputs (const char *__restrict __s, FILE *__restrict __stream);





extern int puts (const char *__s);






extern int ungetc (int __c, FILE *__stream);






extern size_t fread (void *__restrict __ptr, size_t __size,
       size_t __n, FILE *__restrict __stream) ;




extern size_t fwrite (const void *__restrict __ptr, size_t __size,
        size_t __n, FILE *__restrict __s);
# 702 "/usr/include/stdio.h" 3 4
extern size_t fread_unlocked (void *__restrict __ptr, size_t __size,
         size_t __n, FILE *__restrict __stream) ;
extern size_t fwrite_unlocked (const void *__restrict __ptr, size_t __size,
          size_t __n, FILE *__restrict __stream);







extern int fseek (FILE *__stream, long int __off, int __whence);




extern long int ftell (FILE *__stream) ;




extern void rewind (FILE *__stream);
# 736 "/usr/include/stdio.h" 3 4
extern int fseeko (FILE *__stream, __off_t __off, int __whence);




extern __off_t ftello (FILE *__stream) ;
# 760 "/usr/include/stdio.h" 3 4
extern int fgetpos (FILE *__restrict __stream, fpos_t *__restrict __pos);




extern int fsetpos (FILE *__stream, const fpos_t *__pos);
# 786 "/usr/include/stdio.h" 3 4
extern void clearerr (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__));

extern int feof (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;

extern int ferror (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;



extern void clearerr_unlocked (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__));
extern int feof_unlocked (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;
extern int ferror_unlocked (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;







extern void perror (const char *__s);




extern int fileno (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;




extern int fileno_unlocked (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;
# 823 "/usr/include/stdio.h" 3 4
extern int pclose (FILE *__stream);





extern FILE *popen (const char *__command, const char *__modes)
  __attribute__ ((__malloc__)) __attribute__ ((__malloc__ (pclose, 1))) ;






extern char *ctermid (char *__s) __attribute__ ((__nothrow__ , __leaf__))
  __attribute__ ((__access__ (__write_only__, 1)));
# 867 "/usr/include/stdio.h" 3 4
extern void flockfile (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__));



extern int ftrylockfile (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;


extern void funlockfile (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__));
# 885 "/usr/include/stdio.h" 3 4
extern int __uflow (FILE *);
extern int __overflow (FILE *, int);
# 902 "/usr/include/stdio.h" 3 4

# 7 "/home/kalen/src/A/./src/A.h" 2
# 1 "/usr/include/stdlib.h" 1 3 4
# 26 "/usr/include/stdlib.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/libc-header-start.h" 1 3 4
# 27 "/usr/include/stdlib.h" 2 3 4





# 1 "/usr/lib/gcc/x86_64-linux-gnu/11/include/stddef.h" 1 3 4
# 33 "/usr/include/stdlib.h" 2 3 4







# 1 "/usr/include/x86_64-linux-gnu/bits/waitflags.h" 1 3 4
# 41 "/usr/include/stdlib.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/waitstatus.h" 1 3 4
# 42 "/usr/include/stdlib.h" 2 3 4
# 59 "/usr/include/stdlib.h" 3 4
typedef struct
  {
    int quot;
    int rem;
  } div_t;



typedef struct
  {
    long int quot;
    long int rem;
  } ldiv_t;





__extension__ typedef struct
  {
    long long int quot;
    long long int rem;
  } lldiv_t;
# 98 "/usr/include/stdlib.h" 3 4
extern size_t __ctype_get_mb_cur_max (void) __attribute__ ((__nothrow__ , __leaf__)) ;



extern double atof (const char *__nptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;

extern int atoi (const char *__nptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;

extern long int atol (const char *__nptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;



__extension__ extern long long int atoll (const char *__nptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;



extern double strtod (const char *__restrict __nptr,
        char **__restrict __endptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));



extern float strtof (const char *__restrict __nptr,
       char **__restrict __endptr) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

extern long double strtold (const char *__restrict __nptr,
       char **__restrict __endptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
# 177 "/usr/include/stdlib.h" 3 4
extern long int strtol (const char *__restrict __nptr,
   char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

extern unsigned long int strtoul (const char *__restrict __nptr,
      char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));



__extension__
extern long long int strtoq (const char *__restrict __nptr,
        char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

__extension__
extern unsigned long long int strtouq (const char *__restrict __nptr,
           char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));




__extension__
extern long long int strtoll (const char *__restrict __nptr,
         char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

__extension__
extern unsigned long long int strtoull (const char *__restrict __nptr,
     char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
# 386 "/usr/include/stdlib.h" 3 4
extern char *l64a (long int __n) __attribute__ ((__nothrow__ , __leaf__)) ;


extern long int a64l (const char *__s)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;




# 1 "/usr/include/x86_64-linux-gnu/sys/types.h" 1 3 4
# 27 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4






typedef __u_char u_char;
typedef __u_short u_short;
typedef __u_int u_int;
typedef __u_long u_long;
typedef __quad_t quad_t;
typedef __u_quad_t u_quad_t;
typedef __fsid_t fsid_t;


typedef __loff_t loff_t;




typedef __ino_t ino_t;
# 59 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
typedef __dev_t dev_t;




typedef __gid_t gid_t;




typedef __mode_t mode_t;




typedef __nlink_t nlink_t;




typedef __uid_t uid_t;
# 97 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
typedef __pid_t pid_t;





typedef __id_t id_t;
# 114 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;





typedef __key_t key_t;




# 1 "/usr/include/x86_64-linux-gnu/bits/types/clock_t.h" 1 3 4






typedef __clock_t clock_t;
# 127 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4

# 1 "/usr/include/x86_64-linux-gnu/bits/types/clockid_t.h" 1 3 4






typedef __clockid_t clockid_t;
# 129 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/types/time_t.h" 1 3 4
# 10 "/usr/include/x86_64-linux-gnu/bits/types/time_t.h" 3 4
typedef __time_t time_t;
# 130 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/types/timer_t.h" 1 3 4






typedef __timer_t timer_t;
# 131 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4
# 144 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
# 1 "/usr/lib/gcc/x86_64-linux-gnu/11/include/stddef.h" 1 3 4
# 145 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4



typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;




# 1 "/usr/include/x86_64-linux-gnu/bits/stdint-intn.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/bits/stdint-intn.h" 3 4
typedef __int8_t int8_t;
typedef __int16_t int16_t;
typedef __int32_t int32_t;
typedef __int64_t int64_t;
# 156 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4


typedef __uint8_t u_int8_t;
typedef __uint16_t u_int16_t;
typedef __uint32_t u_int32_t;
typedef __uint64_t u_int64_t;


typedef int register_t __attribute__ ((__mode__ (__word__)));
# 176 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
# 1 "/usr/include/endian.h" 1 3 4
# 24 "/usr/include/endian.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/endian.h" 1 3 4
# 35 "/usr/include/x86_64-linux-gnu/bits/endian.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/endianness.h" 1 3 4
# 36 "/usr/include/x86_64-linux-gnu/bits/endian.h" 2 3 4
# 25 "/usr/include/endian.h" 2 3 4
# 35 "/usr/include/endian.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/byteswap.h" 1 3 4
# 33 "/usr/include/x86_64-linux-gnu/bits/byteswap.h" 3 4
static __inline __uint16_t
__bswap_16 (__uint16_t __bsx)
{

  return __builtin_bswap16 (__bsx);



}






static __inline __uint32_t
__bswap_32 (__uint32_t __bsx)
{

  return __builtin_bswap32 (__bsx);



}
# 69 "/usr/include/x86_64-linux-gnu/bits/byteswap.h" 3 4
__extension__ static __inline __uint64_t
__bswap_64 (__uint64_t __bsx)
{

  return __builtin_bswap64 (__bsx);



}
# 36 "/usr/include/endian.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/uintn-identity.h" 1 3 4
# 32 "/usr/include/x86_64-linux-gnu/bits/uintn-identity.h" 3 4
static __inline __uint16_t
__uint16_identity (__uint16_t __x)
{
  return __x;
}

static __inline __uint32_t
__uint32_identity (__uint32_t __x)
{
  return __x;
}

static __inline __uint64_t
__uint64_identity (__uint64_t __x)
{
  return __x;
}
# 37 "/usr/include/endian.h" 2 3 4
# 177 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4


# 1 "/usr/include/x86_64-linux-gnu/sys/select.h" 1 3 4
# 30 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/select.h" 1 3 4
# 31 "/usr/include/x86_64-linux-gnu/sys/select.h" 2 3 4


# 1 "/usr/include/x86_64-linux-gnu/bits/types/sigset_t.h" 1 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/types/__sigset_t.h" 1 3 4




typedef struct
{
  unsigned long int __val[(1024 / (8 * sizeof (unsigned long int)))];
} __sigset_t;
# 5 "/usr/include/x86_64-linux-gnu/bits/types/sigset_t.h" 2 3 4


typedef __sigset_t sigset_t;
# 34 "/usr/include/x86_64-linux-gnu/sys/select.h" 2 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/types/struct_timeval.h" 1 3 4







struct timeval
{




  __time_t tv_sec;
  __suseconds_t tv_usec;

};
# 38 "/usr/include/x86_64-linux-gnu/sys/select.h" 2 3 4

# 1 "/usr/include/x86_64-linux-gnu/bits/types/struct_timespec.h" 1 3 4
# 11 "/usr/include/x86_64-linux-gnu/bits/types/struct_timespec.h" 3 4
struct timespec
{



  __time_t tv_sec;




  __syscall_slong_t tv_nsec;
# 31 "/usr/include/x86_64-linux-gnu/bits/types/struct_timespec.h" 3 4
};
# 40 "/usr/include/x86_64-linux-gnu/sys/select.h" 2 3 4



typedef __suseconds_t suseconds_t;





typedef long int __fd_mask;
# 59 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4
typedef struct
  {






    __fd_mask __fds_bits[1024 / (8 * (int) sizeof (__fd_mask))];


  } fd_set;






typedef __fd_mask fd_mask;
# 91 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4

# 102 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4
extern int select (int __nfds, fd_set *__restrict __readfds,
     fd_set *__restrict __writefds,
     fd_set *__restrict __exceptfds,
     struct timeval *__restrict __timeout);
# 127 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4
extern int pselect (int __nfds, fd_set *__restrict __readfds,
      fd_set *__restrict __writefds,
      fd_set *__restrict __exceptfds,
      const struct timespec *__restrict __timeout,
      const __sigset_t *__restrict __sigmask);
# 153 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4

# 180 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4





typedef __blksize_t blksize_t;






typedef __blkcnt_t blkcnt_t;



typedef __fsblkcnt_t fsblkcnt_t;



typedef __fsfilcnt_t fsfilcnt_t;
# 227 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 1 3 4
# 23 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/thread-shared-types.h" 1 3 4
# 44 "/usr/include/x86_64-linux-gnu/bits/thread-shared-types.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes-arch.h" 1 3 4
# 21 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes-arch.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 22 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes-arch.h" 2 3 4
# 45 "/usr/include/x86_64-linux-gnu/bits/thread-shared-types.h" 2 3 4

# 1 "/usr/include/x86_64-linux-gnu/bits/atomic_wide_counter.h" 1 3 4
# 25 "/usr/include/x86_64-linux-gnu/bits/atomic_wide_counter.h" 3 4
typedef union
{
  __extension__ unsigned long long int __value64;
  struct
  {
    unsigned int __low;
    unsigned int __high;
  } __value32;
} __atomic_wide_counter;
# 47 "/usr/include/x86_64-linux-gnu/bits/thread-shared-types.h" 2 3 4




typedef struct __pthread_internal_list
{
  struct __pthread_internal_list *__prev;
  struct __pthread_internal_list *__next;
} __pthread_list_t;

typedef struct __pthread_internal_slist
{
  struct __pthread_internal_slist *__next;
} __pthread_slist_t;
# 76 "/usr/include/x86_64-linux-gnu/bits/thread-shared-types.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/struct_mutex.h" 1 3 4
# 22 "/usr/include/x86_64-linux-gnu/bits/struct_mutex.h" 3 4
struct __pthread_mutex_s
{
  int __lock;
  unsigned int __count;
  int __owner;

  unsigned int __nusers;



  int __kind;

  short __spins;
  short __elision;
  __pthread_list_t __list;
# 53 "/usr/include/x86_64-linux-gnu/bits/struct_mutex.h" 3 4
};
# 77 "/usr/include/x86_64-linux-gnu/bits/thread-shared-types.h" 2 3 4
# 89 "/usr/include/x86_64-linux-gnu/bits/thread-shared-types.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/struct_rwlock.h" 1 3 4
# 23 "/usr/include/x86_64-linux-gnu/bits/struct_rwlock.h" 3 4
struct __pthread_rwlock_arch_t
{
  unsigned int __readers;
  unsigned int __writers;
  unsigned int __wrphase_futex;
  unsigned int __writers_futex;
  unsigned int __pad3;
  unsigned int __pad4;

  int __cur_writer;
  int __shared;
  signed char __rwelision;




  unsigned char __pad1[7];


  unsigned long int __pad2;


  unsigned int __flags;
# 55 "/usr/include/x86_64-linux-gnu/bits/struct_rwlock.h" 3 4
};
# 90 "/usr/include/x86_64-linux-gnu/bits/thread-shared-types.h" 2 3 4




struct __pthread_cond_s
{
  __atomic_wide_counter __wseq;
  __atomic_wide_counter __g1_start;
  unsigned int __g_refs[2] ;
  unsigned int __g_size[2];
  unsigned int __g1_orig_size;
  unsigned int __wrefs;
  unsigned int __g_signals[2];
};

typedef unsigned int __tss_t;
typedef unsigned long int __thrd_t;

typedef struct
{
  int __data ;
} __once_flag;
# 24 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 2 3 4



typedef unsigned long int pthread_t;




typedef union
{
  char __size[4];
  int __align;
} pthread_mutexattr_t;




typedef union
{
  char __size[4];
  int __align;
} pthread_condattr_t;



typedef unsigned int pthread_key_t;



typedef int pthread_once_t;


union pthread_attr_t
{
  char __size[56];
  long int __align;
};

typedef union pthread_attr_t pthread_attr_t;




typedef union
{
  struct __pthread_mutex_s __data;
  char __size[40];
  long int __align;
} pthread_mutex_t;


typedef union
{
  struct __pthread_cond_s __data;
  char __size[48];
  __extension__ long long int __align;
} pthread_cond_t;





typedef union
{
  struct __pthread_rwlock_arch_t __data;
  char __size[56];
  long int __align;
} pthread_rwlock_t;

typedef union
{
  char __size[8];
  long int __align;
} pthread_rwlockattr_t;





typedef volatile int pthread_spinlock_t;




typedef union
{
  char __size[32];
  long int __align;
} pthread_barrier_t;

typedef union
{
  char __size[4];
  int __align;
} pthread_barrierattr_t;
# 228 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4



# 396 "/usr/include/stdlib.h" 2 3 4






extern long int random (void) __attribute__ ((__nothrow__ , __leaf__));


extern void srandom (unsigned int __seed) __attribute__ ((__nothrow__ , __leaf__));





extern char *initstate (unsigned int __seed, char *__statebuf,
   size_t __statelen) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));



extern char *setstate (char *__statebuf) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));







struct random_data
  {
    int32_t *fptr;
    int32_t *rptr;
    int32_t *state;
    int rand_type;
    int rand_deg;
    int rand_sep;
    int32_t *end_ptr;
  };

extern int random_r (struct random_data *__restrict __buf,
       int32_t *__restrict __result) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));

extern int srandom_r (unsigned int __seed, struct random_data *__buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));

extern int initstate_r (unsigned int __seed, char *__restrict __statebuf,
   size_t __statelen,
   struct random_data *__restrict __buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 4)));

extern int setstate_r (char *__restrict __statebuf,
         struct random_data *__restrict __buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));





extern int rand (void) __attribute__ ((__nothrow__ , __leaf__));

extern void srand (unsigned int __seed) __attribute__ ((__nothrow__ , __leaf__));



extern int rand_r (unsigned int *__seed) __attribute__ ((__nothrow__ , __leaf__));







extern double drand48 (void) __attribute__ ((__nothrow__ , __leaf__));
extern double erand48 (unsigned short int __xsubi[3]) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern long int lrand48 (void) __attribute__ ((__nothrow__ , __leaf__));
extern long int nrand48 (unsigned short int __xsubi[3])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern long int mrand48 (void) __attribute__ ((__nothrow__ , __leaf__));
extern long int jrand48 (unsigned short int __xsubi[3])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern void srand48 (long int __seedval) __attribute__ ((__nothrow__ , __leaf__));
extern unsigned short int *seed48 (unsigned short int __seed16v[3])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
extern void lcong48 (unsigned short int __param[7]) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





struct drand48_data
  {
    unsigned short int __x[3];
    unsigned short int __old_x[3];
    unsigned short int __c;
    unsigned short int __init;
    __extension__ unsigned long long int __a;

  };


extern int drand48_r (struct drand48_data *__restrict __buffer,
        double *__restrict __result) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern int erand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        double *__restrict __result) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern int lrand48_r (struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern int nrand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern int mrand48_r (struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern int jrand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern int srand48_r (long int __seedval, struct drand48_data *__buffer)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));

extern int seed48_r (unsigned short int __seed16v[3],
       struct drand48_data *__buffer) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));

extern int lcong48_r (unsigned short int __param[7],
        struct drand48_data *__buffer)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));




extern void *malloc (size_t __size) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__))
     __attribute__ ((__alloc_size__ (1))) ;

extern void *calloc (size_t __nmemb, size_t __size)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) __attribute__ ((__alloc_size__ (1, 2))) ;






extern void *realloc (void *__ptr, size_t __size)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__warn_unused_result__)) __attribute__ ((__alloc_size__ (2)));


extern void free (void *__ptr) __attribute__ ((__nothrow__ , __leaf__));







extern void *reallocarray (void *__ptr, size_t __nmemb, size_t __size)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__warn_unused_result__))
     __attribute__ ((__alloc_size__ (2, 3)))
    __attribute__ ((__malloc__ (__builtin_free, 1)));


extern void *reallocarray (void *__ptr, size_t __nmemb, size_t __size)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__ (reallocarray, 1)));



# 1 "/usr/include/alloca.h" 1 3 4
# 24 "/usr/include/alloca.h" 3 4
# 1 "/usr/lib/gcc/x86_64-linux-gnu/11/include/stddef.h" 1 3 4
# 25 "/usr/include/alloca.h" 2 3 4







extern void *alloca (size_t __size) __attribute__ ((__nothrow__ , __leaf__));






# 575 "/usr/include/stdlib.h" 2 3 4





extern void *valloc (size_t __size) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__))
     __attribute__ ((__alloc_size__ (1))) ;




extern int posix_memalign (void **__memptr, size_t __alignment, size_t __size)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;




extern void *aligned_alloc (size_t __alignment, size_t __size)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) __attribute__ ((__alloc_align__ (1)))
     __attribute__ ((__alloc_size__ (2))) ;



extern void abort (void) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));



extern int atexit (void (*__func) (void)) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));







extern int at_quick_exit (void (*__func) (void)) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));






extern int on_exit (void (*__func) (int __status, void *__arg), void *__arg)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern void exit (int __status) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));





extern void quick_exit (int __status) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));





extern void _Exit (int __status) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));




extern char *getenv (const char *__name) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;
# 654 "/usr/include/stdlib.h" 3 4
extern int putenv (char *__string) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern int setenv (const char *__name, const char *__value, int __replace)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));


extern int unsetenv (const char *__name) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));






extern int clearenv (void) __attribute__ ((__nothrow__ , __leaf__));
# 682 "/usr/include/stdlib.h" 3 4
extern char *mktemp (char *__template) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
# 695 "/usr/include/stdlib.h" 3 4
extern int mkstemp (char *__template) __attribute__ ((__nonnull__ (1))) ;
# 717 "/usr/include/stdlib.h" 3 4
extern int mkstemps (char *__template, int __suffixlen) __attribute__ ((__nonnull__ (1))) ;
# 738 "/usr/include/stdlib.h" 3 4
extern char *mkdtemp (char *__template) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;
# 791 "/usr/include/stdlib.h" 3 4
extern int system (const char *__command) ;
# 808 "/usr/include/stdlib.h" 3 4
extern char *realpath (const char *__restrict __name,
         char *__restrict __resolved) __attribute__ ((__nothrow__ , __leaf__)) ;






typedef int (*__compar_fn_t) (const void *, const void *);
# 828 "/usr/include/stdlib.h" 3 4
extern void *bsearch (const void *__key, const void *__base,
        size_t __nmemb, size_t __size, __compar_fn_t __compar)
     __attribute__ ((__nonnull__ (1, 2, 5))) ;







extern void qsort (void *__base, size_t __nmemb, size_t __size,
     __compar_fn_t __compar) __attribute__ ((__nonnull__ (1, 4)));
# 848 "/usr/include/stdlib.h" 3 4
extern int abs (int __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;
extern long int labs (long int __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;


__extension__ extern long long int llabs (long long int __x)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;






extern div_t div (int __numer, int __denom)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;
extern ldiv_t ldiv (long int __numer, long int __denom)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;


__extension__ extern lldiv_t lldiv (long long int __numer,
        long long int __denom)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;
# 880 "/usr/include/stdlib.h" 3 4
extern char *ecvt (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4))) ;




extern char *fcvt (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4))) ;




extern char *gcvt (double __value, int __ndigit, char *__buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3))) ;




extern char *qecvt (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4))) ;
extern char *qfcvt (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4))) ;
extern char *qgcvt (long double __value, int __ndigit, char *__buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3))) ;




extern int ecvt_r (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign, char *__restrict __buf,
     size_t __len) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4, 5)));
extern int fcvt_r (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign, char *__restrict __buf,
     size_t __len) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4, 5)));

extern int qecvt_r (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4, 5)));
extern int qfcvt_r (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4, 5)));





extern int mblen (const char *__s, size_t __n) __attribute__ ((__nothrow__ , __leaf__));


extern int mbtowc (wchar_t *__restrict __pwc,
     const char *__restrict __s, size_t __n) __attribute__ ((__nothrow__ , __leaf__));


extern int wctomb (char *__s, wchar_t __wchar) __attribute__ ((__nothrow__ , __leaf__));



extern size_t mbstowcs (wchar_t *__restrict __pwcs,
   const char *__restrict __s, size_t __n) __attribute__ ((__nothrow__ , __leaf__))
    __attribute__ ((__access__ (__read_only__, 2)));

extern size_t wcstombs (char *__restrict __s,
   const wchar_t *__restrict __pwcs, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__))
  __attribute__ ((__access__ (__write_only__, 1, 3)))
  __attribute__ ((__access__ (__read_only__, 2)));






extern int rpmatch (const char *__response) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;
# 967 "/usr/include/stdlib.h" 3 4
extern int getsubopt (char **__restrict __optionp,
        char *const *__restrict __tokens,
        char **__restrict __valuep)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2, 3))) ;
# 1013 "/usr/include/stdlib.h" 3 4
extern int getloadavg (double __loadavg[], int __nelem)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
# 1023 "/usr/include/stdlib.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/stdlib-float.h" 1 3 4
# 1024 "/usr/include/stdlib.h" 2 3 4
# 1035 "/usr/include/stdlib.h" 3 4

# 8 "/home/kalen/src/A/./src/A.h" 2
# 1 "/usr/include/string.h" 1 3 4
# 26 "/usr/include/string.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/libc-header-start.h" 1 3 4
# 27 "/usr/include/string.h" 2 3 4






# 1 "/usr/lib/gcc/x86_64-linux-gnu/11/include/stddef.h" 1 3 4
# 34 "/usr/include/string.h" 2 3 4
# 43 "/usr/include/string.h" 3 4
extern void *memcpy (void *__restrict __dest, const void *__restrict __src,
       size_t __n) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern void *memmove (void *__dest, const void *__src, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));





extern void *memccpy (void *__restrict __dest, const void *__restrict __src,
        int __c, size_t __n)
    __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2))) __attribute__ ((__access__ (__write_only__, 1, 4)));




extern void *memset (void *__s, int __c, size_t __n) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern int memcmp (const void *__s1, const void *__s2, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 80 "/usr/include/string.h" 3 4
extern int __memcmpeq (const void *__s1, const void *__s2, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 107 "/usr/include/string.h" 3 4
extern void *memchr (const void *__s, int __c, size_t __n)
      __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 141 "/usr/include/string.h" 3 4
extern char *strcpy (char *__restrict __dest, const char *__restrict __src)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strncpy (char *__restrict __dest,
        const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern char *strcat (char *__restrict __dest, const char *__restrict __src)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strncat (char *__restrict __dest, const char *__restrict __src,
        size_t __n) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strcmp (const char *__s1, const char *__s2)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern int strncmp (const char *__s1, const char *__s2, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strcoll (const char *__s1, const char *__s2)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern size_t strxfrm (char *__restrict __dest,
         const char *__restrict __src, size_t __n)
    __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2))) __attribute__ ((__access__ (__write_only__, 1, 3)));



# 1 "/usr/include/x86_64-linux-gnu/bits/types/locale_t.h" 1 3 4
# 22 "/usr/include/x86_64-linux-gnu/bits/types/locale_t.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/types/__locale_t.h" 1 3 4
# 27 "/usr/include/x86_64-linux-gnu/bits/types/__locale_t.h" 3 4
struct __locale_struct
{

  struct __locale_data *__locales[13];


  const unsigned short int *__ctype_b;
  const int *__ctype_tolower;
  const int *__ctype_toupper;


  const char *__names[13];
};

typedef struct __locale_struct *__locale_t;
# 23 "/usr/include/x86_64-linux-gnu/bits/types/locale_t.h" 2 3 4

typedef __locale_t locale_t;
# 173 "/usr/include/string.h" 2 3 4


extern int strcoll_l (const char *__s1, const char *__s2, locale_t __l)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2, 3)));


extern size_t strxfrm_l (char *__dest, const char *__src, size_t __n,
    locale_t __l) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 4)))
     __attribute__ ((__access__ (__write_only__, 1, 3)));





extern char *strdup (const char *__s)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));






extern char *strndup (const char *__string, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));
# 246 "/usr/include/string.h" 3 4
extern char *strchr (const char *__s, int __c)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 273 "/usr/include/string.h" 3 4
extern char *strrchr (const char *__s, int __c)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 293 "/usr/include/string.h" 3 4
extern size_t strcspn (const char *__s, const char *__reject)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern size_t strspn (const char *__s, const char *__accept)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 323 "/usr/include/string.h" 3 4
extern char *strpbrk (const char *__s, const char *__accept)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 350 "/usr/include/string.h" 3 4
extern char *strstr (const char *__haystack, const char *__needle)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));




extern char *strtok (char *__restrict __s, const char *__restrict __delim)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));



extern char *__strtok_r (char *__restrict __s,
    const char *__restrict __delim,
    char **__restrict __save_ptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 3)));

extern char *strtok_r (char *__restrict __s, const char *__restrict __delim,
         char **__restrict __save_ptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 3)));
# 407 "/usr/include/string.h" 3 4
extern size_t strlen (const char *__s)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));




extern size_t strnlen (const char *__string, size_t __maxlen)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));




extern char *strerror (int __errnum) __attribute__ ((__nothrow__ , __leaf__));
# 432 "/usr/include/string.h" 3 4
extern int strerror_r (int __errnum, char *__buf, size_t __buflen) __asm__ ("" "__xpg_strerror_r") __attribute__ ((__nothrow__ , __leaf__))

                        __attribute__ ((__nonnull__ (2)))
    __attribute__ ((__access__ (__write_only__, 2, 3)));
# 458 "/usr/include/string.h" 3 4
extern char *strerror_l (int __errnum, locale_t __l) __attribute__ ((__nothrow__ , __leaf__));



# 1 "/usr/include/strings.h" 1 3 4
# 23 "/usr/include/strings.h" 3 4
# 1 "/usr/lib/gcc/x86_64-linux-gnu/11/include/stddef.h" 1 3 4
# 24 "/usr/include/strings.h" 2 3 4










extern int bcmp (const void *__s1, const void *__s2, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern void bcopy (const void *__src, void *__dest, size_t __n)
  __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern void bzero (void *__s, size_t __n) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
# 68 "/usr/include/strings.h" 3 4
extern char *index (const char *__s, int __c)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 96 "/usr/include/strings.h" 3 4
extern char *rindex (const char *__s, int __c)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));






extern int ffs (int __i) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));





extern int ffsl (long int __l) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
__extension__ extern int ffsll (long long int __ll)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));



extern int strcasecmp (const char *__s1, const char *__s2)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strncasecmp (const char *__s1, const char *__s2, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));






extern int strcasecmp_l (const char *__s1, const char *__s2, locale_t __loc)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2, 3)));



extern int strncasecmp_l (const char *__s1, const char *__s2,
     size_t __n, locale_t __loc)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2, 4)));



# 463 "/usr/include/string.h" 2 3 4



extern void explicit_bzero (void *__s, size_t __n) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)))
    __attribute__ ((__access__ (__write_only__, 1, 2)));



extern char *strsep (char **__restrict __stringp,
       const char *__restrict __delim)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));




extern char *strsignal (int __sig) __attribute__ ((__nothrow__ , __leaf__));
# 489 "/usr/include/string.h" 3 4
extern char *__stpcpy (char *__restrict __dest, const char *__restrict __src)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern char *stpcpy (char *__restrict __dest, const char *__restrict __src)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));



extern char *__stpncpy (char *__restrict __dest,
   const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern char *stpncpy (char *__restrict __dest,
        const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
# 539 "/usr/include/string.h" 3 4

# 9 "/home/kalen/src/A/./src/A.h" 2
# 1 "/usr/lib/gcc/x86_64-linux-gnu/11/include/stdint.h" 1 3 4
# 9 "/usr/lib/gcc/x86_64-linux-gnu/11/include/stdint.h" 3 4
# 1 "/usr/include/stdint.h" 1 3 4
# 26 "/usr/include/stdint.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/libc-header-start.h" 1 3 4
# 27 "/usr/include/stdint.h" 2 3 4

# 1 "/usr/include/x86_64-linux-gnu/bits/wchar.h" 1 3 4
# 29 "/usr/include/stdint.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 30 "/usr/include/stdint.h" 2 3 4







# 1 "/usr/include/x86_64-linux-gnu/bits/stdint-uintn.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/bits/stdint-uintn.h" 3 4
typedef __uint8_t uint8_t;
typedef __uint16_t uint16_t;
typedef __uint32_t uint32_t;
typedef __uint64_t uint64_t;
# 38 "/usr/include/stdint.h" 2 3 4





typedef __int_least8_t int_least8_t;
typedef __int_least16_t int_least16_t;
typedef __int_least32_t int_least32_t;
typedef __int_least64_t int_least64_t;


typedef __uint_least8_t uint_least8_t;
typedef __uint_least16_t uint_least16_t;
typedef __uint_least32_t uint_least32_t;
typedef __uint_least64_t uint_least64_t;





typedef signed char int_fast8_t;

typedef long int int_fast16_t;
typedef long int int_fast32_t;
typedef long int int_fast64_t;
# 71 "/usr/include/stdint.h" 3 4
typedef unsigned char uint_fast8_t;

typedef unsigned long int uint_fast16_t;
typedef unsigned long int uint_fast32_t;
typedef unsigned long int uint_fast64_t;
# 87 "/usr/include/stdint.h" 3 4
typedef long int intptr_t;


typedef unsigned long int uintptr_t;
# 101 "/usr/include/stdint.h" 3 4
typedef __intmax_t intmax_t;
typedef __uintmax_t uintmax_t;
# 10 "/usr/lib/gcc/x86_64-linux-gnu/11/include/stdint.h" 2 3 4
# 10 "/home/kalen/src/A/./src/A.h" 2
# 1 "/usr/lib/gcc/x86_64-linux-gnu/11/include/stdarg.h" 1 3 4
# 11 "/home/kalen/src/A/./src/A.h" 2
# 1 "/usr/include/ctype.h" 1 3 4
# 28 "/usr/include/ctype.h" 3 4

# 46 "/usr/include/ctype.h" 3 4
enum
{
  _ISupper = ((0) < 8 ? ((1 << (0)) << 8) : ((1 << (0)) >> 8)),
  _ISlower = ((1) < 8 ? ((1 << (1)) << 8) : ((1 << (1)) >> 8)),
  _ISalpha = ((2) < 8 ? ((1 << (2)) << 8) : ((1 << (2)) >> 8)),
  _ISdigit = ((3) < 8 ? ((1 << (3)) << 8) : ((1 << (3)) >> 8)),
  _ISxdigit = ((4) < 8 ? ((1 << (4)) << 8) : ((1 << (4)) >> 8)),
  _ISspace = ((5) < 8 ? ((1 << (5)) << 8) : ((1 << (5)) >> 8)),
  _ISprint = ((6) < 8 ? ((1 << (6)) << 8) : ((1 << (6)) >> 8)),
  _ISgraph = ((7) < 8 ? ((1 << (7)) << 8) : ((1 << (7)) >> 8)),
  _ISblank = ((8) < 8 ? ((1 << (8)) << 8) : ((1 << (8)) >> 8)),
  _IScntrl = ((9) < 8 ? ((1 << (9)) << 8) : ((1 << (9)) >> 8)),
  _ISpunct = ((10) < 8 ? ((1 << (10)) << 8) : ((1 << (10)) >> 8)),
  _ISalnum = ((11) < 8 ? ((1 << (11)) << 8) : ((1 << (11)) >> 8))
};
# 79 "/usr/include/ctype.h" 3 4
extern const unsigned short int **__ctype_b_loc (void)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern const __int32_t **__ctype_tolower_loc (void)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern const __int32_t **__ctype_toupper_loc (void)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
# 108 "/usr/include/ctype.h" 3 4
extern int isalnum (int) __attribute__ ((__nothrow__ , __leaf__));
extern int isalpha (int) __attribute__ ((__nothrow__ , __leaf__));
extern int iscntrl (int) __attribute__ ((__nothrow__ , __leaf__));
extern int isdigit (int) __attribute__ ((__nothrow__ , __leaf__));
extern int islower (int) __attribute__ ((__nothrow__ , __leaf__));
extern int isgraph (int) __attribute__ ((__nothrow__ , __leaf__));
extern int isprint (int) __attribute__ ((__nothrow__ , __leaf__));
extern int ispunct (int) __attribute__ ((__nothrow__ , __leaf__));
extern int isspace (int) __attribute__ ((__nothrow__ , __leaf__));
extern int isupper (int) __attribute__ ((__nothrow__ , __leaf__));
extern int isxdigit (int) __attribute__ ((__nothrow__ , __leaf__));



extern int tolower (int __c) __attribute__ ((__nothrow__ , __leaf__));


extern int toupper (int __c) __attribute__ ((__nothrow__ , __leaf__));




extern int isblank (int) __attribute__ ((__nothrow__ , __leaf__));
# 142 "/usr/include/ctype.h" 3 4
extern int isascii (int __c) __attribute__ ((__nothrow__ , __leaf__));



extern int toascii (int __c) __attribute__ ((__nothrow__ , __leaf__));



extern int _toupper (int) __attribute__ ((__nothrow__ , __leaf__));
extern int _tolower (int) __attribute__ ((__nothrow__ , __leaf__));
# 251 "/usr/include/ctype.h" 3 4
extern int isalnum_l (int, locale_t) __attribute__ ((__nothrow__ , __leaf__));
extern int isalpha_l (int, locale_t) __attribute__ ((__nothrow__ , __leaf__));
extern int iscntrl_l (int, locale_t) __attribute__ ((__nothrow__ , __leaf__));
extern int isdigit_l (int, locale_t) __attribute__ ((__nothrow__ , __leaf__));
extern int islower_l (int, locale_t) __attribute__ ((__nothrow__ , __leaf__));
extern int isgraph_l (int, locale_t) __attribute__ ((__nothrow__ , __leaf__));
extern int isprint_l (int, locale_t) __attribute__ ((__nothrow__ , __leaf__));
extern int ispunct_l (int, locale_t) __attribute__ ((__nothrow__ , __leaf__));
extern int isspace_l (int, locale_t) __attribute__ ((__nothrow__ , __leaf__));
extern int isupper_l (int, locale_t) __attribute__ ((__nothrow__ , __leaf__));
extern int isxdigit_l (int, locale_t) __attribute__ ((__nothrow__ , __leaf__));

extern int isblank_l (int, locale_t) __attribute__ ((__nothrow__ , __leaf__));



extern int __tolower_l (int __c, locale_t __l) __attribute__ ((__nothrow__ , __leaf__));
extern int tolower_l (int __c, locale_t __l) __attribute__ ((__nothrow__ , __leaf__));


extern int __toupper_l (int __c, locale_t __l) __attribute__ ((__nothrow__ , __leaf__));
extern int toupper_l (int __c, locale_t __l) __attribute__ ((__nothrow__ , __leaf__));
# 327 "/usr/include/ctype.h" 3 4

# 12 "/home/kalen/src/A/./src/A.h" 2
# 1 "/usr/include/libgen.h" 1 3 4
# 23 "/usr/include/libgen.h" 3 4



extern char *dirname (char *__path) __attribute__ ((__nothrow__ , __leaf__));







extern char *__xpg_basename (char *__path) __attribute__ ((__nothrow__ , __leaf__));



# 13 "/home/kalen/src/A/./src/A.h" 2
# 1 "/usr/include/unistd.h" 1 3 4
# 27 "/usr/include/unistd.h" 3 4

# 202 "/usr/include/unistd.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/posix_opt.h" 1 3 4
# 203 "/usr/include/unistd.h" 2 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/environments.h" 1 3 4
# 22 "/usr/include/x86_64-linux-gnu/bits/environments.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 23 "/usr/include/x86_64-linux-gnu/bits/environments.h" 2 3 4
# 207 "/usr/include/unistd.h" 2 3 4
# 226 "/usr/include/unistd.h" 3 4
# 1 "/usr/lib/gcc/x86_64-linux-gnu/11/include/stddef.h" 1 3 4
# 227 "/usr/include/unistd.h" 2 3 4
# 255 "/usr/include/unistd.h" 3 4
typedef __useconds_t useconds_t;
# 274 "/usr/include/unistd.h" 3 4
typedef __socklen_t socklen_t;
# 287 "/usr/include/unistd.h" 3 4
extern int access (const char *__name, int __type) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
# 309 "/usr/include/unistd.h" 3 4
extern int faccessat (int __fd, const char *__file, int __type, int __flag)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2))) ;
# 339 "/usr/include/unistd.h" 3 4
extern __off_t lseek (int __fd, __off_t __offset, int __whence) __attribute__ ((__nothrow__ , __leaf__));
# 358 "/usr/include/unistd.h" 3 4
extern int close (int __fd);




extern void closefrom (int __lowfd) __attribute__ ((__nothrow__ , __leaf__));







extern ssize_t read (int __fd, void *__buf, size_t __nbytes)
    __attribute__ ((__access__ (__write_only__, 2, 3)));





extern ssize_t write (int __fd, const void *__buf, size_t __n)
    __attribute__ ((__access__ (__read_only__, 2, 3)));
# 389 "/usr/include/unistd.h" 3 4
extern ssize_t pread (int __fd, void *__buf, size_t __nbytes,
        __off_t __offset)
    __attribute__ ((__access__ (__write_only__, 2, 3)));






extern ssize_t pwrite (int __fd, const void *__buf, size_t __n,
         __off_t __offset)
    __attribute__ ((__access__ (__read_only__, 2, 3)));
# 437 "/usr/include/unistd.h" 3 4
extern int pipe (int __pipedes[2]) __attribute__ ((__nothrow__ , __leaf__)) ;
# 452 "/usr/include/unistd.h" 3 4
extern unsigned int alarm (unsigned int __seconds) __attribute__ ((__nothrow__ , __leaf__));
# 464 "/usr/include/unistd.h" 3 4
extern unsigned int sleep (unsigned int __seconds);







extern __useconds_t ualarm (__useconds_t __value, __useconds_t __interval)
     __attribute__ ((__nothrow__ , __leaf__));






extern int usleep (__useconds_t __useconds);
# 489 "/usr/include/unistd.h" 3 4
extern int pause (void);



extern int chown (const char *__file, __uid_t __owner, __gid_t __group)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;



extern int fchown (int __fd, __uid_t __owner, __gid_t __group) __attribute__ ((__nothrow__ , __leaf__)) ;




extern int lchown (const char *__file, __uid_t __owner, __gid_t __group)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;






extern int fchownat (int __fd, const char *__file, __uid_t __owner,
       __gid_t __group, int __flag)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2))) ;



extern int chdir (const char *__path) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;



extern int fchdir (int __fd) __attribute__ ((__nothrow__ , __leaf__)) ;
# 531 "/usr/include/unistd.h" 3 4
extern char *getcwd (char *__buf, size_t __size) __attribute__ ((__nothrow__ , __leaf__)) ;
# 545 "/usr/include/unistd.h" 3 4
extern char *getwd (char *__buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) __attribute__ ((__deprecated__))
    __attribute__ ((__access__ (__write_only__, 1)));




extern int dup (int __fd) __attribute__ ((__nothrow__ , __leaf__)) ;


extern int dup2 (int __fd, int __fd2) __attribute__ ((__nothrow__ , __leaf__));
# 564 "/usr/include/unistd.h" 3 4
extern char **__environ;







extern int execve (const char *__path, char *const __argv[],
     char *const __envp[]) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));




extern int fexecve (int __fd, char *const __argv[], char *const __envp[])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));




extern int execv (const char *__path, char *const __argv[])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));



extern int execle (const char *__path, const char *__arg, ...)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));



extern int execl (const char *__path, const char *__arg, ...)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));



extern int execvp (const char *__file, char *const __argv[])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));




extern int execlp (const char *__file, const char *__arg, ...)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
# 619 "/usr/include/unistd.h" 3 4
extern int nice (int __inc) __attribute__ ((__nothrow__ , __leaf__)) ;




extern void _exit (int __status) __attribute__ ((__noreturn__));





# 1 "/usr/include/x86_64-linux-gnu/bits/confname.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/bits/confname.h" 3 4
enum
  {
    _PC_LINK_MAX,

    _PC_MAX_CANON,

    _PC_MAX_INPUT,

    _PC_NAME_MAX,

    _PC_PATH_MAX,

    _PC_PIPE_BUF,

    _PC_CHOWN_RESTRICTED,

    _PC_NO_TRUNC,

    _PC_VDISABLE,

    _PC_SYNC_IO,

    _PC_ASYNC_IO,

    _PC_PRIO_IO,

    _PC_SOCK_MAXBUF,

    _PC_FILESIZEBITS,

    _PC_REC_INCR_XFER_SIZE,

    _PC_REC_MAX_XFER_SIZE,

    _PC_REC_MIN_XFER_SIZE,

    _PC_REC_XFER_ALIGN,

    _PC_ALLOC_SIZE_MIN,

    _PC_SYMLINK_MAX,

    _PC_2_SYMLINKS

  };


enum
  {
    _SC_ARG_MAX,

    _SC_CHILD_MAX,

    _SC_CLK_TCK,

    _SC_NGROUPS_MAX,

    _SC_OPEN_MAX,

    _SC_STREAM_MAX,

    _SC_TZNAME_MAX,

    _SC_JOB_CONTROL,

    _SC_SAVED_IDS,

    _SC_REALTIME_SIGNALS,

    _SC_PRIORITY_SCHEDULING,

    _SC_TIMERS,

    _SC_ASYNCHRONOUS_IO,

    _SC_PRIORITIZED_IO,

    _SC_SYNCHRONIZED_IO,

    _SC_FSYNC,

    _SC_MAPPED_FILES,

    _SC_MEMLOCK,

    _SC_MEMLOCK_RANGE,

    _SC_MEMORY_PROTECTION,

    _SC_MESSAGE_PASSING,

    _SC_SEMAPHORES,

    _SC_SHARED_MEMORY_OBJECTS,

    _SC_AIO_LISTIO_MAX,

    _SC_AIO_MAX,

    _SC_AIO_PRIO_DELTA_MAX,

    _SC_DELAYTIMER_MAX,

    _SC_MQ_OPEN_MAX,

    _SC_MQ_PRIO_MAX,

    _SC_VERSION,

    _SC_PAGESIZE,


    _SC_RTSIG_MAX,

    _SC_SEM_NSEMS_MAX,

    _SC_SEM_VALUE_MAX,

    _SC_SIGQUEUE_MAX,

    _SC_TIMER_MAX,




    _SC_BC_BASE_MAX,

    _SC_BC_DIM_MAX,

    _SC_BC_SCALE_MAX,

    _SC_BC_STRING_MAX,

    _SC_COLL_WEIGHTS_MAX,

    _SC_EQUIV_CLASS_MAX,

    _SC_EXPR_NEST_MAX,

    _SC_LINE_MAX,

    _SC_RE_DUP_MAX,

    _SC_CHARCLASS_NAME_MAX,


    _SC_2_VERSION,

    _SC_2_C_BIND,

    _SC_2_C_DEV,

    _SC_2_FORT_DEV,

    _SC_2_FORT_RUN,

    _SC_2_SW_DEV,

    _SC_2_LOCALEDEF,


    _SC_PII,

    _SC_PII_XTI,

    _SC_PII_SOCKET,

    _SC_PII_INTERNET,

    _SC_PII_OSI,

    _SC_POLL,

    _SC_SELECT,

    _SC_UIO_MAXIOV,

    _SC_IOV_MAX = _SC_UIO_MAXIOV,

    _SC_PII_INTERNET_STREAM,

    _SC_PII_INTERNET_DGRAM,

    _SC_PII_OSI_COTS,

    _SC_PII_OSI_CLTS,

    _SC_PII_OSI_M,

    _SC_T_IOV_MAX,



    _SC_THREADS,

    _SC_THREAD_SAFE_FUNCTIONS,

    _SC_GETGR_R_SIZE_MAX,

    _SC_GETPW_R_SIZE_MAX,

    _SC_LOGIN_NAME_MAX,

    _SC_TTY_NAME_MAX,

    _SC_THREAD_DESTRUCTOR_ITERATIONS,

    _SC_THREAD_KEYS_MAX,

    _SC_THREAD_STACK_MIN,

    _SC_THREAD_THREADS_MAX,

    _SC_THREAD_ATTR_STACKADDR,

    _SC_THREAD_ATTR_STACKSIZE,

    _SC_THREAD_PRIORITY_SCHEDULING,

    _SC_THREAD_PRIO_INHERIT,

    _SC_THREAD_PRIO_PROTECT,

    _SC_THREAD_PROCESS_SHARED,


    _SC_NPROCESSORS_CONF,

    _SC_NPROCESSORS_ONLN,

    _SC_PHYS_PAGES,

    _SC_AVPHYS_PAGES,

    _SC_ATEXIT_MAX,

    _SC_PASS_MAX,


    _SC_XOPEN_VERSION,

    _SC_XOPEN_XCU_VERSION,

    _SC_XOPEN_UNIX,

    _SC_XOPEN_CRYPT,

    _SC_XOPEN_ENH_I18N,

    _SC_XOPEN_SHM,


    _SC_2_CHAR_TERM,

    _SC_2_C_VERSION,

    _SC_2_UPE,


    _SC_XOPEN_XPG2,

    _SC_XOPEN_XPG3,

    _SC_XOPEN_XPG4,


    _SC_CHAR_BIT,

    _SC_CHAR_MAX,

    _SC_CHAR_MIN,

    _SC_INT_MAX,

    _SC_INT_MIN,

    _SC_LONG_BIT,

    _SC_WORD_BIT,

    _SC_MB_LEN_MAX,

    _SC_NZERO,

    _SC_SSIZE_MAX,

    _SC_SCHAR_MAX,

    _SC_SCHAR_MIN,

    _SC_SHRT_MAX,

    _SC_SHRT_MIN,

    _SC_UCHAR_MAX,

    _SC_UINT_MAX,

    _SC_ULONG_MAX,

    _SC_USHRT_MAX,


    _SC_NL_ARGMAX,

    _SC_NL_LANGMAX,

    _SC_NL_MSGMAX,

    _SC_NL_NMAX,

    _SC_NL_SETMAX,

    _SC_NL_TEXTMAX,


    _SC_XBS5_ILP32_OFF32,

    _SC_XBS5_ILP32_OFFBIG,

    _SC_XBS5_LP64_OFF64,

    _SC_XBS5_LPBIG_OFFBIG,


    _SC_XOPEN_LEGACY,

    _SC_XOPEN_REALTIME,

    _SC_XOPEN_REALTIME_THREADS,


    _SC_ADVISORY_INFO,

    _SC_BARRIERS,

    _SC_BASE,

    _SC_C_LANG_SUPPORT,

    _SC_C_LANG_SUPPORT_R,

    _SC_CLOCK_SELECTION,

    _SC_CPUTIME,

    _SC_THREAD_CPUTIME,

    _SC_DEVICE_IO,

    _SC_DEVICE_SPECIFIC,

    _SC_DEVICE_SPECIFIC_R,

    _SC_FD_MGMT,

    _SC_FIFO,

    _SC_PIPE,

    _SC_FILE_ATTRIBUTES,

    _SC_FILE_LOCKING,

    _SC_FILE_SYSTEM,

    _SC_MONOTONIC_CLOCK,

    _SC_MULTI_PROCESS,

    _SC_SINGLE_PROCESS,

    _SC_NETWORKING,

    _SC_READER_WRITER_LOCKS,

    _SC_SPIN_LOCKS,

    _SC_REGEXP,

    _SC_REGEX_VERSION,

    _SC_SHELL,

    _SC_SIGNALS,

    _SC_SPAWN,

    _SC_SPORADIC_SERVER,

    _SC_THREAD_SPORADIC_SERVER,

    _SC_SYSTEM_DATABASE,

    _SC_SYSTEM_DATABASE_R,

    _SC_TIMEOUTS,

    _SC_TYPED_MEMORY_OBJECTS,

    _SC_USER_GROUPS,

    _SC_USER_GROUPS_R,

    _SC_2_PBS,

    _SC_2_PBS_ACCOUNTING,

    _SC_2_PBS_LOCATE,

    _SC_2_PBS_MESSAGE,

    _SC_2_PBS_TRACK,

    _SC_SYMLOOP_MAX,

    _SC_STREAMS,

    _SC_2_PBS_CHECKPOINT,


    _SC_V6_ILP32_OFF32,

    _SC_V6_ILP32_OFFBIG,

    _SC_V6_LP64_OFF64,

    _SC_V6_LPBIG_OFFBIG,


    _SC_HOST_NAME_MAX,

    _SC_TRACE,

    _SC_TRACE_EVENT_FILTER,

    _SC_TRACE_INHERIT,

    _SC_TRACE_LOG,


    _SC_LEVEL1_ICACHE_SIZE,

    _SC_LEVEL1_ICACHE_ASSOC,

    _SC_LEVEL1_ICACHE_LINESIZE,

    _SC_LEVEL1_DCACHE_SIZE,

    _SC_LEVEL1_DCACHE_ASSOC,

    _SC_LEVEL1_DCACHE_LINESIZE,

    _SC_LEVEL2_CACHE_SIZE,

    _SC_LEVEL2_CACHE_ASSOC,

    _SC_LEVEL2_CACHE_LINESIZE,

    _SC_LEVEL3_CACHE_SIZE,

    _SC_LEVEL3_CACHE_ASSOC,

    _SC_LEVEL3_CACHE_LINESIZE,

    _SC_LEVEL4_CACHE_SIZE,

    _SC_LEVEL4_CACHE_ASSOC,

    _SC_LEVEL4_CACHE_LINESIZE,



    _SC_IPV6 = _SC_LEVEL1_ICACHE_SIZE + 50,

    _SC_RAW_SOCKETS,


    _SC_V7_ILP32_OFF32,

    _SC_V7_ILP32_OFFBIG,

    _SC_V7_LP64_OFF64,

    _SC_V7_LPBIG_OFFBIG,


    _SC_SS_REPL_MAX,


    _SC_TRACE_EVENT_NAME_MAX,

    _SC_TRACE_NAME_MAX,

    _SC_TRACE_SYS_MAX,

    _SC_TRACE_USER_EVENT_MAX,


    _SC_XOPEN_STREAMS,


    _SC_THREAD_ROBUST_PRIO_INHERIT,

    _SC_THREAD_ROBUST_PRIO_PROTECT,


    _SC_MINSIGSTKSZ,


    _SC_SIGSTKSZ

  };


enum
  {
    _CS_PATH,


    _CS_V6_WIDTH_RESTRICTED_ENVS,



    _CS_GNU_LIBC_VERSION,

    _CS_GNU_LIBPTHREAD_VERSION,


    _CS_V5_WIDTH_RESTRICTED_ENVS,



    _CS_V7_WIDTH_RESTRICTED_ENVS,



    _CS_LFS_CFLAGS = 1000,

    _CS_LFS_LDFLAGS,

    _CS_LFS_LIBS,

    _CS_LFS_LINTFLAGS,

    _CS_LFS64_CFLAGS,

    _CS_LFS64_LDFLAGS,

    _CS_LFS64_LIBS,

    _CS_LFS64_LINTFLAGS,


    _CS_XBS5_ILP32_OFF32_CFLAGS = 1100,

    _CS_XBS5_ILP32_OFF32_LDFLAGS,

    _CS_XBS5_ILP32_OFF32_LIBS,

    _CS_XBS5_ILP32_OFF32_LINTFLAGS,

    _CS_XBS5_ILP32_OFFBIG_CFLAGS,

    _CS_XBS5_ILP32_OFFBIG_LDFLAGS,

    _CS_XBS5_ILP32_OFFBIG_LIBS,

    _CS_XBS5_ILP32_OFFBIG_LINTFLAGS,

    _CS_XBS5_LP64_OFF64_CFLAGS,

    _CS_XBS5_LP64_OFF64_LDFLAGS,

    _CS_XBS5_LP64_OFF64_LIBS,

    _CS_XBS5_LP64_OFF64_LINTFLAGS,

    _CS_XBS5_LPBIG_OFFBIG_CFLAGS,

    _CS_XBS5_LPBIG_OFFBIG_LDFLAGS,

    _CS_XBS5_LPBIG_OFFBIG_LIBS,

    _CS_XBS5_LPBIG_OFFBIG_LINTFLAGS,


    _CS_POSIX_V6_ILP32_OFF32_CFLAGS,

    _CS_POSIX_V6_ILP32_OFF32_LDFLAGS,

    _CS_POSIX_V6_ILP32_OFF32_LIBS,

    _CS_POSIX_V6_ILP32_OFF32_LINTFLAGS,

    _CS_POSIX_V6_ILP32_OFFBIG_CFLAGS,

    _CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS,

    _CS_POSIX_V6_ILP32_OFFBIG_LIBS,

    _CS_POSIX_V6_ILP32_OFFBIG_LINTFLAGS,

    _CS_POSIX_V6_LP64_OFF64_CFLAGS,

    _CS_POSIX_V6_LP64_OFF64_LDFLAGS,

    _CS_POSIX_V6_LP64_OFF64_LIBS,

    _CS_POSIX_V6_LP64_OFF64_LINTFLAGS,

    _CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS,

    _CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS,

    _CS_POSIX_V6_LPBIG_OFFBIG_LIBS,

    _CS_POSIX_V6_LPBIG_OFFBIG_LINTFLAGS,


    _CS_POSIX_V7_ILP32_OFF32_CFLAGS,

    _CS_POSIX_V7_ILP32_OFF32_LDFLAGS,

    _CS_POSIX_V7_ILP32_OFF32_LIBS,

    _CS_POSIX_V7_ILP32_OFF32_LINTFLAGS,

    _CS_POSIX_V7_ILP32_OFFBIG_CFLAGS,

    _CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS,

    _CS_POSIX_V7_ILP32_OFFBIG_LIBS,

    _CS_POSIX_V7_ILP32_OFFBIG_LINTFLAGS,

    _CS_POSIX_V7_LP64_OFF64_CFLAGS,

    _CS_POSIX_V7_LP64_OFF64_LDFLAGS,

    _CS_POSIX_V7_LP64_OFF64_LIBS,

    _CS_POSIX_V7_LP64_OFF64_LINTFLAGS,

    _CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS,

    _CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS,

    _CS_POSIX_V7_LPBIG_OFFBIG_LIBS,

    _CS_POSIX_V7_LPBIG_OFFBIG_LINTFLAGS,


    _CS_V6_ENV,

    _CS_V7_ENV

  };
# 631 "/usr/include/unistd.h" 2 3 4


extern long int pathconf (const char *__path, int __name)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern long int fpathconf (int __fd, int __name) __attribute__ ((__nothrow__ , __leaf__));


extern long int sysconf (int __name) __attribute__ ((__nothrow__ , __leaf__));



extern size_t confstr (int __name, char *__buf, size_t __len) __attribute__ ((__nothrow__ , __leaf__))
    __attribute__ ((__access__ (__write_only__, 2, 3)));




extern __pid_t getpid (void) __attribute__ ((__nothrow__ , __leaf__));


extern __pid_t getppid (void) __attribute__ ((__nothrow__ , __leaf__));


extern __pid_t getpgrp (void) __attribute__ ((__nothrow__ , __leaf__));


extern __pid_t __getpgid (__pid_t __pid) __attribute__ ((__nothrow__ , __leaf__));

extern __pid_t getpgid (__pid_t __pid) __attribute__ ((__nothrow__ , __leaf__));






extern int setpgid (__pid_t __pid, __pid_t __pgid) __attribute__ ((__nothrow__ , __leaf__));
# 682 "/usr/include/unistd.h" 3 4
extern int setpgrp (void) __attribute__ ((__nothrow__ , __leaf__));






extern __pid_t setsid (void) __attribute__ ((__nothrow__ , __leaf__));



extern __pid_t getsid (__pid_t __pid) __attribute__ ((__nothrow__ , __leaf__));



extern __uid_t getuid (void) __attribute__ ((__nothrow__ , __leaf__));


extern __uid_t geteuid (void) __attribute__ ((__nothrow__ , __leaf__));


extern __gid_t getgid (void) __attribute__ ((__nothrow__ , __leaf__));


extern __gid_t getegid (void) __attribute__ ((__nothrow__ , __leaf__));




extern int getgroups (int __size, __gid_t __list[]) __attribute__ ((__nothrow__ , __leaf__))
    __attribute__ ((__access__ (__write_only__, 2, 1)));
# 722 "/usr/include/unistd.h" 3 4
extern int setuid (__uid_t __uid) __attribute__ ((__nothrow__ , __leaf__)) ;




extern int setreuid (__uid_t __ruid, __uid_t __euid) __attribute__ ((__nothrow__ , __leaf__)) ;




extern int seteuid (__uid_t __uid) __attribute__ ((__nothrow__ , __leaf__)) ;






extern int setgid (__gid_t __gid) __attribute__ ((__nothrow__ , __leaf__)) ;




extern int setregid (__gid_t __rgid, __gid_t __egid) __attribute__ ((__nothrow__ , __leaf__)) ;




extern int setegid (__gid_t __gid) __attribute__ ((__nothrow__ , __leaf__)) ;
# 778 "/usr/include/unistd.h" 3 4
extern __pid_t fork (void) __attribute__ ((__nothrow__));







extern __pid_t vfork (void) __attribute__ ((__nothrow__ , __leaf__));
# 799 "/usr/include/unistd.h" 3 4
extern char *ttyname (int __fd) __attribute__ ((__nothrow__ , __leaf__));



extern int ttyname_r (int __fd, char *__buf, size_t __buflen)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)))
     __attribute__ ((__access__ (__write_only__, 2, 3)));



extern int isatty (int __fd) __attribute__ ((__nothrow__ , __leaf__));




extern int ttyslot (void) __attribute__ ((__nothrow__ , __leaf__));




extern int link (const char *__from, const char *__to)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2))) ;




extern int linkat (int __fromfd, const char *__from, int __tofd,
     const char *__to, int __flags)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 4))) ;




extern int symlink (const char *__from, const char *__to)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2))) ;




extern ssize_t readlink (const char *__restrict __path,
    char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)))
     __attribute__ ((__access__ (__write_only__, 2, 3)));





extern int symlinkat (const char *__from, int __tofd,
        const char *__to) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 3))) ;


extern ssize_t readlinkat (int __fd, const char *__restrict __path,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 3)))
     __attribute__ ((__access__ (__write_only__, 3, 4)));



extern int unlink (const char *__name) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));



extern int unlinkat (int __fd, const char *__name, int __flag)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));



extern int rmdir (const char *__path) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));



extern __pid_t tcgetpgrp (int __fd) __attribute__ ((__nothrow__ , __leaf__));


extern int tcsetpgrp (int __fd, __pid_t __pgrp_id) __attribute__ ((__nothrow__ , __leaf__));






extern char *getlogin (void);







extern int getlogin_r (char *__name, size_t __name_len) __attribute__ ((__nonnull__ (1)))
    __attribute__ ((__access__ (__write_only__, 1, 2)));




extern int setlogin (const char *__name) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));







# 1 "/usr/include/x86_64-linux-gnu/bits/getopt_posix.h" 1 3 4
# 27 "/usr/include/x86_64-linux-gnu/bits/getopt_posix.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/getopt_core.h" 1 3 4
# 28 "/usr/include/x86_64-linux-gnu/bits/getopt_core.h" 3 4








extern char *optarg;
# 50 "/usr/include/x86_64-linux-gnu/bits/getopt_core.h" 3 4
extern int optind;




extern int opterr;



extern int optopt;
# 91 "/usr/include/x86_64-linux-gnu/bits/getopt_core.h" 3 4
extern int getopt (int ___argc, char *const *___argv, const char *__shortopts)
       __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 3)));


# 28 "/usr/include/x86_64-linux-gnu/bits/getopt_posix.h" 2 3 4


# 49 "/usr/include/x86_64-linux-gnu/bits/getopt_posix.h" 3 4

# 904 "/usr/include/unistd.h" 2 3 4







extern int gethostname (char *__name, size_t __len) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)))
    __attribute__ ((__access__ (__write_only__, 1, 2)));






extern int sethostname (const char *__name, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) __attribute__ ((__access__ (__read_only__, 1, 2)));



extern int sethostid (long int __id) __attribute__ ((__nothrow__ , __leaf__)) ;





extern int getdomainname (char *__name, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)))
     __attribute__ ((__access__ (__write_only__, 1, 2)));
extern int setdomainname (const char *__name, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) __attribute__ ((__access__ (__read_only__, 1, 2)));




extern int vhangup (void) __attribute__ ((__nothrow__ , __leaf__));


extern int revoke (const char *__file) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;







extern int profil (unsigned short int *__sample_buffer, size_t __size,
     size_t __offset, unsigned int __scale)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern int acct (const char *__name) __attribute__ ((__nothrow__ , __leaf__));



extern char *getusershell (void) __attribute__ ((__nothrow__ , __leaf__));
extern void endusershell (void) __attribute__ ((__nothrow__ , __leaf__));
extern void setusershell (void) __attribute__ ((__nothrow__ , __leaf__));





extern int daemon (int __nochdir, int __noclose) __attribute__ ((__nothrow__ , __leaf__)) ;






extern int chroot (const char *__path) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;



extern char *getpass (const char *__prompt) __attribute__ ((__nonnull__ (1)));







extern int fsync (int __fd);
# 1002 "/usr/include/unistd.h" 3 4
extern long int gethostid (void);


extern void sync (void) __attribute__ ((__nothrow__ , __leaf__));





extern int getpagesize (void) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));




extern int getdtablesize (void) __attribute__ ((__nothrow__ , __leaf__));
# 1026 "/usr/include/unistd.h" 3 4
extern int truncate (const char *__file, __off_t __length)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;
# 1049 "/usr/include/unistd.h" 3 4
extern int ftruncate (int __fd, __off_t __length) __attribute__ ((__nothrow__ , __leaf__)) ;
# 1070 "/usr/include/unistd.h" 3 4
extern int brk (void *__addr) __attribute__ ((__nothrow__ , __leaf__)) ;





extern void *sbrk (intptr_t __delta) __attribute__ ((__nothrow__ , __leaf__));
# 1091 "/usr/include/unistd.h" 3 4
extern long int syscall (long int __sysno, ...) __attribute__ ((__nothrow__ , __leaf__));
# 1114 "/usr/include/unistd.h" 3 4
extern int lockf (int __fd, int __cmd, __off_t __len) ;
# 1150 "/usr/include/unistd.h" 3 4
extern int fdatasync (int __fildes);
# 1159 "/usr/include/unistd.h" 3 4
extern char *crypt (const char *__key, const char *__salt)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
# 1198 "/usr/include/unistd.h" 3 4
int getentropy (void *__buffer, size_t __length)
    __attribute__ ((__access__ (__write_only__, 1, 2)));
# 1218 "/usr/include/unistd.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/unistd_ext.h" 1 3 4
# 1219 "/usr/include/unistd.h" 2 3 4


# 14 "/home/kalen/src/A/./src/A.h" 2


# 15 "/home/kalen/src/A/./src/A.h"
FILE *popen(const char *command, const char *type);
int pclose(FILE*);


typedef void none;
typedef signed char i8;
typedef short i16;
typedef int i32;
typedef long long i64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef long long num;
typedef unsigned int bool;
typedef float f32;
typedef double f64;
typedef long double f128;
typedef double real;
typedef void* handle;
typedef char* cstr;
typedef const char* symbol;
typedef num sz;
typedef struct A_f* AType;
typedef struct args_t {
    num count;
    AType arg_0, arg_1, arg_2, arg_3,
                    arg_4, arg_5, arg_6, arg_7;
} args_t;

typedef bool(*global_init_fn)();
# 56 "/home/kalen/src/A/./src/A.h"
enum A_TYPE {
    A_TYPE_NONE = 0,
    A_TYPE_PROP = 1,
    A_TYPE_PRIV = 2,
    A_TYPE_INTERN = 4,
    A_TYPE_IMETHOD = 8,
    A_TYPE_SMETHOD = 16,
    A_TYPE_CONSTRUCT = 32,
    A_TYPE_OPERATOR = 64,
    A_TYPE_CAST = 128,
    A_TYPE_INDEX = 256,
    A_TYPE_ENUMV = 512
};

enum A_TRAIT {
    A_TRAIT_PRIMITIVE = 1,
    A_TRAIT_INTEGRAL = 2,
    A_TRAIT_REALISTIC = 4,
    A_TRAIT_ENUM = 8,
    A_TRAIT_ALIAS = 16
};
# 249 "/home/kalen/src/A/./src/A.h"
typedef struct method_t {
    struct array* atypes;
    struct A_f* rtype;
    void* address;
    void* ffi_cif;
    void* ffi_args;
} method_t;

typedef struct prop_t {
    void* address;
} prop_t;



typedef struct member_t {
    char* name;
    AType type;
    num offset;
    enum A_TYPE member_type;
    int operator_type;
    args_t args;
    union {
        method_t* method;
        prop_t* prop;
    };
} member_t;
# 346 "/home/kalen/src/A/./src/A.h"
void A_push_type(AType type);
# 358 "/home/kalen/src/A/./src/A.h"
void A_lazy_init(global_init_fn fn);
# 459 "/home/kalen/src/A/./src/A.h"
void* primitive_ffi_arb(AType);
# 522 "/home/kalen/src/A/./src/A.h"

# 522 "/home/kalen/src/A/./src/A.h"
#pragma pack(push, 1)
# 522 "/home/kalen/src/A/./src/A.h"
 typedef struct A { AType type; num refs; struct A* data; num alloc; num count; struct A* origin; struct A_f* f; } *A;
# 522 "/home/kalen/src/A/./src/A.h"
#pragma pack(pop)
# 522 "/home/kalen/src/A/./src/A.h"
 typedef struct A_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); } A_f; extern A_f A_type;
# 539 "/home/kalen/src/A/./src/A.h"
typedef struct A* numeric; typedef struct numeric_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); numeric (*with_i8)(numeric, i8); numeric (*with_i16)(numeric, i16); numeric (*with_i32)(numeric, i32); numeric (*with_i64)(numeric, i64); numeric (*with_u8)(numeric, u8); numeric (*with_u16)(numeric, u16); numeric (*with_u32)(numeric, u32); numeric (*with_u64)(numeric, u64); numeric (*with_f32)(numeric, f32); numeric (*with_f64)(numeric, f64); numeric (*with_bool)(numeric, bool); numeric (*with_num)(numeric, num); } numeric_f, *numeric_t; extern numeric_f numeric_type;


typedef struct A* string_like; typedef struct string_like_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); } string_like_f, *string_like_t; extern string_like_f string_like_type;


typedef struct A* nil; typedef struct nil_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); } nil_f, *nil_t; extern nil_f nil_type;


typedef struct A* raw; typedef struct raw_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); } raw_f, *raw_t; extern raw_f raw_type;
# 575 "/home/kalen/src/A/./src/A.h"
typedef struct i8_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); numeric (*with_i8)(numeric, i8); numeric (*with_i16)(numeric, i16); numeric (*with_i32)(numeric, i32); numeric (*with_i64)(numeric, i64); numeric (*with_u8)(numeric, u8); numeric (*with_u16)(numeric, u16); numeric (*with_u32)(numeric, u32); numeric (*with_u64)(numeric, u64); numeric (*with_f32)(numeric, f32); numeric (*with_f64)(numeric, f64); numeric (*with_bool)(numeric, bool); numeric (*with_num)(numeric, num); } i8_f, *i8_t; extern i8_f i8_type;
typedef struct i16_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); numeric (*with_i8)(numeric, i8); numeric (*with_i16)(numeric, i16); numeric (*with_i32)(numeric, i32); numeric (*with_i64)(numeric, i64); numeric (*with_u8)(numeric, u8); numeric (*with_u16)(numeric, u16); numeric (*with_u32)(numeric, u32); numeric (*with_u64)(numeric, u64); numeric (*with_f32)(numeric, f32); numeric (*with_f64)(numeric, f64); numeric (*with_bool)(numeric, bool); numeric (*with_num)(numeric, num); } i16_f, *i16_t; extern i16_f i16_type;
typedef struct i32_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); numeric (*with_i8)(numeric, i8); numeric (*with_i16)(numeric, i16); numeric (*with_i32)(numeric, i32); numeric (*with_i64)(numeric, i64); numeric (*with_u8)(numeric, u8); numeric (*with_u16)(numeric, u16); numeric (*with_u32)(numeric, u32); numeric (*with_u64)(numeric, u64); numeric (*with_f32)(numeric, f32); numeric (*with_f64)(numeric, f64); numeric (*with_bool)(numeric, bool); numeric (*with_num)(numeric, num); } i32_f, *i32_t; extern i32_f i32_type;
typedef struct i64_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); numeric (*with_i8)(numeric, i8); numeric (*with_i16)(numeric, i16); numeric (*with_i32)(numeric, i32); numeric (*with_i64)(numeric, i64); numeric (*with_u8)(numeric, u8); numeric (*with_u16)(numeric, u16); numeric (*with_u32)(numeric, u32); numeric (*with_u64)(numeric, u64); numeric (*with_f32)(numeric, f32); numeric (*with_f64)(numeric, f64); numeric (*with_bool)(numeric, bool); numeric (*with_num)(numeric, num); } i64_f, *i64_t; extern i64_f i64_type;
typedef struct u8_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); numeric (*with_i8)(numeric, i8); numeric (*with_i16)(numeric, i16); numeric (*with_i32)(numeric, i32); numeric (*with_i64)(numeric, i64); numeric (*with_u8)(numeric, u8); numeric (*with_u16)(numeric, u16); numeric (*with_u32)(numeric, u32); numeric (*with_u64)(numeric, u64); numeric (*with_f32)(numeric, f32); numeric (*with_f64)(numeric, f64); numeric (*with_bool)(numeric, bool); numeric (*with_num)(numeric, num); } u8_f, *u8_t; extern u8_f u8_type;
typedef struct u16_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); numeric (*with_i8)(numeric, i8); numeric (*with_i16)(numeric, i16); numeric (*with_i32)(numeric, i32); numeric (*with_i64)(numeric, i64); numeric (*with_u8)(numeric, u8); numeric (*with_u16)(numeric, u16); numeric (*with_u32)(numeric, u32); numeric (*with_u64)(numeric, u64); numeric (*with_f32)(numeric, f32); numeric (*with_f64)(numeric, f64); numeric (*with_bool)(numeric, bool); numeric (*with_num)(numeric, num); } u16_f, *u16_t; extern u16_f u16_type;
typedef struct u32_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); numeric (*with_i8)(numeric, i8); numeric (*with_i16)(numeric, i16); numeric (*with_i32)(numeric, i32); numeric (*with_i64)(numeric, i64); numeric (*with_u8)(numeric, u8); numeric (*with_u16)(numeric, u16); numeric (*with_u32)(numeric, u32); numeric (*with_u64)(numeric, u64); numeric (*with_f32)(numeric, f32); numeric (*with_f64)(numeric, f64); numeric (*with_bool)(numeric, bool); numeric (*with_num)(numeric, num); } u32_f, *u32_t; extern u32_f u32_type;
typedef struct u64_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); numeric (*with_i8)(numeric, i8); numeric (*with_i16)(numeric, i16); numeric (*with_i32)(numeric, i32); numeric (*with_i64)(numeric, i64); numeric (*with_u8)(numeric, u8); numeric (*with_u16)(numeric, u16); numeric (*with_u32)(numeric, u32); numeric (*with_u64)(numeric, u64); numeric (*with_f32)(numeric, f32); numeric (*with_f64)(numeric, f64); numeric (*with_bool)(numeric, bool); numeric (*with_num)(numeric, num); } u64_f, *u64_t; extern u64_f u64_type;
typedef struct f32_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); numeric (*with_i8)(numeric, i8); numeric (*with_i16)(numeric, i16); numeric (*with_i32)(numeric, i32); numeric (*with_i64)(numeric, i64); numeric (*with_u8)(numeric, u8); numeric (*with_u16)(numeric, u16); numeric (*with_u32)(numeric, u32); numeric (*with_u64)(numeric, u64); numeric (*with_f32)(numeric, f32); numeric (*with_f64)(numeric, f64); numeric (*with_bool)(numeric, bool); numeric (*with_num)(numeric, num); } f32_f, *f32_t; extern f32_f f32_type;
typedef struct f64_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); numeric (*with_i8)(numeric, i8); numeric (*with_i16)(numeric, i16); numeric (*with_i32)(numeric, i32); numeric (*with_i64)(numeric, i64); numeric (*with_u8)(numeric, u8); numeric (*with_u16)(numeric, u16); numeric (*with_u32)(numeric, u32); numeric (*with_u64)(numeric, u64); numeric (*with_f32)(numeric, f32); numeric (*with_f64)(numeric, f64); numeric (*with_bool)(numeric, bool); numeric (*with_num)(numeric, num); } f64_f, *f64_t; extern f64_f f64_type;
typedef struct f128_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); numeric (*with_i8)(numeric, i8); numeric (*with_i16)(numeric, i16); numeric (*with_i32)(numeric, i32); numeric (*with_i64)(numeric, i64); numeric (*with_u8)(numeric, u8); numeric (*with_u16)(numeric, u16); numeric (*with_u32)(numeric, u32); numeric (*with_u64)(numeric, u64); numeric (*with_f32)(numeric, f32); numeric (*with_f64)(numeric, f64); numeric (*with_bool)(numeric, bool); numeric (*with_num)(numeric, num); } f128_f, *f128_t; extern f128_f f128_type;
typedef struct cstr_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); } cstr_f, *cstr_t; extern cstr_f cstr_type;
typedef struct symbol_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); } symbol_f, *symbol_t; extern symbol_f symbol_type;
typedef struct bool_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); numeric (*with_i8)(numeric, i8); numeric (*with_i16)(numeric, i16); numeric (*with_i32)(numeric, i32); numeric (*with_i64)(numeric, i64); numeric (*with_u8)(numeric, u8); numeric (*with_u16)(numeric, u16); numeric (*with_u32)(numeric, u32); numeric (*with_u64)(numeric, u64); numeric (*with_f32)(numeric, f32); numeric (*with_f64)(numeric, f64); numeric (*with_bool)(numeric, bool); numeric (*with_num)(numeric, num); } bool_f, *bool_t; extern bool_f bool_type;
typedef struct none_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); } none_f, *none_t; extern none_f none_type;
typedef struct num_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); numeric (*with_i8)(numeric, i8); numeric (*with_i16)(numeric, i16); numeric (*with_i32)(numeric, i32); numeric (*with_i64)(numeric, i64); numeric (*with_u8)(numeric, u8); numeric (*with_u16)(numeric, u16); numeric (*with_u32)(numeric, u32); numeric (*with_u64)(numeric, u64); numeric (*with_f32)(numeric, f32); numeric (*with_f64)(numeric, f64); numeric (*with_bool)(numeric, bool); numeric (*with_num)(numeric, num); } num_f, *num_t; extern num_f num_type;
typedef struct sz_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); numeric (*with_i8)(numeric, i8); numeric (*with_i16)(numeric, i16); numeric (*with_i32)(numeric, i32); numeric (*with_i64)(numeric, i64); numeric (*with_u8)(numeric, u8); numeric (*with_u16)(numeric, u16); numeric (*with_u32)(numeric, u32); numeric (*with_u64)(numeric, u64); numeric (*with_f32)(numeric, f32); numeric (*with_f64)(numeric, f64); numeric (*with_bool)(numeric, bool); numeric (*with_num)(numeric, num); } sz_f, *sz_t; extern sz_f sz_type;
typedef struct raw_t_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); } raw_t_f, *raw_t_t; extern raw_t_f raw_t_type;
typedef struct handle_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); } handle_f, *handle_t; extern handle_f handle_type;


typedef struct AType_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); } AType_f, *AType_t; extern AType_f AType_type;
# 606 "/home/kalen/src/A/./src/A.h"

# 606 "/home/kalen/src/A/./src/A.h"
#pragma pack(push, 1)
# 606 "/home/kalen/src/A/./src/A.h"
 typedef struct item { struct item* next; struct item* prev; A val; A key; struct item_f* f; } *item;
# 606 "/home/kalen/src/A/./src/A.h"
#pragma pack(pop)
# 606 "/home/kalen/src/A/./src/A.h"
 typedef struct item_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(item); none (*destructor)(item); i32 (*compare)(item,item); u64 (*hash)(item); bool (*cast_bool)(item); item (*with_cstr)(item, cstr, num); item (*with_symbol)(item, symbol, A); } item_f, *item_t; extern item_f item_type;
# 616 "/home/kalen/src/A/./src/A.h"

# 616 "/home/kalen/src/A/./src/A.h"
#pragma pack(push, 1)
# 616 "/home/kalen/src/A/./src/A.h"
 typedef struct collection { struct collection_f* f; } *collection;
# 616 "/home/kalen/src/A/./src/A.h"
#pragma pack(pop)
# 616 "/home/kalen/src/A/./src/A.h"
 typedef struct collection_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(collection); none (*destructor)(collection); i32 (*compare)(collection,collection); u64 (*hash)(collection); bool (*cast_bool)(collection); collection (*with_cstr)(collection, cstr, num); } collection_f, *collection_t; extern collection_f collection_type;
# 631 "/home/kalen/src/A/./src/A.h"

# 631 "/home/kalen/src/A/./src/A.h"
#pragma pack(push, 1)
# 631 "/home/kalen/src/A/./src/A.h"
 typedef struct list { item first; item last; i64 count; i32 public_integer; struct list_f* f; } *list;
# 631 "/home/kalen/src/A/./src/A.h"
#pragma pack(pop)
# 631 "/home/kalen/src/A/./src/A.h"
 typedef struct list_f { struct collection_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(list); none (*destructor)(list); i32 (*compare)(list,list); u64 (*hash)(list); bool (*cast_bool)(list); list (*with_cstr)(list, cstr, num); A (*pop)(list); A (*push)(list, A); none (*remove)(list, num); none (*remove_item)(list, item); A (*get)(list, i32); num (*count)(list); } list_f, *list_t; extern list_f list_type;
# 642 "/home/kalen/src/A/./src/A.h"
typedef struct string* string;
# 665 "/home/kalen/src/A/./src/A.h"
typedef enum OPType { OPType_add, OPType_sub, OPType_mul, OPType_div, OPType_or, OPType_and, OPType_xor, OPType_right, OPType_left, OPType_assign, OPType_assign_add, OPType_assign_sub, OPType_assign_mul, OPType_assign_div, OPType_assign_or, OPType_assign_and, OPType_assign_xor, OPType_assign_right, OPType_assign_left, OPType_mod_assign, OPType_ENUM_COUNT } OPType; typedef struct OPType_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(OPType); none (*destructor)(OPType); i32 (*compare)(OPType,OPType); u64 (*hash)(OPType); bool (*cast_bool)(OPType); OPType (*with_cstr)(OPType, cstr, num); } OPType_f, *OPType_t; extern OPType_f OPType_type;
# 682 "/home/kalen/src/A/./src/A.h"

# 682 "/home/kalen/src/A/./src/A.h"
#pragma pack(push, 1)
# 682 "/home/kalen/src/A/./src/A.h"
 typedef struct path { cstr chars; struct path_f* f; } *path;
# 682 "/home/kalen/src/A/./src/A.h"
#pragma pack(pop)
# 682 "/home/kalen/src/A/./src/A.h"
 typedef struct path_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(path); none (*destructor)(path); i32 (*compare)(path,path); u64 (*hash)(path); bool (*cast_bool)(path); path (*with_cstr)(path, cstr, num); bool (*exists)(path); bool (*make_dir)(path); bool (*is_empty)(path); path (*directory)(path); string (*stem)(path); string (*filename)(path); path (*absolute)(path); path (*parent)(path); A (*read)(path, AType); path (*change_ext)(path, cstr); path (*cwd)(sz); } path_f, *path_t; extern path_f path_type;
# 705 "/home/kalen/src/A/./src/A.h"

# 705 "/home/kalen/src/A/./src/A.h"
#pragma pack(push, 1)
# 705 "/home/kalen/src/A/./src/A.h"
 typedef struct array { A* elements; i32 alloc; i32 len; struct array_f* f; } *array;
# 705 "/home/kalen/src/A/./src/A.h"
#pragma pack(pop)
# 705 "/home/kalen/src/A/./src/A.h"
 typedef struct array_f { struct collection_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(array); none (*destructor)(array); i32 (*compare)(array,array); u64 (*hash)(array); bool (*cast_bool)(array); array (*with_cstr)(array, cstr, num); array (*of_objects)(A, ...); A (*first)(array); A (*last)(array); array (*pop)(array); array (*remove)(array, num); A (*push)(array,A); A (*get)(array, i32); num (*count)(array); num (*index_of)(array, A); none (*operator_assign_add)(array, A); none (*operator_assign_sub)(array, num); none (*push_symbols)(array, array, cstr, ...); none (*push_objects)(array, array, A, ...); array (*with_sz)(array, sz); A (*index_num)(struct array_f*); } array_f, *array_t; extern array_f array_type;

typedef struct array* ATypes; typedef struct ATypes_f { struct array_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(A); none (*destructor)(A); i32 (*compare)(A,A); u64 (*hash)(A); bool (*cast_bool)(A); A (*with_cstr)(A, cstr, num); ATypes (*of_objects)(A, ...); A (*first)(ATypes); A (*last)(ATypes); ATypes (*pop)(ATypes); ATypes (*remove)(ATypes, num); A (*push)(ATypes,A); A (*get)(ATypes, i32); num (*count)(ATypes); num (*index_of)(ATypes, A); none (*operator_assign_add)(ATypes, A); none (*operator_assign_sub)(ATypes, num); none (*push_symbols)(ATypes, ATypes, cstr, ...); none (*push_objects)(ATypes, ATypes, A, ...); ATypes (*with_sz)(ATypes, sz); A (*index_num)(struct ATypes_f*); } ATypes_f, *ATypes_t; extern ATypes_f ATypes_type;
# 742 "/home/kalen/src/A/./src/A.h"

# 742 "/home/kalen/src/A/./src/A.h"
#pragma pack(push, 1)
# 742 "/home/kalen/src/A/./src/A.h"
 typedef struct hashmap { list* data; num alloc; num count; struct hashmap_f* f; } *hashmap;
# 742 "/home/kalen/src/A/./src/A.h"
#pragma pack(pop)
# 742 "/home/kalen/src/A/./src/A.h"
 typedef struct hashmap_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(hashmap); none (*destructor)(hashmap); i32 (*compare)(hashmap,hashmap); u64 (*hash)(hashmap); bool (*cast_bool)(hashmap); hashmap (*with_cstr)(hashmap, cstr, num); none (*remove)(hashmap, A); none (*set)(hashmap, A, A); A (*get)(hashmap, A); item (*fetch)(hashmap, A); item (*lookup)(hashmap, A); bool (*contains)(hashmap, A); hashmap (*with_sz)(hashmap, sz); A (*index_A)(struct hashmap_f*); } hashmap_f, *hashmap_t; extern hashmap_f hashmap_type;
# 756 "/home/kalen/src/A/./src/A.h"

# 756 "/home/kalen/src/A/./src/A.h"
#pragma pack(push, 1)
# 756 "/home/kalen/src/A/./src/A.h"
 typedef struct fn { method_t* method; A context; struct fn_f* f; } *fn;
# 756 "/home/kalen/src/A/./src/A.h"
#pragma pack(pop)
# 756 "/home/kalen/src/A/./src/A.h"
 typedef struct fn_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(fn); none (*destructor)(fn); i32 (*compare)(fn,fn); u64 (*hash)(fn); bool (*cast_bool)(fn); fn (*with_cstr)(fn, cstr, num); A (*call)(fn, array); fn (*with_ATypes)(fn, ATypes, AType, handle); } fn_f, *fn_t; extern fn_f fn_type;
# 772 "/home/kalen/src/A/./src/A.h"

# 772 "/home/kalen/src/A/./src/A.h"
#pragma pack(push, 1)
# 772 "/home/kalen/src/A/./src/A.h"
 typedef struct string { cstr chars; num alloc; num len; u64 h; struct string_f* f; } *string;
# 772 "/home/kalen/src/A/./src/A.h"
#pragma pack(pop)
# 772 "/home/kalen/src/A/./src/A.h"
 typedef struct string_f { struct A_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(string); none (*destructor)(string); i32 (*compare)(string,string); u64 (*hash)(string); bool (*cast_bool)(string); string (*with_cstr)(string, cstr, num); array (*split)(string, cstr); num (*index_of)(string, cstr); none (*append)(string, cstr); string (*mid)(string, num, num); string (*with_sz)(string, sz); } string_f, *string_t; extern string_f string_type;
# 783 "/home/kalen/src/A/./src/A.h"

# 783 "/home/kalen/src/A/./src/A.h"
#pragma pack(push, 1)
# 783 "/home/kalen/src/A/./src/A.h"
 typedef struct vector { struct vector_f* f; } *vector;
# 783 "/home/kalen/src/A/./src/A.h"
#pragma pack(pop)
# 783 "/home/kalen/src/A/./src/A.h"
 typedef struct vector_f { struct collection_f* parent_type; char* name; num size; num member_count; member_t* members; u64 traits; AType src; args_t meta; void* arb; none (*init)(vector); none (*destructor)(vector); i32 (*compare)(vector,vector); u64 (*hash)(vector); bool (*cast_bool)(vector); vector (*with_cstr)(vector, cstr, num); A (*pop)(vector); A (*push)(vector, A); A (*get)(vector, i32); num (*count)(vector); } vector_f, *vector_t; extern vector_f vector_type;
# 827 "/home/kalen/src/A/./src/A.h"
string format(cstr template, ...);
# 839 "/home/kalen/src/A/./src/A.h"
A A_alloc(AType type, num count);
A A_hold(A a);
void A_drop(A a);
A_f** A_types(num* length);
member_t* A_member(AType type, enum A_TYPE member_type, char* name);
A A_method(AType type, char* method_name, array args);
A A_primitive(AType type, void* data);
A A_enum(AType enum_type, i32 value);
A A_i8(i8);
A A_u8(u8);
A A_i16(i16);
A A_u16(u16);
A A_i32(i32);
A A_u32(u32);
A A_i64(i64);
A A_u64(u64);
A A_f32(f32);
A A_f64(f64);
A A_cstr(cstr);
A A_none();
A A_bool(bool);
A A_realloc(A, num);
void A_push(A, A);
void A_finish_types();
A hold(A a);
void drop(A a);
A fields(A instance);
A data(A instance);
bool is_meta(A a);
bool is_meta_compatible(A a, A b);
# 2 "/home/kalen/src/A/src/A.c" 2
# 1 "/usr/include/x86_64-linux-gnu/ffi.h" 1 3 4
# 59 "/usr/include/x86_64-linux-gnu/ffi.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/ffitarget.h" 1 3 4
# 75 "/usr/include/x86_64-linux-gnu/ffitarget.h" 3 4

# 75 "/usr/include/x86_64-linux-gnu/ffitarget.h" 3 4
typedef unsigned long ffi_arg;
typedef signed long ffi_sarg;



typedef enum ffi_abi {
# 93 "/usr/include/x86_64-linux-gnu/ffitarget.h" 3 4
  FFI_FIRST_ABI = 1,
  FFI_UNIX64,
  FFI_WIN64,
  FFI_EFI64 = FFI_WIN64,
  FFI_GNUW64,
  FFI_LAST_ABI,
  FFI_DEFAULT_ABI = FFI_UNIX64
# 124 "/usr/include/x86_64-linux-gnu/ffitarget.h" 3 4
} ffi_abi;
# 151 "/usr/include/x86_64-linux-gnu/ffitarget.h" 3 4
# 1 "/usr/lib/gcc/x86_64-linux-gnu/11/include/cet.h" 1 3 4
# 152 "/usr/include/x86_64-linux-gnu/ffitarget.h" 2 3 4
# 60 "/usr/include/x86_64-linux-gnu/ffi.h" 2 3 4







# 1 "/usr/lib/gcc/x86_64-linux-gnu/11/include/stddef.h" 1 3 4
# 68 "/usr/include/x86_64-linux-gnu/ffi.h" 2 3 4
# 1 "/usr/lib/gcc/x86_64-linux-gnu/11/include/limits.h" 1 3 4
# 34 "/usr/lib/gcc/x86_64-linux-gnu/11/include/limits.h" 3 4
# 1 "/usr/lib/gcc/x86_64-linux-gnu/11/include/syslimits.h" 1 3 4






# 1 "/usr/lib/gcc/x86_64-linux-gnu/11/include/limits.h" 1 3 4
# 203 "/usr/lib/gcc/x86_64-linux-gnu/11/include/limits.h" 3 4
# 1 "/usr/include/limits.h" 1 3 4
# 26 "/usr/include/limits.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/libc-header-start.h" 1 3 4
# 27 "/usr/include/limits.h" 2 3 4
# 195 "/usr/include/limits.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/posix1_lim.h" 1 3 4
# 27 "/usr/include/x86_64-linux-gnu/bits/posix1_lim.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 28 "/usr/include/x86_64-linux-gnu/bits/posix1_lim.h" 2 3 4
# 161 "/usr/include/x86_64-linux-gnu/bits/posix1_lim.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/local_lim.h" 1 3 4
# 38 "/usr/include/x86_64-linux-gnu/bits/local_lim.h" 3 4
# 1 "/usr/include/linux/limits.h" 1 3 4
# 39 "/usr/include/x86_64-linux-gnu/bits/local_lim.h" 2 3 4
# 81 "/usr/include/x86_64-linux-gnu/bits/local_lim.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/pthread_stack_min-dynamic.h" 1 3 4
# 29 "/usr/include/x86_64-linux-gnu/bits/pthread_stack_min-dynamic.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/pthread_stack_min.h" 1 3 4
# 30 "/usr/include/x86_64-linux-gnu/bits/pthread_stack_min-dynamic.h" 2 3 4
# 82 "/usr/include/x86_64-linux-gnu/bits/local_lim.h" 2 3 4
# 162 "/usr/include/x86_64-linux-gnu/bits/posix1_lim.h" 2 3 4
# 196 "/usr/include/limits.h" 2 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/posix2_lim.h" 1 3 4
# 200 "/usr/include/limits.h" 2 3 4
# 204 "/usr/lib/gcc/x86_64-linux-gnu/11/include/limits.h" 2 3 4
# 8 "/usr/lib/gcc/x86_64-linux-gnu/11/include/syslimits.h" 2 3 4
# 35 "/usr/lib/gcc/x86_64-linux-gnu/11/include/limits.h" 2 3 4
# 69 "/usr/include/x86_64-linux-gnu/ffi.h" 2 3 4
# 104 "/usr/include/x86_64-linux-gnu/ffi.h" 3 4
typedef struct _ffi_type
{
  size_t size;
  unsigned short alignment;
  unsigned short type;
  struct _ffi_type **elements;
} ffi_type;
# 188 "/usr/include/x86_64-linux-gnu/ffi.h" 3 4
extern ffi_type ffi_type_void;
extern ffi_type ffi_type_uint8;
extern ffi_type ffi_type_sint8;
extern ffi_type ffi_type_uint16;
extern ffi_type ffi_type_sint16;
extern ffi_type ffi_type_uint32;
extern ffi_type ffi_type_sint32;
extern ffi_type ffi_type_uint64;
extern ffi_type ffi_type_sint64;
extern ffi_type ffi_type_float;
extern ffi_type ffi_type_double;
extern ffi_type ffi_type_pointer;


extern ffi_type ffi_type_longdouble;





extern ffi_type ffi_type_complex_float;
extern ffi_type ffi_type_complex_double;

extern ffi_type ffi_type_complex_longdouble;






typedef enum {
  FFI_OK = 0,
  FFI_BAD_TYPEDEF,
  FFI_BAD_ABI,
  FFI_BAD_ARGTYPE
} ffi_status;

typedef struct {
  ffi_abi abi;
  unsigned nargs;
  ffi_type **arg_types;
  ffi_type *rtype;
  unsigned bytes;
  unsigned flags;



} ffi_cif;
# 251 "/usr/include/x86_64-linux-gnu/ffi.h" 3 4
typedef union {
  ffi_sarg sint;
  ffi_arg uint;
  float flt;
  char data[8];
  void* ptr;
} ffi_raw;
# 270 "/usr/include/x86_64-linux-gnu/ffi.h" 3 4
typedef ffi_raw ffi_java_raw;




void ffi_raw_call (ffi_cif *cif,
     void (*fn)(void),
     void *rvalue,
     ffi_raw *avalue);

 void ffi_ptrarray_to_raw (ffi_cif *cif, void **args, ffi_raw *raw);
 void ffi_raw_to_ptrarray (ffi_cif *cif, ffi_raw *raw, void **args);
 size_t ffi_raw_size (ffi_cif *cif);







void ffi_java_raw_call (ffi_cif *cif,
   void (*fn)(void),
   void *rvalue,
   ffi_java_raw *avalue) __attribute__((deprecated));



void ffi_java_ptrarray_to_raw (ffi_cif *cif, void **args, ffi_java_raw *raw) __attribute__((deprecated));

void ffi_java_raw_to_ptrarray (ffi_cif *cif, ffi_java_raw *raw, void **args) __attribute__((deprecated));

size_t ffi_java_raw_size (ffi_cif *cif) __attribute__((deprecated));
# 310 "/usr/include/x86_64-linux-gnu/ffi.h" 3 4
typedef struct {




  union {
    char tramp[32];
    void *ftramp;
  };

  ffi_cif *cif;
  void (*fun)(ffi_cif*,void*,void**,void*);
  void *user_data;
} ffi_closure

    __attribute__((aligned (8)))

    ;







 void *ffi_closure_alloc (size_t size, void **code);
 void ffi_closure_free (void *);
# 346 "/usr/include/x86_64-linux-gnu/ffi.h" 3 4
 ffi_status
ffi_prep_closure (ffi_closure*,
    ffi_cif *,
    void (*fun)(ffi_cif*,void*,void**,void*),
    void *user_data)

  __attribute__((deprecated ("use ffi_prep_closure_loc instead")))



  ;

 ffi_status
ffi_prep_closure_loc (ffi_closure*,
        ffi_cif *,
        void (*fun)(ffi_cif*,void*,void**,void*),
        void *user_data,
        void*codeloc);




typedef struct {




  char tramp[32];

  ffi_cif *cif;







  void (*translate_args)(ffi_cif*,void*,void**,void*);
  void *this_closure;



  void (*fun)(ffi_cif*,void*,ffi_raw*,void*);
  void *user_data;

} ffi_raw_closure;

typedef struct {




  char tramp[32];


  ffi_cif *cif;







  void (*translate_args)(ffi_cif*,void*,void**,void*);
  void *this_closure;



  void (*fun)(ffi_cif*,void*,ffi_java_raw*,void*);
  void *user_data;

} ffi_java_raw_closure;

 ffi_status
ffi_prep_raw_closure (ffi_raw_closure*,
        ffi_cif *cif,
        void (*fun)(ffi_cif*,void*,ffi_raw*,void*),
        void *user_data);

 ffi_status
ffi_prep_raw_closure_loc (ffi_raw_closure*,
     ffi_cif *cif,
     void (*fun)(ffi_cif*,void*,ffi_raw*,void*),
     void *user_data,
     void *codeloc);


 ffi_status
ffi_prep_java_raw_closure (ffi_java_raw_closure*,
             ffi_cif *cif,
             void (*fun)(ffi_cif*,void*,ffi_java_raw*,void*),
             void *user_data) __attribute__((deprecated));

 ffi_status
ffi_prep_java_raw_closure_loc (ffi_java_raw_closure*,
          ffi_cif *cif,
          void (*fun)(ffi_cif*,void*,ffi_java_raw*,void*),
          void *user_data,
          void *codeloc) __attribute__((deprecated));






typedef struct {
  void *tramp;
  ffi_cif *cif;
  void (*fun)(ffi_cif*,void*,void**,void*);
} ffi_go_closure;

 ffi_status ffi_prep_go_closure (ffi_go_closure*, ffi_cif *,
    void (*fun)(ffi_cif*,void*,void**,void*));

 void ffi_call_go (ffi_cif *cif, void (*fn)(void), void *rvalue,
    void **avalue, void *closure);






ffi_status ffi_prep_cif(ffi_cif *cif,
   ffi_abi abi,
   unsigned int nargs,
   ffi_type *rtype,
   ffi_type **atypes);


ffi_status ffi_prep_cif_var(ffi_cif *cif,
       ffi_abi abi,
       unsigned int nfixedargs,
       unsigned int ntotalargs,
       ffi_type *rtype,
       ffi_type **atypes);


void ffi_call(ffi_cif *cif,
       void (*fn)(void),
       void *rvalue,
       void **avalue);


ffi_status ffi_get_struct_offsets (ffi_abi abi, ffi_type *struct_type,
       size_t *offsets);
# 3 "/home/kalen/src/A/src/A.c" 2
# 1 "/usr/include/x86_64-linux-gnu/sys/stat.h" 1 3 4
# 99 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4


# 1 "/usr/include/x86_64-linux-gnu/bits/stat.h" 1 3 4
# 25 "/usr/include/x86_64-linux-gnu/bits/stat.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/struct_stat.h" 1 3 4
# 26 "/usr/include/x86_64-linux-gnu/bits/struct_stat.h" 3 4
struct stat
  {



    __dev_t st_dev;




    __ino_t st_ino;







    __nlink_t st_nlink;
    __mode_t st_mode;

    __uid_t st_uid;
    __gid_t st_gid;

    int __pad0;

    __dev_t st_rdev;




    __off_t st_size;



    __blksize_t st_blksize;

    __blkcnt_t st_blocks;
# 74 "/usr/include/x86_64-linux-gnu/bits/struct_stat.h" 3 4
    struct timespec st_atim;
    struct timespec st_mtim;
    struct timespec st_ctim;
# 89 "/usr/include/x86_64-linux-gnu/bits/struct_stat.h" 3 4
    __syscall_slong_t __glibc_reserved[3];
# 99 "/usr/include/x86_64-linux-gnu/bits/struct_stat.h" 3 4
  };
# 26 "/usr/include/x86_64-linux-gnu/bits/stat.h" 2 3 4
# 102 "/usr/include/x86_64-linux-gnu/sys/stat.h" 2 3 4
# 205 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4
extern int stat (const char *__restrict __file,
   struct stat *__restrict __buf) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));



extern int fstat (int __fd, struct stat *__buf) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));
# 264 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4
extern int fstatat (int __fd, const char *__restrict __file,
      struct stat *__restrict __buf, int __flag)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 3)));
# 313 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4
extern int lstat (const char *__restrict __file,
    struct stat *__restrict __buf) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
# 352 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4
extern int chmod (const char *__file, __mode_t __mode)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern int lchmod (const char *__file, __mode_t __mode)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));




extern int fchmod (int __fd, __mode_t __mode) __attribute__ ((__nothrow__ , __leaf__));





extern int fchmodat (int __fd, const char *__file, __mode_t __mode,
       int __flag)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2))) ;






extern __mode_t umask (__mode_t __mask) __attribute__ ((__nothrow__ , __leaf__));
# 389 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4
extern int mkdir (const char *__path, __mode_t __mode)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern int mkdirat (int __fd, const char *__path, __mode_t __mode)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));






extern int mknod (const char *__path, __mode_t __mode, __dev_t __dev)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern int mknodat (int __fd, const char *__path, __mode_t __mode,
      __dev_t __dev) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));





extern int mkfifo (const char *__path, __mode_t __mode)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern int mkfifoat (int __fd, const char *__path, __mode_t __mode)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));






extern int utimensat (int __fd, const char *__path,
        const struct timespec __times[2],
        int __flags)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));
# 452 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4
extern int futimens (int __fd, const struct timespec __times[2]) __attribute__ ((__nothrow__ , __leaf__));
# 468 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4

# 4 "/home/kalen/src/A/src/A.c" 2
# 1 "/usr/include/dirent.h" 1 3 4
# 27 "/usr/include/dirent.h" 3 4

# 61 "/usr/include/dirent.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/dirent.h" 1 3 4
# 22 "/usr/include/x86_64-linux-gnu/bits/dirent.h" 3 4
struct dirent
  {

    __ino_t d_ino;
    __off_t d_off;




    unsigned short int d_reclen;
    unsigned char d_type;
    char d_name[256];
  };
# 62 "/usr/include/dirent.h" 2 3 4
# 97 "/usr/include/dirent.h" 3 4
enum
  {
    DT_UNKNOWN = 0,

    DT_FIFO = 1,

    DT_CHR = 2,

    DT_DIR = 4,

    DT_BLK = 6,

    DT_REG = 8,

    DT_LNK = 10,

    DT_SOCK = 12,

    DT_WHT = 14

  };
# 127 "/usr/include/dirent.h" 3 4
typedef struct __dirstream DIR;






extern DIR *opendir (const char *__name) __attribute__ ((__nonnull__ (1)));






extern DIR *fdopendir (int __fd);







extern int closedir (DIR *__dirp) __attribute__ ((__nonnull__ (1)));
# 162 "/usr/include/dirent.h" 3 4
extern struct dirent *readdir (DIR *__dirp) __attribute__ ((__nonnull__ (1)));
# 183 "/usr/include/dirent.h" 3 4
extern int readdir_r (DIR *__restrict __dirp,
        struct dirent *__restrict __entry,
        struct dirent **__restrict __result)
     __attribute__ ((__nonnull__ (1, 2, 3))) __attribute__ ((__deprecated__));
# 209 "/usr/include/dirent.h" 3 4
extern void rewinddir (DIR *__dirp) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern void seekdir (DIR *__dirp, long int __pos) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern long int telldir (DIR *__dirp) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern int dirfd (DIR *__dirp) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
# 245 "/usr/include/dirent.h" 3 4
# 1 "/usr/lib/gcc/x86_64-linux-gnu/11/include/stddef.h" 1 3 4
# 246 "/usr/include/dirent.h" 2 3 4
# 255 "/usr/include/dirent.h" 3 4
extern int scandir (const char *__restrict __dir,
      struct dirent ***__restrict __namelist,
      int (*__selector) (const struct dirent *),
      int (*__cmp) (const struct dirent **,
      const struct dirent **))
     __attribute__ ((__nonnull__ (1, 2)));
# 325 "/usr/include/dirent.h" 3 4
extern int alphasort (const struct dirent **__e1,
        const struct dirent **__e2)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 353 "/usr/include/dirent.h" 3 4
extern __ssize_t getdirentries (int __fd, char *__restrict __buf,
    size_t __nbytes,
    __off_t *__restrict __basep)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 4)));
# 402 "/usr/include/dirent.h" 3 4


# 1 "/usr/include/x86_64-linux-gnu/bits/dirent_ext.h" 1 3 4
# 23 "/usr/include/x86_64-linux-gnu/bits/dirent_ext.h" 3 4

# 33 "/usr/include/x86_64-linux-gnu/bits/dirent_ext.h" 3 4

# 405 "/usr/include/dirent.h" 2 3 4
# 5 "/home/kalen/src/A/src/A.c" 2


# 6 "/home/kalen/src/A/src/A.c"
static global_init_fn* call_after;
static num call_after_alloc;
static num call_after_count;
static A_f** types;
static num types_alloc;
static num types_len;

void A_lazy_init(global_init_fn fn) {
    if (call_after_count == call_after_alloc) {
        global_init_fn prev = call_after;
        num alloc_prev = call_after_alloc;
        call_after = calloc(128 + (call_after_alloc << 1), sizeof(global_init_fn));
        if (prev) {
            memcpy(call_after, prev, sizeof(global_init_fn) * alloc_prev);
            free(prev);
        }
    }
    call_after[call_after_count++] = fn;
}

void A_push_type(A_f* type) {
    if (types_alloc == types_len) {
        A_f** prev = types;
        num alloc_prev = types_alloc;
        types_alloc = 128 + (types_alloc << 1);
        types = calloc(types_alloc, sizeof(A_f*));
        if (alloc_prev) {
            memcpy(types, prev, sizeof(A_f*) * alloc_prev);
            free(prev);
        }
    }
    types[types_len++] = type;
}

A_f** A_types(num* length) {
    *length = types_len;
    return types;
}

A A_alloc(A_f* type, num count) {
    A a = calloc(1, (type == ((struct A_f*)&A_type) ? 0 : sizeof(struct A)) + type->size * count);
    a->type = type;
    a->origin = a;
    a->data = &a[1];
    a->count = count;
    a->alloc = count;
    A_f* a_type = &A_type;
    A_f* current = type;
    while (current) {
        if (current->init)
            current->init(a->data);
        if (current == a_type)
            break;
        current = current->parent_type;
    }
    return a->data;
}

A A_realloc(A a, num alloc) {
    A obj = fields(a);
    
# 66 "/home/kalen/src/A/src/A.c" 3 4
   ((void) sizeof ((
# 66 "/home/kalen/src/A/src/A.c"
   obj->type->traits == A_TRAIT_PRIMITIVE
# 66 "/home/kalen/src/A/src/A.c" 3 4
   ) ? 1 : 0), __extension__ ({ if (
# 66 "/home/kalen/src/A/src/A.c"
   obj->type->traits == A_TRAIT_PRIMITIVE
# 66 "/home/kalen/src/A/src/A.c" 3 4
   ) ; else __assert_fail (
# 66 "/home/kalen/src/A/src/A.c"
   "obj->type->traits == A_TRAIT_PRIMITIVE"
# 66 "/home/kalen/src/A/src/A.c" 3 4
   , "/home/kalen/src/A/src/A.c", 66, __extension__ __PRETTY_FUNCTION__); }))
# 66 "/home/kalen/src/A/src/A.c"
                                                 ;
    A re = calloc(1, sizeof(struct A) + obj->type->size * alloc);
    num count = obj->count < alloc ? obj->count : alloc;
    memcpy(&re[1], obj->data, obj->type->size * count);
    if (obj->data != &obj[1])
        free(&obj->data[-1]);
    re->origin = obj;
    obj->data = &re[1];
    obj->count = count;
    obj->alloc = alloc;
    return obj->data;
}

void A_push(A a, A value) {
    A obj = fields(a);
    
# 81 "/home/kalen/src/A/src/A.c" 3 4
   ((void) sizeof ((
# 81 "/home/kalen/src/A/src/A.c"
   obj->type->traits == A_TRAIT_PRIMITIVE
# 81 "/home/kalen/src/A/src/A.c" 3 4
   ) ? 1 : 0), __extension__ ({ if (
# 81 "/home/kalen/src/A/src/A.c"
   obj->type->traits == A_TRAIT_PRIMITIVE
# 81 "/home/kalen/src/A/src/A.c" 3 4
   ) ; else __assert_fail (
# 81 "/home/kalen/src/A/src/A.c"
   "obj->type->traits == A_TRAIT_PRIMITIVE"
# 81 "/home/kalen/src/A/src/A.c" 3 4
   , "/home/kalen/src/A/src/A.c", 81, __extension__ __PRETTY_FUNCTION__); }))
# 81 "/home/kalen/src/A/src/A.c"
                                                 ;
    num sz = obj->type->size;
    if (obj->count == obj->alloc)
        A_realloc(a, 32 + obj->alloc << 1);
    memcpy(&((u8*)obj->data)[obj->count++ * sz], value, sz);
}

method_t* method_with_address(handle address, AType rtype, array atypes) {
    const num max_args = 8;
    method_t* method = calloc(1, sizeof(method_t));
    method->ffi_cif = calloc(1, sizeof(ffi_cif));
    method->ffi_args = calloc(max_args, sizeof(ffi_type*));
    method->atypes = ((A)((A)A_alloc(((struct A_f*)&array_type), 1)));
    method->rtype = rtype;
    ffi_type **ffi_args = (ffi_type**)method->ffi_args;
    for (num i = 0; i < atypes->len; i++) {
        A_f* a_type = atypes->elements[i];
        bool is_prim = a_type->traits & A_TRAIT_PRIMITIVE;
        ffi_args[i] = is_prim ? a_type->arb : &ffi_type_pointer;
        array_type.push(method->atypes, a_type);
    }
    ffi_status status = ffi_prep_cif(
        (ffi_cif*) method->ffi_cif, FFI_DEFAULT_ABI, atypes->len,
        (ffi_type*)rtype->arb, ffi_args);
    
# 105 "/home/kalen/src/A/src/A.c" 3 4
   ((void) sizeof ((
# 105 "/home/kalen/src/A/src/A.c"
   status == FFI_OK
# 105 "/home/kalen/src/A/src/A.c" 3 4
   ) ? 1 : 0), __extension__ ({ if (
# 105 "/home/kalen/src/A/src/A.c"
   status == FFI_OK
# 105 "/home/kalen/src/A/src/A.c" 3 4
   ) ; else __assert_fail (
# 105 "/home/kalen/src/A/src/A.c"
   "status == FFI_OK"
# 105 "/home/kalen/src/A/src/A.c" 3 4
   , "/home/kalen/src/A/src/A.c", 105, __extension__ __PRETTY_FUNCTION__); }))
# 105 "/home/kalen/src/A/src/A.c"
                           ;
    return method;
}

A method_call(method_t* a, array args) {
    const num max_args = 8;
    void* arg_values[max_args];
    
# 112 "/home/kalen/src/A/src/A.c" 3 4
   ((void) sizeof ((
# 112 "/home/kalen/src/A/src/A.c"
   args->len == a->atypes->len
# 112 "/home/kalen/src/A/src/A.c" 3 4
   ) ? 1 : 0), __extension__ ({ if (
# 112 "/home/kalen/src/A/src/A.c"
   args->len == a->atypes->len
# 112 "/home/kalen/src/A/src/A.c" 3 4
   ) ; else __assert_fail (
# 112 "/home/kalen/src/A/src/A.c"
   "args->len == a->atypes->len"
# 112 "/home/kalen/src/A/src/A.c" 3 4
   , "/home/kalen/src/A/src/A.c", 112, __extension__ __PRETTY_FUNCTION__); }))
# 112 "/home/kalen/src/A/src/A.c"
                                      ;
    for (num i = 0; i < args->len; i++) {
        A_f* arg_type = a->atypes->elements[i];
        arg_values[i] = (arg_type->traits & (A_TRAIT_PRIMITIVE | A_TRAIT_ENUM)) ?
            (void*)args->elements[i] : (void*)&args->elements[i];
    }
    void* result[8];
    ffi_call((ffi_cif*)a->ffi_cif, a->address, result, arg_values);
    if (a->rtype->traits & A_TRAIT_PRIMITIVE)
        return A_primitive(a->rtype, result);
    else if (a->rtype->traits & A_TRAIT_ENUM)
        return A_enum(a->rtype, *(i32*)result);
    else
        return (A) result[0];
}


A A_method(A_f* type, cstr method_name, array args) {
    member_t* mem = A_member(type, A_TYPE_IMETHOD | A_TYPE_SMETHOD, method_name);
    
# 131 "/home/kalen/src/A/src/A.c" 3 4
   ((void) sizeof ((
# 131 "/home/kalen/src/A/src/A.c"
   mem->method
# 131 "/home/kalen/src/A/src/A.c" 3 4
   ) ? 1 : 0), __extension__ ({ if (
# 131 "/home/kalen/src/A/src/A.c"
   mem->method
# 131 "/home/kalen/src/A/src/A.c" 3 4
   ) ; else __assert_fail (
# 131 "/home/kalen/src/A/src/A.c"
   "mem->method"
# 131 "/home/kalen/src/A/src/A.c" 3 4
   , "/home/kalen/src/A/src/A.c", 131, __extension__ __PRETTY_FUNCTION__); }))
# 131 "/home/kalen/src/A/src/A.c"
                      ;
    return method_call(mem->method, args);
}

void A_finish_types() {
    int remaining = call_after_count;
    while (remaining)
        for (int i = 0; i < call_after_count; i++) {
            global_init_fn fn = call_after[i];
            if (fn && fn()) {
                call_after[i] = ((void*)0);
                remaining--;
            }
        }

    num types_len;
    A_f** types = A_types(&types_len);
    const num max_args = 8;


    for (num i = 0; i < types_len; i++) {
        A_f* type = types[i];

        for (num m = 0; m < type->member_count; m++) {
            member_t* mem = &type->members[m];
            if (mem->member_type & (A_TYPE_IMETHOD | A_TYPE_SMETHOD | A_TYPE_CONSTRUCT)) {
                void* address = 0;
                memcpy(&address, &((u8*)type)[mem->offset], sizeof(void*));
                
# 159 "/home/kalen/src/A/src/A.c" 3 4
               ((void) sizeof ((
# 159 "/home/kalen/src/A/src/A.c"
               address
# 159 "/home/kalen/src/A/src/A.c" 3 4
               ) ? 1 : 0), __extension__ ({ if (
# 159 "/home/kalen/src/A/src/A.c"
               address
# 159 "/home/kalen/src/A/src/A.c" 3 4
               ) ; else __assert_fail (
# 159 "/home/kalen/src/A/src/A.c"
               "address"
# 159 "/home/kalen/src/A/src/A.c" 3 4
               , "/home/kalen/src/A/src/A.c", 159, __extension__ __PRETTY_FUNCTION__); }))
# 159 "/home/kalen/src/A/src/A.c"
                              ;
                array args = array_type.with_sz((A)((A)A_alloc(((struct A_f*)&array_type), 1)), mem->args.count);
                for (num i = 0; i < mem->args.count; i++)
                    args->elements[i] = ((A_f**)&mem->args.arg_0)[i];
                args->len = mem->args.count;
                mem->method = method_with_address(address, mem->type, args);
            }
        }
    }
}

member_t* A_member(A_f* type, enum A_TYPE member_type, char* name) {
    for (num i = 0; i < type->member_count; i++) {
        member_t* mem = &type->members[i];
        if (mem->member_type & member_type && strcmp(mem->name, name) == 0)
            return mem;
    }
    return 0;
}





A A_primitive(A_f* type, void* data) {
    
# 184 "/home/kalen/src/A/src/A.c" 3 4
   ((void) sizeof ((
# 184 "/home/kalen/src/A/src/A.c"
   type->traits & A_TRAIT_PRIMITIVE
# 184 "/home/kalen/src/A/src/A.c" 3 4
   ) ? 1 : 0), __extension__ ({ if (
# 184 "/home/kalen/src/A/src/A.c"
   type->traits & A_TRAIT_PRIMITIVE
# 184 "/home/kalen/src/A/src/A.c" 3 4
   ) ; else __assert_fail (
# 184 "/home/kalen/src/A/src/A.c"
   "type->traits & A_TRAIT_PRIMITIVE"
# 184 "/home/kalen/src/A/src/A.c" 3 4
   , "/home/kalen/src/A/src/A.c", 184, __extension__ __PRETTY_FUNCTION__); }))
# 184 "/home/kalen/src/A/src/A.c"
                                           ;
    A copy = A_alloc(type, type->size);
    memcpy(copy, data, type->size);
    return copy;
}

A A_enum(A_f* type, i32 data) {
    
# 191 "/home/kalen/src/A/src/A.c" 3 4
   ((void) sizeof ((
# 191 "/home/kalen/src/A/src/A.c"
   type->traits & A_TRAIT_ENUM
# 191 "/home/kalen/src/A/src/A.c" 3 4
   ) ? 1 : 0), __extension__ ({ if (
# 191 "/home/kalen/src/A/src/A.c"
   type->traits & A_TRAIT_ENUM
# 191 "/home/kalen/src/A/src/A.c" 3 4
   ) ; else __assert_fail (
# 191 "/home/kalen/src/A/src/A.c"
   "type->traits & A_TRAIT_ENUM"
# 191 "/home/kalen/src/A/src/A.c" 3 4
   , "/home/kalen/src/A/src/A.c", 191, __extension__ __PRETTY_FUNCTION__); }))
# 191 "/home/kalen/src/A/src/A.c"
                                      ;
    
# 192 "/home/kalen/src/A/src/A.c" 3 4
   ((void) sizeof ((
# 192 "/home/kalen/src/A/src/A.c"
   type->size == sizeof(i32)
# 192 "/home/kalen/src/A/src/A.c" 3 4
   ) ? 1 : 0), __extension__ ({ if (
# 192 "/home/kalen/src/A/src/A.c"
   type->size == sizeof(i32)
# 192 "/home/kalen/src/A/src/A.c" 3 4
   ) ; else __assert_fail (
# 192 "/home/kalen/src/A/src/A.c"
   "type->size == sizeof(i32)"
# 192 "/home/kalen/src/A/src/A.c" 3 4
   , "/home/kalen/src/A/src/A.c", 192, __extension__ __PRETTY_FUNCTION__); }))
# 192 "/home/kalen/src/A/src/A.c"
                                    ;
    A copy = A_alloc(type, type->size);
    memcpy(copy, &data, type->size);
    return copy;
}

A A_i8(i8 data) { return A_primitive(&i8_type, &data); }
A A_u8(u8 data) { return A_primitive(&u8_type, &data); }
A A_i16(i16 data) { return A_primitive(&i16_type, &data); }
A A_u16(u16 data) { return A_primitive(&u16_type, &data); }
A A_i32(i32 data) { return A_primitive(&i32_type, &data); }
A A_u32(u32 data) { return A_primitive(&u32_type, &data); }
A A_i64(i64 data) { return A_primitive(&i64_type, &data); }
A A_u64(u64 data) { return A_primitive(&u64_type, &data); }
A A_f32(f32 data) { return A_primitive(&f32_type, &data); }
A A_f64(f64 data) { return A_primitive(&f64_type, &data); }
A A_cstr(cstr data) { return A_primitive(&cstr_type, &data); }
A A_none() { return A_primitive(&none_type, &data); }
A A_bool(bool data) { return A_primitive(&bool_type, &data); }


A A_hold(A a) { ++a->refs; return a; }
static A A_new_default(AType type, num count) {
    return A_alloc(type, count);
}
static void A_init (A a) { }
static void A_destructor(A a) {

    AType type = ((AType)((A)a)[-1].type);
    for (num i = 0; i < type->member_count; i++) {
        member_t* m = &type->members[i];
        if ((m->member_type == A_TYPE_PROP || m->member_type == A_TYPE_PRIV) && !(m->type->traits & A_TRAIT_PRIMITIVE)) {
            u8* ptr = (u8*)a + m->offset;
            A ref = ptr;
            drop(ref);
            *((A*)&ptr) = ((void*)0);
        }
    }
}
static u64 A_hash (A a) { return (u64)(size_t)a; }
static bool A_cast_bool (A a) { return (bool)(size_t)a; }

static A A_with_cstr(A a, cstr cs, num len) {
    A f = fields(a);
    AType type = f->type;
    if (type == ((struct A_f*)&f64_type)) sscanf(cs, "%lf", (f64*)a);
    else if (type == ((struct A_f*)&f32_type)) sscanf(cs, "%f", (f32*)a);
    else if (type == ((struct A_f*)&i32_type)) sscanf(cs, "%i", (i32*)a);
    else if (type == ((struct A_f*)&u32_type)) sscanf(cs, "%u", (u32*)a);
    else if (type == ((struct A_f*)&i64_type)) sscanf(cs, "%lli", (i64*)a);
    else if (type == ((struct A_f*)&u64_type)) sscanf(cs, "%llu", (u64*)a);
    else {
        printf("implement construct cstr for %s\n", f->type->name);
        exit(-1);
    }

    return a;
}




static A numeric_with_i8 (A a, i8 v) { *(i8*)&a = v; }
static A numeric_with_i16(A a, i16 v) { *(i16*)&a = v; }
static A numeric_with_i32(A a, i32 v) { *(i32*)&a = v; }
static A numeric_with_i64(A a, i64 v) { *(i64*)&a = v; }
static A numeric_with_u8 (A a, u8 v) { *(u8*)&a = v; }
static A numeric_with_u16(A a, u16 v) { *(u16*)&a = v; }
static A numeric_with_u32(A a, u32 v) { *(u32*)&a = v; }
static A numeric_with_u64(A a, u64 v) { *(u64*)&a = v; }
static A numeric_with_f32(A a, f32 v) { *(f32*)&a = v; }
static A numeric_with_f64(A a, f64 v) { *(f64*)&a = v; }
static A numeric_with_bool(A a, bool v) { *(bool*)&a = v; }
static A numeric_with_num(A a, num v) { *(num*)&a = v; }

A A_method(A_f* type, char* method_name, array args);


static i32 A_compare(A a, A b) {
    return (i32)(a - b);
}

string format(cstr template, ...) {
    va_list args;
    
# 276 "/home/kalen/src/A/src/A.c" 3 4
   __builtin_va_start(
# 276 "/home/kalen/src/A/src/A.c"
   args
# 276 "/home/kalen/src/A/src/A.c" 3 4
   ,
# 276 "/home/kalen/src/A/src/A.c"
   template
# 276 "/home/kalen/src/A/src/A.c" 3 4
   )
# 276 "/home/kalen/src/A/src/A.c"
                           ;
    char buf[1024];
    sz len = vsnprintf(buf, sizeof(buf), template, args);
    
# 279 "/home/kalen/src/A/src/A.c" 3 4
   __builtin_va_end(
# 279 "/home/kalen/src/A/src/A.c"
   args
# 279 "/home/kalen/src/A/src/A.c" 3 4
   )
# 279 "/home/kalen/src/A/src/A.c"
               ;
    return string_type.with_cstr((A)((A)A_alloc(((struct A_f*)&string_type), 1)), buf, len);
}

static void string_destructor(string a) { free(a->chars); }
static num string_compare(string a, string b) { return strcmp(a->chars, b->chars); }

static array string_split(string a, cstr sp) {
    cstr next = a->chars;
    sz slen = strlen(sp);
    while (next) {
        cstr n = strstr(&next[1], sp);
        string v = string_type.with_cstr((A)((A)A_alloc(((struct A_f*)&string_type), 1)), next, n ? (sz)(next - n) : -1);
        next = n ? n + slen : ((void*)0);
    }
    return ((void*)0);
}

static void string_alloc_sz(string a, sz alloc) {
    char* chars = calloc(alloc, sizeof(char));
    memcpy(chars, a->chars, sizeof(char) * a->len);
    chars[a->len] = 0;
    free(a->chars);
    a->chars = chars;
    a->alloc = alloc;
}

static string string_mid(string a, num start, num len) {
    return string_type.with_cstr((A)((A)A_alloc(((struct A_f*)&string_type), 1)), &a->chars[start], len);
}

static none string_append(string a, cstr b) {
    sz blen = strlen(b);
    string_alloc_sz(a, (a->alloc << 1) + blen);
    memcpy(&a->chars[a->len], b, blen);
    a->len += blen;
    a->chars[a->len] = 0;
}

static num string_index_of(string a, cstr cs) {
    char* f = strstr(a->chars, cs);
    return f ? (num)(f - a->chars) : (num)-1;
}

static bool string_cast_bool(string a) {
    return a->len > 0;
}

u64 fnv1a_hash(const void* data, size_t length, u64 hash) {
    const u8* bytes = (const u8*)data;
    for (size_t i = 0; i < length; ++i) {
        hash ^= bytes[i];
        hash *= 0x100000001b3;
    }
    return hash;
}

static u64 item_hash(item f) {
    return A_type.hash(f->key ? f->key : f->val);
}

static u64 string_hash(string a) {
    if (a->h) return a->h;
    a->h = fnv1a_hash(a->chars, a->len, 0xcbf29ce484222325);
    return a->h;
}

static string string_with_sz(string a, sz size) {
    a->alloc = size;
    a->chars = (char*)calloc(1, a->alloc);
    return a;
}

static string string_with_cstr(string a, cstr value, num len) {
    if (len == -1) len = strlen(value);
    a->alloc = len + 1;
    a->len = len;
    a->chars = (char*)calloc(1, a->alloc);
    memcpy(a->chars, value, len);
    a->chars[len] = 0;
    return a;
}

static item hashmap_fetch(hashmap a, A key) {
    u64 h = fnv1a_hash(key, strlen(key), 0xcbf29ce484222325);
    u64 k = h % a->alloc;
    list bucket = a->data[k];
    for (item f = bucket->first; f; f = f->next)
        if (((__typeof__((f->key)->f)) ((A)(f->key))[-1].type) -> compare(f->key, key) == 0)
            return f;
    item n = ((__typeof__((bucket)->f)) ((A)(bucket))[-1].type) -> push(bucket, ((void*)0));
    n->key = hold(key);
    a->count++;
    return n;
}

static item hashmap_lookup(hashmap a, A key) {
    u64 h = fnv1a_hash(key, strlen(key), 0xcbf29ce484222325);
    u64 k = h % a->alloc;
    list bucket = a->data[k];
    for (item f = bucket->first; f; f = f->next)
        if (((__typeof__((f->key)->f)) ((A)(f->key))[-1].type) -> compare(f->key, key) == 0)
            return f;
    return ((void*)0);
}

static none hashmap_set(hashmap a, A key, A value) {
    item i = ((__typeof__((a)->f)) ((A)(a))[-1].type) -> fetch(a, key);
    A prev = i->val;
    i->val = hold(value);
    drop(prev);
}

static A hashmap_get(hashmap a, A key) {
    item i = ((__typeof__((a)->f)) ((A)(a))[-1].type) -> lookup(a, key);
    return i ? i->val : ((void*)0);
}

static bool hashmap_contains(hashmap a, A key) {
    item i = ((__typeof__((a)->f)) ((A)(a))[-1].type) -> lookup(a, key);
    return i != ((void*)0);
}

static none hashmap_remove(hashmap a, A key) {
    u64 h = fnv1a_hash(key, strlen(key), 0xcbf29ce484222325);
    u64 k = h % a->alloc;
    list bucket = a->data[k];
    for (item f = bucket->first; f; f = f->next)
        if (((__typeof__((f->key)->f)) ((A)(f->key))[-1].type) -> compare(f->key, key) == 0) {
            ((__typeof__((bucket)->f)) ((A)(bucket))[-1].type) -> remove_item(bucket, f);
            a->count--;
            break;
        }
}

static bool hashmap_cast_bool(hashmap a) {
    return a->count > 0;
}

static A hashmap_index_A(hashmap a, A key) {
    return ((__typeof__((a)->f)) ((A)(a))[-1].type) -> get(a, key);
}

static hashmap hashmap_with_sz(hashmap a, sz size) {
    a->alloc = size;
    a->data = (list*)calloc(size, sizeof(list));
    return a;
}

A hashmap_header; hashmap_f hashmap_type; static __attribute__((constructor)) bool global_hashmap() { hashmap_f* type_ref = &hashmap_type; A_f* base_ref = &A_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_hashmap); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(A_f)); static member_t members[16 + sizeof(hashmap_type) / sizeof(void*)]; hashmap_type.parent_type = &A_type; hashmap_type.name = "hashmap"; hashmap_type.size = sizeof(struct hashmap); hashmap_type.members = members; hashmap_type.traits = 0; hashmap_type.arb = primitive_ffi_arb(((struct A_f*)&cstr_type)); hashmap_type . remove = & hashmap_remove; hashmap_type.members[hashmap_type.member_count].name = "remove"; hashmap_type.members[hashmap_type.member_count].args = (args_t) { 3, &hashmap_type, &hashmap_type, &A_type }; hashmap_type.members[hashmap_type.member_count].type = &none_type; hashmap_type.members[hashmap_type.member_count].offset = 
# 428 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 428 "/home/kalen/src/A/src/A.c"
hashmap_f
# 428 "/home/kalen/src/A/src/A.c" 3 4
, 
# 428 "/home/kalen/src/A/src/A.c"
remove
# 428 "/home/kalen/src/A/src/A.c" 3 4
)
# 428 "/home/kalen/src/A/src/A.c"
; hashmap_type.members[hashmap_type.member_count].member_type = A_TYPE_IMETHOD; hashmap_type.member_count++; hashmap_type . set = & hashmap_set; hashmap_type.members[hashmap_type.member_count].name = "set"; hashmap_type.members[hashmap_type.member_count].args = (args_t) { 4, &hashmap_type, &hashmap_type, &A_type, &A_type }; hashmap_type.members[hashmap_type.member_count].type = &none_type; hashmap_type.members[hashmap_type.member_count].offset = 
# 428 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 428 "/home/kalen/src/A/src/A.c"
hashmap_f
# 428 "/home/kalen/src/A/src/A.c" 3 4
, 
# 428 "/home/kalen/src/A/src/A.c"
set
# 428 "/home/kalen/src/A/src/A.c" 3 4
)
# 428 "/home/kalen/src/A/src/A.c"
; hashmap_type.members[hashmap_type.member_count].member_type = A_TYPE_IMETHOD; hashmap_type.member_count++; hashmap_type . get = & hashmap_get; hashmap_type.members[hashmap_type.member_count].name = "get"; hashmap_type.members[hashmap_type.member_count].args = (args_t) { 3, &hashmap_type, &hashmap_type, &A_type }; hashmap_type.members[hashmap_type.member_count].type = &A_type; hashmap_type.members[hashmap_type.member_count].offset = 
# 428 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 428 "/home/kalen/src/A/src/A.c"
hashmap_f
# 428 "/home/kalen/src/A/src/A.c" 3 4
, 
# 428 "/home/kalen/src/A/src/A.c"
get
# 428 "/home/kalen/src/A/src/A.c" 3 4
)
# 428 "/home/kalen/src/A/src/A.c"
; hashmap_type.members[hashmap_type.member_count].member_type = A_TYPE_IMETHOD; hashmap_type.member_count++; hashmap_type . fetch = & hashmap_fetch; hashmap_type.members[hashmap_type.member_count].name = "fetch"; hashmap_type.members[hashmap_type.member_count].args = (args_t) { 3, &hashmap_type, &hashmap_type, &A_type }; hashmap_type.members[hashmap_type.member_count].type = &item_type; hashmap_type.members[hashmap_type.member_count].offset = 
# 428 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 428 "/home/kalen/src/A/src/A.c"
hashmap_f
# 428 "/home/kalen/src/A/src/A.c" 3 4
, 
# 428 "/home/kalen/src/A/src/A.c"
fetch
# 428 "/home/kalen/src/A/src/A.c" 3 4
)
# 428 "/home/kalen/src/A/src/A.c"
; hashmap_type.members[hashmap_type.member_count].member_type = A_TYPE_IMETHOD; hashmap_type.member_count++; hashmap_type . lookup = & hashmap_lookup; hashmap_type.members[hashmap_type.member_count].name = "lookup"; hashmap_type.members[hashmap_type.member_count].args = (args_t) { 3, &hashmap_type, &hashmap_type, &A_type }; hashmap_type.members[hashmap_type.member_count].type = &item_type; hashmap_type.members[hashmap_type.member_count].offset = 
# 428 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 428 "/home/kalen/src/A/src/A.c"
hashmap_f
# 428 "/home/kalen/src/A/src/A.c" 3 4
, 
# 428 "/home/kalen/src/A/src/A.c"
lookup
# 428 "/home/kalen/src/A/src/A.c" 3 4
)
# 428 "/home/kalen/src/A/src/A.c"
; hashmap_type.members[hashmap_type.member_count].member_type = A_TYPE_IMETHOD; hashmap_type.member_count++; hashmap_type . contains = & hashmap_contains; hashmap_type.members[hashmap_type.member_count].name = "contains"; hashmap_type.members[hashmap_type.member_count].args = (args_t) { 3, &hashmap_type, &hashmap_type, &A_type }; hashmap_type.members[hashmap_type.member_count].type = &bool_type; hashmap_type.members[hashmap_type.member_count].offset = 
# 428 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 428 "/home/kalen/src/A/src/A.c"
hashmap_f
# 428 "/home/kalen/src/A/src/A.c" 3 4
, 
# 428 "/home/kalen/src/A/src/A.c"
contains
# 428 "/home/kalen/src/A/src/A.c" 3 4
)
# 428 "/home/kalen/src/A/src/A.c"
; hashmap_type.members[hashmap_type.member_count].member_type = A_TYPE_IMETHOD; hashmap_type.member_count++; hashmap_type.with_sz = & hashmap_with_sz; hashmap_type.members[hashmap_type.member_count].name = "sz"; hashmap_type.members[hashmap_type.member_count].args = (args_t) { 2, &hashmap_type, &sz_type }; hashmap_type.members[hashmap_type.member_count].type = &hashmap_type; hashmap_type.members[hashmap_type.member_count].offset = 
# 428 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 428 "/home/kalen/src/A/src/A.c"
hashmap_f
# 428 "/home/kalen/src/A/src/A.c" 3 4
, 
# 428 "/home/kalen/src/A/src/A.c"
with_sz
# 428 "/home/kalen/src/A/src/A.c" 3 4
)
# 428 "/home/kalen/src/A/src/A.c"
; hashmap_type.members[hashmap_type.member_count].member_type = A_TYPE_CONSTRUCT; hashmap_type.member_count++; hashmap_type . index_A = & hashmap_index_A; hashmap_type.members[hashmap_type.member_count].name = "combine_tokens(index_, A)"; hashmap_type.members[hashmap_type.member_count].args = (args_t) { 2, &hashmap_type, &A_type }; hashmap_type.members[hashmap_type.member_count].type = &A_type; hashmap_type.members[hashmap_type.member_count].offset = 
# 428 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 428 "/home/kalen/src/A/src/A.c"
hashmap_f
# 428 "/home/kalen/src/A/src/A.c" 3 4
, 
# 428 "/home/kalen/src/A/src/A.c"
index_A
# 428 "/home/kalen/src/A/src/A.c" 3 4
)
# 428 "/home/kalen/src/A/src/A.c"
; hashmap_type.members[hashmap_type.member_count].member_type = A_TYPE_INDEX; hashmap_type.member_count++; hashmap_type . cast_bool = & hashmap_cast_bool;; A_push_type(&hashmap_type); return (bool)1; } }


static A collection_push(collection a, A b) {
    
# 432 "/home/kalen/src/A/src/A.c" 3 4
   ((void) sizeof ((
# 432 "/home/kalen/src/A/src/A.c"
   (bool)0
# 432 "/home/kalen/src/A/src/A.c" 3 4
   ) ? 1 : 0), __extension__ ({ if (
# 432 "/home/kalen/src/A/src/A.c"
   (bool)0
# 432 "/home/kalen/src/A/src/A.c" 3 4
   ) ; else __assert_fail (
# 432 "/home/kalen/src/A/src/A.c"
   "false"
# 432 "/home/kalen/src/A/src/A.c" 3 4
   , "/home/kalen/src/A/src/A.c", 432, __extension__ __PRETTY_FUNCTION__); }))
# 432 "/home/kalen/src/A/src/A.c"
                ;
    return ((void*)0);
}

static A collection_pop(collection a) {
    
# 437 "/home/kalen/src/A/src/A.c" 3 4
   ((void) sizeof ((
# 437 "/home/kalen/src/A/src/A.c"
   (bool)0
# 437 "/home/kalen/src/A/src/A.c" 3 4
   ) ? 1 : 0), __extension__ ({ if (
# 437 "/home/kalen/src/A/src/A.c"
   (bool)0
# 437 "/home/kalen/src/A/src/A.c" 3 4
   ) ; else __assert_fail (
# 437 "/home/kalen/src/A/src/A.c"
   "false"
# 437 "/home/kalen/src/A/src/A.c" 3 4
   , "/home/kalen/src/A/src/A.c", 437, __extension__ __PRETTY_FUNCTION__); }))
# 437 "/home/kalen/src/A/src/A.c"
                ;
    return ((void*)0);
}

static num collection_compare(array a, collection b) {
    
# 442 "/home/kalen/src/A/src/A.c" 3 4
   ((void) sizeof ((
# 442 "/home/kalen/src/A/src/A.c"
   (bool)0
# 442 "/home/kalen/src/A/src/A.c" 3 4
   ) ? 1 : 0), __extension__ ({ if (
# 442 "/home/kalen/src/A/src/A.c"
   (bool)0
# 442 "/home/kalen/src/A/src/A.c" 3 4
   ) ; else __assert_fail (
# 442 "/home/kalen/src/A/src/A.c"
   "false"
# 442 "/home/kalen/src/A/src/A.c" 3 4
   , "/home/kalen/src/A/src/A.c", 442, __extension__ __PRETTY_FUNCTION__); }))
# 442 "/home/kalen/src/A/src/A.c"
                ;
    return 0;
}

A hold(A a) {
    if (a) (a - 1)->refs++;
    return a;
}

void drop(A a) {
    if (a && --(a - 1)->refs == -1) {
        A aa = (a - 1);
        A_f* type = aa->type;
        void* prev = ((void*)0);
        while (type) {
            if (prev != type->destructor) {
                type->destructor(a);
                prev = type->destructor;
            }
            if (type == &A_type)
                break;
            type = type->parent_type;
        }
        free(aa);
    }
}

A fields(A instance) {
    return (instance - 1)->origin;
}

A data(A instance) {
    A obj = fields(instance);
    return obj->data;
}


static item list_push(list a, A e) {
    item n = ((A)((A)A_alloc(((struct A_f*)&item_type), 1)));
    n->val = e;
    if (a->last) {
        a->last->next = n;
        n->prev = a->last;
    } else {
        a->first = n;
    }
    a->last = n;
    a->count++;
    return n;
}

static A list_remove(list a, num index) {
    num i = 0;
    for (item ai = a->first; ai; ai = ai->next) {
        if (i++ == index) {
            if (ai == a->first) a->first = ai->next;
            if (ai == a->last) a->last = ai->prev;
            if (ai->prev) ai->prev->next = ai->next;
            if (ai->next) ai->next->prev = ai->prev;
            a->count--;
        }
    }
}

static A list_remove_item(list a, item ai) {
    num i = 0;
    if (ai) {
        if (ai == a->first) a->first = ai->next;
        if (ai == a->last) a->last = ai->prev;
        if (ai->prev) ai->prev->next = ai->next;
        if (ai->next) ai->next->prev = ai->prev;
        a->count--;
    }
}

static num list_compare(list a, list b) {
    num diff = a->count - b->count;
    if (diff != 0)
        return diff;
    for (item ai = a->first, bi = b->first; ai; ai = ai->next, bi = bi->next) {
        A_f* ai_t = *(A_f**)&((A)ai->val)[-1];
        num cmp = ai_t->compare(ai, bi);
        if (cmp != 0) return cmp;
    }
    return 0;
}

static A list_pop(list a) {
    item l = a->last;
    a->last = a->last->prev;
    if (!a->last)
        a->first = ((void*)0);
    l->prev = ((void*)0);
    a->count--;
    return l;
}

static A list_get(list a, num at_index) {
    num index = 0;
    for (item i = a->first; i; i = i->next) {
        if (at_index == index)
            return i->val;
        index++;
    }
    
# 546 "/home/kalen/src/A/src/A.c" 3 4
   ((void) sizeof ((
# 546 "/home/kalen/src/A/src/A.c"
   (bool)0
# 546 "/home/kalen/src/A/src/A.c" 3 4
   ) ? 1 : 0), __extension__ ({ if (
# 546 "/home/kalen/src/A/src/A.c"
   (bool)0
# 546 "/home/kalen/src/A/src/A.c" 3 4
   ) ; else __assert_fail (
# 546 "/home/kalen/src/A/src/A.c"
   "false"
# 546 "/home/kalen/src/A/src/A.c" 3 4
   , "/home/kalen/src/A/src/A.c", 546, __extension__ __PRETTY_FUNCTION__); }))
# 546 "/home/kalen/src/A/src/A.c"
                ;
    return ((void*)0);
}

static num list_count(list a) {
    return a->count;
}

static void array_alloc_sz(array a, sz alloc) {
    A* elements = (A*)calloc(alloc, sizeof(struct A*));
    memcpy(elements, a->elements, sizeof(struct A*) * a->len);
    free(a->elements);
    a->elements = elements;
    a->alloc = alloc;
}


static void array_expand(array a) {
    num alloc = 32 + (a->alloc << 1);
    array_alloc_sz(a, alloc);
}

bool is_meta(A a) {
    AType t = ((AType)((A)a)[-1].type);
    return t->meta.count > 0;
}

bool is_meta_compatible(A a, A b) {
    AType t = ((AType)((A)a)[-1].type);
    if (is_meta(a)) {
        AType bt = ((AType)((A)b)[-1].type);
        num found = 0;
        for (num i = 0; i < t->meta.count; i++) {
            AType mt = ((AType*)&t->meta.arg_0)[i];
            if (mt == bt)
                found++;
        }
        return found > 0;
    }
    return (bool)0;
}

static A array_push(array a, A b) {
    if (a->alloc == a->len) {
        array_expand(a);
    }
    AType t = ((AType)((A)a)[-1].type);
    if (is_meta(a))
        
# 594 "/home/kalen/src/A/src/A.c" 3 4
       ((void) sizeof ((
# 594 "/home/kalen/src/A/src/A.c"
       is_meta_compatible(a, b)
# 594 "/home/kalen/src/A/src/A.c" 3 4
       ) ? 1 : 0), __extension__ ({ if (
# 594 "/home/kalen/src/A/src/A.c"
       is_meta_compatible(a, b)
# 594 "/home/kalen/src/A/src/A.c" 3 4
       ) ; else __assert_fail (
# 594 "/home/kalen/src/A/src/A.c"
       "is_meta_compatible(a, b)"
# 594 "/home/kalen/src/A/src/A.c" 3 4
       , "/home/kalen/src/A/src/A.c", 594, __extension__ __PRETTY_FUNCTION__); }))
# 594 "/home/kalen/src/A/src/A.c"
                                       ;
    a->elements[a->len++] = b;
    return b;
}

static A array_index_num(array a, num i) {
    return a->elements[i];
}

static A array_remove(array a, num b) {
    A before = a->elements[b];
    for (num i = b; i < a->len; i++) {
        A prev = a->elements[b];
        a->elements[b] = a->elements[b + 1];
        drop(prev);
    }
    a->elements[--a->len] = ((void*)0);
    return before;
}

static void array_operator_assign_add(array a, A b) {
    array_push(a, b);
}

static void array_operator_assign_sub(array a, num b) {
    array_remove(a, b);
}

static A array_first(array a) {
    
# 623 "/home/kalen/src/A/src/A.c" 3 4
   ((void) sizeof ((
# 623 "/home/kalen/src/A/src/A.c"
   a->len
# 623 "/home/kalen/src/A/src/A.c" 3 4
   ) ? 1 : 0), __extension__ ({ if (
# 623 "/home/kalen/src/A/src/A.c"
   a->len
# 623 "/home/kalen/src/A/src/A.c" 3 4
   ) ; else __assert_fail (
# 623 "/home/kalen/src/A/src/A.c"
   "a->len"
# 623 "/home/kalen/src/A/src/A.c" 3 4
   , "/home/kalen/src/A/src/A.c", 623, __extension__ __PRETTY_FUNCTION__); }))
# 623 "/home/kalen/src/A/src/A.c"
                 ;
    return a->elements[a->len - 1];
}

static A array_last(array a) {
    
# 628 "/home/kalen/src/A/src/A.c" 3 4
   ((void) sizeof ((
# 628 "/home/kalen/src/A/src/A.c"
   a->len
# 628 "/home/kalen/src/A/src/A.c" 3 4
   ) ? 1 : 0), __extension__ ({ if (
# 628 "/home/kalen/src/A/src/A.c"
   a->len
# 628 "/home/kalen/src/A/src/A.c" 3 4
   ) ; else __assert_fail (
# 628 "/home/kalen/src/A/src/A.c"
   "a->len"
# 628 "/home/kalen/src/A/src/A.c" 3 4
   , "/home/kalen/src/A/src/A.c", 628, __extension__ __PRETTY_FUNCTION__); }))
# 628 "/home/kalen/src/A/src/A.c"
                 ;
    return a->elements[a->len - 1];
}

static void array_push_symbols(array a, ...) {
    va_list args;
    
# 634 "/home/kalen/src/A/src/A.c" 3 4
   __builtin_va_start(
# 634 "/home/kalen/src/A/src/A.c"
   args
# 634 "/home/kalen/src/A/src/A.c" 3 4
   ,
# 634 "/home/kalen/src/A/src/A.c"
   a
# 634 "/home/kalen/src/A/src/A.c" 3 4
   )
# 634 "/home/kalen/src/A/src/A.c"
                    ;
    char* value;
    while ((value = 
# 636 "/home/kalen/src/A/src/A.c" 3 4
                   __builtin_va_arg(
# 636 "/home/kalen/src/A/src/A.c"
                   args
# 636 "/home/kalen/src/A/src/A.c" 3 4
                   ,
# 636 "/home/kalen/src/A/src/A.c"
                   char*
# 636 "/home/kalen/src/A/src/A.c" 3 4
                   )
# 636 "/home/kalen/src/A/src/A.c"
                                      ) != ((void*)0)) {
        string s = string_type.with_cstr((A)((A)A_alloc(((struct A_f*)&string_type), 1)), value, strlen(value));
        array_type.push(a, s);
    }
    
# 640 "/home/kalen/src/A/src/A.c" 3 4
   __builtin_va_end(
# 640 "/home/kalen/src/A/src/A.c"
   args
# 640 "/home/kalen/src/A/src/A.c" 3 4
   )
# 640 "/home/kalen/src/A/src/A.c"
               ;
}

static void array_push_objects(array a, A f, ...) {
    va_list args;
    
# 645 "/home/kalen/src/A/src/A.c" 3 4
   __builtin_va_start(
# 645 "/home/kalen/src/A/src/A.c"
   args
# 645 "/home/kalen/src/A/src/A.c" 3 4
   ,
# 645 "/home/kalen/src/A/src/A.c"
   f
# 645 "/home/kalen/src/A/src/A.c" 3 4
   )
# 645 "/home/kalen/src/A/src/A.c"
                    ;
    A value;
    while ((value = 
# 647 "/home/kalen/src/A/src/A.c" 3 4
                   __builtin_va_arg(
# 647 "/home/kalen/src/A/src/A.c"
                   args
# 647 "/home/kalen/src/A/src/A.c" 3 4
                   ,
# 647 "/home/kalen/src/A/src/A.c"
                   A
# 647 "/home/kalen/src/A/src/A.c" 3 4
                   )
# 647 "/home/kalen/src/A/src/A.c"
                                  ) != ((void*)0))
        array_type.push(a, value);
    
# 649 "/home/kalen/src/A/src/A.c" 3 4
   __builtin_va_end(
# 649 "/home/kalen/src/A/src/A.c"
   args
# 649 "/home/kalen/src/A/src/A.c" 3 4
   )
# 649 "/home/kalen/src/A/src/A.c"
               ;
}

static array array_of_objects(AType validate, ...) {
    array a = ((A)((A)A_alloc(((struct A_f*)&array_type), 1)));
    va_list args;
    
# 655 "/home/kalen/src/A/src/A.c" 3 4
   __builtin_va_start(
# 655 "/home/kalen/src/A/src/A.c"
   args
# 655 "/home/kalen/src/A/src/A.c" 3 4
   ,((void *)0))
# 655 "/home/kalen/src/A/src/A.c"
                       ;

    for (;;) {
        A arg = 
# 658 "/home/kalen/src/A/src/A.c" 3 4
               __builtin_va_arg(
# 658 "/home/kalen/src/A/src/A.c"
               args
# 658 "/home/kalen/src/A/src/A.c" 3 4
               ,
# 658 "/home/kalen/src/A/src/A.c"
               A
# 658 "/home/kalen/src/A/src/A.c" 3 4
               )
# 658 "/home/kalen/src/A/src/A.c"
                              ;
        if (!arg)
            break;
        
# 661 "/home/kalen/src/A/src/A.c" 3 4
       ((void) sizeof ((
# 661 "/home/kalen/src/A/src/A.c"
       !validate || validate == ((AType)((A)arg)[-1].type)
# 661 "/home/kalen/src/A/src/A.c" 3 4
       ) ? 1 : 0), __extension__ ({ if (
# 661 "/home/kalen/src/A/src/A.c"
       !validate || validate == ((AType)((A)arg)[-1].type)
# 661 "/home/kalen/src/A/src/A.c" 3 4
       ) ; else __assert_fail (
# 661 "/home/kalen/src/A/src/A.c"
       "!validate || validate == typeid(arg)"
# 661 "/home/kalen/src/A/src/A.c" 3 4
       , "/home/kalen/src/A/src/A.c", 661, __extension__ __PRETTY_FUNCTION__); }))
# 661 "/home/kalen/src/A/src/A.c"
                                                   ;
        array_type.push(a, arg);
    }
    return a;
}

static A array_pop(array a) {
    
# 668 "/home/kalen/src/A/src/A.c" 3 4
   ((void) sizeof ((
# 668 "/home/kalen/src/A/src/A.c"
   a->len > 0
# 668 "/home/kalen/src/A/src/A.c" 3 4
   ) ? 1 : 0), __extension__ ({ if (
# 668 "/home/kalen/src/A/src/A.c"
   a->len > 0
# 668 "/home/kalen/src/A/src/A.c" 3 4
   ) ; else __assert_fail (
# 668 "/home/kalen/src/A/src/A.c"
   "a->len > 0"
# 668 "/home/kalen/src/A/src/A.c" 3 4
   , "/home/kalen/src/A/src/A.c", 668, __extension__ __PRETTY_FUNCTION__); }))
# 668 "/home/kalen/src/A/src/A.c"
                     ;
    return a->elements[a->len--];
}

static num array_compare(array a, array b) {
    num diff = a->len - b->len;
    if (diff != 0)
        return diff;
    for (num i = 0; i < a->len; i++) {
        num cmp = A_type.compare(a->elements[i], b->elements[i]);
        if (cmp != 0)
            return cmp;
    }
    return 0;
}

static A array_get(array a, num i) {
    return a->elements[i];
}

static num array_count(array a) {
    return a->len;
}


static num array_index_of(array a, A b) {
    for (num i = 0; i < a->len; i++) {
        if (((__typeof__((a -> elements[i])->f)) ((A)(a -> elements[i]))[-1].type) -> compare(a -> elements[i], b) == 0)
            return i;
    }
    return -1;
}

static bool array_cast_bool(array a) { return a && a->len > 0; }

static array array_with_sz(array a, sz size) {
    array_alloc_sz(a, size);
}

static u64 fn_hash(fn f) {
    return (u64)f->method->address;
}

static A fn_call(fn f, array args) {
    return method_call(f->method, args);
}

static fn fn_with_ATypes(fn f, ATypes atypes, AType rtype, handle address) {
    
# 716 "/home/kalen/src/A/src/A.c" 3 4
   ((void) sizeof ((
# 716 "/home/kalen/src/A/src/A.c"
   atypes->len <= 8
# 716 "/home/kalen/src/A/src/A.c" 3 4
   ) ? 1 : 0), __extension__ ({ if (
# 716 "/home/kalen/src/A/src/A.c"
   atypes->len <= 8
# 716 "/home/kalen/src/A/src/A.c" 3 4
   ) ; else __assert_fail (
# 716 "/home/kalen/src/A/src/A.c"
   "atypes->len <= 8"
# 716 "/home/kalen/src/A/src/A.c" 3 4
   , "/home/kalen/src/A/src/A.c", 716, __extension__ __PRETTY_FUNCTION__); }))
# 716 "/home/kalen/src/A/src/A.c"
                           ;
    f->method = method_with_address(address, rtype, atypes);
    return f;
}

A fn_header; fn_f fn_type; static __attribute__((constructor)) bool global_fn() { fn_f* type_ref = &fn_type; A_f* base_ref = &A_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_fn); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(A_f)); static member_t members[16 + sizeof(fn_type) / sizeof(void*)]; fn_type.parent_type = &A_type; fn_type.name = "fn"; fn_type.size = sizeof(struct fn); fn_type.members = members; fn_type.traits = 0; fn_type.arb = primitive_ffi_arb(((struct A_f*)&cstr_type)); fn_type . call = & fn_call; fn_type.members[fn_type.member_count].name = "call"; fn_type.members[fn_type.member_count].args = (args_t) { 3, &fn_type, &fn_type, &array_type }; fn_type.members[fn_type.member_count].type = &A_type; fn_type.members[fn_type.member_count].offset = 
# 721 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 721 "/home/kalen/src/A/src/A.c"
fn_f
# 721 "/home/kalen/src/A/src/A.c" 3 4
, 
# 721 "/home/kalen/src/A/src/A.c"
call
# 721 "/home/kalen/src/A/src/A.c" 3 4
)
# 721 "/home/kalen/src/A/src/A.c"
; fn_type.members[fn_type.member_count].member_type = A_TYPE_IMETHOD; fn_type.member_count++; fn_type.with_ATypes = & fn_with_ATypes; fn_type.members[fn_type.member_count].name = "ATypes"; fn_type.members[fn_type.member_count].args = (args_t) { 4, &fn_type, &ATypes_type, &AType_type, &handle_type }; fn_type.members[fn_type.member_count].type = &fn_type; fn_type.members[fn_type.member_count].offset = 
# 721 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 721 "/home/kalen/src/A/src/A.c"
fn_f
# 721 "/home/kalen/src/A/src/A.c" 3 4
, 
# 721 "/home/kalen/src/A/src/A.c"
with_ATypes
# 721 "/home/kalen/src/A/src/A.c" 3 4
)
# 721 "/home/kalen/src/A/src/A.c"
; fn_type.members[fn_type.member_count].member_type = A_TYPE_CONSTRUCT; fn_type.member_count++; fn_type . hash = & fn_hash;; A_push_type(&fn_type); return (bool)1; } }


static A vector_push(vector a, A b) {
    A obj = fields(a);
    if (obj->alloc == obj->count)
        A_realloc(obj, 32 + (obj->alloc < 1));
    u8* dst = obj->data;
    num sz = obj->type->size;
    A cp = &dst[sz * obj->count++];
    memcpy(cp, b, sz);
    return cp;
}



static A vector_pop(vector a) {
    A obj = fields(a);
    
# 739 "/home/kalen/src/A/src/A.c" 3 4
   ((void) sizeof ((
# 739 "/home/kalen/src/A/src/A.c"
   obj->count > 0
# 739 "/home/kalen/src/A/src/A.c" 3 4
   ) ? 1 : 0), __extension__ ({ if (
# 739 "/home/kalen/src/A/src/A.c"
   obj->count > 0
# 739 "/home/kalen/src/A/src/A.c" 3 4
   ) ; else __assert_fail (
# 739 "/home/kalen/src/A/src/A.c"
   "obj->count > 0"
# 739 "/home/kalen/src/A/src/A.c" 3 4
   , "/home/kalen/src/A/src/A.c", 739, __extension__ __PRETTY_FUNCTION__); }))
# 739 "/home/kalen/src/A/src/A.c"
                         ;
    u8* dst = obj->data;
    num sz = obj->type->size;
    return (A)&dst[sz * --obj->count];
}

static num vector_compare(vector a, vector b) {
    A a_object = fields(a);
    A b_object = fields(b);
    num diff = a_object->count - b_object->count;
    if (diff != 0)
        return diff;
    
# 751 "/home/kalen/src/A/src/A.c" 3 4
   ((void) sizeof ((
# 751 "/home/kalen/src/A/src/A.c"
   a_object->type == b_object->type
# 751 "/home/kalen/src/A/src/A.c" 3 4
   ) ? 1 : 0), __extension__ ({ if (
# 751 "/home/kalen/src/A/src/A.c"
   a_object->type == b_object->type
# 751 "/home/kalen/src/A/src/A.c" 3 4
   ) ; else __assert_fail (
# 751 "/home/kalen/src/A/src/A.c"
   "a_object->type == b_object->type"
# 751 "/home/kalen/src/A/src/A.c" 3 4
   , "/home/kalen/src/A/src/A.c", 751, __extension__ __PRETTY_FUNCTION__); }))
# 751 "/home/kalen/src/A/src/A.c"
                                           ;
    u8* a_data = data(a);
    u8* b_data = data(b);
    num sz = a_object->type->size;
    for (num i = 0; i < a_object->count; i++) {
        num cmp = memcmp(&a_data[sz * i], &b_data[sz * i], sz);
        if (cmp != 0)
            return cmp;
    }
    return 0;
}

static A vector_get(vector a, num i) {
    A a_object = fields(a);
    u8* a_data = data(a);
    num sz = a_object->type->size;
    return (A)&a_data[i * sz];
}

static num vector_count(vector a) {
    A a_object = fields(a);
    return a_object->count;
}

static num vector_index_of(vector a, A b) {
    A a_object = fields(a);
    u8* a_data = data(a);
    u8* b_data = data(b);
    num sz = a_object->type->size;
    for (num i = 0; i < a_object->count; i++) {
        if (memcmp(&a_data[sz * i], b_data, sz) == 0)
            return i;
    }
    return -1;
}

static bool vector_cast_bool(vector a) {
    A a_object = fields(a);
    return a_object->count > 0;
}

u64 vector_hash(vector a) {
    A obj = fields(a);
    return fnv1a_hash(obj->data, obj->type->size * obj->count, 0xcbf29ce484222325);
}

static path path_with_cstr(path a, cstr path, num sz) {
    num len = sz == -1 ? strlen(path) : sz;
    a->chars = calloc(len + 1, 1);
    memcpy(a->chars, path, len + 1);
    return a;
}

static bool path_make_dir(path a) {
    cstr cs = a->chars;
    sz len = strlen(cs);
    for (num i = 1; i < len; i++) {
        if (cs[i] == '/' || i == len - 1) {
            bool set = (bool)0;
            if (cs[i] == '/') { cs[i] = 0; set = (bool)1; }
            mkdir(cs, 0755);
            if (set) cs[i] = '/';
        }
    }
    struct stat st = {0};
    return stat(cs, &st) == 0 && 
# 816 "/home/kalen/src/A/src/A.c" 3 4
                                ((((
# 816 "/home/kalen/src/A/src/A.c"
                                st.st_mode
# 816 "/home/kalen/src/A/src/A.c" 3 4
                                )) & 0170000) == (0040000))
# 816 "/home/kalen/src/A/src/A.c"
                                                   ;
}

static bool path_is_empty(path a) {
    int n = 0;
    struct dirent *d;
    DIR *dir = opendir(a->chars);

    if (dir == 
# 824 "/home/kalen/src/A/src/A.c" 3 4
              ((void *)0)
# 824 "/home/kalen/src/A/src/A.c"
                  )
        return (bool)0;

    while ((d = readdir(dir)) != 
# 827 "/home/kalen/src/A/src/A.c" 3 4
                                ((void *)0)
# 827 "/home/kalen/src/A/src/A.c"
                                    ) {
        if (++n > 2)
            break;
    }
    closedir(dir);
    return n <= 2;
}

static string path_stem(path a) {
    cstr cs = a->chars;
    sz len = strlen(cs);
    string res = string_type.with_sz((A)((A)A_alloc(((struct A_f*)&string_type), 1)), 256);
    sz dot = 0;
    for (num i = len - 1; i >= 0; i--) {
        if (cs[i] == '.')
            dot = i;
        if (cs[i] == '/' || i == 0) {
            int offset = cs[i] == '/';
            cstr start = &cs[i + offset];
            int n_bytes = (dot > 0 ? dot : len) - (i + offset);
            memcpy(res->chars, start, n_bytes);
            res->len = n_bytes;
            break;
        }
    }
    return res;
}

static string path_filename(path a) {
    cstr cs = a->chars;
    sz len = strlen(cs);
    string res = string_type.with_sz((A)((A)A_alloc(((struct A_f*)&string_type), 1)), 256);
    for (num i = len - 1; i >= 0; i--) {
        if (cs[i] == '/' || i == 0) {
            cstr start = &cs[i + cs[i] == '/'];
            int n_bytes = len - i;
            memcpy(res->chars, start, n_bytes);
            res->len = n_bytes;
            break;
        }
    }
    return res;
}

static path path_absolute(path a) {
    path result = ((A)((A)A_alloc(((struct A_f*)&path_type), 1)));
    result->chars = strdup(realpath(a->chars, ((void*)0)));
    return result;
}

static path path_directory(path a) {
    path result = ((A)((A)A_alloc(((struct A_f*)&path_type), 1)));
    result->chars = dirname(strdup(a->chars));
    return result;
}

static path path_parent(path a) {
    int len = strlen(a->chars);
    char *cp = calloc(len + 4, 1);
    memcpy(cp, a->chars, len);
    if (a->chars[len - 1] == '\\' || a->chars[len - 1] == '/')
        memcpy(&cp[len], "..", 3);
    else
        memcpy(&cp[len], "/..", 4);
    char *dir_name = dirname(cp);
    path result = ((A)((A)A_alloc(((struct A_f*)&path_type), 1)));
    result->chars = strdup(dir_name);
    return result;
}

static path path_change_ext(path a, cstr ext) {
    int e_len = strlen(ext);
    int len = strlen(a->chars);
    int ext_pos = -1;
    for (int i = len - 1; i >= 0; i--) {
        if (a->chars[i] == '/')
            break;
        if (a->chars[i] == '.') {
            ext_pos = i;
            break;
        }
    }
    path res = ((A)((A)A_alloc(((struct A_f*)&path_type), 1)));
    res->chars = calloc(32 + len + e_len, 1);
    if (ext_pos >= 0) {
        memcpy( res->chars, a->chars, ext_pos + 1);
        if (e_len)
            memcpy(&res->chars[ext_pos + 1], ext, e_len);
        else
            res->chars[ext_pos] = 0;
    } else {
        memcpy( res->chars, a->chars, len);
        if (e_len) {
            memcpy(&res->chars[len], ".", 1);
            memcpy(&res->chars[len + 1], ext, e_len);
        }
    }
    return res;
}

static path path_cwd(sz size) {
    path a = ((A)((A)A_alloc(((struct A_f*)&path_type), 1)));
    a->chars = calloc(size, 1);
    getcwd(a->chars, size);
    return a;
}

static bool path_exists(path a) {
    FILE *file = fopen(a->chars, "r");
    if (file) {
        fclose(file);
        return (bool)1;
    }
    return (bool)0;
}

static u64 path_hash(path a) {
    return fnv1a_hash(a->chars, strlen(a->chars), 0xcbf29ce484222325);
}


static A path_read(path a, AType type) {
    FILE* f = fopen(a->chars, "rb");
    if (!f) return ((void*)0);
    if (type == ((struct A_f*)&string_type)) {
        fseek(f, 0, 
# 952 "/home/kalen/src/A/src/A.c" 3 4
                   2
# 952 "/home/kalen/src/A/src/A.c"
                           );
        sz flen = ftell(f);
        fseek(f, 0, 
# 954 "/home/kalen/src/A/src/A.c" 3 4
                   0
# 954 "/home/kalen/src/A/src/A.c"
                           );
        string a = string_type.with_sz((A)((A)A_alloc(((struct A_f*)&string_type), 1)), flen + 1);
        size_t n = fread(a->chars, 1, flen, f);
        fclose(f);
        
# 958 "/home/kalen/src/A/src/A.c" 3 4
       ((void) sizeof ((
# 958 "/home/kalen/src/A/src/A.c"
       n == flen
# 958 "/home/kalen/src/A/src/A.c" 3 4
       ) ? 1 : 0), __extension__ ({ if (
# 958 "/home/kalen/src/A/src/A.c"
       n == flen
# 958 "/home/kalen/src/A/src/A.c" 3 4
       ) ; else __assert_fail (
# 958 "/home/kalen/src/A/src/A.c"
       "n == flen"
# 958 "/home/kalen/src/A/src/A.c" 3 4
       , "/home/kalen/src/A/src/A.c", 958, __extension__ __PRETTY_FUNCTION__); }))
# 958 "/home/kalen/src/A/src/A.c"
                        ;
        a->len = flen;
        return a;
    }
    
# 962 "/home/kalen/src/A/src/A.c" 3 4
   ((void) sizeof ((
# 962 "/home/kalen/src/A/src/A.c"
   (bool)0
# 962 "/home/kalen/src/A/src/A.c" 3 4
   ) ? 1 : 0), __extension__ ({ if (
# 962 "/home/kalen/src/A/src/A.c"
   (bool)0
# 962 "/home/kalen/src/A/src/A.c" 3 4
   ) ; else __assert_fail (
# 962 "/home/kalen/src/A/src/A.c"
   "false"
# 962 "/home/kalen/src/A/src/A.c" 3 4
   , "/home/kalen/src/A/src/A.c", 962, __extension__ __PRETTY_FUNCTION__); }))
# 962 "/home/kalen/src/A/src/A.c"
                ;
    return ((void*)0);
}

item item_with_symbol(item f, symbol key, A val) {
    f->key = string_type.with_cstr((A)((A)A_alloc(((struct A_f*)&string_type), 1)), (cstr)key, strlen(key));
    f->val = hold(val);
    return f;
}

void* primitive_ffi_arb(AType ptype) {
    if (ptype == ((struct A_f*)&u8_type)) return &ffi_type_uint8;
    if (ptype == ((struct A_f*)&i8_type)) return &ffi_type_sint8;
    if (ptype == ((struct A_f*)&u16_type)) return &ffi_type_uint16;
    if (ptype == ((struct A_f*)&i16_type)) return &ffi_type_sint16;
    if (ptype == ((struct A_f*)&u32_type)) return &ffi_type_uint32;
    if (ptype == ((struct A_f*)&i32_type)) return &ffi_type_sint32;
    if (ptype == ((struct A_f*)&u64_type)) return &ffi_type_uint64;
    if (ptype == ((struct A_f*)&i64_type)) return &ffi_type_sint64;
    if (ptype == ((struct A_f*)&f32_type)) return &ffi_type_float;
    if (ptype == ((struct A_f*)&f64_type)) return &ffi_type_double;
    if (ptype == ((struct A_f*)&f128_type)) return &ffi_type_longdouble;
    if (ptype == ((struct A_f*)&cstr_type)) return &ffi_type_pointer;
    if (ptype == ((struct A_f*)&symbol_type)) return &ffi_type_pointer;
    if (ptype == ((struct A_f*)&bool_type)) return &ffi_type_uint32;
    if (ptype == ((struct A_f*)&num_type)) return &ffi_type_sint64;
    if (ptype == ((struct A_f*)&sz_type)) return &ffi_type_sint64;
    if (ptype == ((struct A_f*)&none_type)) return &ffi_type_void;
    if (ptype == ((struct A_f*)&AType_type)) return &ffi_type_pointer;
    if (ptype == ((struct A_f*)&handle_type)) return &ffi_type_pointer;
    
# 992 "/home/kalen/src/A/src/A.c" 3 4
   ((void) sizeof ((
# 992 "/home/kalen/src/A/src/A.c"
   (bool)0
# 992 "/home/kalen/src/A/src/A.c" 3 4
   ) ? 1 : 0), __extension__ ({ if (
# 992 "/home/kalen/src/A/src/A.c"
   (bool)0
# 992 "/home/kalen/src/A/src/A.c" 3 4
   ) ; else __assert_fail (
# 992 "/home/kalen/src/A/src/A.c"
   "false"
# 992 "/home/kalen/src/A/src/A.c" 3 4
   , "/home/kalen/src/A/src/A.c", 992, __extension__ __PRETTY_FUNCTION__); }))
# 992 "/home/kalen/src/A/src/A.c"
                ;
    return ((void*)0);
}

A A_header; A_f A_type; static __attribute__((constructor)) bool global_A() { A_f* type_ref = &A_type; A_f* base_ref = &A_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_A); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(A_f)); static member_t members[16 + sizeof(A_type) / sizeof(void*)]; A_type.parent_type = &A_type; A_type.name = "A"; A_type.size = sizeof(struct A); A_type.members = members; A_type.traits = 0; A_type.arb = primitive_ffi_arb(((struct A_f*)&cstr_type)); A_type . init = & A_init; A_type.members[A_type.member_count].name = "init"; A_type.members[A_type.member_count].args = (args_t) { 2, &A_type, &A_type }; A_type.members[A_type.member_count].type = &none_type; A_type.members[A_type.member_count].offset = 
# 996 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 996 "/home/kalen/src/A/src/A.c"
A_f
# 996 "/home/kalen/src/A/src/A.c" 3 4
, 
# 996 "/home/kalen/src/A/src/A.c"
init
# 996 "/home/kalen/src/A/src/A.c" 3 4
)
# 996 "/home/kalen/src/A/src/A.c"
; A_type.members[A_type.member_count].member_type = A_TYPE_IMETHOD; A_type.member_count++; A_type . destructor = & A_destructor; A_type.members[A_type.member_count].name = "destructor"; A_type.members[A_type.member_count].args = (args_t) { 2, &A_type, &A_type }; A_type.members[A_type.member_count].type = &none_type; A_type.members[A_type.member_count].offset = 
# 996 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 996 "/home/kalen/src/A/src/A.c"
A_f
# 996 "/home/kalen/src/A/src/A.c" 3 4
, 
# 996 "/home/kalen/src/A/src/A.c"
destructor
# 996 "/home/kalen/src/A/src/A.c" 3 4
)
# 996 "/home/kalen/src/A/src/A.c"
; A_type.members[A_type.member_count].member_type = A_TYPE_IMETHOD; A_type.member_count++; A_type . compare = & A_compare; A_type.members[A_type.member_count].name = "compare"; A_type.members[A_type.member_count].args = (args_t) { 3, &A_type, &A_type, &A_type }; A_type.members[A_type.member_count].type = &i32_type; A_type.members[A_type.member_count].offset = 
# 996 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 996 "/home/kalen/src/A/src/A.c"
A_f
# 996 "/home/kalen/src/A/src/A.c" 3 4
, 
# 996 "/home/kalen/src/A/src/A.c"
compare
# 996 "/home/kalen/src/A/src/A.c" 3 4
)
# 996 "/home/kalen/src/A/src/A.c"
; A_type.members[A_type.member_count].member_type = A_TYPE_IMETHOD; A_type.member_count++; A_type . hash = & A_hash; A_type.members[A_type.member_count].name = "hash"; A_type.members[A_type.member_count].args = (args_t) { 2, &A_type, &A_type }; A_type.members[A_type.member_count].type = &u64_type; A_type.members[A_type.member_count].offset = 
# 996 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 996 "/home/kalen/src/A/src/A.c"
A_f
# 996 "/home/kalen/src/A/src/A.c" 3 4
, 
# 996 "/home/kalen/src/A/src/A.c"
hash
# 996 "/home/kalen/src/A/src/A.c" 3 4
)
# 996 "/home/kalen/src/A/src/A.c"
; A_type.members[A_type.member_count].member_type = A_TYPE_IMETHOD; A_type.member_count++; A_type.cast_bool = & A_cast_bool; A_type.members[A_type.member_count].name = "combine_tokens(cast_, bool)"; A_type.members[A_type.member_count].args = (args_t) { 1, &A_type }; A_type.members[A_type.member_count].type = &bool_type; A_type.members[A_type.member_count].offset = 
# 996 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 996 "/home/kalen/src/A/src/A.c"
A_f
# 996 "/home/kalen/src/A/src/A.c" 3 4
, 
# 996 "/home/kalen/src/A/src/A.c"
cast_bool
# 996 "/home/kalen/src/A/src/A.c" 3 4
)
# 996 "/home/kalen/src/A/src/A.c"
; A_type.members[A_type.member_count].member_type = A_TYPE_CAST; A_type.member_count++; A_type.with_cstr = & A_with_cstr; A_type.members[A_type.member_count].name = "cstr"; A_type.members[A_type.member_count].args = (args_t) { 3, &A_type, &cstr_type, &num_type }; A_type.members[A_type.member_count].type = &A_type; A_type.members[A_type.member_count].offset = 
# 996 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 996 "/home/kalen/src/A/src/A.c"
A_f
# 996 "/home/kalen/src/A/src/A.c" 3 4
, 
# 996 "/home/kalen/src/A/src/A.c"
with_cstr
# 996 "/home/kalen/src/A/src/A.c" 3 4
)
# 996 "/home/kalen/src/A/src/A.c"
; A_type.members[A_type.member_count].member_type = A_TYPE_CONSTRUCT; A_type.member_count++;; A_push_type(&A_type); return (bool)1; } }

A numeric_header; numeric_f numeric_type; static __attribute__((constructor)) bool global_numeric() { numeric_f* type_ref = &numeric_type; A_f* base_ref = &A_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_numeric); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(A_f)); static member_t members[16 + sizeof(numeric_type) / sizeof(void*)]; numeric_type.parent_type = &A_type; numeric_type.name = "numeric"; numeric_type.size = 0; numeric_type.members = members; numeric_type.traits = 0; numeric_type.arb = ((void*)0); numeric_type.with_i8 = & numeric_with_i8; numeric_type.members[numeric_type.member_count].name = "i8"; numeric_type.members[numeric_type.member_count].args = (args_t) { 2, &numeric_type, &i8_type }; numeric_type.members[numeric_type.member_count].type = &numeric_type; numeric_type.members[numeric_type.member_count].offset = 
# 998 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 998 "/home/kalen/src/A/src/A.c"
numeric_f
# 998 "/home/kalen/src/A/src/A.c" 3 4
, 
# 998 "/home/kalen/src/A/src/A.c"
with_i8
# 998 "/home/kalen/src/A/src/A.c" 3 4
)
# 998 "/home/kalen/src/A/src/A.c"
; numeric_type.members[numeric_type.member_count].member_type = A_TYPE_CONSTRUCT; numeric_type.member_count++; numeric_type.with_i16 = & numeric_with_i16; numeric_type.members[numeric_type.member_count].name = "i16"; numeric_type.members[numeric_type.member_count].args = (args_t) { 2, &numeric_type, &i16_type }; numeric_type.members[numeric_type.member_count].type = &numeric_type; numeric_type.members[numeric_type.member_count].offset = 
# 998 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 998 "/home/kalen/src/A/src/A.c"
numeric_f
# 998 "/home/kalen/src/A/src/A.c" 3 4
, 
# 998 "/home/kalen/src/A/src/A.c"
with_i16
# 998 "/home/kalen/src/A/src/A.c" 3 4
)
# 998 "/home/kalen/src/A/src/A.c"
; numeric_type.members[numeric_type.member_count].member_type = A_TYPE_CONSTRUCT; numeric_type.member_count++; numeric_type.with_i32 = & numeric_with_i32; numeric_type.members[numeric_type.member_count].name = "i32"; numeric_type.members[numeric_type.member_count].args = (args_t) { 2, &numeric_type, &i32_type }; numeric_type.members[numeric_type.member_count].type = &numeric_type; numeric_type.members[numeric_type.member_count].offset = 
# 998 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 998 "/home/kalen/src/A/src/A.c"
numeric_f
# 998 "/home/kalen/src/A/src/A.c" 3 4
, 
# 998 "/home/kalen/src/A/src/A.c"
with_i32
# 998 "/home/kalen/src/A/src/A.c" 3 4
)
# 998 "/home/kalen/src/A/src/A.c"
; numeric_type.members[numeric_type.member_count].member_type = A_TYPE_CONSTRUCT; numeric_type.member_count++; numeric_type.with_i64 = & numeric_with_i64; numeric_type.members[numeric_type.member_count].name = "i64"; numeric_type.members[numeric_type.member_count].args = (args_t) { 2, &numeric_type, &i64_type }; numeric_type.members[numeric_type.member_count].type = &numeric_type; numeric_type.members[numeric_type.member_count].offset = 
# 998 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 998 "/home/kalen/src/A/src/A.c"
numeric_f
# 998 "/home/kalen/src/A/src/A.c" 3 4
, 
# 998 "/home/kalen/src/A/src/A.c"
with_i64
# 998 "/home/kalen/src/A/src/A.c" 3 4
)
# 998 "/home/kalen/src/A/src/A.c"
; numeric_type.members[numeric_type.member_count].member_type = A_TYPE_CONSTRUCT; numeric_type.member_count++; numeric_type.with_u8 = & numeric_with_u8; numeric_type.members[numeric_type.member_count].name = "u8"; numeric_type.members[numeric_type.member_count].args = (args_t) { 2, &numeric_type, &u8_type }; numeric_type.members[numeric_type.member_count].type = &numeric_type; numeric_type.members[numeric_type.member_count].offset = 
# 998 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 998 "/home/kalen/src/A/src/A.c"
numeric_f
# 998 "/home/kalen/src/A/src/A.c" 3 4
, 
# 998 "/home/kalen/src/A/src/A.c"
with_u8
# 998 "/home/kalen/src/A/src/A.c" 3 4
)
# 998 "/home/kalen/src/A/src/A.c"
; numeric_type.members[numeric_type.member_count].member_type = A_TYPE_CONSTRUCT; numeric_type.member_count++; numeric_type.with_u16 = & numeric_with_u16; numeric_type.members[numeric_type.member_count].name = "u16"; numeric_type.members[numeric_type.member_count].args = (args_t) { 2, &numeric_type, &u16_type }; numeric_type.members[numeric_type.member_count].type = &numeric_type; numeric_type.members[numeric_type.member_count].offset = 
# 998 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 998 "/home/kalen/src/A/src/A.c"
numeric_f
# 998 "/home/kalen/src/A/src/A.c" 3 4
, 
# 998 "/home/kalen/src/A/src/A.c"
with_u16
# 998 "/home/kalen/src/A/src/A.c" 3 4
)
# 998 "/home/kalen/src/A/src/A.c"
; numeric_type.members[numeric_type.member_count].member_type = A_TYPE_CONSTRUCT; numeric_type.member_count++; numeric_type.with_u32 = & numeric_with_u32; numeric_type.members[numeric_type.member_count].name = "u32"; numeric_type.members[numeric_type.member_count].args = (args_t) { 2, &numeric_type, &u32_type }; numeric_type.members[numeric_type.member_count].type = &numeric_type; numeric_type.members[numeric_type.member_count].offset = 
# 998 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 998 "/home/kalen/src/A/src/A.c"
numeric_f
# 998 "/home/kalen/src/A/src/A.c" 3 4
, 
# 998 "/home/kalen/src/A/src/A.c"
with_u32
# 998 "/home/kalen/src/A/src/A.c" 3 4
)
# 998 "/home/kalen/src/A/src/A.c"
; numeric_type.members[numeric_type.member_count].member_type = A_TYPE_CONSTRUCT; numeric_type.member_count++; numeric_type.with_u64 = & numeric_with_u64; numeric_type.members[numeric_type.member_count].name = "u64"; numeric_type.members[numeric_type.member_count].args = (args_t) { 2, &numeric_type, &u64_type }; numeric_type.members[numeric_type.member_count].type = &numeric_type; numeric_type.members[numeric_type.member_count].offset = 
# 998 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 998 "/home/kalen/src/A/src/A.c"
numeric_f
# 998 "/home/kalen/src/A/src/A.c" 3 4
, 
# 998 "/home/kalen/src/A/src/A.c"
with_u64
# 998 "/home/kalen/src/A/src/A.c" 3 4
)
# 998 "/home/kalen/src/A/src/A.c"
; numeric_type.members[numeric_type.member_count].member_type = A_TYPE_CONSTRUCT; numeric_type.member_count++; numeric_type.with_f32 = & numeric_with_f32; numeric_type.members[numeric_type.member_count].name = "f32"; numeric_type.members[numeric_type.member_count].args = (args_t) { 2, &numeric_type, &f32_type }; numeric_type.members[numeric_type.member_count].type = &numeric_type; numeric_type.members[numeric_type.member_count].offset = 
# 998 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 998 "/home/kalen/src/A/src/A.c"
numeric_f
# 998 "/home/kalen/src/A/src/A.c" 3 4
, 
# 998 "/home/kalen/src/A/src/A.c"
with_f32
# 998 "/home/kalen/src/A/src/A.c" 3 4
)
# 998 "/home/kalen/src/A/src/A.c"
; numeric_type.members[numeric_type.member_count].member_type = A_TYPE_CONSTRUCT; numeric_type.member_count++; numeric_type.with_f64 = & numeric_with_f64; numeric_type.members[numeric_type.member_count].name = "f64"; numeric_type.members[numeric_type.member_count].args = (args_t) { 2, &numeric_type, &f64_type }; numeric_type.members[numeric_type.member_count].type = &numeric_type; numeric_type.members[numeric_type.member_count].offset = 
# 998 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 998 "/home/kalen/src/A/src/A.c"
numeric_f
# 998 "/home/kalen/src/A/src/A.c" 3 4
, 
# 998 "/home/kalen/src/A/src/A.c"
with_f64
# 998 "/home/kalen/src/A/src/A.c" 3 4
)
# 998 "/home/kalen/src/A/src/A.c"
; numeric_type.members[numeric_type.member_count].member_type = A_TYPE_CONSTRUCT; numeric_type.member_count++; numeric_type.with_bool = & numeric_with_bool; numeric_type.members[numeric_type.member_count].name = "bool"; numeric_type.members[numeric_type.member_count].args = (args_t) { 2, &numeric_type, &bool_type }; numeric_type.members[numeric_type.member_count].type = &numeric_type; numeric_type.members[numeric_type.member_count].offset = 
# 998 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 998 "/home/kalen/src/A/src/A.c"
numeric_f
# 998 "/home/kalen/src/A/src/A.c" 3 4
, 
# 998 "/home/kalen/src/A/src/A.c"
with_bool
# 998 "/home/kalen/src/A/src/A.c" 3 4
)
# 998 "/home/kalen/src/A/src/A.c"
; numeric_type.members[numeric_type.member_count].member_type = A_TYPE_CONSTRUCT; numeric_type.member_count++; numeric_type.with_num = & numeric_with_num; numeric_type.members[numeric_type.member_count].name = "num"; numeric_type.members[numeric_type.member_count].args = (args_t) { 2, &numeric_type, &num_type }; numeric_type.members[numeric_type.member_count].type = &numeric_type; numeric_type.members[numeric_type.member_count].offset = 
# 998 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 998 "/home/kalen/src/A/src/A.c"
numeric_f
# 998 "/home/kalen/src/A/src/A.c" 3 4
, 
# 998 "/home/kalen/src/A/src/A.c"
with_num
# 998 "/home/kalen/src/A/src/A.c" 3 4
)
# 998 "/home/kalen/src/A/src/A.c"
; numeric_type.members[numeric_type.member_count].member_type = A_TYPE_CONSTRUCT; numeric_type.member_count++;; A_push_type(&numeric_type); return (bool)1; } }
A string_like_header; string_like_f string_like_type; static __attribute__((constructor)) bool global_string_like() { string_like_f* type_ref = &string_like_type; A_f* base_ref = &A_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_string_like); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(A_f)); static member_t members[16 + sizeof(string_like_type) / sizeof(void*)]; string_like_type.parent_type = &A_type; string_like_type.name = "string_like"; string_like_type.size = 0; string_like_type.members = members; string_like_type.traits = 0; string_like_type.arb = ((void*)0); ; A_push_type(&string_like_type); return (bool)1; } }
A nil_header; nil_f nil_type; static __attribute__((constructor)) bool global_nil() { nil_f* type_ref = &nil_type; A_f* base_ref = &A_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_nil); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(A_f)); static member_t members[16 + sizeof(nil_type) / sizeof(void*)]; nil_type.parent_type = &A_type; nil_type.name = "nil"; nil_type.size = 0; nil_type.members = members; nil_type.traits = 0; nil_type.arb = ((void*)0); ; A_push_type(&nil_type); return (bool)1; } }
A raw_header; raw_f raw_type; static __attribute__((constructor)) bool global_raw() { raw_f* type_ref = &raw_type; A_f* base_ref = &A_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_raw); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(A_f)); static member_t members[16 + sizeof(raw_type) / sizeof(void*)]; raw_type.parent_type = &A_type; raw_type.name = "raw"; raw_type.size = 0; raw_type.members = members; raw_type.traits = 0; raw_type.arb = ((void*)0); ; A_push_type(&raw_type); return (bool)1; } }

A u8_header; u8_f u8_type; static __attribute__((constructor)) bool global_u8() { u8_f* type_ref = &u8_type; numeric_f* base_ref = &numeric_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_u8); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(numeric_f)); static member_t members[16 + sizeof(u8_type) / sizeof(void*)]; u8_type.parent_type = &numeric_type; u8_type.name = "u8"; u8_type.size = sizeof(u8); u8_type.members = members; u8_type.traits = 0; u8_type.arb = primitive_ffi_arb(((struct A_f*)&u8_type)); A_push_type(&u8_type); return (bool)1; } }
A u16_header; u16_f u16_type; static __attribute__((constructor)) bool global_u16() { u16_f* type_ref = &u16_type; numeric_f* base_ref = &numeric_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_u16); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(numeric_f)); static member_t members[16 + sizeof(u16_type) / sizeof(void*)]; u16_type.parent_type = &numeric_type; u16_type.name = "u16"; u16_type.size = sizeof(u16); u16_type.members = members; u16_type.traits = 0; u16_type.arb = primitive_ffi_arb(((struct A_f*)&u16_type)); A_push_type(&u16_type); return (bool)1; } }
A u32_header; u32_f u32_type; static __attribute__((constructor)) bool global_u32() { u32_f* type_ref = &u32_type; numeric_f* base_ref = &numeric_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_u32); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(numeric_f)); static member_t members[16 + sizeof(u32_type) / sizeof(void*)]; u32_type.parent_type = &numeric_type; u32_type.name = "u32"; u32_type.size = sizeof(u32); u32_type.members = members; u32_type.traits = 0; u32_type.arb = primitive_ffi_arb(((struct A_f*)&u32_type)); A_push_type(&u32_type); return (bool)1; } }
A u64_header; u64_f u64_type; static __attribute__((constructor)) bool global_u64() { u64_f* type_ref = &u64_type; numeric_f* base_ref = &numeric_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_u64); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(numeric_f)); static member_t members[16 + sizeof(u64_type) / sizeof(void*)]; u64_type.parent_type = &numeric_type; u64_type.name = "u64"; u64_type.size = sizeof(u64); u64_type.members = members; u64_type.traits = 0; u64_type.arb = primitive_ffi_arb(((struct A_f*)&u64_type)); A_push_type(&u64_type); return (bool)1; } }
A i8_header; i8_f i8_type; static __attribute__((constructor)) bool global_i8() { i8_f* type_ref = &i8_type; numeric_f* base_ref = &numeric_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_i8); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(numeric_f)); static member_t members[16 + sizeof(i8_type) / sizeof(void*)]; i8_type.parent_type = &numeric_type; i8_type.name = "i8"; i8_type.size = sizeof(i8); i8_type.members = members; i8_type.traits = 0; i8_type.arb = primitive_ffi_arb(((struct A_f*)&i8_type)); A_push_type(&i8_type); return (bool)1; } }
A i16_header; i16_f i16_type; static __attribute__((constructor)) bool global_i16() { i16_f* type_ref = &i16_type; numeric_f* base_ref = &numeric_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_i16); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(numeric_f)); static member_t members[16 + sizeof(i16_type) / sizeof(void*)]; i16_type.parent_type = &numeric_type; i16_type.name = "i16"; i16_type.size = sizeof(i16); i16_type.members = members; i16_type.traits = 0; i16_type.arb = primitive_ffi_arb(((struct A_f*)&i16_type)); A_push_type(&i16_type); return (bool)1; } }
A i32_header; i32_f i32_type; static __attribute__((constructor)) bool global_i32() { i32_f* type_ref = &i32_type; numeric_f* base_ref = &numeric_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_i32); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(numeric_f)); static member_t members[16 + sizeof(i32_type) / sizeof(void*)]; i32_type.parent_type = &numeric_type; i32_type.name = "i32"; i32_type.size = sizeof(i32); i32_type.members = members; i32_type.traits = 0; i32_type.arb = primitive_ffi_arb(((struct A_f*)&i32_type)); A_push_type(&i32_type); return (bool)1; } }
A i64_header; i64_f i64_type; static __attribute__((constructor)) bool global_i64() { i64_f* type_ref = &i64_type; numeric_f* base_ref = &numeric_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_i64); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(numeric_f)); static member_t members[16 + sizeof(i64_type) / sizeof(void*)]; i64_type.parent_type = &numeric_type; i64_type.name = "i64"; i64_type.size = sizeof(i64); i64_type.members = members; i64_type.traits = 0; i64_type.arb = primitive_ffi_arb(((struct A_f*)&i64_type)); A_push_type(&i64_type); return (bool)1; } }
A bool_header; bool_f bool_type; static __attribute__((constructor)) bool global_bool() { bool_f* type_ref = &bool_type; numeric_f* base_ref = &numeric_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_bool); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(numeric_f)); static member_t members[16 + sizeof(bool_type) / sizeof(void*)]; bool_type.parent_type = &numeric_type; bool_type.name = "bool"; bool_type.size = sizeof(bool); bool_type.members = members; bool_type.traits = 0; bool_type.arb = primitive_ffi_arb(((struct A_f*)&bool_type)); A_push_type(&bool_type); return (bool)1; } }
A num_header; num_f num_type; static __attribute__((constructor)) bool global_num() { num_f* type_ref = &num_type; numeric_f* base_ref = &numeric_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_num); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(numeric_f)); static member_t members[16 + sizeof(num_type) / sizeof(void*)]; num_type.parent_type = &numeric_type; num_type.name = "num"; num_type.size = sizeof(num); num_type.members = members; num_type.traits = 0; num_type.arb = primitive_ffi_arb(((struct A_f*)&num_type)); A_push_type(&num_type); return (bool)1; } }
A sz_header; sz_f sz_type; static __attribute__((constructor)) bool global_sz() { sz_f* type_ref = &sz_type; numeric_f* base_ref = &numeric_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_sz); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(numeric_f)); static member_t members[16 + sizeof(sz_type) / sizeof(void*)]; sz_type.parent_type = &numeric_type; sz_type.name = "sz"; sz_type.size = sizeof(sz); sz_type.members = members; sz_type.traits = 0; sz_type.arb = primitive_ffi_arb(((struct A_f*)&sz_type)); A_push_type(&sz_type); return (bool)1; } }
A f32_header; f32_f f32_type; static __attribute__((constructor)) bool global_f32() { f32_f* type_ref = &f32_type; numeric_f* base_ref = &numeric_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_f32); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(numeric_f)); static member_t members[16 + sizeof(f32_type) / sizeof(void*)]; f32_type.parent_type = &numeric_type; f32_type.name = "f32"; f32_type.size = sizeof(f32); f32_type.members = members; f32_type.traits = 0; f32_type.arb = primitive_ffi_arb(((struct A_f*)&f32_type)); A_push_type(&f32_type); return (bool)1; } }
A f64_header; f64_f f64_type; static __attribute__((constructor)) bool global_f64() { f64_f* type_ref = &f64_type; numeric_f* base_ref = &numeric_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_f64); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(numeric_f)); static member_t members[16 + sizeof(f64_type) / sizeof(void*)]; f64_type.parent_type = &numeric_type; f64_type.name = "f64"; f64_type.size = sizeof(f64); f64_type.members = members; f64_type.traits = 0; f64_type.arb = primitive_ffi_arb(((struct A_f*)&f64_type)); A_push_type(&f64_type); return (bool)1; } }
A f128_header; f128_f f128_type; static __attribute__((constructor)) bool global_f128() { f128_f* type_ref = &f128_type; numeric_f* base_ref = &numeric_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_f128); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(numeric_f)); static member_t members[16 + sizeof(f128_type) / sizeof(void*)]; f128_type.parent_type = &numeric_type; f128_type.name = "f128"; f128_type.size = sizeof(f128); f128_type.members = members; f128_type.traits = 0; f128_type.arb = primitive_ffi_arb(((struct A_f*)&f128_type)); A_push_type(&f128_type); return (bool)1; } }
A cstr_header; cstr_f cstr_type; static __attribute__((constructor)) bool global_cstr() { cstr_f* type_ref = &cstr_type; string_like_f* base_ref = &string_like_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_cstr); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(string_like_f)); static member_t members[16 + sizeof(cstr_type) / sizeof(void*)]; cstr_type.parent_type = &string_like_type; cstr_type.name = "cstr"; cstr_type.size = sizeof(cstr); cstr_type.members = members; cstr_type.traits = 0; cstr_type.arb = primitive_ffi_arb(((struct A_f*)&cstr_type)); A_push_type(&cstr_type); return (bool)1; } }
A symbol_header; symbol_f symbol_type; static __attribute__((constructor)) bool global_symbol() { symbol_f* type_ref = &symbol_type; string_like_f* base_ref = &string_like_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_symbol); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(string_like_f)); static member_t members[16 + sizeof(symbol_type) / sizeof(void*)]; symbol_type.parent_type = &string_like_type; symbol_type.name = "symbol"; symbol_type.size = sizeof(symbol); symbol_type.members = members; symbol_type.traits = 0; symbol_type.arb = primitive_ffi_arb(((struct A_f*)&symbol_type)); A_push_type(&symbol_type); return (bool)1; } }
A none_header; none_f none_type; static __attribute__((constructor)) bool global_none() { none_f* type_ref = &none_type; nil_f* base_ref = &nil_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_none); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(nil_f)); static member_t members[16 + sizeof(none_type) / sizeof(void*)]; none_type.parent_type = &nil_type; none_type.name = "none"; none_type.size = sizeof(none); none_type.members = members; none_type.traits = 0; none_type.arb = primitive_ffi_arb(((struct A_f*)&none_type)); A_push_type(&none_type); return (bool)1; } }
A AType_header; AType_f AType_type; static __attribute__((constructor)) bool global_AType() { AType_f* type_ref = &AType_type; raw_f* base_ref = &raw_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_AType); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(raw_f)); static member_t members[16 + sizeof(AType_type) / sizeof(void*)]; AType_type.parent_type = &raw_type; AType_type.name = "AType"; AType_type.size = sizeof(AType); AType_type.members = members; AType_type.traits = 0; AType_type.arb = primitive_ffi_arb(((struct A_f*)&AType_type)); A_push_type(&AType_type); return (bool)1; } }
A handle_header; handle_f handle_type; static __attribute__((constructor)) bool global_handle() { handle_f* type_ref = &handle_type; raw_f* base_ref = &raw_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_handle); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(raw_f)); static member_t members[16 + sizeof(handle_type) / sizeof(void*)]; handle_type.parent_type = &raw_type; handle_type.name = "handle"; handle_type.size = sizeof(handle); handle_type.members = members; handle_type.traits = 0; handle_type.arb = primitive_ffi_arb(((struct A_f*)&handle_type)); A_push_type(&handle_type); return (bool)1; } }

OPType_f OPType_type; static __attribute__((constructor)) bool global_OPType() { OPType_f* type_ref = &OPType_type; A_f* base_ref = &A_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_OPType); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(A_f)); static member_t members[16 + OPType_ENUM_COUNT + sizeof(OPType_type) / sizeof(void*)]; OPType_type.parent_type = & A_type; OPType_type.name = "OPType"; OPType_type.size = sizeof(enum OPType); OPType_type.members = members; OPType_type.traits = A_TRAIT_ENUM; OPType_type.arb = primitive_ffi_arb(((struct A_f*)&i32_type)); OPType_type.members[OPType_type.member_count].name = "add"; OPType_type.members[OPType_type.member_count].offset = OPType_add; OPType_type.members[OPType_type.member_count].type = &i32_type; OPType_type.members[OPType_type.member_count].member_type = A_TYPE_ENUMV; OPType_type.member_count++; OPType_type.members[OPType_type.member_count].name = "sub"; OPType_type.members[OPType_type.member_count].offset = OPType_sub; OPType_type.members[OPType_type.member_count].type = &i32_type; OPType_type.members[OPType_type.member_count].member_type = A_TYPE_ENUMV; OPType_type.member_count++; OPType_type.members[OPType_type.member_count].name = "mul"; OPType_type.members[OPType_type.member_count].offset = OPType_mul; OPType_type.members[OPType_type.member_count].type = &i32_type; OPType_type.members[OPType_type.member_count].member_type = A_TYPE_ENUMV; OPType_type.member_count++; OPType_type.members[OPType_type.member_count].name = "div"; OPType_type.members[OPType_type.member_count].offset = OPType_div; OPType_type.members[OPType_type.member_count].type = &i32_type; OPType_type.members[OPType_type.member_count].member_type = A_TYPE_ENUMV; OPType_type.member_count++; OPType_type.members[OPType_type.member_count].name = "or"; OPType_type.members[OPType_type.member_count].offset = OPType_or; OPType_type.members[OPType_type.member_count].type = &i32_type; OPType_type.members[OPType_type.member_count].member_type = A_TYPE_ENUMV; OPType_type.member_count++; OPType_type.members[OPType_type.member_count].name = "and"; OPType_type.members[OPType_type.member_count].offset = OPType_and; OPType_type.members[OPType_type.member_count].type = &i32_type; OPType_type.members[OPType_type.member_count].member_type = A_TYPE_ENUMV; OPType_type.member_count++; OPType_type.members[OPType_type.member_count].name = "xor"; OPType_type.members[OPType_type.member_count].offset = OPType_xor; OPType_type.members[OPType_type.member_count].type = &i32_type; OPType_type.members[OPType_type.member_count].member_type = A_TYPE_ENUMV; OPType_type.member_count++; OPType_type.members[OPType_type.member_count].name = "right"; OPType_type.members[OPType_type.member_count].offset = OPType_right; OPType_type.members[OPType_type.member_count].type = &i32_type; OPType_type.members[OPType_type.member_count].member_type = A_TYPE_ENUMV; OPType_type.member_count++; OPType_type.members[OPType_type.member_count].name = "left"; OPType_type.members[OPType_type.member_count].offset = OPType_left; OPType_type.members[OPType_type.member_count].type = &i32_type; OPType_type.members[OPType_type.member_count].member_type = A_TYPE_ENUMV; OPType_type.member_count++; OPType_type.members[OPType_type.member_count].name = "assign"; OPType_type.members[OPType_type.member_count].offset = OPType_assign; OPType_type.members[OPType_type.member_count].type = &i32_type; OPType_type.members[OPType_type.member_count].member_type = A_TYPE_ENUMV; OPType_type.member_count++; OPType_type.members[OPType_type.member_count].name = "assign_add"; OPType_type.members[OPType_type.member_count].offset = OPType_assign_add; OPType_type.members[OPType_type.member_count].type = &i32_type; OPType_type.members[OPType_type.member_count].member_type = A_TYPE_ENUMV; OPType_type.member_count++; OPType_type.members[OPType_type.member_count].name = "assign_sub"; OPType_type.members[OPType_type.member_count].offset = OPType_assign_sub; OPType_type.members[OPType_type.member_count].type = &i32_type; OPType_type.members[OPType_type.member_count].member_type = A_TYPE_ENUMV; OPType_type.member_count++; OPType_type.members[OPType_type.member_count].name = "assign_mul"; OPType_type.members[OPType_type.member_count].offset = OPType_assign_mul; OPType_type.members[OPType_type.member_count].type = &i32_type; OPType_type.members[OPType_type.member_count].member_type = A_TYPE_ENUMV; OPType_type.member_count++; OPType_type.members[OPType_type.member_count].name = "assign_div"; OPType_type.members[OPType_type.member_count].offset = OPType_assign_div; OPType_type.members[OPType_type.member_count].type = &i32_type; OPType_type.members[OPType_type.member_count].member_type = A_TYPE_ENUMV; OPType_type.member_count++; OPType_type.members[OPType_type.member_count].name = "assign_or"; OPType_type.members[OPType_type.member_count].offset = OPType_assign_or; OPType_type.members[OPType_type.member_count].type = &i32_type; OPType_type.members[OPType_type.member_count].member_type = A_TYPE_ENUMV; OPType_type.member_count++; OPType_type.members[OPType_type.member_count].name = "assign_and"; OPType_type.members[OPType_type.member_count].offset = OPType_assign_and; OPType_type.members[OPType_type.member_count].type = &i32_type; OPType_type.members[OPType_type.member_count].member_type = A_TYPE_ENUMV; OPType_type.member_count++; OPType_type.members[OPType_type.member_count].name = "assign_xor"; OPType_type.members[OPType_type.member_count].offset = OPType_assign_xor; OPType_type.members[OPType_type.member_count].type = &i32_type; OPType_type.members[OPType_type.member_count].member_type = A_TYPE_ENUMV; OPType_type.member_count++; OPType_type.members[OPType_type.member_count].name = "assign_right"; OPType_type.members[OPType_type.member_count].offset = OPType_assign_right; OPType_type.members[OPType_type.member_count].type = &i32_type; OPType_type.members[OPType_type.member_count].member_type = A_TYPE_ENUMV; OPType_type.member_count++; OPType_type.members[OPType_type.member_count].name = "assign_left"; OPType_type.members[OPType_type.member_count].offset = OPType_assign_left; OPType_type.members[OPType_type.member_count].type = &i32_type; OPType_type.members[OPType_type.member_count].member_type = A_TYPE_ENUMV; OPType_type.member_count++; OPType_type.members[OPType_type.member_count].name = "mod_assign"; OPType_type.members[OPType_type.member_count].offset = OPType_mod_assign; OPType_type.members[OPType_type.member_count].type = &i32_type; OPType_type.members[OPType_type.member_count].member_type = A_TYPE_ENUMV; OPType_type.member_count++;; A_push_type(&OPType_type); return (bool)1; } }
A path_header; path_f path_type; static __attribute__((constructor)) bool global_path() { path_f* type_ref = &path_type; A_f* base_ref = &A_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_path); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(A_f)); static member_t members[16 + sizeof(path_type) / sizeof(void*)]; path_type.parent_type = &A_type; path_type.name = "path"; path_type.size = sizeof(struct path); path_type.members = members; path_type.traits = 0; path_type.arb = primitive_ffi_arb(((struct A_f*)&cstr_type)); path_type . exists = & path_exists; path_type.members[path_type.member_count].name = "exists"; path_type.members[path_type.member_count].args = (args_t) { 2, &path_type, &path_type }; path_type.members[path_type.member_count].type = &bool_type; path_type.members[path_type.member_count].offset = 
# 1024 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1024 "/home/kalen/src/A/src/A.c"
path_f
# 1024 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1024 "/home/kalen/src/A/src/A.c"
exists
# 1024 "/home/kalen/src/A/src/A.c" 3 4
)
# 1024 "/home/kalen/src/A/src/A.c"
; path_type.members[path_type.member_count].member_type = A_TYPE_IMETHOD; path_type.member_count++; path_type . make_dir = & path_make_dir; path_type.members[path_type.member_count].name = "make_dir"; path_type.members[path_type.member_count].args = (args_t) { 2, &path_type, &path_type }; path_type.members[path_type.member_count].type = &bool_type; path_type.members[path_type.member_count].offset = 
# 1024 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1024 "/home/kalen/src/A/src/A.c"
path_f
# 1024 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1024 "/home/kalen/src/A/src/A.c"
make_dir
# 1024 "/home/kalen/src/A/src/A.c" 3 4
)
# 1024 "/home/kalen/src/A/src/A.c"
; path_type.members[path_type.member_count].member_type = A_TYPE_IMETHOD; path_type.member_count++; path_type . is_empty = & path_is_empty; path_type.members[path_type.member_count].name = "is_empty"; path_type.members[path_type.member_count].args = (args_t) { 2, &path_type, &path_type }; path_type.members[path_type.member_count].type = &bool_type; path_type.members[path_type.member_count].offset = 
# 1024 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1024 "/home/kalen/src/A/src/A.c"
path_f
# 1024 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1024 "/home/kalen/src/A/src/A.c"
is_empty
# 1024 "/home/kalen/src/A/src/A.c" 3 4
)
# 1024 "/home/kalen/src/A/src/A.c"
; path_type.members[path_type.member_count].member_type = A_TYPE_IMETHOD; path_type.member_count++; path_type . directory = & path_directory; path_type.members[path_type.member_count].name = "directory"; path_type.members[path_type.member_count].args = (args_t) { 2, &path_type, &path_type }; path_type.members[path_type.member_count].type = &path_type; path_type.members[path_type.member_count].offset = 
# 1024 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1024 "/home/kalen/src/A/src/A.c"
path_f
# 1024 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1024 "/home/kalen/src/A/src/A.c"
directory
# 1024 "/home/kalen/src/A/src/A.c" 3 4
)
# 1024 "/home/kalen/src/A/src/A.c"
; path_type.members[path_type.member_count].member_type = A_TYPE_IMETHOD; path_type.member_count++; path_type . stem = & path_stem; path_type.members[path_type.member_count].name = "stem"; path_type.members[path_type.member_count].args = (args_t) { 2, &path_type, &path_type }; path_type.members[path_type.member_count].type = &string_type; path_type.members[path_type.member_count].offset = 
# 1024 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1024 "/home/kalen/src/A/src/A.c"
path_f
# 1024 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1024 "/home/kalen/src/A/src/A.c"
stem
# 1024 "/home/kalen/src/A/src/A.c" 3 4
)
# 1024 "/home/kalen/src/A/src/A.c"
; path_type.members[path_type.member_count].member_type = A_TYPE_IMETHOD; path_type.member_count++; path_type . filename = & path_filename; path_type.members[path_type.member_count].name = "filename"; path_type.members[path_type.member_count].args = (args_t) { 2, &path_type, &path_type }; path_type.members[path_type.member_count].type = &string_type; path_type.members[path_type.member_count].offset = 
# 1024 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1024 "/home/kalen/src/A/src/A.c"
path_f
# 1024 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1024 "/home/kalen/src/A/src/A.c"
filename
# 1024 "/home/kalen/src/A/src/A.c" 3 4
)
# 1024 "/home/kalen/src/A/src/A.c"
; path_type.members[path_type.member_count].member_type = A_TYPE_IMETHOD; path_type.member_count++; path_type . absolute = & path_absolute; path_type.members[path_type.member_count].name = "absolute"; path_type.members[path_type.member_count].args = (args_t) { 2, &path_type, &path_type }; path_type.members[path_type.member_count].type = &path_type; path_type.members[path_type.member_count].offset = 
# 1024 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1024 "/home/kalen/src/A/src/A.c"
path_f
# 1024 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1024 "/home/kalen/src/A/src/A.c"
absolute
# 1024 "/home/kalen/src/A/src/A.c" 3 4
)
# 1024 "/home/kalen/src/A/src/A.c"
; path_type.members[path_type.member_count].member_type = A_TYPE_IMETHOD; path_type.member_count++; path_type . parent = & path_parent; path_type.members[path_type.member_count].name = "parent"; path_type.members[path_type.member_count].args = (args_t) { 2, &path_type, &path_type }; path_type.members[path_type.member_count].type = &path_type; path_type.members[path_type.member_count].offset = 
# 1024 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1024 "/home/kalen/src/A/src/A.c"
path_f
# 1024 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1024 "/home/kalen/src/A/src/A.c"
parent
# 1024 "/home/kalen/src/A/src/A.c" 3 4
)
# 1024 "/home/kalen/src/A/src/A.c"
; path_type.members[path_type.member_count].member_type = A_TYPE_IMETHOD; path_type.member_count++; path_type . read = & path_read; path_type.members[path_type.member_count].name = "read"; path_type.members[path_type.member_count].args = (args_t) { 3, &path_type, &path_type, &AType_type }; path_type.members[path_type.member_count].type = &A_type; path_type.members[path_type.member_count].offset = 
# 1024 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1024 "/home/kalen/src/A/src/A.c"
path_f
# 1024 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1024 "/home/kalen/src/A/src/A.c"
read
# 1024 "/home/kalen/src/A/src/A.c" 3 4
)
# 1024 "/home/kalen/src/A/src/A.c"
; path_type.members[path_type.member_count].member_type = A_TYPE_IMETHOD; path_type.member_count++; path_type . change_ext = & path_change_ext; path_type.members[path_type.member_count].name = "change_ext"; path_type.members[path_type.member_count].args = (args_t) { 3, &path_type, &path_type, &cstr_type }; path_type.members[path_type.member_count].type = &path_type; path_type.members[path_type.member_count].offset = 
# 1024 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1024 "/home/kalen/src/A/src/A.c"
path_f
# 1024 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1024 "/home/kalen/src/A/src/A.c"
change_ext
# 1024 "/home/kalen/src/A/src/A.c" 3 4
)
# 1024 "/home/kalen/src/A/src/A.c"
; path_type.members[path_type.member_count].member_type = A_TYPE_IMETHOD; path_type.member_count++; path_type . cwd = & path_cwd; path_type.members[path_type.member_count].name = "cwd"; path_type.members[path_type.member_count].args = (args_t) { 1, &sz_type }; path_type.members[path_type.member_count].type = &path_type; path_type.members[path_type.member_count].offset = 
# 1024 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1024 "/home/kalen/src/A/src/A.c"
path_f
# 1024 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1024 "/home/kalen/src/A/src/A.c"
cwd
# 1024 "/home/kalen/src/A/src/A.c" 3 4
)
# 1024 "/home/kalen/src/A/src/A.c"
; path_type.members[path_type.member_count].member_type = A_TYPE_SMETHOD; path_type.member_count++; path_type . hash = & path_hash; path_type.with_cstr = & path_with_cstr;; A_push_type(&path_type); return (bool)1; } }
A string_header; string_f string_type; static __attribute__((constructor)) bool global_string() { string_f* type_ref = &string_type; A_f* base_ref = &A_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_string); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(A_f)); static member_t members[16 + sizeof(string_type) / sizeof(void*)]; string_type.parent_type = &A_type; string_type.name = "string"; string_type.size = sizeof(struct string); string_type.members = members; string_type.traits = 0; string_type.arb = primitive_ffi_arb(((struct A_f*)&cstr_type)); string_type . split = & string_split; string_type.members[string_type.member_count].name = "split"; string_type.members[string_type.member_count].args = (args_t) { 3, &string_type, &string_type, &cstr_type }; string_type.members[string_type.member_count].type = &array_type; string_type.members[string_type.member_count].offset = 
# 1025 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1025 "/home/kalen/src/A/src/A.c"
string_f
# 1025 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1025 "/home/kalen/src/A/src/A.c"
split
# 1025 "/home/kalen/src/A/src/A.c" 3 4
)
# 1025 "/home/kalen/src/A/src/A.c"
; string_type.members[string_type.member_count].member_type = A_TYPE_IMETHOD; string_type.member_count++; string_type . index_of = & string_index_of; string_type.members[string_type.member_count].name = "index_of"; string_type.members[string_type.member_count].args = (args_t) { 3, &string_type, &string_type, &cstr_type }; string_type.members[string_type.member_count].type = &num_type; string_type.members[string_type.member_count].offset = 
# 1025 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1025 "/home/kalen/src/A/src/A.c"
string_f
# 1025 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1025 "/home/kalen/src/A/src/A.c"
index_of
# 1025 "/home/kalen/src/A/src/A.c" 3 4
)
# 1025 "/home/kalen/src/A/src/A.c"
; string_type.members[string_type.member_count].member_type = A_TYPE_IMETHOD; string_type.member_count++; string_type . append = & string_append; string_type.members[string_type.member_count].name = "append"; string_type.members[string_type.member_count].args = (args_t) { 3, &string_type, &string_type, &cstr_type }; string_type.members[string_type.member_count].type = &none_type; string_type.members[string_type.member_count].offset = 
# 1025 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1025 "/home/kalen/src/A/src/A.c"
string_f
# 1025 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1025 "/home/kalen/src/A/src/A.c"
append
# 1025 "/home/kalen/src/A/src/A.c" 3 4
)
# 1025 "/home/kalen/src/A/src/A.c"
; string_type.members[string_type.member_count].member_type = A_TYPE_IMETHOD; string_type.member_count++; string_type . mid = & string_mid; string_type.members[string_type.member_count].name = "mid"; string_type.members[string_type.member_count].args = (args_t) { 4, &string_type, &string_type, &num_type, &num_type }; string_type.members[string_type.member_count].type = &string_type; string_type.members[string_type.member_count].offset = 
# 1025 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1025 "/home/kalen/src/A/src/A.c"
string_f
# 1025 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1025 "/home/kalen/src/A/src/A.c"
mid
# 1025 "/home/kalen/src/A/src/A.c" 3 4
)
# 1025 "/home/kalen/src/A/src/A.c"
; string_type.members[string_type.member_count].member_type = A_TYPE_IMETHOD; string_type.member_count++; string_type.with_sz = & string_with_sz; string_type.members[string_type.member_count].name = "sz"; string_type.members[string_type.member_count].args = (args_t) { 2, &string_type, &sz_type }; string_type.members[string_type.member_count].type = &string_type; string_type.members[string_type.member_count].offset = 
# 1025 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1025 "/home/kalen/src/A/src/A.c"
string_f
# 1025 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1025 "/home/kalen/src/A/src/A.c"
with_sz
# 1025 "/home/kalen/src/A/src/A.c" 3 4
)
# 1025 "/home/kalen/src/A/src/A.c"
; string_type.members[string_type.member_count].member_type = A_TYPE_CONSTRUCT; string_type.member_count++; string_type.with_cstr = & string_with_cstr; string_type . hash = & string_hash; string_type . compare = & string_compare; string_type . cast_bool = & string_cast_bool;; A_push_type(&string_type); return (bool)1; } }
A item_header; item_f item_type; static __attribute__((constructor)) bool global_item() { item_f* type_ref = &item_type; A_f* base_ref = &A_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_item); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(A_f)); static member_t members[16 + sizeof(item_type) / sizeof(void*)]; item_type.parent_type = &A_type; item_type.name = "item"; item_type.size = sizeof(struct item); item_type.members = members; item_type.traits = 0; item_type.arb = primitive_ffi_arb(((struct A_f*)&cstr_type)); item_type . hash = & item_hash; item_type.with_symbol = & item_with_symbol; item_type.members[item_type.member_count].name = "symbol"; item_type.members[item_type.member_count].args = (args_t) { 3, &item_type, &symbol_type, &A_type }; item_type.members[item_type.member_count].type = &item_type; item_type.members[item_type.member_count].offset = 
# 1026 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1026 "/home/kalen/src/A/src/A.c"
item_f
# 1026 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1026 "/home/kalen/src/A/src/A.c"
with_symbol
# 1026 "/home/kalen/src/A/src/A.c" 3 4
)
# 1026 "/home/kalen/src/A/src/A.c"
; item_type.members[item_type.member_count].member_type = A_TYPE_CONSTRUCT; item_type.member_count++;; A_push_type(&item_type); return (bool)1; } }
A collection_header; collection_f collection_type; static __attribute__((constructor)) bool global_collection() { collection_f* type_ref = &collection_type; A_f* base_ref = &A_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_collection); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(A_f)); static member_t members[16 + sizeof(collection_type) / sizeof(void*)]; collection_type.parent_type = &A_type; collection_type.name = "collection"; collection_type.size = sizeof(struct collection); collection_type.members = members; collection_type.traits = 0; ; A_push_type(&collection_type); return (bool)1; } }
A list_header; list_f list_type; static __attribute__((constructor)) bool global_list() { list_f* type_ref = &list_type; A_f* base_ref = &A_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_list); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(A_f)); static member_t members[16 + sizeof(list_type) / sizeof(void*)]; list_type.parent_type = &A_type; list_type.name = "list"; list_type.size = sizeof(struct list); list_type.members = members; list_type.traits = 0; list_type.arb = primitive_ffi_arb(((struct A_f*)&cstr_type)); list_type.members[list_type.member_count].name = "public_integer"; list_type.members[list_type.member_count].offset = 
# 1028 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1028 "/home/kalen/src/A/src/A.c"
struct list
# 1028 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1028 "/home/kalen/src/A/src/A.c"
public_integer
# 1028 "/home/kalen/src/A/src/A.c" 3 4
)
# 1028 "/home/kalen/src/A/src/A.c"
; list_type.members[list_type.member_count].type = &i32_type; list_type.members[list_type.member_count].member_type = A_TYPE_PROP; list_type.member_count++; list_type . pop = & list_pop; list_type.members[list_type.member_count].name = "pop"; list_type.members[list_type.member_count].args = (args_t) { 2, &list_type, &list_type }; list_type.members[list_type.member_count].type = &A_type; list_type.members[list_type.member_count].offset = 
# 1028 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1028 "/home/kalen/src/A/src/A.c"
list_f
# 1028 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1028 "/home/kalen/src/A/src/A.c"
pop
# 1028 "/home/kalen/src/A/src/A.c" 3 4
)
# 1028 "/home/kalen/src/A/src/A.c"
; list_type.members[list_type.member_count].member_type = A_TYPE_IMETHOD; list_type.member_count++; list_type . push = & list_push; list_type.members[list_type.member_count].name = "push"; list_type.members[list_type.member_count].args = (args_t) { 3, &list_type, &list_type, &A_type }; list_type.members[list_type.member_count].type = &A_type; list_type.members[list_type.member_count].offset = 
# 1028 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1028 "/home/kalen/src/A/src/A.c"
list_f
# 1028 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1028 "/home/kalen/src/A/src/A.c"
push
# 1028 "/home/kalen/src/A/src/A.c" 3 4
)
# 1028 "/home/kalen/src/A/src/A.c"
; list_type.members[list_type.member_count].member_type = A_TYPE_IMETHOD; list_type.member_count++; list_type . remove = & list_remove; list_type.members[list_type.member_count].name = "remove"; list_type.members[list_type.member_count].args = (args_t) { 3, &list_type, &list_type, &num_type }; list_type.members[list_type.member_count].type = &none_type; list_type.members[list_type.member_count].offset = 
# 1028 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1028 "/home/kalen/src/A/src/A.c"
list_f
# 1028 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1028 "/home/kalen/src/A/src/A.c"
remove
# 1028 "/home/kalen/src/A/src/A.c" 3 4
)
# 1028 "/home/kalen/src/A/src/A.c"
; list_type.members[list_type.member_count].member_type = A_TYPE_IMETHOD; list_type.member_count++; list_type . remove_item = & list_remove_item; list_type.members[list_type.member_count].name = "remove_item"; list_type.members[list_type.member_count].args = (args_t) { 3, &list_type, &list_type, &item_type }; list_type.members[list_type.member_count].type = &none_type; list_type.members[list_type.member_count].offset = 
# 1028 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1028 "/home/kalen/src/A/src/A.c"
list_f
# 1028 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1028 "/home/kalen/src/A/src/A.c"
remove_item
# 1028 "/home/kalen/src/A/src/A.c" 3 4
)
# 1028 "/home/kalen/src/A/src/A.c"
; list_type.members[list_type.member_count].member_type = A_TYPE_IMETHOD; list_type.member_count++; list_type . get = & list_get; list_type.members[list_type.member_count].name = "get"; list_type.members[list_type.member_count].args = (args_t) { 3, &list_type, &list_type, &i32_type }; list_type.members[list_type.member_count].type = &A_type; list_type.members[list_type.member_count].offset = 
# 1028 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1028 "/home/kalen/src/A/src/A.c"
list_f
# 1028 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1028 "/home/kalen/src/A/src/A.c"
get
# 1028 "/home/kalen/src/A/src/A.c" 3 4
)
# 1028 "/home/kalen/src/A/src/A.c"
; list_type.members[list_type.member_count].member_type = A_TYPE_IMETHOD; list_type.member_count++; list_type . count = & list_count; list_type.members[list_type.member_count].name = "count"; list_type.members[list_type.member_count].args = (args_t) { 2, &list_type, &list_type }; list_type.members[list_type.member_count].type = &num_type; list_type.members[list_type.member_count].offset = 
# 1028 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1028 "/home/kalen/src/A/src/A.c"
list_f
# 1028 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1028 "/home/kalen/src/A/src/A.c"
count
# 1028 "/home/kalen/src/A/src/A.c" 3 4
)
# 1028 "/home/kalen/src/A/src/A.c"
; list_type.members[list_type.member_count].member_type = A_TYPE_IMETHOD; list_type.member_count++;; A_push_type(&list_type); return (bool)1; } }
A array_header; array_f array_type; static __attribute__((constructor)) bool global_array() { array_f* type_ref = &array_type; A_f* base_ref = &A_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_array); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(A_f)); static member_t members[16 + sizeof(array_type) / sizeof(void*)]; array_type.parent_type = &A_type; array_type.name = "array"; array_type.size = sizeof(struct array); array_type.members = members; array_type.traits = 0; array_type.arb = primitive_ffi_arb(((struct A_f*)&cstr_type)); array_type . of_objects = & array_of_objects; array_type.members[array_type.member_count].name = "of_objects"; array_type.members[array_type.member_count].args = (args_t) { 1, &A_type }; array_type.members[array_type.member_count].type = &array_type; array_type.members[array_type.member_count].offset = 
# 1029 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1029 "/home/kalen/src/A/src/A.c"
array_f
# 1029 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1029 "/home/kalen/src/A/src/A.c"
of_objects
# 1029 "/home/kalen/src/A/src/A.c" 3 4
)
# 1029 "/home/kalen/src/A/src/A.c"
; array_type.members[array_type.member_count].member_type = A_TYPE_SMETHOD; array_type.member_count++; array_type . first = & array_first; array_type.members[array_type.member_count].name = "first"; array_type.members[array_type.member_count].args = (args_t) { 2, &array_type, &array_type }; array_type.members[array_type.member_count].type = &A_type; array_type.members[array_type.member_count].offset = 
# 1029 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1029 "/home/kalen/src/A/src/A.c"
array_f
# 1029 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1029 "/home/kalen/src/A/src/A.c"
first
# 1029 "/home/kalen/src/A/src/A.c" 3 4
)
# 1029 "/home/kalen/src/A/src/A.c"
; array_type.members[array_type.member_count].member_type = A_TYPE_IMETHOD; array_type.member_count++; array_type . last = & array_last; array_type.members[array_type.member_count].name = "last"; array_type.members[array_type.member_count].args = (args_t) { 2, &array_type, &array_type }; array_type.members[array_type.member_count].type = &A_type; array_type.members[array_type.member_count].offset = 
# 1029 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1029 "/home/kalen/src/A/src/A.c"
array_f
# 1029 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1029 "/home/kalen/src/A/src/A.c"
last
# 1029 "/home/kalen/src/A/src/A.c" 3 4
)
# 1029 "/home/kalen/src/A/src/A.c"
; array_type.members[array_type.member_count].member_type = A_TYPE_IMETHOD; array_type.member_count++; array_type . pop = & array_pop; array_type.members[array_type.member_count].name = "pop"; array_type.members[array_type.member_count].args = (args_t) { 2, &array_type, &array_type }; array_type.members[array_type.member_count].type = &array_type; array_type.members[array_type.member_count].offset = 
# 1029 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1029 "/home/kalen/src/A/src/A.c"
array_f
# 1029 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1029 "/home/kalen/src/A/src/A.c"
pop
# 1029 "/home/kalen/src/A/src/A.c" 3 4
)
# 1029 "/home/kalen/src/A/src/A.c"
; array_type.members[array_type.member_count].member_type = A_TYPE_IMETHOD; array_type.member_count++; array_type . remove = & array_remove; array_type.members[array_type.member_count].name = "remove"; array_type.members[array_type.member_count].args = (args_t) { 3, &array_type, &array_type, &num_type }; array_type.members[array_type.member_count].type = &array_type; array_type.members[array_type.member_count].offset = 
# 1029 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1029 "/home/kalen/src/A/src/A.c"
array_f
# 1029 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1029 "/home/kalen/src/A/src/A.c"
remove
# 1029 "/home/kalen/src/A/src/A.c" 3 4
)
# 1029 "/home/kalen/src/A/src/A.c"
; array_type.members[array_type.member_count].member_type = A_TYPE_IMETHOD; array_type.member_count++; array_type . push = & array_push; array_type.members[array_type.member_count].name = "push"; array_type.members[array_type.member_count].args = (args_t) { 3, &array_type, &array_type, &A_type }; array_type.members[array_type.member_count].type = &A_type; array_type.members[array_type.member_count].offset = 
# 1029 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1029 "/home/kalen/src/A/src/A.c"
array_f
# 1029 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1029 "/home/kalen/src/A/src/A.c"
push
# 1029 "/home/kalen/src/A/src/A.c" 3 4
)
# 1029 "/home/kalen/src/A/src/A.c"
; array_type.members[array_type.member_count].member_type = A_TYPE_IMETHOD; array_type.member_count++; array_type . get = & array_get; array_type.members[array_type.member_count].name = "get"; array_type.members[array_type.member_count].args = (args_t) { 3, &array_type, &array_type, &i32_type }; array_type.members[array_type.member_count].type = &A_type; array_type.members[array_type.member_count].offset = 
# 1029 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1029 "/home/kalen/src/A/src/A.c"
array_f
# 1029 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1029 "/home/kalen/src/A/src/A.c"
get
# 1029 "/home/kalen/src/A/src/A.c" 3 4
)
# 1029 "/home/kalen/src/A/src/A.c"
; array_type.members[array_type.member_count].member_type = A_TYPE_IMETHOD; array_type.member_count++; array_type . count = & array_count; array_type.members[array_type.member_count].name = "count"; array_type.members[array_type.member_count].args = (args_t) { 2, &array_type, &array_type }; array_type.members[array_type.member_count].type = &num_type; array_type.members[array_type.member_count].offset = 
# 1029 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1029 "/home/kalen/src/A/src/A.c"
array_f
# 1029 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1029 "/home/kalen/src/A/src/A.c"
count
# 1029 "/home/kalen/src/A/src/A.c" 3 4
)
# 1029 "/home/kalen/src/A/src/A.c"
; array_type.members[array_type.member_count].member_type = A_TYPE_IMETHOD; array_type.member_count++; array_type . index_of = & array_index_of; array_type.members[array_type.member_count].name = "index_of"; array_type.members[array_type.member_count].args = (args_t) { 3, &array_type, &array_type, &A_type }; array_type.members[array_type.member_count].type = &num_type; array_type.members[array_type.member_count].offset = 
# 1029 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1029 "/home/kalen/src/A/src/A.c"
array_f
# 1029 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1029 "/home/kalen/src/A/src/A.c"
index_of
# 1029 "/home/kalen/src/A/src/A.c" 3 4
)
# 1029 "/home/kalen/src/A/src/A.c"
; array_type.members[array_type.member_count].member_type = A_TYPE_IMETHOD; array_type.member_count++; array_type . operator_assign_add = & array_operator_assign_add; array_type.members[array_type.member_count].name = "combine_tokens(operator_, assign_add)"; array_type.members[array_type.member_count].args = (args_t) { 2, &array_type, &A_type }; array_type.members[array_type.member_count].type = &none_type; array_type.members[array_type.member_count].offset = 
# 1029 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1029 "/home/kalen/src/A/src/A.c"
array_f
# 1029 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1029 "/home/kalen/src/A/src/A.c"
operator_assign_add
# 1029 "/home/kalen/src/A/src/A.c" 3 4
)
# 1029 "/home/kalen/src/A/src/A.c"
; array_type.members[array_type.member_count].member_type = A_TYPE_OPERATOR; array_type.members[array_type.member_count].operator_type = OPType_assign_add; array_type.member_count++; array_type . operator_assign_sub = & array_operator_assign_sub; array_type.members[array_type.member_count].name = "combine_tokens(operator_, assign_sub)"; array_type.members[array_type.member_count].args = (args_t) { 2, &array_type, &num_type }; array_type.members[array_type.member_count].type = &none_type; array_type.members[array_type.member_count].offset = 
# 1029 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1029 "/home/kalen/src/A/src/A.c"
array_f
# 1029 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1029 "/home/kalen/src/A/src/A.c"
operator_assign_sub
# 1029 "/home/kalen/src/A/src/A.c" 3 4
)
# 1029 "/home/kalen/src/A/src/A.c"
; array_type.members[array_type.member_count].member_type = A_TYPE_OPERATOR; array_type.members[array_type.member_count].operator_type = OPType_assign_sub; array_type.member_count++; array_type . push_symbols = & array_push_symbols; array_type.members[array_type.member_count].name = "push_symbols"; array_type.members[array_type.member_count].args = (args_t) { 3, &array_type, &array_type, &cstr_type }; array_type.members[array_type.member_count].type = &none_type; array_type.members[array_type.member_count].offset = 
# 1029 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1029 "/home/kalen/src/A/src/A.c"
array_f
# 1029 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1029 "/home/kalen/src/A/src/A.c"
push_symbols
# 1029 "/home/kalen/src/A/src/A.c" 3 4
)
# 1029 "/home/kalen/src/A/src/A.c"
; array_type.members[array_type.member_count].member_type = A_TYPE_IMETHOD; array_type.member_count++; array_type . push_objects = & array_push_objects; array_type.members[array_type.member_count].name = "push_objects"; array_type.members[array_type.member_count].args = (args_t) { 3, &array_type, &array_type, &A_type }; array_type.members[array_type.member_count].type = &none_type; array_type.members[array_type.member_count].offset = 
# 1029 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1029 "/home/kalen/src/A/src/A.c"
array_f
# 1029 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1029 "/home/kalen/src/A/src/A.c"
push_objects
# 1029 "/home/kalen/src/A/src/A.c" 3 4
)
# 1029 "/home/kalen/src/A/src/A.c"
; array_type.members[array_type.member_count].member_type = A_TYPE_IMETHOD; array_type.member_count++; array_type.with_sz = & array_with_sz; array_type.members[array_type.member_count].name = "sz"; array_type.members[array_type.member_count].args = (args_t) { 2, &array_type, &sz_type }; array_type.members[array_type.member_count].type = &array_type; array_type.members[array_type.member_count].offset = 
# 1029 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1029 "/home/kalen/src/A/src/A.c"
array_f
# 1029 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1029 "/home/kalen/src/A/src/A.c"
with_sz
# 1029 "/home/kalen/src/A/src/A.c" 3 4
)
# 1029 "/home/kalen/src/A/src/A.c"
; array_type.members[array_type.member_count].member_type = A_TYPE_CONSTRUCT; array_type.member_count++; array_type . index_num = & array_index_num; array_type.members[array_type.member_count].name = "combine_tokens(index_, num)"; array_type.members[array_type.member_count].args = (args_t) { 2, &array_type, &num_type }; array_type.members[array_type.member_count].type = &A_type; array_type.members[array_type.member_count].offset = 
# 1029 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1029 "/home/kalen/src/A/src/A.c"
array_f
# 1029 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1029 "/home/kalen/src/A/src/A.c"
index_num
# 1029 "/home/kalen/src/A/src/A.c" 3 4
)
# 1029 "/home/kalen/src/A/src/A.c"
; array_type.members[array_type.member_count].member_type = A_TYPE_INDEX; array_type.member_count++; array_type . cast_bool = & array_cast_bool;; A_push_type(&array_type); return (bool)1; } }
A vector_header; vector_f vector_type; static __attribute__((constructor)) bool global_vector() { vector_f* type_ref = &vector_type; A_f* base_ref = &A_type; if ((AType)type_ref != (AType)base_ref && base_ref->size == 0) { A_lazy_init((global_init_fn)&global_vector); return (bool)0; } else { memcpy(type_ref, base_ref, sizeof(A_f)); static member_t members[16 + sizeof(vector_type) / sizeof(void*)]; vector_type.parent_type = &A_type; vector_type.name = "vector"; vector_type.size = sizeof(struct vector); vector_type.members = members; vector_type.traits = 0; vector_type.arb = primitive_ffi_arb(((struct A_f*)&cstr_type)); vector_type . pop = & vector_pop; vector_type.members[vector_type.member_count].name = "pop"; vector_type.members[vector_type.member_count].args = (args_t) { 2, &vector_type, &vector_type }; vector_type.members[vector_type.member_count].type = &A_type; vector_type.members[vector_type.member_count].offset = 
# 1030 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1030 "/home/kalen/src/A/src/A.c"
vector_f
# 1030 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1030 "/home/kalen/src/A/src/A.c"
pop
# 1030 "/home/kalen/src/A/src/A.c" 3 4
)
# 1030 "/home/kalen/src/A/src/A.c"
; vector_type.members[vector_type.member_count].member_type = A_TYPE_IMETHOD; vector_type.member_count++; vector_type . push = & vector_push; vector_type.members[vector_type.member_count].name = "push"; vector_type.members[vector_type.member_count].args = (args_t) { 3, &vector_type, &vector_type, &A_type }; vector_type.members[vector_type.member_count].type = &A_type; vector_type.members[vector_type.member_count].offset = 
# 1030 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1030 "/home/kalen/src/A/src/A.c"
vector_f
# 1030 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1030 "/home/kalen/src/A/src/A.c"
push
# 1030 "/home/kalen/src/A/src/A.c" 3 4
)
# 1030 "/home/kalen/src/A/src/A.c"
; vector_type.members[vector_type.member_count].member_type = A_TYPE_IMETHOD; vector_type.member_count++; vector_type . get = & vector_get; vector_type.members[vector_type.member_count].name = "get"; vector_type.members[vector_type.member_count].args = (args_t) { 3, &vector_type, &vector_type, &i32_type }; vector_type.members[vector_type.member_count].type = &A_type; vector_type.members[vector_type.member_count].offset = 
# 1030 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1030 "/home/kalen/src/A/src/A.c"
vector_f
# 1030 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1030 "/home/kalen/src/A/src/A.c"
get
# 1030 "/home/kalen/src/A/src/A.c" 3 4
)
# 1030 "/home/kalen/src/A/src/A.c"
; vector_type.members[vector_type.member_count].member_type = A_TYPE_IMETHOD; vector_type.member_count++; vector_type . count = & vector_count; vector_type.members[vector_type.member_count].name = "count"; vector_type.members[vector_type.member_count].args = (args_t) { 2, &vector_type, &vector_type }; vector_type.members[vector_type.member_count].type = &num_type; vector_type.members[vector_type.member_count].offset = 
# 1030 "/home/kalen/src/A/src/A.c" 3 4
__builtin_offsetof (
# 1030 "/home/kalen/src/A/src/A.c"
vector_f
# 1030 "/home/kalen/src/A/src/A.c" 3 4
, 
# 1030 "/home/kalen/src/A/src/A.c"
count
# 1030 "/home/kalen/src/A/src/A.c" 3 4
)
# 1030 "/home/kalen/src/A/src/A.c"
; vector_type.members[vector_type.member_count].member_type = A_TYPE_IMETHOD; vector_type.member_count++; vector_type . compare = & vector_compare; vector_type . hash = & vector_hash; vector_type . cast_bool = & vector_cast_bool;; A_push_type(&vector_type); return (bool)1; } }

A ATypes_header; ATypes_f ATypes_type; static __attribute__((constructor)) bool global_ATypes() { ATypes_f* type_ref = &ATypes_type; array_f* src_ref = &array_type; if (src_ref->size == 0) { A_lazy_init((global_init_fn)&global_ATypes); return (bool)0; } else { memcpy(&ATypes_type, &array_type, sizeof(array_f)); ATypes_type.name = "ATypes"; ATypes_type.src = &array_type; ATypes_type.meta = (args_t) { 1, &AType_type }; ATypes_type.traits = A_TRAIT_ALIAS; return (bool)1; } }
