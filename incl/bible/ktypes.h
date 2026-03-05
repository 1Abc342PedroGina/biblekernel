#ifndef _BIBLE_KTYPES_H_
#define _BIBLE_KTYPES_H_

#include <bible/cdefs.h>
#include <bible/endian.h>
#include <bible/types.h>
#include <bible/pthread.h>


#if __BK_KERNL_VISIBLE
typedef	BK_UI8		BK_UCHAR;
typedef	BK_UI16		BK_USHORT;
typedef	BK_UI32		BK_UINT;
typedef	BK_UL3264	BK_ULONG;

#ifndef _BK_KERNEL
typedef	BK_USHORT	BK_USHORT_SYSV;		/* Compatibilidade Sys V */
typedef	BK_UINT		BK_UINT_SYSV;		/* Compatibilidade Sys V */
#endif
#endif

/* Tipos inteiros com tamanho específico */
typedef BK_UI8		__BK_UINT8;
typedef BK_UI8		BK_UINT8;
typedef BK_UI16		BK_UINT16;
typedef BK_UI32		BK_UINT32;
typedef BK_UI32		BK_UINT32;
typedef BK_UL3264	BK_UINT64;

typedef BK_UL3264	BK_UQUAD;		/* quads (deprecated) */
typedef BK_L3264	BK_QUAD;
typedef BK_QUAD *	BK_QADDR;

typedef char *		BK_CADDR;		/* endereço de core */
typedef const char *	BK_CCADDR;		/* endereço de core, ponteiro para const */

#ifndef _BK_BLKSIZE_T_DECLARED
typedef	__BLK_SIZE_KT	BK_BLKSIZE;
#define	_BK_BLKSIZE_T_DECLARED
#endif

typedef	__CPU_WHICH_KT	BK_CPUWHICH;
typedef	__CPU_LEVEL_CPUT	BK_CPULEVEL;
typedef	__CPU_SETID_CPUT	BK_CPUSETID;

#ifndef _BK_BLKCNT_T_DECLARED
typedef	__BLK_CNT_KT	BK_BLKCNT;
#define	_BK_BLKCNT_T_DECLARED
#endif

#ifndef _BK_CLOCK_T_DECLARED
typedef	__BK_I32	BK_CLOCK;
#define	_BK_CLOCK_T_DECLARED
#endif

#ifndef _BK_CLOCKID_T_DECLARED
typedef	__BK_CLOCK_ID	BK_CLOCKID;
#define	_BK_CLOCKID_T_DECLARED
#endif

typedef	__BK_I32	BK_CRITICAL;		/* Valor de seção crítica */
typedef	__DADDR_KT	BK_DADDR;		/* endereço de disco */

#ifndef _BK_DEV_T_DECLARED
typedef	__DEVICE_KT	BK_DEV;			/* número de dispositivo ou struct cdev */
#define	_BK_DEV_T_DECLARED
#endif

#ifndef _BK_FFLAGS_T_DECLARED
typedef	__FFLAGS_KT	BK_FFLAGS;		/* flags de arquivo */
#define	_BK_FFLAGS_T_DECLARED
#endif

typedef	__FIX_PIT_KT	BK_FIXPT;		/* número de ponto fixo */

#ifndef _BK_FSBLKCNT_T_DECLARED		/* para statvfs() */
typedef	__FSBLKCNT	BK_FSBLKCNT;
typedef	__FSFILCNT	BK_FSFILCNT;
#define	_BK_FSBLKCNT_T_DECLARED
#endif

#ifndef _BK_GID_T_DECLARED
typedef	__GID_KT	BK_GID;			/* ID de grupo */
#define	_BK_GID_T_DECLARED
#endif

#ifndef _BK_IN_ADDR_T_DECLARED
typedef	BK_UINT32	BK_IN_ADDR;		/* tipo base para endereço internet */
#define	_BK_IN_ADDR_T_DECLARED
#endif

#ifndef _BK_IN_PORT_T_DECLARED
typedef	BK_UINT16	BK_IN_PORT;
#define	_BK_IN_PORT_T_DECLARED
#endif

#ifndef _BK_ID_T_DECLARED
typedef	__ID__KT	BK_ID;			/* pode conter um BK_UID ou BK_PID */
#define	_BK_ID_T_DECLARED
#endif

#ifndef _BK_INO_T_DECLARED
typedef	__INO		BK_INO;			/* número de inode */
#define	_BK_INO_T_DECLARED
#endif

#ifndef _BK_KEY_T_DECLARED
typedef	__BK_I32	BK_KEY;			/* chave IPC (para Sys V IPC) */
#define	_BK_KEY_T_DECLARED
#endif

#ifndef _BK_LWPID_T_DECLARED
typedef	__LWPID_KT	BK_LWPID;		/* Thread ID (também conhecido como LWP) */
#define	_BK_LWPID_T_DECLARED
#endif

#ifndef _BK_MODE_T_DECLARED
typedef	__MODE_T	BK_MODE;		/* permissões */
#define	_BK_MODE_T_DECLARED
#endif

