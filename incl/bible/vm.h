#ifndef _BIBLE_VM_H_
#define _BIBLE_VM_H_

#include <bible/cdefs.h>
#include <bible/ktypes.h>
#include <bible/types.h>
#include <bible/queue.h>
#include <bible/regsister.h>

typedef BK_REGISTER_HANDLE BK_BOOLEAN;

/* Valores booleanos predefinidos (singletons do sistema) */
#define BK_BOOLEAN_TRUE		((BK_BOOLEAN)0x0001000000000001ULL)  /* Registro TRUE global */
#define BK_BOOLEAN_FALSE	((BK_BOOLEAN)0x0001000000000000ULL)  /* Registro FALSE global */
#define BK_BOOLEAN_UNKNOWN	((BK_BOOLEAN)0x0001000000000002ULL)  /* Registro UNKNOWN */
#define BK_BOOLEAN_UNDEFINED	BK_REGISTER_NULL                     /* Indefinido */

/* Atalhos para facilitar o uso */
#define BK_TRUE			BK_BOOLEAN_TRUE
#define BK_FALSE		BK_BOOLEAN_FALSE
#define BK_UNKNOWN		BK_BOOLEAN_UNKNOWN
#define BK_BOOL_REG			BK_BOOLEAN  /* Alias para compatibilidade */


/*
 * Definições básicas de memória virtual
 */
#ifndef BK_VM_PAGE_SIZE
#define BK_VM_PAGE_SIZE		4096	/* Tamanho de página padrão */
#endif

#ifndef BK_VM_PAGE_SHIFT
#define BK_VM_PAGE_SHIFT	12	/* 2^12 = 4096 */
#endif

/* Máscara de offset de página */
#define BK_VM_PAGE_MASK		(BK_VM_PAGE_SIZE - 1)

/* Conversões entre endereços virtuais e números de página */
#define BK_VM_PAGE_ALIGN(addr)	((addr) & ~BK_VM_PAGE_MASK)
#define BK_VM_PAGE_OFFSET(addr)	((addr) & BK_VM_PAGE_MASK)
#define BK_VM_PAGE_NUMBER(addr)	((addr) >> BK_VM_PAGE_SHIFT)
#define BK_VM_PAGE_ADDR(pg)	((pg) << BK_VM_PAGE_SHIFT)

/*
 * Regiões de memória virtual do kernel
 */
#define BK_VM_KERNEL_BASE	0xFFFFFFFF80000000ULL	/* Base do kernel */
#define BK_VM_KERNEL_END	0xFFFFFFFFFFFFFFFFULL	/* Fim do espaço do kernel */
#define BK_VM_USER_BASE		0x0000000000000000ULL	/* Base do espaço usuário */
#define BK_VM_USER_END		0x00007FFFFFFFFFFFULL	/* Fim do espaço usuário */
#define BK_VM_STACK_TOP		0x00007FFFFFFFFFFFULL	/* Topo da stack do usuário */

/*
 * Flags de proteção de página
 */
#define BK_VM_PROT_NONE		0x00	/* Nenhum acesso */
#define BK_VM_PROT_READ		0x01	/* Leitura permitida */
#define BK_VM_PROT_WRITE	0x02	/* Escrita permitida */
#define BK_VM_PROT_EXEC		0x04	/* Execução permitida */
#define BK_VM_PROT_USER		0x08	/* Acesso do usuário permitido */
#define BK_VM_PROT_KERNEL	0x10	/* Acesso apenas do kernel */
#define BK_VM_PROT_COW		0x20	/* Copy-On-Write */
#define BK_VM_PROT_SHARED	0x40	/* Memória compartilhada */
#define BK_VM_PROT_GUARD	0x80	/* Página guarda */

/*
 * Flags de mapeamento de memória
 */
#define BK_VM_MAP_FIXED		0x0001	/* Endereço fixo */
#define BK_VM_MAP_ANON		0x0002	/* Mapeamento anônimo */
#define BK_VM_MAP_PRIVATE	0x0004	/* Mapeamento privado */
#define BK_VM_MAP_SHARED	0x0008	/* Mapeamento compartilhado */
#define BK_VM_MAP_STACK		0x0010	/* Para stack */
#define BK_VM_MAP_EXECUTABLE	0x0020	/* Para código executável */
#define BK_VM_MAP_NO_RESERVE	0x0040	/* Não reservar swap */
#define BK_VM_MAP_LOCKED	0x0080	/* Páginas bloqueadas na memória */
#define BK_VM_MAP_GROWSDOWN	0x0100	/* Cresce para baixo (stack) */
#define BK_VM_MAP_DENYWRITE	0x0200	/* Recusar write no arquivo subjacente */

/*
 * Flags para operações de memória
 */
#define BK_VM_OP_READ		0x01	/* Operação de leitura */
#define BK_VM_OP_WRITE		0x02	/* Operação de escrita */
#define BK_VM_OP_EXECUTE	0x04	/* Operação de execução */
#define BK_VM_OP_USER		0x08	/* Operação do usuário */
#define BK_VM_OP_KERNEL		0x10	/* Operação do kernel */

