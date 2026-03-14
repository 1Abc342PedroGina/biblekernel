#ifndef _BIBLE_BITMAP_H_
#define _BIBLE_BITMAP_H_

/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2024 Bible System Developers
 *
 * bitmap.h - Escalonador baseado em Bitmap com conceitos de MLFQs, EDGE, CFS e EEVDF
 * 
 * FILOSOFIA DE ESCALONAMENTO:
 * ===========================
 * 
 * 1. MLFQs (Multi-Level Feedback Queues):
 *    - Múltiplas filas com prioridades decrescentes
 *    - Feedback baseado no comportamento da tarefa
 *    - Prevenção de starvation com envelhecimento
 * 
 * 2. EDGE (Earliest Deadline Group Eligibility):
 *    - Agrupamento por prazos
 *    - Elegibilidade baseada em deadlines
 *    - Balanceamento entre grupos
 * 
 * 3. CFS (Completely Fair Scheduler):
 *    - Árvore rubro-negra por vruntime
 *    - Equilíbrio de tempo virtual
 *    - Latência alvo e granularidade mínima
 * 
 * 4. EEVDF (Earliest Eligible Virtual Deadline First):
 *    - Prazo virtual elegível
 *    - Pesos e fatias de tempo
 *    - Justiça proporcional
 * 
 * INTEGRAÇÃO:
 * ==========
 * - Bitmap para seleção O(1) da fila ativa
 * - Cada fila pode ser CFS (rb-tree) ou EEVDF (deadline)
 * - EDGE para agrupamento cross-fila
 * - MLFQs para prioridades dinâmicas
 */

#include <bible/cdefs.h>
#include <bible/types.h>
#include <bible/ktypes.h>
#include <bible/queue.h>
#include <bible/task.h>
#include <bible/regsister.h>
#include <bible/printl.h>
#include <bible/vm.h>
#include <bible/atomic_common.h>
#include <bible/tree_sched.h>
#include <bible/string.h>

__BK_BEGIN_DECLS

/* ============================================================================
 * DEFINIÇÕES DE ÁRVORE RUBRO-NEGRA (RB-Tree)
 * ============================================================================ */

/* Estrutura de nó RB */
struct bk_rb_node {
    BK_UL3264                  rb_parent_color;
    struct bk_rb_node         *rb_right;
    struct bk_rb_node         *rb_left;
};

typedef struct bk_rb_node BK_RB_NODE;

/* Estrutura de raiz RB */
struct bk_rb_root {
    BK_RB_NODE                 *rb_node;
};

typedef struct bk_rb_root BK_RB_ROOT;

#define BK_RB_ROOT_INIT { NULL }

/* Macros para manipulação RB */
#define BK_RB_ENTRY(type) \
    struct { \
        struct bk_rb_node rb_node; \
    }

#define bk_rb_entry(ptr, type, member) \
    ((type *)((char *)(ptr) - __builtin_offsetof(type, member)))

#define bk_rb_first(root) \
    ((root)->rb_node)

#define bk_rb_next(node) \
    ((node)->rb_right)


/* ============================================================================
 * CONSTANTES DO ESCALONADOR
 * ============================================================================ */

/* Níveis de prioridade (MLFQ) */
#define BK_SCHED_PRIO_LEVELS        32      /* 32 níveis de prioridade */
#define BK_SCHED_PRIO_MAX           0       /* Maior prioridade (0) */
#define BK_SCHED_PRIO_MIN           31      /* Menor prioridade (31) */
#define BK_SCHED_PRIO_DEFAULT       16      /* Prioridade padrão */

/* Time slices por prioridade (MLFQ) */
#define BK_SCHED_TIMESLICE_BASE     10000000 /* 10ms base */
#define BK_SCHED_TIMESLICE_MIN      2000000  /* 2ms mínimo */
#define BK_SCHED_TIMESLICE_MAX      50000000 /* 50ms máximo */

/* Envelhecimento (Aging) */
#define BK_SCHED_AGING_THRESHOLD    10000000 /* 10ms sem execução */
#define BK_SCHED_AGING_FACTOR       1        /* Aumento de prioridade por aging */

/* CFS parâmetros */
#define BK_SCHED_CFS_LATENCY        20000000 /* 20ms latência alvo */
#define BK_SCHED_CFS_GRANULARITY    4000000  /* 4ms granularidade mínima */

/* EEVDF parâmetros */
#define BK_SCHED_EEVDF_WEIGHT_SHIFT 10       /* Shift para pesos */
#define BK_SCHED_EEVDF_WEIGHT_UNIT  1024     /* Peso unitário (nice 0) */

/* EDGE parâmetros */
#define BK_SCHED_EDGE_GROUPS        16       /* Número de grupos de deadline */
#define BK_SCHED_EDGE_GROUP_SHIFT   4        /* Shift para agrupamento */

/* Bitmap tamanhos */
#define BK_SCHED_BITMAP_WORDS       1        /* Palavras para 32 níveis */

/* ============================================================================
 * TIPOS DE FILAS DE ESCALONAMENTO
 * ============================================================================ */

/* Tipo de algoritmo por fila */
typedef enum {
    BK_SCHED_QUEUE_CFS = 0,        /* Completely Fair Scheduler */
    BK_SCHED_QUEUE_EEVDF,          /* Earliest Eligible Virtual Deadline First */
    BK_SCHED_QUEUE_RR,             /* Round Robin (fallback) */
    BK_SCHED_QUEUE_FIFO,           /* First In First Out (tempo real) */
} bk_sched_queue_type_t;

/* Tipo de entidade escalonável */
typedef enum {
    BK_SCHED_ENTITY_PROCESS = 0,
    BK_SCHED_ENTITY_TASK,
    BK_SCHED_ENTITY_THREAD,
    BK_SCHED_ENTITY_ACTIVITY,
} bk_sched_entity_type_t;

