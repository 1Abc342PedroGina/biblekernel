#ifndef _BIBLE_TYPES_H
#define _BIBLE_TYPES_H

#include <bible/cdefs.h>

typedef signed char __BK_C8;
typedef unsigned char __BK_UI8;
typedef short  __BK_S16;
typedef unsigned short __BK_UI16;
typedef int            __BK_I32;
typedef unsigned int   __BK_UI32;
typedef long           __BK_L3264;
typedef unsigned long  __BK_UL3264;
typedef long long      __BK_LL64;
typedef unsigned long long  __BK_ULL64;

typedef __BK_C8  _bible_sig_8;
typedef __BK_S16 _bible_sig_16;
typedef __BK_I32 _bible_sig_32;
typedef __BK_L3264 _bible_sig_64;
typedef __BK_LL64 _bible_sig_long_max_64;
typedef __BK_UI8 _bible_usig_8;
typedef __BK_UI16 _bible_unsig_16;
typedef __BK_UI32 _bible_unsig_32;
typedef __BK_UL3264 _bible_unsig_64;
typedef __BK_ULL64 _bible_unsig_long_max_64;

typedef __BK_UI8 BK_UI8;
typedef __BK_UI16 BK_UI16;
typedef __BK_UI32 BK_UI32;
typedef __BK_UL3264 BK_UL3264;
typedef __BK_ULL64 BK_ULL64;

typedef __BK_ULL64 BK_UI64;

typedef __BK_C8 BK_C8;
typedef __BK_S16 BK_S16;
typedef __BK_I32 BK_I32;
typedef __BK_L3264 BK_L3264;
typedef __BK_LL64 BK_LL64;

typedef __BK_UL3264 __uintptr_kt;
typedef __BK_L3264 __intptr_kt;

#ifdef __CHERI__
typedef __intcapt_t __intcapt_kt;
typedef __intcapt_kt __intptr_kt;
typedef __intcapt_kt __intfptr_kt;
typedef __intcapt_kt __int64ptr_kt;
typedef __uintcap_t  __uintcapt_kt;
typedef __uintcapt_kt __uintptr_kt;
typedef __uintcapt_kt __uintfptr_kt;
typedef __uintcapt_kt __uint64ptr_kt;
#elif __SIZE_OF_POINTER__ == 8
typedef __BK_L3264 __intptr_kt;
typedef __BK_L3264 __intfptr_kt;
typedef __BK_L3264 __int64ptr_kt;
typedef __BK_UL3264 __uintptr_kt;
typedef __BK_UL3264 __uintfptr_kt;
typedef __BK_UL3264 __uint64ptr_kt;
#elif __SIZE_OF_POINTER__ ==4
typedef __BK_I32 __intptr_kt;
typedef __BK_I32 __intfptr_kt;
typedef __BK_L3264 __int64ptr_kt;
typedef __BK_UI32 __uintptr_kt;
typedef __BK_UI32 __uintfptr_kt;
typedef __BK_UL3264 __uint64ptr_kt;
#endif

typedef __BK_UL3264 __bible_size_t;

#define __BK_ALWAYS_INLINE

#if __SIZE_OF_SIZE_T__ ==8
typedef __BK_UL3264 __bible_size_t;
typedef __BK_L3264 __bible__size_t;
#elif __SIZE_OF_SIZE_T__ ==4
typedef __BK_UI32 __bible_size_t;
typedef __BK_I32 __bible__size_t;
#endif

#if __SIZE_OF_PTRDIFF_T__ ==8
typedef __BK_L3264 __bible_ptrdiff_kt;
#elif __SIZE_OF__PTRDIFF_T__ ==4
typedef __BK_I32 __bible_ptrdiff_kt;
#endif

typedef __builtin_va_list  ___kernl_va_list;

#ifdef __PTRADDR_KERNTYPE__
#define __PTRADDR_KERNTYPE__
typedef	__PTRADDR_KERNTYPE__	__ptraddr_kt;
#else
typedef	__BK_UI32 __ptraddr_kt;
#endif

