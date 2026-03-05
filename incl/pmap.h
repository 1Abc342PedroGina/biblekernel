#ifndef _BIBLE_PMAP_H_
#define _BIBLE_PMAP_H_

#include <bible/cdefs.h>
#include <bible/types.h>
#include <bible/queue.h>
#include <bible/vm.h>


/*
 * Definições de memória física
 */
#ifndef BK_PM_PAGE_SIZE
#define BK_PM_PAGE_SIZE		BK_VM_PAGE_SIZE	/* Tamanho de página física */
#endif

#ifndef BK_PM_PAGE_SHIFT
#define BK_PM_PAGE_SHIFT	BK_VM_PAGE_SHIFT
#endif

#define BK_PM_PAGE_MASK		(BK_PM_PAGE_SIZE - 1)

/* Endereços físicos */
typedef BK_VM_PADDR BK_PM_PADDR;

/* Frame de página física */
typedef BK_UI32 BK_PM_FRAME;

/* Conversões */
#define BK_PM_FRAME_TO_ADDR(frame)	((BK_PM_PADDR)(frame) << BK_PM_PAGE_SHIFT)
#define BK_PM_ADDR_TO_FRAME(addr)	((BK_PM_FRAME)((addr) >> BK_PM_PAGE_SHIFT))
#define BK_PM_IS_PAGE_ALIGNED(addr)	(((addr) & BK_PM_PAGE_MASK) == 0)

/*
 * Estados de uma página física
 */
#define BK_PM_PAGE_FREE		0x0000	/* Página livre */
#define BK_PM_PAGE_ALLOCATED	0x0001	/* Página alocada */
#define BK_PM_PAGE_RESERVED	0x0002	/* Página reservada */
#define BK_PM_PAGE_WIRED	0x0004	/* Página wired (não pode ser pageout) */
#define BK_PM_PAGE_MODIFIED	0x0008	/* Página modificada (dirty) */
#define BK_PM_PAGE_REFERENCED	0x0010	/* Página referenciada */
#define BK_PM_PAGE_COW		0x0020	/* Copy-On-Write */
#define BK_PM_PAGE_CACHE	0x0040	/* Cache de página */
#define BK_PM_PAGE_ZERO		0x0080	/* Página zerada */
#define BK_PM_PAGE_KERNEL	0x0100	/* Página do kernel */
#define BK_PM_PAGE_USER		0x0200	• Página do usuário */
#define BK_PM_PAGE_SWAPPED	0x0400	• Página no swap */
#define BK_PM_PAGE_GUARD	0x0800	/* Página guarda */

/*
 * Flags de alocação de páginas
 */
#define BK_PM_ALLOC_ZERO	0x0001	/* Zerar página após alocação */
#define BK_PM_ALLOC_WIRED	0x0002	/* Alocar página wired */
#define BK_PM_ALLOC_NOWAIT	0x0004	/* Não bloquear se sem memória */
#define BK_PM_ALLOC_KERNEL	0x0008	/* Alocar para o kernel */
#define BK_PM_ALLOC_USER	0x0010	/* Alocar para usuário */
#define BK_PM_ALLOC_COW		0x0020	/* Para Copy-On-Write */
#define BK_PM_ALLOC_CACHE	0x0040	/* Para cache */
#define BK_PM_ALLOC_HIGHMEM	0x0080	/* Alocar em high memory */

/*
 * Zonas de memória
 */
#define BK_PM_ZONE_DMA		0	/* Zona DMA (baixa memória) */
#define BK_PM_ZONE_NORMAL	1	/* Zona normal */
#define BK_PM_ZONE_HIGHMEM	2	/* Zona high memory */
#define BK_PM_ZONE_COUNT	3	/* Número de zonas */

/*
 * Estruturas de dados para gerenciamento de memória física
 */

/* Descritor de página física */
struct __bk_pm_page {
	BK_UINT16	pp_flags;	/* Flags da página */
	BK_UINT8	pp_refcnt;	• Contador de referências */
	BK_UINT8	pp_zone;	• Zona de memória */
	BK_PM_FRAME	pp_frame;	/* Número do frame */
	BK_VM_OFFSET	pp_vaddr;	/* Endereço virtual mapeado (se houver) */
	BK_LIST_ENTRY(__bk_pm_page) pp_link;	/* Link em listas */
	BK_TAILQ_ENTRY(__bk_pm_page) pp_hashlink;	/* Link na hash table */
};

typedef struct __bk_pm_page BK_PM_PAGE;

/* Lista de páginas livres */
BK_LIST_HEAD(__bk_pm_free_list, __bk_pm_page);
typedef struct __bk_pm_free_list BK_PM_FREE_LIST;