/* ============================================================================
 * ESTRUTURAS BÁSICAS DE ESCALONAMENTO
 * ============================================================================ */

/* Entidade base escalonável */
struct bk_sched_entity {
    /* Identificação */
    BK_REGISTER_HANDLE      se_handle;          /* Handle do registro */
    bk_sched_entity_type_t  se_type;            /* Tipo da entidade */
    BK_UINT32               se_id;              /* ID único */
    
    /* Parâmetros de escalonamento */
    BK_UINT8                se_prio;            /* Prioridade atual (0-31) */
    BK_UINT8                se_base_prio;       /* Prioridade base */
    BK_UINT8                se_static_prio;     /* Prioridade estática (nice) */
    BK_UINT8                se_dynamic_prio;    /* Prioridade dinâmica */
    
    /* Pesos (para EEVDF/CFS) */
    BK_UINT32               se_weight;          /* Peso atual */
    BK_UINT32               se_inv_weight;      /* Peso inverso (1/weight) */
    
    /* Tempos virtuais */
    BK_TIME                 se_vruntime;        /* Tempo virtual (CFS) */
    BK_TIME                 se_deadline;        /* Prazo absoluto */
    BK_TIME                 se_eligible;        /* Tempo elegível (EEVDF) */
    BK_TIME                 se_slice;           /* Fatia de tempo atual */
    
    /* Estatísticas */
    BK_TIME                 se_exec_start;      /* Início da execução atual */
    BK_TIME                 se_exec_total;      /* Tempo total executado */
    BK_UINT64               se_exec_count;      /* Número de execuções */
    BK_TIME                 se_last_ran;        /* Último momento de execução */
    BK_TIME                 se_last_enqueued;   /* Último enqueue */
    
    /* Flags e estado */
    BK_UINT32               se_flags;
#define BK_SE_FLAG_ON_RQ        0x0001  /* Na fila de pronto */
#define BK_SE_FLAG_YIELD        0x0002  /* Cedeu CPU voluntariamente */
#define BK_SE_FLAG_PREEMPT      0x0004  /* Foi preemptado */
#define BK_SE_FLAG_REALTIME     0x0008  /* Tempo real */
#define BK_SE_FLAG_SYSTEM       0x0010  /* Entidade do sistema */
#define BK_SE_FLAG_BOOSTED      0x0020  /* Prioridade boosted */
#define BK_SE_FLAG_AGED         0x0040  /* Sofreu aging */
#define BK_SE_FLAG_IDLE         0x0080  /* Entidade idle */
    
    /* Link para a fila */
    BK_LIST_ENTRY(bk_sched_entity) se_queue_link;
    
    /* Link para a árvore CFS/EEVDF */
    BK_RB_ENTRY(bk_sched_entity) se_rb_node;
    
    /* Link para lista global */
    BK_LIST_ENTRY(bk_sched_entity) se_global_link;
};

typedef struct bk_sched_entity bk_sched_entity_t;

/* ============================================================================
 * FILA CFS (Baseada em Árvore Rubro-Negra)
 * ============================================================================ */

struct bk_cfs_rq {
    /* Raiz da árvore rubro-negra (ordenada por vruntime) */
    BK_RB_ROOT              cfs_root;
    BK_RB_NODE              *cfs_leftmost;      /* Nó mais à esquerda (menor vruntime) */
    
    /* Estatísticas da fila */
    BK_UINT32               cfs_nr_running;     /* Número de entidades */
    BK_TIME                 cfs_min_vruntime;    /* Menor vruntime na fila */
    BK_TIME                 cfs_max_vruntime;    /* Maior vruntime na fila */
    BK_TIME                 cfs_avg_vruntime;    /* Média de vruntime */
    BK_UINT64               cfs_load_weight;     /* Carga total (soma dos pesos) */
    
    /* Parâmetros */
    BK_TIME                 cfs_period;          /* Período de latência */
    BK_TIME                 cfs_granularity;     /* Granularidade mínima */
    
    /* Balanço */
    BK_TIME                 cfs_exec_clock;      /* Clock de execução */
    BK_UINT64               cfs_exec_count;      /* Contador de execuções */
    
    /* Lock da fila */
    BK_SPINLOCK             cfs_lock;
};

typedef struct bk_cfs_rq bk_cfs_rq_t;

/* ============================================================================
 * FILA EEVDF (Earliest Eligible Virtual Deadline First)
 * ============================================================================ */

struct bk_eevdf_rq {
    /* Árvore por eligible time */
    BK_RB_ROOT              eevdf_eligible_root;   /* Ordenada por eligible time */
    BK_RB_NODE              *eevdf_eligible_leftmost;
    
    /* Árvore por deadline */
    BK_RB_ROOT              eevdf_deadline_root;    /* Ordenada por deadline */
    BK_RB_NODE              *eevdf_deadline_leftmost;
    
    /* Estatísticas */
    BK_UINT32               eevdf_nr_running;       /* Número de entidades */
    BK_UINT64               eevdf_total_weight;     /* Soma dos pesos */
    BK_TIME                 eevdf_virtual_time;     /* Tempo virtual da fila */
    BK_TIME                 eevdf_min_deadline;     /* Menor deadline */
    BK_TIME                 eevdf_min_eligible;     /* Menor eligible time */
    
    /* Parâmetros */
    BK_TIME                 eevdf_latency;          /* Latência alvo */
    BK_UINT32               eevdf_weight_shift;     /* Shift para pesos */
    
    /* Lock da fila */
    BK_SPINLOCK             eevdf_lock;
};

typedef struct bk_eevdf_rq bk_eevdf_rq_t;

/* ============================================================================
 * FILA MLFQ (Multi-Level Feedback Queue)
 * ============================================================================ */

/* Uma fila em um nível do MLFQ */
struct bk_mlfq_level {
    /* Bitmap de prioridade (para seleção O(1)) */
    BK_UINT32               ml_bitmap;               /* Bitmap de atividades */
    