/*
 * Erros de memória virtual
 */
#define BK_VM_ERR_NONE		0	/* Sem erro */
#define BK_VM_ERR_INVALID	1	/* Endereço inválido */
#define BK_VM_ERR_PROTECTION	2	/* Violação de proteção */
#define BK_VM_ERR_NOT_PRESENT	3	/* Página não presente */
#define BK_VM_ERR_BUSY		4	/* Recurso ocupado */
#define BK_VM_ERR_NO_MEMORY	5	/* Sem memória disponível */
#define BK_VM_ERR_NO_SPACE	6	/* Sem espaço de endereço */
#define BK_VM_ERR_ACCESS	7	/* Violação de acesso */

/*
 * Estruturas de dados para gerenciamento de memória virtual
 */

struct __bk_vm_page {
    BK_UINT64       p_pfn;          /* Número da página física */
    BK_REFCOUNT     p_refcnt;       /* Contador de referências */
    BK_UINT32       p_flags;        /* Flags da página */
    BK_LIST_ENTRY(__bk_vm_page) p_list;   /* Links para listas de páginas */
};

typedef struct __bk_vm_page BK_VM_PAGE;

/* Entrada de endereço virtual (Virtual Address Descriptor) */
struct __bk_vm_vad {
	BK_VM_OFFSET	v_start;	/* Endereço inicial */
	BK_VM_OFFSET	v_end;		/* Endereço final */
	BK_VM_SIZE	v_size;		/* Tamanho da região */
	BK_UINT32	v_prot;		/* Proteção */
	BK_UINT32	v_flags;	/* Flags */
	BK_VM_PAGE	*v_pages;	/* Array de páginas físicas */
	BK_UI32		v_npages;	/* Número de páginas */
	BK_LIST_ENTRY(__bk_vm_vad) v_link;	/* Link na lista de VADs */
};

typedef struct __bk_vm_vad BK_VM_VAD;

/* Lista de VADs */
BK_LIST_HEAD(__bk_vm_vad_list, __bk_vm_vad);
typedef struct __bk_vm_vad_list BK_VM_VAD_LIST;

/* Espaço de endereço de um processo */
struct __bk_vm_addrspace {
	BK_VM_VAD_LIST	as_vads;	/* Lista de regiões VAD */
	BK_VM_SIZE	as_size;	/* Tamanho total */
	BK_VM_OFFSET	as_brk;		/* Break atual */
	BK_VM_OFFSET	as_stack_base;	/* Base da stack */
	BK_VM_OFFSET	as_stack_top;	/* Topo da stack */
	BK_VM_OFFSET	as_heap_base;	/* Base do heap */
	BK_VM_OFFSET	as_heap_top;	/* Topo do heap */
	BK_SPINLOCK	as_lock;	/* Lock para concorrência */
	BK_REFCOUNT	as_refcnt;	/* Contador de referências */
};

typedef struct __bk_vm_addrspace BK_VM_ADDRSPACE;

/* Estatísticas de memória virtual */
struct __bk_vm_stats {
	BK_VM_SIZE	vs_total;	/* Total de memória virtual */
	BK_VM_SIZE	vs_used;	/* Memória virtual usada */
	BK_VM_SIZE	vs_free;	/* Memória virtual livre */
	BK_VM_SIZE	vs_kernel;	/* Memória virtual do kernel */
	BK_VM_SIZE	vs_user;	/* Memória virtual do usuário */
	BK_UI64		vs_faults;	/* Número de page faults */
	BK_UI64		vs_cow_faults;	/* Page faults de COW */
	BK_UI64		vs_swapins;	/* Páginas trazidas do swap */
	BK_UI64		vs_swapouts;	/* Páginas enviadas para swap */
};

typedef struct __bk_vm_stats BK_VM_STATS;

/*
 * Funções para gerenciamento de memória virtual
 */

/* Inicialização do sistema de VM */
BK_I32 bk_vm_init(void);

/* Criação/destruição de espaços de endereço */
BK_VM_ADDRSPACE *bk_vm_addrspace_create(void);
void bk_vm_addrspace_destroy(BK_VM_ADDRSPACE *as);
BK_VM_ADDRSPACE *bk_vm_addrspace_clone(BK_VM_ADDRSPACE *src);

/* Mapeamento de memória */
BK_VM_OFFSET bk_vm_map(BK_VM_ADDRSPACE *as, BK_VM_OFFSET addr,
		      BK_VM_SIZE size, BK_UINT32 prot, BK_UINT32 flags);
BK_I32 bk_vm_unmap(BK_VM_ADDRSPACE *as, BK_VM_OFFSET addr, BK_VM_SIZE size);
BK_I32 bk_vm_protect(BK_VM_ADDRSPACE *as, BK_VM_OFFSET addr,
		    BK_VM_SIZE size, BK_UINT32 prot);