/* Hash table para páginas físicas */
#define BK_PM_HASH_SIZE		1024
BK_TAILQ_HEAD(__bk_pm_hash_bucket, __bk_pm_page);
typedef struct __bk_pm_hash_bucket BK_PM_HASH_BUCKET;

/* Zona de memória */
struct __bk_pm_zone {
	BK_PM_PADDR	z_start;	/* Endereço inicial */
	BK_PM_PADDR	z_end;		• Endereço final */
	BK_PM_SIZE	z_size;		/* Tamanho da zona */
	BK_UI32		z_npages;	• Número de páginas */
	BK_UI32		z_free_pages;	/* Páginas livres */
	BK_PM_FREE_LIST	z_free_list;	/* Lista de páginas livres */
	BK_SPINLOCK	z_lock;		/* Lock da zona */
};

typedef struct __bk_pm_zone BK_PM_ZONE;

/* Pool de páginas */
struct __bk_pm_pool {
	BK_PM_ZONE	mp_zones[BK_PM_ZONE_COUNT];	/* Zonas de memória */
	BK_PM_PAGE	*mp_pages;	/* Array de descritores de página */
	BK_UI32		mp_npages;	• Número total de páginas */
	BK_PM_HASH_BUCKET mp_hash[BK_PM_HASH_SIZE];	/* Hash table */
	BK_PM_FREE_LIST	mp_free_pages;	/* Lista global de páginas livres */
	BK_SPINLOCK	mp_lock;	/* Lock global */
};

typedef struct __bk_pm_pool BK_PM_POOL;

/* Estatísticas de memória física */
struct __bk_pm_stats {
	BK_PM_SIZE	ps_total;	/* Total de memória física */
	BK_PM_SIZE	ps_free;	/* Memória física livre */
	BK_PM_SIZE	ps_used;	/* Memória física usada */
	BK_PM_SIZE	ps_wired;	• Memória wired */
	BK_PM_SIZE	ps_cache;	/* Memória em cache */
	BK_PM_SIZE	ps_dma;		/* Memória DMA */
	BK_PM_SIZE	ps_normal;	/* Memória normal */
	BK_PM_SIZE	ps_highmem;	/* High memory */
	BK_UI64		ps_allocations;	• Alocações de página */
	BK_UI64		ps_frees;	/* Liberações de página */
	BK_UI64		ps_pageins;	• Páginas trazidas do swap */
	BK_UI64		ps_pageouts;	• Páginas enviadas para swap */
	BK_UI64		ps_zeroed;	• Páginas zeradas */
};

typedef struct __bk_pm_stats BK_PM_STATS;

/*
 * Funções para gerenciamento de memória física
 */

/* Inicialização do gerenciador de memória física */
BK_I32 bk_pm_init(BK_PM_PADDR start, BK_PM_PADDR end);

/* Alocação/liberação de páginas */
BK_PM_PAGE *bk_pm_alloc_page(BK_UINT32 flags);
BK_PM_PAGE *bk_pm_alloc_pages(BK_UI32 npages, BK_UINT32 flags);
void bk_pm_free_page(BK_PM_PAGE *page);
void bk_pm_free_pages(BK_PM_PAGE *pages, BK_UI32 npages);

/* Alocação wired (não paginável) */
BK_PM_PAGE *bk_pm_alloc_wired(BK_UINT32 flags);
void bk_pm_free_wired(BK_PM_PAGE *page);

/* Alocação de páginas zeradas */
BK_PM_PAGE *bk_pm_alloc_zeroed(BK_UINT32 flags);

/* Manipulação de referências */
void bk_pm_hold_page(BK_PM_PAGE *page);
void bk_pm_release_page(BK_PM_PAGE *page);
BK_UINT8 bk_pm_page_refcnt(BK_PM_PAGE *page);

/* Consulta de informações */
BK_PM_PAGE *bk_pm_addr_to_page(BK_PM_PADDR addr);
BK_PM_PADDR bk_pm_page_to_addr(BK_PM_PAGE *page);
BK_PM_PAGE *bk_pm_frame_to_page(BK_PM_FRAME frame);
BK_PM_FRAME bk_pm_page_to_frame(BK_PM_PAGE *page);

/* Modificação de flags */
void bk_pm_set_page_flags(BK_PM_PAGE *page, BK_UINT16 flags);
void bk_pm_clear_page_flags(BK_PM_PAGE *page, BK_UINT16 flags);
BK_UINT16 bk_pm_get_page_flags(BK_PM_PAGE *page);
BK_BOOLEAN bk_pm_test_page_flags(BK_PM_PAGE *page, BK_UINT16 flags);