    /* Filas por algoritmo */
    union {
        bk_cfs_rq_t         ml_cfs;                  /* Fila CFS */
        bk_eevdf_rq_t       ml_eevdf;                /* Fila EEVDF */
        BK_LIST_HEAD(, bk_sched_entity) ml_list;     /* Fila simples (RR/FIFO) */
    } ml_queue;
    
    /* Tipo de algoritmo usado neste nível */
    bk_sched_queue_type_t   ml_type;
    
    /* Time slice para este nível (MLFQ) */
    BK_TIME                 ml_timeslice;
    
    /* Prioridade base deste nível */
    BK_UINT8                ml_priority;
    
    /* Número de entidades neste nível */
    BK_UINT32               ml_count;
    
    /* Flags do nível */
    BK_UINT32               ml_flags;
#define BK_MLFQ_FLAG_EXPIRED    0x0001   /* Time slice expirado */
#define BK_MLFQ_FLAG_ACTIVE     0x0002   /* Nível ativo */
#define BK_MLFQ_FLAG_AGING      0x0004   /* Aging ativo neste nível */
};

typedef struct bk_mlfq_level bk_mlfq_level_t;

/* ============================================================================
 * GRUPO EDGE (Earliest Deadline Group Eligibility)
 * ============================================================================ */

struct bk_edge_group {
    /* ID do grupo */
    BK_UINT32               eg_id;
    
    /* Deadline do grupo */
    BK_TIME                 eg_deadline;
    
    /* Limite de tempo para o grupo */
    BK_TIME                 eg_budget;
    BK_TIME                 eg_used;
    
    /* Filas pertencentes a este grupo */
    BK_UINT32               eg_level_mask;          /* Máscara de níveis */
    bk_mlfq_level_t         *eg_levels[BK_SCHED_PRIO_LEVELS];
    
    /* Estatísticas */
    BK_UINT32               eg_entities;
    BK_UINT64               eg_total_weight;
    BK_TIME                 eg_min_deadline;
    
    /* Link para lista de grupos */
    BK_LIST_ENTRY(bk_edge_group) eg_link;
};

typedef struct bk_edge_group bk_edge_group_t;

/* ============================================================================
 * ESTRUTURA PRINCIPAL DO ESCALONADOR (Bitmap-based)
 * ============================================================================ */

struct bk_bitmap_sched {
    /* Bitmap global de prioridades ativas */
    BK_UINT32               bs_prio_bitmap[BK_SCHED_BITMAP_WORDS];
    
    /* Níveis MLFQ */
    bk_mlfq_level_t         bs_levels[BK_SCHED_PRIO_LEVELS];
    
    /* Grupos EDGE */
    BK_LIST_HEAD(, bk_edge_group) bs_edge_groups;
    BK_UINT32               bs_edge_group_count;
    
    /* Estatísticas globais */
    BK_TIME                 bs_total_runtime;
    BK_UINT64               bs_total_switches;
    BK_UINT32               bs_total_entities;
    BK_UINT32               bs_running_entities;
    
    /* Clock do sistema */
    BK_TIME                 bs_clock;
    BK_TIME                 bs_last_tick;
    
    /* Entidade idle */
    bk_sched_entity_t       *bs_idle;
    
    /* Entidade atual em execução */
    bk_sched_entity_t       *bs_current;
    
    /* Parâmetros configuráveis */
    struct {
        BK_UINT32           aging_threshold;        /* Threshold para aging */
        BK_UINT32           aging_factor;           /* Fator de aging */
        BK_UINT32           boost_priority;         /* Prioridade boost */
        BK_UINT32           latency_target;         /* Latência alvo */
        BK_UINT32           min_granularity;        /* Granularidade mínima */
    } bs_params;
    
    /* Estatísticas de debug */
#ifdef _BK_SCHED_DEBUG
    BK_UINT64               bs_enqueues;
    BK_UINT64               bs_dequeues;
    BK_UINT64               bs_preemptions;
    BK_UINT64               bs_migrations;
#endif
    
    /* Lock global do escalonador */
    BK_SPINLOCK             bs_lock;
};

typedef struct bk_bitmap_sched bk_bitmap_sched_t;

/* ============================================================================
 * FUNÇÕES DE ÁRVORE RUBRO-NEGRA
 * ============================================================================ */

static __BK_ALWAYS_INLINE struct bk_rb_node *
rb_first(struct bk_rb_root *root)
{
    struct bk_rb_node *n = root->rb_node;
    if (!n)
        return NULL;
    while (n->rb_left)
        n = n->rb_left;
    return n;
}

static __BK_ALWAYS_INLINE struct bk_rb_node *
rb_next(struct bk_rb_node *node)
{
    if (node->rb_right) {
        node = node->rb_right;
        while (node->rb_left)
            node = node->rb_left;
        return node;
    }
    return NULL;
}

#define rb_entry(ptr, type, member) \
    ((type *)((char *)(ptr) - __builtin_offsetof(type, member)))

/* ============================================================================
 * FUNÇÕES DE MANIPULAÇÃO DE BITMAP
 * ============================================================================ */

/**
 * bk_sched_find_first_bit - Encontra o primeiro bit setado
 * @bitmap: Palavra do bitmap
 * 
 * Retorna: Posição do primeiro bit (0-31) ou -1 se nenhum
 */
static __BK_ALWAYS_INLINE BK_I32
bk_sched_find_first_bit(BK_UINT32 bitmap)
{
    if (bitmap == 0)
        return -1;
    return __builtin_ctz(bitmap);
}

/**
 * bk_sched_find_next_bit - Encontra o próximo bit após uma posição
 * @bitmap: Palavra do bitmap
 * @start: Posição inicial
 * 
 * Retorna: Posição do próximo bit ou -1 se nenhum
 */