#ifndef _BK_ACCMODE_T_DECLARED
typedef	__ACCMD_KT	BK_ACCMODE;		/* permissões de acesso */
#define	_BK_ACCMODE_T_DECLARED
#endif

#ifndef _BK_NLINK_T_DECLARED
typedef	__NLINK		BK_NLINK;		/* contador de links */
#define	_BK_NLINK_T_DECLARED
#endif

#ifndef _BK_OFF_T_DECLARED
typedef	__OFF_KT	BK_OFF;			/* offset de arquivo */
#define	_BK_OFF_T_DECLARED
#endif

#ifndef _BK_OFF64_T_DECLARED
typedef	__OFF64__KT	BK_OFF64;		/* offset de arquivo (alias) */
#define	_BK_OFF64_T_DECLARED
#endif

#ifndef _BK_PID_T_DECLARED
typedef	__KPID_KT	BK_PID;			/* ID de processo */
#define	_BK_PID_T_DECLARED
#endif

typedef	__BK_I32	BK_REGISTER_INT;

#ifndef _BK_RLIM_T_DECLARED
typedef	__RLIM_KT	BK_RLIM;		/* limite de recurso */
#define	_BK_RLIM_T_DECLARED
#endif

typedef	__BK_L3264	BK_SBINTIME;

typedef	__BK_I32	BK_SEGSZ;		/* tamanho de segmento (em páginas) */

#ifndef _BK_SIZE_T_DECLARED
typedef	__bible_size_t	BK_SIZE;
#define	_BK_SIZE_T_DECLARED
#endif

#ifndef _BK_SSIZE_T_DECLARED
typedef	__BK_I32	BK_SSIZE;
#define	_BK_SSIZE_T_DECLARED
#endif

#ifndef _BK_SUSECONDS_T_DECLARED
typedef	__SUSECONDS	BK_SUSECONDS;		/* microssegundos (com sinal) */
#define	_BK_SUSECONDS_T_DECLARED
#endif

#ifndef _BK_TIME_T_DECLARED
typedef	__BK_L3264	BK_TIME;
#define	_BK_TIME_T_DECLARED
#endif

#ifndef _BK_TIMER_T_DECLARED
typedef	__BK_I32	BK_TIMER;
#define	_BK_TIMER_T_DECLARED
#endif

#ifndef _BK_MQD_T_DECLARED
typedef	__BK_I32	BK_MQD;
#define	_BK_MQD_T_DECLARED
#endif

typedef	__BK_UI32	BK_U_REGISTER;

#ifndef _BK_UID_T_DECLARED
typedef	__UID_KT	BK_UID;			/* ID de usuário */
#define	_BK_UID_T_DECLARED
#endif

#ifndef _BK_USECONDS_T_DECLARED
typedef	__USECONDS_KT	BK_USECONDS;		/* microssegundos (sem sinal) */
#define	_BK_USECONDS_T_DECLARED
#endif

#ifndef _BK_CAP_IOCTL_T_DECLARED
#define	_BK_CAP_IOCTL_T_DECLARED
typedef	BK_UL3264	BK_CAP_IOCTL;
#endif

#ifndef _BK_CAP_RIGHTS_T_DECLARED
#define	_BK_CAP_RIGHTS_T_DECLARED
struct __bk_cap_rights;

typedef	struct __bk_cap_rights	BK_CAP_RIGHTS;
#endif

/*
 * Tipos adequados para exportar endereços físicos, endereços virtuais
 * (ponteiros) e tamanhos de objetos de memória do kernel independentemente
 * do tamanho da palavra nativa.
 */
typedef	BK_UINT64	BK_KPADDR;
typedef	BK_UINT64	BK_KVADDR;
typedef	BK_UINT64	BK_KSIZE;
typedef	BK_L3264	BK_KSSIZE;

typedef	__vmkern_offset_kt	BK_VM_OFFSET;
typedef	BK_UINT64		BK_VM_OOFFSET;
typedef	__BK_UL3264		BK_VM_PADDR;
typedef	BK_UINT64		BK_VM_PINDEX;
typedef	__vmkern_size_kt	BK_VM_SIZE;

typedef __rman_res_kt	BK_RMAN_RES;

typedef BK_REGISTER_INT	BK_SYSCALLARG;

#ifdef _BK_KERNEL
typedef	BK_UINT		BK_BOOLEAN;
typedef	struct __bk_device	*BK_DEVICE;
typedef	__intfptr_kt	BK_INTFPTR;

/* Máscara de interrupção */
typedef	BK_UINT32	BK_INTRMASK;

typedef	__uintfptr_kt	BK_UINTFPTR;
typedef	BK_UINT64	BK_UOFF;
typedef	char		BK_VM_MEMATTR;	/* códigos de atributo de memória */
typedef	struct __bk_vm_page	*BK_VM_PAGE;

#define BK_OFFSETOF(type, field) __BK_OFFSETOF(type, field)
#endif /* _BK_KERNEL */