/* Alocação/liberação de memória */
BK_VM_OFFSET bk_vm_alloc(BK_VM_ADDRSPACE *as, BK_VM_SIZE size,
			BK_UINT32 prot, BK_UINT32 flags);
BK_I32 bk_vm_free(BK_VM_ADDRSPACE *as, BK_VM_OFFSET addr, BK_VM_SIZE size);

/* Heap management */
BK_VM_OFFSET bk_vm_sbrk(BK_VM_ADDRSPACE *as, BK_I64 increment);

/* Consulta de informações */
BK_I32 bk_vm_query(BK_VM_ADDRSPACE *as, BK_VM_OFFSET addr,
		  BK_VM_SIZE *size, BK_UINT32 *prot, BK_UINT32 *flags);
BK_VM_VAD *bk_vm_find_vad(BK_VM_ADDRSPACE *as, BK_VM_OFFSET addr);

/* Copy-On-Write */
BK_I32 bk_vm_cow_fault(BK_VM_ADDRSPACE *as, BK_VM_OFFSET addr,
		      BK_UINT32 fault_type);

/* Manipulação de páginas */
BK_I32 bk_vm_pagein(BK_VM_ADDRSPACE *as, BK_VM_OFFSET addr);
BK_I32 bk_vm_pageout(BK_VM_ADDRSPACE *as, BK_VM_OFFSET addr);

/* Statistics */
void bk_vm_get_stats(BK_VM_STATS *stats);

/* Validação de endereços */
static __BK_ALWAYS_INLINE BK_BOOLEAN
bk_vm_is_user_address(BK_VM_OFFSET addr)
{
	return (addr >= BK_VM_USER_BASE && addr <= BK_VM_USER_END);
}

static __BK_ALWAYS_INLINE BK_BOOLEAN
bk_vm_is_kernel_address(BK_VM_OFFSET addr)
{
	return (addr >= BK_VM_KERNEL_BASE && addr <= BK_VM_KERNEL_END);
}

static __BK_ALWAYS_INLINE BK_BOOLEAN
bk_vm_is_valid_address(BK_VM_OFFSET addr)
{
	return bk_vm_is_user_address(addr) || bk_vm_is_kernel_address(addr);
}

/* Funções de utilidade */
static __BK_ALWAYS_INLINE BK_VM_SIZE
bk_vm_round_up(BK_VM_SIZE size)
{
	return (size + BK_VM_PAGE_MASK) & ~BK_VM_PAGE_MASK;
}

static __BK_ALWAYS_INLINE BK_VM_SIZE
bk_vm_round_down(BK_VM_SIZE size)
{
	return size & ~BK_VM_PAGE_MASK;
}

static __BK_ALWAYS_INLINE BK_UI32
bk_vm_size_to_pages(BK_VM_SIZE size)
{
	return (size + BK_VM_PAGE_MASK) >> BK_VM_PAGE_SHIFT;
}

static __BK_ALWAYS_INLINE BK_VM_SIZE
bk_vm_pages_to_size(BK_UI32 pages)
{
	return (BK_VM_SIZE)pages << BK_VM_PAGE_SHIFT;
}

/*
 * Macros para manipulação de páginas virtuais
 */
#define BK_VM_PAGE_ROUND_UP(addr)	BK_VM_PAGE_ALIGN((addr) + BK_VM_PAGE_MASK)
#define BK_VM_PAGE_ROUND_DOWN(addr)	BK_VM_PAGE_ALIGN(addr)
#define BK_VM_IS_PAGE_ALIGNED(addr)	(((addr) & BK_VM_PAGE_MASK) == 0)

/*
 * Funções para debug e diagnóstico
 */
#ifdef _BK_VM_DEBUG
void bk_vm_dump_addrspace(BK_VM_ADDRSPACE *as);
void bk_vm_dump_vad(BK_VM_VAD *vad);
void bk_vm_dump_stats(void);
#endif

/*
 * Handler de page fault
 */
BK_I32 bk_vm_page_fault_handler(BK_VM_OFFSET addr, BK_UINT32 fault_type,
			       BK_UINT32 access_flags);

/*
 * Compatibilidade com APIs POSIX
 */
#ifndef _BK_NO_COMPAT

#define	PROT_NONE	BK_VM_PROT_NONE
#define	PROT_READ	BK_VM_PROT_READ
#define	PROT_WRITE	BK_VM_PROT_WRITE
#define	PROT_EXEC	BK_VM_PROT_EXEC

#define	MAP_FIXED	BK_VM_MAP_FIXED
#define	MAP_ANON	BK_VM_MAP_ANON
#define	MAP_PRIVATE	BK_VM_MAP_PRIVATE
#define	MAP_SHARED	BK_VM_MAP_SHARED

#endif /* !_BK_NO_COMPAT */


#endif /* !_BIBLE_VM_H_ */