static __BK_ALWAYS_INLINE BK_I32
bk_sched_find_next_bit(BK_UINT32 bitmap, BK_I32 start)
{
    if (start >= 31)
        return -1;
    
    bitmap &= ~((1ULL << (start + 1)) - 1);
    if (bitmap == 0)
        return -1;
    return __builtin_ctz(bitmap);
}

/**
 * bk_sched_set_prio_bit - Ativa um nível de prioridade no bitmap
 * @sched: Escalonador
 * @prio: Prioridade (0-31)
 */
static __BK_ALWAYS_INLINE void
bk_sched_set_prio_bit(bk_bitmap_sched_t *sched, BK_UINT8 prio)
{
    BK_UINT32 word = prio / 32;
    BK_UINT32 bit = prio % 32;
    sched->bs_prio_bitmap[word] |= (1ULL << bit);
}

/**
 * bk_sched_clear_prio_bit - Desativa um nível de prioridade
 * @sched: Escalonador
 * @prio: Prioridade (0-31)
 */
static __BK_ALWAYS_INLINE void
bk_sched_clear_prio_bit(bk_bitmap_sched_t *sched, BK_UINT8 prio)
{
    BK_UINT32 word = prio / 32;
    BK_UINT32 bit = prio % 32;
    sched->bs_prio_bitmap[word] &= ~(1ULL << bit);
}

/**
 * bk_sched_test_prio_bit - Testa se um nível está ativo
 * @sched: Escalonador
 * @prio: Prioridade (0-31)
 */
static __BK_ALWAYS_INLINE BK_BOOL
bk_sched_test_prio_bit(bk_bitmap_sched_t *sched, BK_UINT8 prio)
{
    BK_UINT32 word = prio / 32;
    BK_UINT32 bit = prio % 32;
    return (sched->bs_prio_bitmap[word] & (1ULL << bit)) != 0;
}

/**
 * bk_sched_highest_prio - Retorna a maior prioridade ativa
 * @sched: Escalonador
 * 
 * Retorna: Prioridade (0-31) ou -1 se nenhuma
 */
static __BK_ALWAYS_INLINE BK_I32
bk_sched_highest_prio(bk_bitmap_sched_t *sched)
{
    BK_I32 i;
    for (i = 0; i < BK_SCHED_BITMAP_WORDS; i++) {
        if (sched->bs_prio_bitmap[i]) {
            return i * 32 + __builtin_ctz(sched->bs_prio_bitmap[i]);
        }
    }
    return -1;
}

/* ============================================================================
 * FUNÇÕES DE FILA CFS
 * ============================================================================ */

/**
 * bk_cfs_rq_init - Inicializa uma fila CFS
 */
static __BK_ALWAYS_INLINE void
bk_cfs_rq_init(bk_cfs_rq_t *cfs)
{
    cfs->cfs_root = (struct bk_rb_root)BK_RB_ROOT_INIT;
    cfs->cfs_leftmost = NULL;
    cfs->cfs_nr_running = 0;
    cfs->cfs_min_vruntime = 0;
    cfs->cfs_max_vruntime = 0;
    cfs->cfs_avg_vruntime = 0;
    cfs->cfs_load_weight = 0;
    cfs->cfs_period = BK_SCHED_CFS_LATENCY;
    cfs->cfs_granularity = BK_SCHED_CFS_GRANULARITY;
    cfs->cfs_exec_clock = 0;
    cfs->cfs_exec_count = 0;
    cfs->cfs_lock.s_lock = BK_SPIN_UNLOCKED;
}

/**
 * bk_cfs_enqueue - Adiciona entidade à fila CFS
 */
void bk_cfs_enqueue(bk_cfs_rq_t *cfs, bk_sched_entity_t *se);

/**
 * bk_cfs_dequeue - Remove entidade da fila CFS
 */
void bk_cfs_dequeue(bk_cfs_rq_t *cfs, bk_sched_entity_t *se);

/**
 * bk_cfs_pick_next - Escolhe a próxima entidade (menor vruntime)
 */
bk_sched_entity_t *bk_cfs_pick_next(bk_cfs_rq_t *cfs);

/**
 * bk_cfs_update_curr - Atualiza a entidade atual
 */
void bk_cfs_update_curr(bk_cfs_rq_t *cfs, bk_sched_entity_t *curr, BK_TIME now);

/* ============================================================================
 * FUNÇÕES DE FILA EEVDF
 * ============================================================================ */

/**
 * bk_eevdf_rq_init - Inicializa uma fila EEVDF
 */
static __BK_ALWAYS_INLINE void
bk_eevdf_rq_init(bk_eevdf_rq_t *eevdf)
{
    eevdf->eevdf_eligible_root.rb_node = NULL;
    eevdf->eevdf_eligible_leftmost = NULL;
    eevdf->eevdf_deadline_root.rb_node = NULL;
    eevdf->eevdf_deadline_leftmost = NULL;
    eevdf->eevdf_nr_running = 0;
    eevdf->eevdf_total_weight = 0;
    eevdf->eevdf_virtual_time = 0;
    eevdf->eevdf_min_deadline = 0;
    eevdf->eevdf_min_eligible = 0;
    eevdf->eevdf_latency = BK_SCHED_CFS_LATENCY;
    eevdf->eevdf_weight_shift = BK_SCHED_EEVDF_WEIGHT_SHIFT;
    eevdf->eevdf_lock.s_lock = BK_SPIN_UNLOCKED;
}

/**
 * bk_eevdf_enqueue - Adiciona entidade à fila EEVDF
 */
void bk_eevdf_enqueue(bk_eevdf_rq_t *eevdf, bk_sched_entity_t *se);

/**
 * bk_eevdf_dequeue - Remove entidade da fila EEVDF
 */
void bk_eevdf_dequeue(bk_eevdf_rq_t *eevdf, bk_sched_entity_t *se);