/* Mapeamento de páginas */
BK_I32 bk_pm_map_page(BK_PM_PAGE *page, BK_VM_OFFSET vaddr,
		     BK_UINT32 prot, BK_UINT32 flags);
BK_I32 bk_pm_unmap_page(BK_PM_PAGE *page);
BK_VM_OFFSET bk_pm_page_vaddr(BK_PM_PAGE *page);

/* Operações de página */
void bk_pm_zero_page(BK_PM_PAGE *page);
void bk_pm_copy_page(BK_PM_PAGE *dst, BK_PM_PAGE *src);
BK_I32 bk_pm_fill_page(BK_PM_PAGE *page, BK_UI8 value);

/* Gerenciamento de cache */
BK_PM_PAGE *bk_pm_cache_alloc(BK_UI32 npages);
void bk_pm_cache_free(BK_PM_PAGE *pages, BK_UI32 npages);
void bk_pm_cache_release(BK_PM_SIZE size);

/* Zonas de memória */
BK_PM_PAGE *bk_pm_alloc_from_zone(BK_UINT8 zone, BK_UINT32 flags);
BK_UI32 bk_pm_zone_free_pages(BK_UINT8 zone);
BK_UI32 bk_pm_zone_total_pages(BK_UINT8 zone);

/* Statistics */
void bk_pm_get_stats(BK_PM_STATS *stats);
void bk_pm_update_stats(void);

/* Debug e diagnóstico */
#ifdef _BK_PM_DEBUG
void bk_pm_dump_page(BK_PM_PAGE *page);
void bk_pm_dump_zone(BK_UINT8 zone);
void bk_pm_dump_stats(void);
void bk_pm_check_integrity(void);
#endif

/* Funções de utilidade */
static __BK_ALWAYS_INLINE BK_BOOLEAN
bk_pm_is_valid_addr(BK_PM_PADDR addr)
{
	extern BK_PM_PADDR bk_pm_start_addr, bk_pm_end_addr;
	return (addr >= bk_pm_start_addr && addr < bk_pm_end_addr);
}

static __BK_ALWAYS_INLINE BK_BOOLEAN
bk_pm_is_aligned(BK_PM_PADDR addr)
{
	return (addr & BK_PM_PAGE_MASK) == 0;
}

static __BK_ALWAYS_INLINE BK_PM_PADDR
bk_pm_align_up(BK_PM_PADDR addr)
{
	return (addr + BK_PM_PAGE_MASK) & ~BK_PM_PAGE_MASK;
}

static __BK_ALWAYS_INLINE BK_PM_PADDR
bk_pm_align_down(BK_PM_PADDR addr)
{
	return addr & ~BK_PM_PAGE_MASK;
}

static __BK_ALWAYS_INLINE BK_UI32
bk_pm_addr_to_frame(BK_PM_PADDR addr)
{
	return (BK_UI32)(addr >> BK_PM_PAGE_SHIFT);
}

static __BK_ALWAYS_INLINE BK_PM_PADDR
bk_pm_frame_to_addr(BK_UI32 frame)
{
	return (BK_PM_PADDR)frame << BK_PM_PAGE_SHIFT;
}

/*
 * Macros para manipulação de páginas físicas
 */
#define BK_PM_PAGE_STATE(page, state)	((page)->pp_flags & (state))
#define BK_PM_SET_PAGE_STATE(page, state)	((page)->pp_flags |= (state))
#define BK_PM_CLEAR_PAGE_STATE(page, state)	((page)->pp_flags &= ~(state))

/*
 * Interface com o gerenciador de memória virtual
 */
BK_I32 bk_pm_vm_page_fault(BK_VM_OFFSET vaddr, BK_UINT32 fault_type);
BK_I32 bk_pm_vm_pagein(BK_VM_PAGE *vpage);
BK_I32 bk_pm_vm_pageout(BK_VM_PAGE *vpage);

/*
 * Inicialização de emergência (usada durante boot)
 */
BK_PM_PAGE *bk_pm_early_alloc(BK_UINT32 flags);
void bk_pm_early_free(BK_PM_PAGE *page);

/*
 * Reserva de memória para dispositivos/boot
 */
BK_I32 bk_pm_reserve(BK_PM_PADDR start, BK_PM_PADDR end);
BK_I32 bk_pm_unreserve(BK_PM_PADDR start, BK_PM_PADDR end);
BK_BOOLEAN bk_pm_is_reserved(BK_PM_PADDR addr);

#endif /* !_BIBLE_PMAP_H_ */