#if	defined(_BK_KERNEL) || defined(_BK_STANDALONE)
#if !defined(__bool_true_false_are_defined) && !defined(__cplusplus)
#define	__bool_true_false_are_defined	1
#if __STDC_VERSION__ < 202311L
#define	BK_FALSE	0
#define	BK_TRUE		1
typedef	_Bool		BK_BOOL;
#endif /* __STDC_VERSION__ < 202311L */
#endif /* !__bool_true_false_are_defined && !__cplusplus */
#endif /* _BK_KERNEL || _BK_STANDALONE */

/*
 * As seguintes são coisas que realmente não deveriam existir neste header,
 * já que seu propósito é fornecer typedefs, não coisas diversas.
 */
#include <bible/bitcount.h>

#if __BK_KERNL_VISIBLE

#ifndef _BK_STANDALONE
#include <bible/select.h>
#endif

typedef __BK_UI32 BK_UINT32;

/*
 * Os números major e minor são codificados em BK_DEV como MMMmmmMm (onde
 * letras correspondem a bytes). A codificação dos 4 bytes inferiores é
 * restringida pela compatibilidade com BK_DEV de 16 e 32 bits.
 */
#define	BK_MAJOR(d)	__BK_MAJOR(d)
static __BK_ALWAYS_INLINE BK_I32
__BK_MAJOR(BK_DEV _d)
{
	return (((_d >> 32) & 0xffffff00) | ((_d >> 8) & 0xff));
}

#define	BK_MINOR(d)	__BK_MINOR(d)
static __BK_ALWAYS_INLINE BK_I32
__BK_MINOR(BK_DEV _d)
{
	return (((_d >> 24) & 0xff00) | (_d & 0xffff00ff));
}

#define	BK_MAKEDEV(M, m)	__BK_MAKEDEV((M), (m))
static __BK_ALWAYS_INLINE BK_DEV
__BK_MAKEDEV(BK_I32 _Major, BK_I32 _Minor)
{
	return (((BK_DEV)(_Major & 0xffffff00) << 32) | ((_Major & 0xff) << 8) |
	    ((BK_DEV)(_Minor & 0xff00) << 24) | (_Minor & 0xffff00ff));
}

#if (defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 13))
#define __BK_ENUM_UINT8_DECL(name)	enum enum_ ## name ## _uint8 : BK_UI8
#define __BK_ENUM_UINT8(name)		enum enum_ ## name ## _uint8
#else
#define __BK_ENUM_UINT8_DECL(name)	enum __attribute__((packed)) enum_ ## name ## _uint8
#define __BK_ENUM_UINT8(name)		enum __attribute__((packed)) enum_ ## name ## _uint8
#endif

/*
 * Estas declarações pertencem a outro lugar, mas são repetidas aqui e em
 * <stdio.h> para dar a programas quebrados uma chance melhor de funcionar
 * com BK_OFF de 64 bits.
 */
#ifndef _BK_KERNEL
__BK_BEGIN_DECLS
#ifndef _BK_FTRUNCATE_DECLARED
#define	_BK_FTRUNCATE_DECLARED
BK_I32	 bk_ftruncate(BK_I32, BK_OFF);
#endif

#ifndef _BK_LSEEK_DECLARED
#define	_BK_LSEEK_DECLARED
BK_OFF	 bk_lseek(BK_I32, BK_OFF, BK_I32);
#endif

#ifndef _BK_MMAP_DECLARED
#define	_BK_MMAP_DECLARED
void *	 bk_mmap(void *, BK_SIZE, BK_I32, BK_I32, BK_I32, BK_OFF);
#endif

#ifndef _BK_TRUNCATE_DECLARED
#define	_BK_TRUNCATE_DECLARED
BK_I32	 bk_truncate(const char *, BK_OFF);
#endif
__BK_END_DECLS
#endif /* !_BK_KERNEL */

#endif /* __BK_KERNL_VISIBLE */

/* Compatibilidade com tipos POSIX padrão */
#ifndef _BK_NO_POSIX_COMPAT

typedef BK_DEV		dev_t;
typedef BK_GID		gid_t;
typedef BK_UID		uid_t;
typedef BK_PID		pid_t;
typedef BK_MODE		mode_t;
typedef BK_SIZE		size_t;
typedef BK_SSIZE	ssize_t;
typedef BK_OFF		off_t;
typedef BK_TIME		time_t;
typedef BK_CLOCK	clock_t;
typedef BK_CLOCKID	clockid_t;
typedef BK_INO		ino_t;
typedef BK_NLINK	nlink_t;
typedef BK_KEY		key_t;
typedef BK_SUSECONDS	suseconds_t;
typedef BK_USECONDS	useconds_t;
typedef BK_RLIM		rlim_t;
typedef BK_BLKSIZE	blksize_t;
typedef BK_BLKCNT	blkcnt_t;
typedef BK_FSBLKCNT	fsblkcnt_t;
typedef BK_FSFILCNT	fsfilcnt_t;
typedef BK_LWPID	lwpid_t;
typedef BK_IN_ADDR	in_addr_t;
typedef BK_IN_PORT	in_port_t;
typedef BK_ID		id_t;

#ifdef X86 
typedef BK_L3264 BK_I64;
#endif

typedef __BK_LL64 BK_I64;

#endif /* !_BK_NO_POSIX_COMPAT */

#endif /* !_BIBLE_KTYPES_H_ */