/**
 * bk_eevdf_pick_next - Escolhe a próxima entidade (menor deadline elegível)
 */
bk_sched_entity_t *bk_eevdf_pick_next(bk_eevdf_rq_t *eevdf);

/**
 * bk_eevdf_update - Atualiza parâmetros EEVDF
 */
void bk_eevdf_update(bk_eevdf_rq_t *eevdf, bk_sched_entity_t *se, BK_TIME now);

/* ============================================================================
 * FUNÇÕES MLFQ (Multi-Level Feedback Queue)
 * ============================================================================ */

/**
 * bk_mlfq_init - Inicializa todos os níveis do MLFQ
 */
void
bk_mlfq_init(bk_bitmap_sched_t *sched)
{
    BK_UINT32 i;
    
    for (i = 0; i < BK_SCHED_PRIO_LEVELS; i++) {
        bk_mlfq_level_t *level = &sched->bs_levels[i];
        
        /* Prioridade deste nível */
        level->ml_priority = i;
        
        /* Time slice decresce com a prioridade */
        level->ml_timeslice = BK_SCHED_TIMESLICE_BASE / (i + 1);
        if (level->ml_timeslice < BK_SCHED_TIMESLICE_MIN)
            level->ml_timeslice = BK_SCHED_TIMESLICE_MIN;
        
        /* Níveis mais altos (0-15) usam EEVDF, níveis mais baixos usam CFS */
        if (i < 16) {
            level->ml_type = BK_SCHED_QUEUE_EEVDF;
            bk_eevdf_rq_init(&level->ml_queue.ml_eevdf);
        } else {
            level->ml_type = BK_SCHED_QUEUE_CFS;
            bk_cfs_rq_init(&level->ml_queue.ml_cfs);
        }
        
        level->ml_bitmap = 0;
        level->ml_count = 0;
        level->ml_flags = BK_MLFQ_FLAG_ACTIVE;
        
        /* Aging em níveis baixos */
        if (i > 20)
            level->ml_flags |= BK_MLFQ_FLAG_AGING;
    }
    
    /* Inicializa bitmap de prioridades */
    for (i = 0; i < BK_SCHED_BITMAP_WORDS; i++)
        sched->bs_prio_bitmap[i] = 0;
}

/**
 * bk_mlfq_enqueue - Adiciona entidade ao MLFQ no nível apropriado
 */
void
bk_mlfq_enqueue(bk_bitmap_sched_t *sched, bk_sched_entity_t *se)
{
    BK_UINT8 prio = se->se_prio;
    bk_mlfq_level_t *level = &sched->bs_levels[prio];
    
    sched_spin_lock(&level->ml_queue.ml_eevdf.eevdf_lock);  /* Lock genérico */
    
    switch (level->ml_type) {
        case BK_SCHED_QUEUE_CFS:
            bk_cfs_enqueue(&level->ml_queue.ml_cfs, se);
            break;
        case BK_SCHED_QUEUE_EEVDF:
            bk_eevdf_enqueue(&level->ml_queue.ml_eevdf, se);
            break;
        case BK_SCHED_QUEUE_RR:
        case BK_SCHED_QUEUE_FIFO:
            BK_LIST_INSERT_HEAD(&level->ml_queue.ml_list, se, se_queue_link);
            break;
    }
    
    level->ml_count++;
    if (level->ml_count == 1) {
        /* Primeira entidade neste nível - ativa o bit */
        bk_sched_set_prio_bit(sched, prio);
    }
    
    sched_spin_unlock(&level->ml_queue.ml_eevdf.eevdf_lock);
    
    se->se_flags |= BK_SE_FLAG_ON_RQ;
    se->se_last_enqueued = sched->bs_clock;
    
#ifdef _BK_SCHED_DEBUG
    sched->bs_enqueues++;
#endif
}

/**
 * bk_mlfq_dequeue - Remove entidade do MLFQ
 */
void
bk_mlfq_dequeue(bk_bitmap_sched_t *sched, bk_sched_entity_t *se)
{
    BK_UINT8 prio = se->se_prio;
    bk_mlfq_level_t *level = &sched->bs_levels[prio];
    
    sched_spin_lock(&level->ml_queue.ml_eevdf.eevdf_lock);
    
    switch (level->ml_type) {
        case BK_SCHED_QUEUE_CFS:
            bk_cfs_dequeue(&level->ml_queue.ml_cfs, se);
            break;
        case BK_SCHED_QUEUE_EEVDF:
            bk_eevdf_dequeue(&level->ml_queue.ml_eevdf, se);
            break;
        case BK_SCHED_QUEUE_RR:
        case BK_SCHED_QUEUE_FIFO:
            BK_LIST_REMOVE(se, se_queue_link);
            break;
    }
    
    level->ml_count--;
    if (level->ml_count == 0) {
        /* Última entidade removida - desativa o bit */
        bk_sched_clear_prio_bit(sched, prio);
    }
    
    sched_spin_unlock(&level->ml_queue.ml_eevdf.eevdf_lock);
    
    se->se_flags &= ~BK_SE_FLAG_ON_RQ;
    
#ifdef _BK_SCHED_DEBUG
    sched->bs_dequeues++;
#endif
}

/**
 * bk_mlfq_pick_next - Escolhe a próxima entidade da mais alta prioridade
 */