typedef	__ptraddr_kt __vmkern_offset_kt;
typedef	__BK_UI32 __vmkern_size_kt;

typedef __BK_I32 __BLK_SIZE_KT;
typedef __BK_I32 __BK_CLOCK_ID;
typedef __BK_I32 __LWPID_KT;
typedef __BK_I32 __ACCMD_KT;
typedef __BK_I32 __NL_ITEM;
typedef __BK_I32 __KPID_KT;
typedef __BK_I32 __CPU_WHICH_KT;
typedef __BK_I32 __CPU_LEVEL_CPUT;
typedef __BK_I32 __CPU_SETID_CPUT;
typedef __BK_L3264 __BLK_CNT_KT;
typedef __BK_L3264 __ID__KT;
typedef __BK_L3264 __OFF_KT;
typedef __BK_L3264 __OFF64__KT;
typedef __BK_L3264 __OFFSBINTM_KT;
typedef __BK_L3264 __RLIM_KT;
typedef __BK_L3264 __DADDR_KT;
typedef __BK_L3264 __SUSECONDS;
typedef __BK_UI32 __FFLAGS_KT;
typedef __BK_UI32 __GID_KT;
typedef __BK_UI32 __UID_KT;
typedef __BK_UI32 __SOCKLEN_KT;
typedef __BK_UI32 __USECONDS_KT;
typedef __BK_UI8 __SA_FAML_KT;
typedef __BK_UL3264  __FSBLKCNT;
typedef __BK_UL3264  __FSFILCNT;
typedef __BK_UL3264  __INO;
typedef __BK_UL3264  __NLINK;
typedef __BK_UI16  __MODE_T;

#ifndef __SIZEOF_INTCAP

typedef	__intptr_kt __intcap_kt;
typedef	__uintptr_kt __uintcap_kt;
#endif

typedef int _INT_;
typedef _INT_ ct_rune_kt;
typedef _INT_ rune_kt;
typedef _INT_ _wint_kt;

#if !defined(__clang__) || !defined(__cplusplus) || !defined(__GNUC__)
typedef _bible_unsig_16 __bchar_u16;
typedef _bible_unsig_32 __bchar_u32;
#endif

#if defined(__cplusplus) && __cplusplus >= 201103L
#define _BCHAR_U16_DECLARED
#define _BCHAR_U32_DECLARED
#endif

#if defined(__cplusplus) && __cplusplus >= 202002L
#define _BCHAR_U8_DECLARED
#endif

typedef struct max_align {
  long long __max_align1
   __attribute__((__aligned__(__alignof__(long long))));
#ifndef _STANDALONE
  long double __max_align2
   __attribute__((__aligned__(__alignof__(long double))));
#endif
  void *__max_align3 __attribute__((__aligned__(__alignof__(void *))));
} __max_align_t;

typedef __BK_UI32 __ACL_TAG_KT;
typedef __BK_UI32 __ACL_PERM_KT;
typedef __BK_UI32 __ACL_TYPE;
typedef __BK_UI32 *__ACL_PERMSET;
typedef __BK_UI16 *__ACL_FLAGSET;
typedef __BK_UI16 __ACL_ETYPE;
typedef __BK_UI32 __ACL_FLAG;


typedef __BK_UL3264 __DEVICE_KT;
typedef __BK_UL3264 __FIX_PIT_KT;

typedef union __mbstate_t {
char		__mbstate8[128];
__BK_L3264 __mbstateL;
__intptr_kt __mbstateP;
} __mbstate_t;

typedef _bible_unsig_long_max_64 __rman_res_kt;

#if !defined(__GNUC_VA_LIST) && !defined(__NO_GNUC_VA_LIST)
#define __GNUC_VA_LIST
typedef ___kernl_va_list __gnuc_va_list;	/* compatibility w/GNU headers*/
#endif

#endif