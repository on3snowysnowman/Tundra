typedef signed char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef char TUNDRA_ASSERT58[(sizeof(int8) == 1) ? 1 : -1];
typedef char TUNDRA_ASSERT59[(sizeof(int16) == 2) ? 1 : -1];
typedef char TUNDRA_ASSERT60[(sizeof(int32) == 4) ? 1 : -1];
typedef char TUNDRA_ASSERT61[(sizeof(int64) == 8) ? 1 : -1];
typedef char TUNDRA_ASSERT62[(sizeof(void*) == 8) ? 1 : -1];
typedef struct Tundra_DynamicArrayint
{
    int *data;
    uint64 num_elem;
    uint64 cap;
    void (*copy_func)(const int*, int*, uint64);
    void (*free_func)(int*, uint64);
} Tundra_DynamicArrayint;
void InTundra_DynArrint_init(Tundra_DynamicArrayint *arr, uint64 init_cap);
void InTundra_DynArrint_def_copy(const int *src, int *dst,
    uint64 num_elem);
void InTundra_DynArrint_def_free(int *mem, uint64 );
void Tundra_DynArrint_init(Tundra_DynamicArrayint *arr);
void Tundra_DynArrint_init_w_cap(Tundra_DynamicArrayint *arr, uint64 init_cap);
void Tundra_DynArrint_init_w_elems(Tundra_DynamicArrayint *arr, const int *elements,
    uint64 num_elem, _Bool strict_alloc);
void Tundra_DynArrint_free(Tundra_DynamicArrayint *arr);
uint64 InTundra_calc_new_capacity_by_doubling(uint64 required_bytes,
    uint64 curr_capacity);
void* Tundra_alloc_mem(uint64 num_bytes);
void Tundra_free_mem(void *mem_ptr);
void Tundra_alloc_reserve_mem(void **mem_out, uint64 *capacity_out,
    uint64 num_bytes);
void* Tundra_alloc_copy_mem(const void *src, uint64 num_alloc_bytes,
    uint64 num_copy_bytes);
void Tundra_reserve_mem(void **mem_out, uint64* capacity_out,
    uint64 num_used_bytes, uint64 num_reserve_bytes);
void Tundra_copy_mem_fwd(const void *src, void *dst, uint64 num_bytes);
void Tundra_copy_mem_bwd(const void *src, void *dst, uint64 num_bytes);
void Tundra_copy_mem_safe(const void *src, void *dst, uint64 num_bytes);
_Bool Tundra_cmp_mem(const void *mem1, const void *mem2, uint64 num_bytes);
void Tundra_erase_shift_left(void *mem, uint64 start_index, uint64 num_bytes,
    uint64 shift_amt);
typedef long unsigned int size_t;
typedef int wchar_t;

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
extern long int strtol (const char *__restrict __nptr,
   char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
extern unsigned long int strtoul (const char *__restrict __nptr,
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
extern int rand (void) __attribute__ ((__nothrow__ , __leaf__));
extern void srand (unsigned int __seed) __attribute__ ((__nothrow__ , __leaf__));
extern void *malloc (size_t __size) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__))
     __attribute__ ((__alloc_size__ (1))) ;
extern void *calloc (size_t __nmemb, size_t __size)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) __attribute__ ((__alloc_size__ (1, 2))) ;
extern void *realloc (void *__ptr, size_t __size)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__warn_unused_result__)) __attribute__ ((__alloc_size__ (2)));
extern void free (void *__ptr) __attribute__ ((__nothrow__ , __leaf__));
extern void abort (void) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__)) __attribute__ ((__cold__));
extern int atexit (void (*__func) (void)) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
extern void exit (int __status) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));
extern void _Exit (int __status) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));
extern char *getenv (const char *__name) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;
extern int system (const char *__command) ;
typedef int (*__compar_fn_t) (const void *, const void *);
extern void *bsearch (const void *__key, const void *__base,
        size_t __nmemb, size_t __size, __compar_fn_t __compar)
     __attribute__ ((__nonnull__ (1, 2, 5))) ;
extern void qsort (void *__base, size_t __nmemb, size_t __size,
     __compar_fn_t __compar) __attribute__ ((__nonnull__ (1, 4)));
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


extern void *memcpy (void *__restrict __dest, const void *__restrict __src,
       size_t __n) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern void *memmove (void *__dest, const void *__src, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern void *memset (void *__s, int __c, size_t __n) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
extern int memcmp (const void *__s1, const void *__s2, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
extern int __memcmpeq (const void *__s1, const void *__s2, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
extern void *memchr (const void *__s, int __c, size_t __n)
      __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
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
extern char *strchr (const char *__s, int __c)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
extern char *strrchr (const char *__s, int __c)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
extern size_t strcspn (const char *__s, const char *__reject)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
extern size_t strspn (const char *__s, const char *__accept)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
extern char *strpbrk (const char *__s, const char *__accept)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
extern char *strstr (const char *__haystack, const char *__needle)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
extern char *strtok (char *__restrict __s, const char *__restrict __delim)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));
extern char *__strtok_r (char *__restrict __s,
    const char *__restrict __delim,
    char **__restrict __save_ptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 3)));
extern size_t strlen (const char *__s)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
extern char *strerror (int __errnum) __attribute__ ((__nothrow__ , __leaf__));

void InTundra_DynArrint_init(Tundra_DynamicArrayint *arr, uint64 init_cap)
{
    arr->data = (int*)Tundra_alloc_mem(init_cap * sizeof(int));
    arr->num_elem = 0;
    arr->cap = init_cap;
    arr->copy_func = InTundra_DynArrint_def_copy;
    arr->free_func = InTundra_DynArrint_def_free;
}
void InTundra_DynArrint_def_copy(const int *src, int *dst,
    uint64 num_elem)
{
    memcpy((void*)dst, (const void*)src, num_elem * sizeof(int));
}
void InTundra_DynArrint_def_free(int *mem, uint64 )
{
    Tundra_free_mem((void*)mem);
}
void Tundra_DynArrint_init(Tundra_DynamicArrayint *arr)
{
    InTundra_DynArrint_init(arr, 4);
}
void Tundra_DynArrint_init_w_cap(Tundra_DynamicArrayint *arr, uint64 init_cap)
{
    init_cap = (init_cap == 0) ? 4 : init_cap;
    InTundra_DynArrint_init(arr, init_cap);
}
void Tundra_DynArrint_init_w_elems(Tundra_DynamicArrayint *arr, const int *elements,
    uint64 num_elem, _Bool strict_alloc)
{
    arr->copy_func = InTundra_DynArrint_def_copy;
    arr->free_func = InTundra_DynArrint_def_free;
    const uint64 NUM_CPY_BYTE = num_elem * sizeof(int);
    if(strict_alloc)
    {
        arr->data = (int*)(Tundra_alloc_mem(NUM_CPY_BYTE));
        arr->copy_func(elements, arr->data, num_elem);
        arr->num_elem = num_elem;
        arr->cap = num_elem;
        return;
    }
    uint64 temp_cap_bytes;
    Tundra_alloc_reserve_mem(
        (void**)(&arr->data),
        &temp_cap_bytes,
        NUM_CPY_BYTE);
    arr->copy_func(elements, arr->data, num_elem);
    arr->num_elem = num_elem;
    arr->cap = temp_cap_bytes / sizeof(int);
}
void Tundra_DynArrint_free(Tundra_DynamicArrayint *arr)
{
    arr->free_func(arr->data, arr->num_elem);
    arr->data = ((void *)0);
    arr->copy_func = ((void *)0);
    arr->free_func = ((void *)0);
}