bk_sched_entity_t *
bk_mlfq_pick_next(bk_bitmap_sched_t *sched)
{
    BK_I32 prio;
    bk_sched_entity_t *next = NULL;
    bk_mlfq_level_t *level;
    
    /* Encontra a maior prioridade ativa */
    prio = bk_sched_highest_prio(sched);
    if (prio < 0)
        return sched->bs_idle;  /* Nada pronto - retorna idle */
    
    level = &sched->bs_levels[prio];
    
    sched_spin_lock(&level->ml_queue.ml_eevdf.eevdf_lock);
    
    switch (level->ml_type) {
        case BK_SCHED_QUEUE_CFS:
            next = bk_cfs_pick_next(&level->ml_queue.ml_cfs);
            break;
        case BK_SCHED_QUEUE_EEVDF:
            next = bk_eevdf_pick_next(&level->ml_queue.ml_eevdf);
            break;
        case BK_SCHED_QUEUE_RR:
        case BK_SCHED_QUEUE_FIFO:
            next = BK_LIST_FIRST(&level->ml_queue.ml_list);
        if (next && level->ml_type == BK_SCHED_QUEUE_RR) {
    /* Round Robin - rotaciona a fila manualmente */
    bk_sched_entity_t *first = BK_LIST_FIRST(&level->ml_queue.ml_list);
    if (first) {
        /* Remove o primeiro e insere no final */
        BK_LIST_REMOVE(first, se_queue_link);
        /* Insere no final percorrendo a lista */
        bk_sched_entity_t *last = first;
        while (BK_LIST_NEXT(last, se_queue_link)) {
            last = BK_LIST_NEXT(last, se_queue_link);
        }
        BK_LIST_INSERT_AFTER(last, first, se_queue_link);
    }
    next = BK_LIST_FIRST(&level->ml_queue.ml_list); /* Pega o novo primeiro */
}
            break;
    }
    
    sched_spin_unlock(&level->ml_queue.ml_eevdf.eevdf_lock);
    
    return next;
}

/* ============================================================================
 * FUNÇÕES DE AGING (MLFQ)
 * ============================================================================ */

/**
 * bk_mlfq_aging - Aplica aging para prevenir starvation
 * 
 * Aumenta a prioridade de tarefas que esperam muito tempo
 */
void
bk_mlfq_aging(bk_bitmap_sched_t *sched)
{
    BK_UINT32 i;
    BK_TIME now = sched->bs_clock;
    
    for (i = BK_SCHED_PRIO_MIN; i > BK_SCHED_PRIO_MAX; i--) {
        bk_mlfq_level_t *level = &sched->bs_levels[i];
        
        if (!(level->ml_flags & BK_MLFQ_FLAG_AGING))
            continue;
        
        sched_spin_lock(&level->ml_queue.ml_eevdf.eevdf_lock);
        
        /* Aging em filas CFS */
        if (level->ml_type == BK_SCHED_QUEUE_CFS) {
            bk_cfs_rq_t *cfs = &level->ml_queue.ml_cfs;
            BK_RB_NODE *node;
            
            /* Itera sobre a árvore CFS */
            for (node = rb_first(&cfs->cfs_root); node; node = rb_next(node)) {
                bk_sched_entity_t *se = rb_entry(node, bk_sched_entity_t, se_rb_node);
                BK_TIME wait_time = now - se->se_last_ran;
                
                if (wait_time > sched->bs_params.aging_threshold) {
                    /* Aumenta prioridade (diminui o número) */
                    if (se->se_prio > BK_SCHED_PRIO_MAX + sched->bs_params.aging_factor) {
                        se->se_prio -= sched->bs_params.aging_factor;
                        se->se_flags |= BK_SE_FLAG_AGED;
                    }
                }
            }
        }
        
        sched_spin_unlock(&level->ml_queue.ml_eevdf.eevdf_lock);
    }
}

/* ============================================================================
 * FUNÇÕES EDGE (Earliest Deadline Group Eligibility)
 * ============================================================================ */

/**
 * bk_edge_group_create - Cria um novo grupo EDGE
 */
bk_edge_group_t *
bk_edge_group_create(bk_bitmap_sched_t *sched, BK_TIME deadline)
{
    bk_edge_group_t *group;
    
    group = (bk_edge_group_t *)bk_vm_alloc(NULL, sizeof(bk_edge_group_t),
                                           BK_VM_PROT_READ | BK_VM_PROT_WRITE,
                                           BK_VM_MAP_ANON);
    if (!group)
        return NULL;
    
    group->eg_id = sched->bs_edge_group_count++;
    group->eg_deadline = deadline;
    group->eg_budget = deadline / 10;  /* 10% do deadline como budget inicial */
    group->eg_used = 0;
    group->eg_level_mask = 0;
    group->eg_entities = 0;
    group->eg_total_weight = 0;
    group->eg_min_deadline = deadline;
    
    BK_LIST_INSERT_HEAD(&sched->bs_edge_groups, group, eg_link);
    
    return group;
}

/**
 * bk_edge_assign_level - Atribui um nível MLFQ a um grupo EDGE
 */
void
bk_edge_assign_level(bk_edge_group_t *group, BK_UINT8 prio)
{
    if (prio < BK_SCHED_PRIO_LEVELS) {
        group->eg_level_mask |= (1 << prio);
    }
}

/**
 * bk_edge_update_deadlines - Atualiza deadlines do grupo
 */
void
bk_edge_update_deadlines(bk_bitmap_sched_t *sched)
{
    bk_edge_group_t *group;
    BK_TIME now = sched->bs_clock;
    
    BK_LIST_FOREACH(group, &sched->bs_edge_groups, eg_link) {
        BK_TIME group_deadline = group->eg_deadline;
        BK_UINT32 i;
        
        /* Atualiza deadlines das entidades nos níveis do grupo */
        for (i = 0; i < BK_SCHED_PRIO_LEVELS; i++) {
            if (group->eg_level_mask & (1 << i)) {
                bk_mlfq_level_t *level = &sched->bs_levels[i];
                
                if (level->ml_type == BK_SCHED_QUEUE_EEVDF) {
                    bk_eevdf_rq_t *eevdf = &level->ml_queue.ml_eevdf;
                    BK_RB_NODE *node;
                    
                    /* Ajusta deadlines das entidades EEVDF */
                    for (node = rb_first(&eevdf->eevdf_deadline_root); node;
                         node = rb_next(node)) {
                        bk_sched_entity_t *se = rb_entry(node, bk_sched_entity_t, se_rb_node);
                        /* Deadline não pode passar do deadline do grupo */
                        if (se->se_deadline > group_deadline)
                            se->se_deadline = group_deadline;
                    }
                }
            }
        }
        
        /* Verifica se o grupo estourou o budget */
        if (group->eg_used > group->eg_budget) {
            /* Penaliza o grupo - aumenta deadlines */
            group->eg_deadline += BK_SCHED_CFS_GRANULARITY;
        }
    }
}

/* ============================================================================
 * FUNÇÕES PRINCIPAIS DO ESCALONADOR
 * ============================================================================ */

/**
 * bk_bitmap_sched_init - Inicializa o escalonador bitmap
 */
bk_bitmap_sched_t *
bk_bitmap_sched_init(void)
{
    bk_bitmap_sched_t *sched;
    
    sched = (bk_bitmap_sched_t *)bk_vm_alloc(NULL, sizeof(bk_bitmap_sched_t),
                                             BK_VM_PROT_READ | BK_VM_PROT_WRITE,
                                             BK_VM_MAP_ANON);
    if (!sched)
        return NULL;
    
    /* Inicializa bitmap de prioridades */
    bk_memset(sched->bs_prio_bitmap, 0, sizeof(sched->bs_prio_bitmap));
    
    /* Inicializa níveis MLFQ */
    bk_mlfq_init(sched);
    
    /* Inicializa lista de grupos EDGE */
    BK_LIST_INIT(&sched->bs_edge_groups);
    sched->bs_edge_group_count = 0;
    
    /* Estatísticas */
    sched->bs_total_runtime = 0;
    sched->bs_total_switches = 0;
    sched->bs_total_entities = 0;
    sched->bs_running_entities = 0;
    
    /* Clock */
    sched->bs_clock = 0;
    sched->bs_last_tick = 0;
    
    /* Idle e current */
    sched->bs_idle = NULL;
    sched->bs_current = NULL;
    
    /* Parâmetros */
    sched->bs_params.aging_threshold = BK_SCHED_AGING_THRESHOLD;
    sched->bs_params.aging_factor = BK_SCHED_AGING_FACTOR;
    sched->bs_params.boost_priority = 0;
    sched->bs_params.latency_target = BK_SCHED_CFS_LATENCY;
    sched->bs_params.min_granularity = BK_SCHED_CFS_GRANULARITY;
    
    /* Lock */
    sched->bs_lock.s_lock = BK_SPIN_UNLOCKED;
    
    return sched;
}

/**
 * bk_sched_tick - Tick do escalonador (chamado pelo timer)
 */
void
bk_sched_tick(bk_bitmap_sched_t *sched)
{
    sched_spin_lock(&sched->bs_lock);
    
    sched->bs_clock += BK_SCHED_CFS_GRANULARITY;
    sched->bs_last_tick = sched->bs_clock;
    
    /* Aging periódico */
    if ((sched->bs_clock % (BK_SCHED_AGING_THRESHOLD * 10)) == 0) {
        bk_mlfq_aging(sched);
    }
    
    /* Atualiza deadlines EDGE */
    if ((sched->bs_clock % (BK_SCHED_CFS_LATENCY / 4)) == 0) {
        bk_edge_update_deadlines(sched);
    }
    
    /* Verifica se precisa preemptar a tarefa atual */
    if (sched->bs_current) {
        BK_TIME runtime = sched->bs_clock - sched->bs_current->se_exec_start;
        BK_UINT8 current_prio = sched->bs_current->se_prio;
        BK_I32 highest_prio = bk_sched_highest_prio(sched);
        
        /* Preempta se:
         * 1. Expirou o time slice
         * 2. Apareceu uma tarefa de maior prioridade
         */
        if (runtime >= sched->bs_levels[current_prio].ml_timeslice ||
            (highest_prio >= 0 && highest_prio < current_prio)) {
            sched->bs_current->se_flags |= BK_SE_FLAG_PREEMPT;
#ifdef _BK_SCHED_DEBUG
            sched->bs_preemptions++;
#endif
        }
    }
    
    sched_spin_unlock(&sched->bs_lock);
}

/**
 * bk_sched_entity_create - Cria uma nova entidade escalonável
 */
bk_sched_entity_t *
bk_sched_entity_create(BK_REGISTER_HANDLE handle, bk_sched_entity_type_t type)
{
    bk_sched_entity_t *se;
    
    se = (bk_sched_entity_t *)bk_vm_alloc(NULL, sizeof(bk_sched_entity_t),
                                          BK_VM_PROT_READ | BK_VM_PROT_WRITE,
                                          BK_VM_MAP_ANON);
    if (!se)
        return NULL;
    
    se->se_handle = handle;
    se->se_type = type;
    se->se_id = (BK_UINT32)handle;  /* Simplificado */
    
    /* Prioridades */
    se->se_prio = BK_SCHED_PRIO_DEFAULT;
    se->se_base_prio = BK_SCHED_PRIO_DEFAULT;
    se->se_static_prio = 0;  /* nice 0 */
    se->se_dynamic_prio = BK_SCHED_PRIO_DEFAULT;
    
    /* Pesos (EEVDF) */
    se->se_weight = BK_SCHED_EEVDF_WEIGHT_UNIT;
    se->se_inv_weight = (1ULL << BK_SCHED_EEVDF_WEIGHT_SHIFT) / se->se_weight;
    
    /* Tempos */
    se->se_vruntime = 0;
    se->se_deadline = 0;
    se->se_eligible = 0;
    se->se_slice = BK_SCHED_TIMESLICE_BASE;
    
    /* Estatísticas */
    se->se_exec_start = 0;
    se->se_exec_total = 0;
    se->se_exec_count = 0;
    se->se_last_ran = 0;
    se->se_last_enqueued = 0;
    
    /* Flags */
    se->se_flags = 0;
    
    return se;
}

/**
 * bk_sched_enqueue - Adiciona entidade ao escalonador
 */
void
bk_sched_enqueue(bk_bitmap_sched_t *sched, bk_sched_entity_t *se)
{
    sched_spin_lock(&sched->bs_lock);
    
    /* Atualiza prioridade dinâmica baseada em comportamento */
    if (se->se_exec_count > 0) {
        BK_TIME iowait = se->se_last_ran - se->se_last_enqueued;
        if (iowait > BK_SCHED_AGING_THRESHOLD * 2) {
            /* Tarefa I/O bound - aumenta prioridade */
            if (se->se_prio > 0)
                se->se_prio--;
        } else {
            /* Tarefa CPU bound - diminui prioridade (MLFQ feedback) */
            if (se->se_prio < BK_SCHED_PRIO_MIN)
                se->se_prio++;
        }
    }
    
    bk_mlfq_enqueue(sched, se);
    sched->bs_total_entities++;
    sched->bs_running_entities++;
    
    sched_spin_unlock(&sched->bs_lock);
}

/**
 * bk_sched_dequeue - Remove entidade do escalonador
 */
void
bk_sched_dequeue(bk_bitmap_sched_t *sched, bk_sched_entity_t *se)
{
    sched_spin_lock(&sched->bs_lock);
    
    bk_mlfq_dequeue(sched, se);
    sched->bs_running_entities--;
    
    sched_spin_unlock(&sched->bs_lock);
}

/**
 * bk_sched_pick_next - Escolhe a próxima entidade a executar
 */
bk_sched_entity_t *
bk_bitmap_pick_next(bk_bitmap_sched_t *sched)
{
    bk_sched_entity_t *next;
    
    sched_spin_lock(&sched->bs_lock);
    
    next = bk_mlfq_pick_next(sched);
    
    if (next && next != sched->bs_current) {
        sched->bs_total_switches++;
        
        /* Atualiza a entidade atual */
        if (sched->bs_current) {
            BK_TIME runtime = sched->bs_clock - sched->bs_current->se_exec_start;
            sched->bs_current->se_exec_total += runtime;
            sched->bs_current->se_exec_count++;
            sched->bs_current->se_last_ran = sched->bs_clock;
            
            /* Atualiza vruntime para CFS */
            sched->bs_current->se_vruntime += runtime;
        }
        
        /* Prepara a próxima entidade */
        next->se_exec_start = sched->bs_clock;
        next->se_flags &= ~(BK_SE_FLAG_PREEMPT | BK_SE_FLAG_YIELD);
        
        sched->bs_current = next;
    }
    
    sched_spin_unlock(&sched->bs_lock);
    
    return next ? next : sched->bs_idle;
}

/* ============================================================================
 * FUNÇÕES DE INTERFACE COM O SISTEMA DE REGISTROS
 * ============================================================================ */

/**
 * bk_sched_register_entity - Registra uma entidade do sistema
 */
bk_sched_entity_t *
bk_sched_register_entity(bk_bitmap_sched_t *sched, BK_REGISTER_HANDLE handle)
{
    bk_sched_entity_t *se;
    BK_REGISTER_TYPE type = bk_register_get_type(handle);
    bk_sched_entity_type_t entity_type;
    
    /* Converte tipo do registro para tipo de entidade */
    switch (type) {
        case BK_REGISTER_TYPE_PROCESS:
            entity_type = BK_SCHED_ENTITY_PROCESS;
            break;
        case BK_REGISTER_TYPE_TASK:
            entity_type = BK_SCHED_ENTITY_TASK;
            break;
        case BK_REGISTER_TYPE_THREAD:
            entity_type = BK_SCHED_ENTITY_THREAD;
            break;
        case BK_REGISTER_TYPE_ACTIVITY:
            entity_type = BK_SCHED_ENTITY_ACTIVITY;
            break;
        default:
            return NULL;
    }
    
    se = bk_sched_entity_create(handle, entity_type);
    if (se) {
        bk_sched_enqueue(sched, se);
    }
    
    return se;
}

/* ============================================================================
 * FUNÇÕES DE DEBUG
 * ============================================================================ */

#ifdef _BK_SCHED_DEBUG

void
bk_sched_dump(bk_bitmap_sched_t *sched)
{
    BK_UINT32 i;
    
    printl("Bitmap Scheduler [%p]:\n", sched);
    printl("  Prioridades ativas: 0x%08x\n", sched->bs_prio_bitmap[0]);
    printl("  Total entidades: %u, Executando: %u\n",
              sched->bs_total_entities, sched->bs_running_entities);
    printl("  Total switches: %llu, Preemptions: %llu\n",
              sched->bs_total_switches, sched->bs_preemptions);
    printl("  Enqueues: %llu, Dequeues: %llu, Migrations: %llu\n",
              sched->bs_enqueues, sched->bs_dequeues, sched->bs_migrations);
    printl("  Clock: %llu\n", sched->bs_clock);
    
    printl("\nNíveis MLFQ:\n");
    for (i = 0; i < BK_SCHED_PRIO_LEVELS; i++) {
        bk_mlfq_level_t *level = &sched->bs_levels[i];
        if (level->ml_count > 0) {
            printl("  Nível %2u: count=%u type=%s timeslice=%llu\n",
                     i, level->ml_count,
                     level->ml_type == BK_SCHED_QUEUE_CFS ? "CFS" :
                     level->ml_type == BK_SCHED_QUEUE_EEVDF ? "EEVDF" :
                     level->ml_type == BK_SCHED_QUEUE_RR ? "RR" : "FIFO",
                     level->ml_timeslice);
        }
    }
}

#endif /* _BK_SCHED_DEBUG */

__BK_END_DECLS

#endif /* !_BIBLE_BITMAP_H_ */
