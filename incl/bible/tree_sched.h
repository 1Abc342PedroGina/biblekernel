#ifndef _BIBLE_TREE_SCHED_H_
#define _BIBLE_TREE_SCHED_H_



/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2024 Bible System Developers
 *
 * tree_sched.h - Escalonador Kernel baseado em Árvore Rubro-Negra
 * 
 * FILOSOFIA MATEMÁTICA:
 * =====================
 * 
 * VEi  : Virtual Eligibility Time - tempo que a tarefa se torna elegível
 * ri   : Request - fatia de tempo solicitada
 * wi   : Weight - peso (prioridade/nice)
 * S    : Tempo de execução virtual médio do sistema
 * vi   : Tempo virtual da entidade
 * τ    : Constante de tempo do "fogo"
 * δ    : Função delta de Dirac - "só importa o instante presente"
 * 
 * FÓRMULAS FUNDAMENTAIS:
 * =====================
 * 
 * 1. Prazo Virtual:    VDi = Vei + (ri/wi)
 * 2. Lag Ice:          LagIcei = S - vi
 * 3. Lag:              Lagi = LagIcei · e^(|LagIcei|/τ)
 * 4. Virtual Time Lag: VTi = VDi + Lagi/max(ϵ, VDi-S)
 * 5. Lag Sofredor:     Λi(t) = Lagi(t) · (1 + γ · ∫|Lagi|·e^(-(t-τ)/θ) dτ)
 * 6. Misericórdia:     Mi = ∫ (Lagi·e^(-(S-vi)/λ)·Φ(Lagi,t)) dt
 * 7. Dívida:           Di(t) = ∫ Lagi·e^(-(t-τ)/μ) dτ + ω·∫|Lagi| dτ
 * 8. Justiça:          Ji = lim_{t→∞} (vi(t) - S(t)) = 0
 * 9. Virtual Error:    VEri = (VDi - Mi)/(1 + |VTi|)
 * 10. Virtual Fire:    VFi = (VDi · e^(-Mi/κ)) / (1 + |VTi*|^α)
 * 11. Custo:           Custi = (VDi* - Mi)/(1+|VTi|) + κ·|Di|·(1 - e^(-Mi/δ))
 * 12. Real Fire:       RFi = lim_{κ→0} lim_{α→∞} (VDi·e^(-Mi/κ))/(1+|VTi|^α)
 */

#include <bible/printl.h>
#include <bible/cdefs.h>
#include <bible/types.h>
#include <bible/ktypes.h>
#include <bible/queue.h>
#include <bible/task.h>
#include <bible/regsister.h>
#include <bible/vm.h>
#include <bible/ipc.h>
#include <bible/pmap.h>


/* ============================================================================
 * CONSTANTES DO ESCALONADOR KERNEL
 * ============================================================================ */

/* Constantes matemáticas em formato de ponto fixo (Q16.16) */
#define BK_SCHED_TAU             0x100000ULL     /* τ = 1.0 em Q16.16 */
#define BK_SCHED_EPSILON          0x000001ULL    /* ϵ mínimo */
#define BK_SCHED_GAMMA            0x080000ULL    /* γ = 0.5 em Q16.16 */
#define BK_SCHED_THETA           0x7D00000ULL    /* θ = 500.0 em Q16.16 */
#define BK_SCHED_LAMBDA           0x01999AULL    /* λ = 0.1 em Q16.16 */
#define BK_SCHED_ETA              0x0028F6ULL    /* η = 0.01 em Q16.16 */
#define BK_SCHED_MU              0x6400000ULL    /* μ = 100.0 em Q16.16 */
#define BK_SCHED_OMEGA            0x01999AULL    /* ω = 0.1 em Q16.16 */
#define BK_SCHED_KAPPA            0x100000ULL    /* κ = 1.0 em Q16.16 */
#define BK_SCHED_ALPHA            0x200000ULL    /* α = 2.0 em Q16.16 */
#define BK_SCHED_DELTA            0x080000ULL    /* δ = 0.5 em Q16.16 */

/* Pesos padrão (nice values convertidos para Q16.16) */
#define BK_SCHED_WEIGHT_IDLE      0x01999AULL    /* 0.1 */
#define BK_SCHED_WEIGHT_LOW       0x080000ULL    /* 0.5 */
#define BK_SCHED_WEIGHT_NORMAL    0x100000ULL    /* 1.0 */
#define BK_SCHED_WEIGHT_HIGH      0x200000ULL    /* 2.0 */
#define BK_SCHED_WEIGHT_REALTIME  0x400000ULL    /* 4.0 */

/* Quantum padrão em nanossegundos */
#define BK_SCHED_QUANTUM_NS       10000000ULL    /* 10ms */

/* Tamanho máximo do histórico */
#define BK_SCHED_HISTORY_MAX      64

/* Cores da árvore rubro-negra */
typedef enum {
    BK_RB_BLACK = 0,
    BK_RB_RED   = 1
} BK_RB_COLOR;

/* Tipo de ponto fixo Q16.16 para cálculos matemáticos */
typedef BK_UINT32 bk_fixed_t;

/* ============================================================================
 * FUNÇÕES AUXILIARES DE MATEMÁTICA EM PONTO FIXO
 * ============================================================================ */

/* Converte inteiro para fixed point */
static __BK_ALWAYS_INLINE bk_fixed_t
fixed_from_int(BK_I32 x)
{
    return (bk_fixed_t)x << 16;
}

/* Converte fixed point para inteiro (trunca) */
static __BK_ALWAYS_INLINE BK_I32
fixed_to_int(bk_fixed_t x)
{
    return (BK_I32)(x >> 16);
}

/* Multiplicação de fixed point */
static __BK_ALWAYS_INLINE bk_fixed_t
fixed_mul(bk_fixed_t a, bk_fixed_t b)
{
    return (bk_fixed_t)(((BK_UINT64)a * b) >> 16);
}

/* Divisão de fixed point */
static __BK_ALWAYS_INLINE bk_fixed_t
fixed_div(bk_fixed_t a, bk_fixed_t b)
{
    if (b == 0) return 0;
    return (bk_fixed_t)(((BK_UINT64)a << 16) / b);
}

/* Valor absoluto em fixed point */
static __BK_ALWAYS_INLINE bk_fixed_t
fixed_abs(bk_fixed_t x)
{
    return (x & 0x80000000) ? 0 - x : x;
}

/* Máximo entre dois fixed points */
static __BK_ALWAYS_INLINE bk_fixed_t
fixed_max(bk_fixed_t a, bk_fixed_t b)
{
    return (a > b) ? a : b;
}

/* Exponencial aproximada para fixed point (série de Taylor) */
static __BK_ALWAYS_INLINE bk_fixed_t
fixed_exp(bk_fixed_t x)
{
    /* e^x ≈ 1 + x + x²/2! + x³/3! + x⁴/4! */
    bk_fixed_t x2 = fixed_mul(x, x);
    bk_fixed_t x3 = fixed_mul(x2, x);
    bk_fixed_t x4 = fixed_mul(x3, x);
    
    bk_fixed_t result = fixed_from_int(1) + x;
    result += fixed_div(x2, fixed_from_int(2));
    result += fixed_div(x3, fixed_from_int(6));
    result += fixed_div(x4, fixed_from_int(24));
    
    return result;
}

/* ============================================================================
 * HISTÓRICO PARA CÁLCULOS INTEGRAIS
 * ============================================================================ */

/* Entrada de histórico de Lag */
struct bk_sched_lag_entry {
    BK_TIME          l_time;                    /* Timestamp */
    bk_fixed_t       l_value;                    /* Valor do Lag */
    BK_LIST_ENTRY(bk_sched_lag_entry) l_link;
};

/* Entrada de histórico de dívida */
struct bk_sched_debt_entry {
    BK_TIME          d_time;                    /* Timestamp */
    bk_fixed_t       d_value;                    /* Valor da dívida */
    BK_LIST_ENTRY(bk_sched_debt_entry) d_link;
};

/* Estatísticas de justiça */
struct bk_sched_justice {
    bk_fixed_t       j_last_vi;                  /* Último vi */
    bk_fixed_t       j_last_s;                    /* Último S */
    bk_fixed_t       j_error_accum;               /* Erro acumulado */
    BK_UINT64        j_samples;                   /* Número de amostras */
    BK_UINT64        j_overhead;                  /* Overhead de justiça */
};

/* ============================================================================
 * ESTRUTURA PRINCIPAL DE ESCALONAMENTO (KERNEL MODE)
 * ============================================================================ */

/* Nó da árvore rubro-negra */
struct bk_rb_sched_node {
    /* Árvore rubro-negra */
    BK_RB_COLOR                 n_color;
    struct bk_rb_sched_node    *n_parent;
    struct bk_rb_sched_node    *n_left;
    struct bk_rb_sched_node    *n_right;
    
    /* Identificação (registro) */
    BK_REGISTER_HANDLE          n_entity;        /* Handle da entidade */
    BK_REGISTER_TYPE            n_type;          /* Tipo (process/task/thread) */
    
    /* ========================================================================
     * PARÂMETROS BASE (configuráveis)
     * ======================================================================== */
    
    BK_TIME                      n_ve;            /* VEi: Virtual Eligibility Time */
    BK_TIME                      n_request;       /* ri: Request (time slice) */
    bk_fixed_t                   n_weight;        /* wi: Weight (prioridade) */
    
    /* ========================================================================
     * VARIÁVEIS DINÂMICAS (calculadas)
     * ======================================================================== */
    
    BK_TIME                      n_virtual_time;  /* vi: Tempo virtual */
    bk_fixed_t                   n_lag_ice;       /* LagIcei: Lag base */
    bk_fixed_t                   n_lag;           /* Lagi: Lag com exponencial */
    bk_fixed_t                   n_vtl;           /* VTi: Virtual Time Lag */
    
    /* ========================================================================
     * FÓRMULAS AVANÇADAS
     * ======================================================================== */
    
    bk_fixed_t                   n_suffering_lag; /* Λi: Lag Sofredor */
    bk_fixed_t                   n_mercy;         /* Mi: Misericórdia */
    bk_fixed_t                   n_debt;          /* Di: Dívida */
    bk_fixed_t                   n_virtual_error; /* VEri: Virtual Error */
    bk_fixed_t                   n_virtual_fire;  /* VFi: Virtual Fire */
    bk_fixed_t                   n_cost;          /* Custi: Custo */
    bk_fixed_t                   n_real_fire;     /* RFi: Real Fire (aprox) */
    
    /* ========================================================================
     * PRAZOS
     * ======================================================================== */
    
    BK_TIME                      n_vd;            /* VDi: Virtual Deadline */
    BK_TIME                      n_vd_star;       /* VDi*: Virtual Deadline ajustado */
    
    /* ========================================================================
     * HISTÓRICOS
     * ======================================================================== */
    
    BK_LIST_HEAD(, bk_sched_lag_entry) n_lag_history;   /* Histórico de Lag */
    BK_LIST_HEAD(, bk_sched_debt_entry) n_debt_history; /* Histórico de dívida */
    BK_UINT32                    n_history_count;     /* Contador do histórico */
    
    /* ========================================================================
     * ESTATÍSTICAS
     * ======================================================================== */
    
    struct bk_sched_justice      n_justice;        /* Estatísticas de justiça */
    BK_UINT64                    n_executions;     /* Número de execuções */
    BK_TIME                      n_total_time;     /* Tempo total executado */
    BK_TIME                      n_last_start;     /* Último início */
    BK_TIME                      n_last_finish;    /* Último término */
    BK_TIME                      n_last_update;    /* Última atualização */
    
    /* ========================================================================
     * FLAGS
     * ======================================================================== */
    
    BK_UINT32                    n_flags;
#define BK_SCHED_FLAG_ACTIVE        0x00000001  /* Ativo no escalonador */
#define BK_SCHED_FLAG_IDLE          0x00000002  /* Entidade ociosa */
#define BK_SCHED_FLAG_REALTIME      0x00000004  /* Tempo real */
#define BK_SCHED_FLAG_SYSTEM        0x00000008  /* Entidade do sistema */
#define BK_SCHED_FLAG_USER          0x00000010  /* Entidade do usuário */
#define BK_SCHED_FLAG_KERNEL        0x00000020  /* Thread kernel */
#define BK_SCHED_FLAG_MERCY_APPLIED 0x00000040  /* Misericórdia aplicada */
#define BK_SCHED_FLAG_FIRE_TRIGGERED 0x00000080 /* Fogo disparado */
};

typedef struct bk_rb_sched_node BK_RB_SCHED_NODE;

/* ============================================================================
 * ESTRUTURA DA ÁRVORE RUBRO-NEGRA GLOBAL
 * ============================================================================ */

struct bk_rb_sched_tree {
    BK_RB_SCHED_NODE    *t_root;            /* Raiz da árvore */
    BK_RB_SCHED_NODE    *t_nil;              /* Nó sentinela NIL */
    BK_UINT32            t_count;            /* Número de nós ativos */
    BK_SPINLOCK          t_lock;             /* Lock da árvore (do regsister) */
    
    /* Estatísticas globais (S) */
    bk_fixed_t           t_system_time;      /* S: Tempo virtual médio do sistema */
    bk_fixed_t           t_total_weight;     /* Soma total dos pesos */
    BK_TIME              t_total_runtime;    /* Tempo total de execução */
    BK_UINT64            t_total_execs;      /* Total de execuções */
    
    /* Parâmetros globais (configuráveis via sysctl) */
    bk_fixed_t           t_tau;               /* τ: constante do fogo */
    bk_fixed_t           t_gamma;             /* γ: fator de consequência */
    bk_fixed_t           t_theta;             /* θ: memória do sofrimento */
    bk_fixed_t           t_lambda;            /* λ: decaimento da misericórdia */
    bk_fixed_t           t_eta;               /* η: acumulação do sofrimento */
    bk_fixed_t           t_mu;                /* μ: constante da dívida */
    bk_fixed_t           t_omega;             /* ω: peso da dívida acumulada */
    bk_fixed_t           t_kappa;             /* κ: constante de misericórdia */
    bk_fixed_t           t_alpha;             /* α: expoente do Virtual Fire */
    bk_fixed_t           t_delta;             /* δ: constante de misericórdia */
    bk_fixed_t           t_epsilon;           /* ϵ: mínimo para divisão */
    
    /* Última atualização */
    BK_TIME              t_last_update;       /* Timestamp da última atualização */
    
    /* Estatísticas de debug */
#ifdef _BK_SCHED_DEBUG
    BK_UINT64            t_insertions;
    BK_UINT64            t_deletions;
    BK_UINT64            t_rotations;
    BK_UINT64            t_lookups;
#endif
};

typedef struct bk_rb_sched_tree BK_RB_SCHED_TREE;

/* ============================================================================
 * FUNÇÕES DE CÁLCULO MATEMÁTICO (KERNEL MODE)
 * ============================================================================ */

/**
 * Calcula o Prazo Virtual: VDi = Vei + (ri/wi)
 */
static __BK_ALWAYS_INLINE BK_TIME
sched_calc_vd(BK_TIME ve, BK_TIME request, bk_fixed_t weight)
{
    bk_fixed_t request_fixed = fixed_from_int((BK_I32)request);
    bk_fixed_t ri_over_wi = fixed_div(request_fixed, weight);
    return ve + fixed_to_int(ri_over_wi);
}

/**
 * Calcula o Lag Ice: LagIcei = S - vi
 */
static __BK_ALWAYS_INLINE bk_fixed_t
sched_calc_lag_ice(bk_fixed_t S, bk_fixed_t vi)
{
    return S - vi;
}

/**
 * Calcula o Lag: Lagi = LagIcei * e^(|LagIcei|/τ)
 */
static __BK_ALWAYS_INLINE bk_fixed_t
sched_calc_lag(bk_fixed_t lag_ice, bk_fixed_t tau)
{
    bk_fixed_t abs_lag = fixed_abs(lag_ice);
    bk_fixed_t exponent = fixed_div(abs_lag, tau);
    bk_fixed_t exp_val = fixed_exp(exponent);
    return fixed_mul(lag_ice, exp_val);
}

/**
 * Calcula o Virtual Time Lag: VTi = VDi + Lagi/max(ϵ, VDi-S)
 */
static __BK_ALWAYS_INLINE bk_fixed_t
sched_calc_vtl(BK_TIME vd, bk_fixed_t lag, bk_fixed_t S, bk_fixed_t epsilon)
{
    bk_fixed_t vd_fixed = fixed_from_int((BK_I32)vd);
    bk_fixed_t denominator = fixed_max(epsilon, vd_fixed - S);
    bk_fixed_t term = fixed_div(lag, denominator);
    return vd_fixed + term;
}

/**
 * Calcula o Virtual Error: VEri = (VDi - Mi)/(1 + |VTi|)
 */
static __BK_ALWAYS_INLINE bk_fixed_t
sched_calc_ver(BK_TIME vd, bk_fixed_t mercy, bk_fixed_t vtl)
{
    bk_fixed_t vd_fixed = fixed_from_int((BK_I32)vd);
    bk_fixed_t numerator = vd_fixed - mercy;
    bk_fixed_t denominator = fixed_from_int(1) + fixed_abs(vtl);
    return fixed_div(numerator, denominator);
}

/**
 * Calcula o Lag Sofredor: Λi(t) = Lagi(t) · (1 + γ·∫|Lagi|·e^(-(t-τ)/θ) dτ)
 */
bk_fixed_t
sched_calc_suffering_lag(BK_RB_SCHED_NODE *node, BK_RB_SCHED_TREE *tree, BK_TIME now)
{
    bk_fixed_t integral = 0;
    bk_fixed_t gamma = tree->t_gamma;
    bk_fixed_t theta = tree->t_theta;
    BK_TIME last_time = node->n_last_update;
    struct bk_sched_lag_entry *entry;
    
    /* Itera sobre o histórico de Lag */
    BK_LIST_FOREACH(entry, &node->n_lag_history, l_link) {
        BK_TIME delta_t = now - entry->l_time;
        if (delta_t > 0) {
            /* e^(-(t-τ)/θ) */
            bk_fixed_t exponent = fixed_div(fixed_from_int((BK_I32)delta_t), theta);
            exponent = 0 - exponent;  /* negativo */
            bk_fixed_t decay = fixed_exp(exponent);
            
            /* |Lagi| * decay */
            bk_fixed_t term = fixed_mul(fixed_abs(entry->l_value), decay);
            integral += term;
        }
    }
    
    /* (1 + γ * integral) */
    bk_fixed_t factor = fixed_from_int(1) + fixed_mul(gamma, integral);
    
    /* Lagi(t) * factor */
    return fixed_mul(node->n_lag, factor);
}

/**
 * Calcula a Misericórdia: Mi = ∫ (Lagi·e^(-(S-vi)/λ)·Φ(Lagi,t)) dt
 * Φ(Lagi,t) = e^(-η·∫|Lagi| dτ)
 */
bk_fixed_t
sched_calc_mercy(BK_RB_SCHED_NODE *node, BK_RB_SCHED_TREE *tree, BK_TIME now)
{
    bk_fixed_t mercy = 0;
    bk_fixed_t lambda = tree->t_lambda;
    bk_fixed_t eta = tree->t_eta;
    bk_fixed_t S = tree->t_system_time;
    bk_fixed_t vi = node->n_virtual_time;
    BK_TIME last_time = node->n_last_update;
    struct bk_sched_lag_entry *entry;
    
    /* Acumulador de |Lagi| para Φ */
    bk_fixed_t abs_integral = 0;
    
    /* Primeiro calcula ∫|Lagi| dτ para Φ */
    BK_LIST_FOREACH(entry, &node->n_lag_history, l_link) {
        BK_TIME delta_t = (entry->l_link.le_next) ? 
            (BK_LIST_NEXT(entry, l_link))->l_time - entry->l_time : 
            now - entry->l_time;
        
        if (delta_t > 0) {
            abs_integral += fixed_mul(fixed_abs(entry->l_value), 
                                     fixed_from_int((BK_I32)delta_t));
        }
    }
    
    /* Φ = e^(-η·∫|Lagi| dτ) */
    bk_fixed_t phi = fixed_exp(fixed_mul(0 - eta, abs_integral));
    
    /* Calcula a integral da misericórdia */
    BK_LIST_FOREACH(entry, &node->n_lag_history, l_link) {
        BK_TIME delta_t = (entry->l_link.le_next) ? 
            (BK_LIST_NEXT(entry, l_link))->l_time - entry->l_time : 
            now - entry->l_time;
        
        if (delta_t > 0) {
            /* e^(-(S-vi)/λ) */
            bk_fixed_t diff = S - vi;
            bk_fixed_t exponent = fixed_div(0 - diff, lambda);  /* negativo se diff positivo */
            bk_fixed_t decay = fixed_exp(exponent);
            
            /* Lagi * decay * phi * dt */
            bk_fixed_t term = fixed_mul(entry->l_value, decay);
            term = fixed_mul(term, phi);
            term = fixed_mul(term, fixed_from_int((BK_I32)delta_t));
            
            mercy += term;
        }
    }
    
    return mercy;
}

/**
 * Calcula a Dívida: Di(t) = ∫ Lagi·e^(-(t-τ)/μ) dτ + ω·∫|Lagi| dτ
 */
bk_fixed_t
sched_calc_debt(BK_RB_SCHED_NODE *node, BK_RB_SCHED_TREE *tree, BK_TIME now)
{
    bk_fixed_t integral1 = 0;  /* ∫ Lagi·e^(-(t-τ)/μ) dτ */
    bk_fixed_t integral2 = 0;  /* ∫ |Lagi| dτ */
    bk_fixed_t mu = tree->t_mu;
    bk_fixed_t omega = tree->t_omega;
    struct bk_sched_lag_entry *entry;
    
    BK_LIST_FOREACH(entry, &node->n_lag_history, l_link) {
        BK_TIME delta_t = now - entry->l_time;
        if (delta_t > 0) {
            /* e^(-(t-τ)/μ) */
            bk_fixed_t exponent = fixed_div(fixed_from_int((BK_I32)delta_t), mu);
            exponent = 0 - exponent;
            bk_fixed_t decay = fixed_exp(exponent);
            
            /* Primeira integral: Lagi * decay * dt */
            integral1 += fixed_mul(fixed_mul(entry->l_value, decay),
                                  fixed_from_int((BK_I32)delta_t));
            
            /* Segunda integral: |Lagi| * dt */
            integral2 += fixed_mul(fixed_abs(entry->l_value),
                                  fixed_from_int((BK_I32)delta_t));
        }
    }
    
    return integral1 + fixed_mul(omega, integral2);
}

/**
 * Calcula o Virtual Fire: VFi = (VDi · e^(-Mi/κ)) / (1 + |VTi*|^α)
 */
static __BK_ALWAYS_INLINE bk_fixed_t
sched_calc_vf(BK_TIME vd, bk_fixed_t mercy, bk_fixed_t vtl_star, 
              bk_fixed_t kappa, bk_fixed_t alpha)
{
    bk_fixed_t vd_fixed = fixed_from_int((BK_I32)vd);
    
    /* e^(-Mi/κ) */
    bk_fixed_t exponent = fixed_div(0 - mercy, kappa);
    bk_fixed_t exp_val = fixed_exp(exponent);
    
    /* |VTi*|^α (aproximação polinomial) */
    bk_fixed_t abs_vtl = fixed_abs(vtl_star);
    bk_fixed_t power = fixed_from_int(1);
    int i;
    for (i = 0; i < fixed_to_int(alpha); i++) {
        power = fixed_mul(power, abs_vtl);
    }
    
    bk_fixed_t numerator = fixed_mul(vd_fixed, exp_val);
    bk_fixed_t denominator = fixed_from_int(1) + power;
    
    return fixed_div(numerator, denominator);
}

/**
 * Calcula o Custo: Custi = (VDi* - Mi)/(1 + |VTi|) + κ·|Di|·(1 - e^(-Mi/δ))
 */
static __BK_ALWAYS_INLINE bk_fixed_t
sched_calc_cost(BK_TIME vd_star, bk_fixed_t mercy, bk_fixed_t vtl, 
                bk_fixed_t debt, bk_fixed_t kappa, bk_fixed_t delta)
{
    bk_fixed_t vd_fixed = fixed_from_int((BK_I32)vd_star);
    
    /* Primeiro termo: (VDi* - Mi)/(1 + |VTi|) */
    bk_fixed_t term1_num = vd_fixed - mercy;
    bk_fixed_t term1_den = fixed_from_int(1) + fixed_abs(vtl);
    bk_fixed_t term1 = fixed_div(term1_num, term1_den);
    
    /* Segundo termo: κ·|Di|·(1 - e^(-Mi/δ)) */
    bk_fixed_t exponent = fixed_div(0 - mercy, delta);
    bk_fixed_t exp_val = fixed_exp(exponent);
    bk_fixed_t term2 = fixed_mul(kappa, fixed_abs(debt));
    term2 = fixed_mul(term2, fixed_from_int(1) - exp_val);
    
    return term1 + term2;
}

/**
 * Calcula o Real Fire: RFi (aproximação com κ pequeno e α grande)
 */
static __BK_ALWAYS_INLINE bk_fixed_t
sched_calc_rf(BK_TIME vd, bk_fixed_t mercy)
{
    /* Aproximação do limite: κ→0, α→∞ */
    if (mercy > 0) {
        return 0;  /* Com misericórdia positiva, RFi tende a 0 */
    } else {
        return fixed_from_int((BK_I32)vd);  /* Sem misericórdia, RFi ≈ VDi */
    }
}

/* ============================================================================
 * FUNÇÕES DA ÁRVORE RUBRO-NEGRA
 * ============================================================================ */

/* Inicializa a árvore */
void bk_sched_tree_init(BK_RB_SCHED_TREE *tree)
{
    /* Aloca nó NIL */
   tree->t_nil = (BK_RB_SCHED_NODE*)BK_VM_KERNEL_BASE + 
              bk_vm_alloc(NULL, sizeof(BK_RB_SCHED_NODE), 
                         BK_VM_PROT_READ | BK_VM_PROT_WRITE, 
                         BK_VM_MAP_ANON);
    if (!tree->t_nil) return;
    
    tree->t_nil->n_color = BK_RB_BLACK;
    tree->t_nil->n_left = tree->t_nil->n_right = tree->t_nil->n_parent = tree->t_nil;
    tree->t_root = tree->t_nil;
    tree->t_count = 0;
    
    /* Inicializa lock do regsister */
    tree->t_lock.s_lock = BK_SPIN_UNLOCKED;
    
    /* Inicializa parâmetros globais */
    tree->t_system_time = 0;
    tree->t_total_weight = 0;
    tree->t_total_runtime = 0;
    tree->t_total_execs = 0;
    
    /* Parâmetros matemáticos */
    tree->t_tau = BK_SCHED_TAU;
    tree->t_gamma = BK_SCHED_GAMMA;
    tree->t_theta = BK_SCHED_THETA;
    tree->t_lambda = BK_SCHED_LAMBDA;
    tree->t_eta = BK_SCHED_ETA;
    tree->t_mu = BK_SCHED_MU;
    tree->t_omega = BK_SCHED_OMEGA;
    tree->t_kappa = BK_SCHED_KAPPA;
    tree->t_alpha = BK_SCHED_ALPHA;
    tree->t_delta = BK_SCHED_DELTA;
    tree->t_epsilon = BK_SCHED_EPSILON;
    
    tree->t_last_update = 0;
    
#ifdef _BK_SCHED_DEBUG
    tree->t_insertions = 0;
    tree->t_deletions = 0;
    tree->t_rotations = 0;
    tree->t_lookups = 0;
#endif
}

/* Rotação à esquerda */
static void
rb_rotate_left(BK_RB_SCHED_TREE *tree, BK_RB_SCHED_NODE *x)
{
    BK_RB_SCHED_NODE *y = x->n_right;
    x->n_right = y->n_left;
    
    if (y->n_left != tree->t_nil)
        y->n_left->n_parent = x;
    
    y->n_parent = x->n_parent;
    
    if (x->n_parent == tree->t_nil)
        tree->t_root = y;
    else if (x == x->n_parent->n_left)
        x->n_parent->n_left = y;
    else
        x->n_parent->n_right = y;
    
    y->n_left = x;
    x->n_parent = y;
    
#ifdef _BK_SCHED_DEBUG
    tree->t_rotations++;
#endif
}

/* Rotação à direita */
static void
rb_rotate_right(BK_RB_SCHED_TREE *tree, BK_RB_SCHED_NODE *y)
{
    BK_RB_SCHED_NODE *x = y->n_left;
    y->n_left = x->n_right;
    
    if (x->n_right != tree->t_nil)
        x->n_right->n_parent = y;
    
    x->n_parent = y->n_parent;
    
    if (y->n_parent == tree->t_nil)
        tree->t_root = x;
    else if (y == y->n_parent->n_right)
        y->n_parent->n_right = x;
    else
        y->n_parent->n_left = x;
    
    x->n_right = y;
    y->n_parent = x;
    
#ifdef _BK_SCHED_DEBUG
    tree->t_rotations++;
#endif
}


static void rb_transplant(BK_RB_SCHED_TREE *tree, BK_RB_SCHED_NODE *u, BK_RB_SCHED_NODE *v);
static BK_RB_SCHED_NODE *rb_minimum(BK_RB_SCHED_TREE *tree, BK_RB_SCHED_NODE *node);
static void rb_delete_fixup(BK_RB_SCHED_TREE *tree, BK_RB_SCHED_NODE *x);

/* Inserção na árvore rubro-negra */
void
bk_sched_insert(BK_RB_SCHED_TREE *tree, BK_RB_SCHED_NODE *node)
{
    BK_RB_SCHED_NODE *y = tree->t_nil;
    BK_RB_SCHED_NODE *x = tree->t_root;
    
    /* Busca posição de inserção (ordenado por Virtual Fire) */
    while (x != tree->t_nil) {
        y = x;
        if (node->n_virtual_fire < x->n_virtual_fire)
            x = x->n_left;
        else
            x = x->n_right;
    }
    
    node->n_parent = y;
    
    if (y == tree->t_nil)
        tree->t_root = node;
    else if (node->n_virtual_fire < y->n_virtual_fire)
        y->n_left = node;
    else
        y->n_right = node;
    
    node->n_left = tree->t_nil;
    node->n_right = tree->t_nil;
    node->n_color = BK_RB_RED;
    
    /* Rebalanceamento */
    while (node->n_parent->n_color == BK_RB_RED) {
        if (node->n_parent == node->n_parent->n_parent->n_left) {
            y = node->n_parent->n_parent->n_right;
            if (y->n_color == BK_RB_RED) {
                node->n_parent->n_color = BK_RB_BLACK;
                y->n_color = BK_RB_BLACK;
                node->n_parent->n_parent->n_color = BK_RB_RED;
                node = node->n_parent->n_parent;
            } else {
                if (node == node->n_parent->n_right) {
                    node = node->n_parent;
                    rb_rotate_left(tree, node);
                }
                node->n_parent->n_color = BK_RB_BLACK;
                node->n_parent->n_parent->n_color = BK_RB_RED;
                rb_rotate_right(tree, node->n_parent->n_parent);
            }
        } else {
            y = node->n_parent->n_parent->n_left;
            if (y->n_color == BK_RB_RED) {
                node->n_parent->n_color = BK_RB_BLACK;
                y->n_color = BK_RB_BLACK;
                node->n_parent->n_parent->n_color = BK_RB_RED;
                node = node->n_parent->n_parent;
            } else {
                if (node == node->n_parent->n_left) {
                    node = node->n_parent;
                    rb_rotate_right(tree, node);
                }
                node->n_parent->n_color = BK_RB_BLACK;
                node->n_parent->n_parent->n_color = BK_RB_RED;
                rb_rotate_left(tree, node->n_parent->n_parent);
            }
        }
    }
    
    tree->t_root->n_color = BK_RB_BLACK;
    tree->t_count++;
    
#ifdef _BK_SCHED_DEBUG
    tree->t_insertions++;
#endif
}

/* Remove nó da árvore */
void
bk_sched_remove(BK_RB_SCHED_TREE *tree, BK_RB_SCHED_NODE *node)
{
    BK_RB_SCHED_NODE *y = node;
    BK_RB_SCHED_NODE *x;
    BK_RB_COLOR y_original_color = y->n_color;
    
    if (node->n_left == tree->t_nil) {
        x = node->n_right;
        rb_transplant(tree, node, node->n_right);
    } else if (node->n_right == tree->t_nil) {
        x = node->n_left;
        rb_transplant(tree, node, node->n_left);
    } else {
        y = rb_minimum(tree, node->n_right);
        y_original_color = y->n_color;
        x = y->n_right;
        
        if (y->n_parent == node) {
            x->n_parent = y;
        } else {
            rb_transplant(tree, y, y->n_right);
            y->n_right = node->n_right;
            y->n_right->n_parent = y;
        }
        
        rb_transplant(tree, node, y);
        y->n_left = node->n_left;
        y->n_left->n_parent = y;
        y->n_color = node->n_color;
    }
    
    if (y_original_color == BK_RB_BLACK) {
        rb_delete_fixup(tree, x);
    }
    
    tree->t_count--;
    
#ifdef _BK_SCHED_DEBUG
    tree->t_deletions++;
#endif
}

/* ============================================================================
 * FUNÇÕES PRINCIPAIS DO ESCALONADOR (KERNEL API)
 * ============================================================================ */


static __BK_ALWAYS_INLINE void
sched_spin_lock(BK_SPINLOCK *lock)
{
    /* Implementação simples de spinlock usando o campo s_lock */
    while (__sync_lock_test_and_set(&lock->s_lock, 1)) {
        /* spin */;
    }
}

static __BK_ALWAYS_INLINE void
sched_spin_unlock(BK_SPINLOCK *lock)
{
    __sync_lock_release(&lock->s_lock);
}

/*
 * Cria um novo nó de escalonamento para uma entidade
 */
BK_RB_SCHED_NODE *
bk_sched_node_create(BK_REGISTER_HANDLE entity, BK_REGISTER_TYPE type)
{
    BK_RB_SCHED_NODE *node;
    
    /* Aloca do heap do kernel */
    node = (BK_RB_SCHED_NODE*)BK_VM_KERNEL_BASE + 
       bk_vm_alloc(NULL, sizeof(BK_RB_SCHED_NODE), 
                  BK_VM_PROT_READ | BK_VM_PROT_WRITE, 
                  BK_VM_MAP_ANON);
    if (!node)
        return NULL;
    
    /* Inicializa campos básicos */
    node->n_entity = entity;
    node->n_type = type;
    
    /* Parâmetros base (valores padrão) */
    node->n_ve = 0;
    node->n_request = BK_SCHED_QUANTUM_NS;
    node->n_weight = BK_SCHED_WEIGHT_NORMAL;
    
    /* Variáveis dinâmicas */
    node->n_virtual_time = 0;
    node->n_lag_ice = 0;
    node->n_lag = 0;
    node->n_vtl = 0;
    
    /* Fórmulas avançadas */
    node->n_suffering_lag = 0;
    node->n_mercy = 0;
    node->n_debt = 0;
    node->n_virtual_error = 0;
    node->n_virtual_fire = 0;
    node->n_cost = 0;
    node->n_real_fire = 0;
    
    /* Prazos */
    node->n_vd = 0;
    node->n_vd_star = 0;
    
    /* Históricos */
    BK_LIST_INIT(&node->n_lag_history);
    BK_LIST_INIT(&node->n_debt_history);
    node->n_history_count = 0;
    
    /* Estatísticas */
    node->n_justice.j_last_vi = 0;
    node->n_justice.j_last_s = 0;
    node->n_justice.j_error_accum = 0;
    node->n_justice.j_samples = 0;
    node->n_justice.j_overhead = 0;
    
    node->n_executions = 0;
    node->n_total_time = 0;
    node->n_last_start = 0;
    node->n_last_finish = 0;
    node->n_last_update = 0;
    
    /* Flags */
    node->n_flags = BK_SCHED_FLAG_ACTIVE;
    
    return node;
}

/**
 * Atualiza todos os parâmetros de um nó
 */
void
bk_sched_node_update(BK_RB_SCHED_TREE *tree, BK_RB_SCHED_NODE *node, BK_TIME now)
{
    sched_spin_lock(&tree->t_lock);
    
    /* Atualiza tempo virtual baseado no tempo executado */
    if (node->n_last_start && node->n_last_finish) {
        BK_TIME exec_time = node->n_last_finish - node->n_last_start;
        node->n_virtual_time += fixed_from_int((BK_I32)exec_time);
        node->n_total_time += exec_time;
    }
    
    /* Adiciona entrada no histórico de Lag */
    if (node->n_history_count < BK_SCHED_HISTORY_MAX) {
        struct bk_sched_lag_entry *entry;
        entry = (struct bk_sched_lag_entry*)BK_VM_KERNEL_BASE + 
        bk_vm_alloc(NULL, sizeof(struct bk_sched_lag_entry), 
                   BK_VM_PROT_READ | BK_VM_PROT_WRITE, 
                   BK_VM_MAP_ANON);
        if (entry) {
            entry->l_time = now;
            entry->l_value = node->n_lag;
            BK_LIST_INSERT_HEAD(&node->n_lag_history, entry, l_link);
            node->n_history_count++;
        }
    }
    
    /* Calcula Lag Ice */
    node->n_lag_ice = sched_calc_lag_ice(tree->t_system_time, node->n_virtual_time);
    
    /* Calcula Lag */
    node->n_lag = sched_calc_lag(node->n_lag_ice, tree->t_tau);
    
    /* Calcula Virtual Deadline */
    node->n_vd = sched_calc_vd(node->n_ve, node->n_request, node->n_weight);
    
    /* Calcula Virtual Time Lag */
    node->n_vtl = sched_calc_vtl(node->n_vd, node->n_lag, 
                                  tree->t_system_time, tree->t_epsilon);
    
    /* Calcula Lag Sofredor */
    node->n_suffering_lag = sched_calc_suffering_lag(node, tree, now);
    
    /* Calcula Misericórdia */
    node->n_mercy = sched_calc_mercy(node, tree, now);
    
    /* Calcula Dívida */
    node->n_debt = sched_calc_debt(node, tree, now);
    
    /* Ajusta VDi* (VDi ajustado pela misericórdia) */
    node->n_vd_star = node->n_vd - fixed_to_int(node->n_mercy);
    if (node->n_vd_star < 0) node->n_vd_star = 0;
    
    /* Calcula Virtual Error */
    node->n_virtual_error = sched_calc_ver(node->n_vd, node->n_mercy, node->n_vtl);
    
    /* Calcula Virtual Fire */
    node->n_virtual_fire = sched_calc_vf(node->n_vd, node->n_mercy, node->n_vtl,
                                         tree->t_kappa, tree->t_alpha);
    
    /* Calcula Custo */
    node->n_cost = sched_calc_cost(node->n_vd_star, node->n_mercy, node->n_vtl,
                                   node->n_debt, tree->t_kappa, tree->t_delta);
    
    /* Calcula Real Fire (aproximado) */
    node->n_real_fire = sched_calc_rf(node->n_vd, node->n_mercy);
    
    /* Atualiza estatísticas de justiça */
    node->n_justice.j_last_vi = node->n_virtual_time;
    node->n_justice.j_last_s = tree->t_system_time;
    node->n_justice.j_error_accum += (node->n_virtual_time - tree->t_system_time);
    node->n_justice.j_samples++;
    
    node->n_last_update = now;
    
    sched_spin_unlock(&tree->t_lock);
}

/**
 * Escolhe o próximo nó a executar (menor Virtual Fire)
 */
BK_RB_SCHED_NODE *
bk_sched_pick_next(BK_RB_SCHED_TREE *tree, BK_TIME now)
{
    BK_RB_SCHED_NODE *node = NULL;
    
    sched_spin_lock(&tree->t_lock);
    
    if (tree->t_root != tree->t_nil) {
        /* Atualiza tempo do sistema */
        tree->t_last_update = now;
        
        /* O nó mais à esquerda tem o menor Virtual Fire */
        node = tree->t_root;
        while (node->n_left != tree->t_nil)
            node = node->n_left;
        
        /* Atualiza o nó escolhido */
        if (node) {
            node->n_last_start = now;
            node->n_executions++;
            tree->t_total_execs++;
        }
    }
    
    sched_spin_unlock(&tree->t_lock);
    
    return node;
}

/**
 * Atualiza o tempo do sistema (S)
 */
void
bk_sched_update_system_time(BK_RB_SCHED_TREE *tree, BK_TIME now)
{
    sched_spin_lock(&tree->t_lock);
    
    /* S = (soma dos tempos virtuais) / (número de entidades) */
    if (tree->t_count > 0) {
        /* Cálculo aproximado - em produção seria uma média ponderada real */
        tree->t_system_time += fixed_div(fixed_from_int(1), 
                                         fixed_from_int((BK_I32)tree->t_count));
    }
    
    sched_spin_unlock(&tree->t_lock);
}

/**
 * Marca o fim da execução de um nó
 */
void
bk_sched_finish_node(BK_RB_SCHED_TREE *tree, BK_RB_SCHED_NODE *node, BK_TIME now)
{
    sched_spin_lock(&tree->t_lock);
    
    node->n_last_finish = now;
    
    /* Atualiza o nó */
    bk_sched_node_update(tree, node, now);
    
    /* Reinsere na árvore (pode ter mudado de posição) */
    bk_sched_remove(tree, node);
    bk_sched_insert(tree, node);
    
    sched_spin_unlock(&tree->t_lock);
}

/**
 * Ajusta o peso (nice) de um nó
 */
void
bk_sched_set_weight(BK_RB_SCHED_TREE *tree, BK_RB_SCHED_NODE *node, bk_fixed_t weight)
{
    sched_spin_lock(&tree->t_lock);
    node->n_weight = weight;
    sched_spin_unlock(&tree->t_lock);
}

/**
 * Ajusta o request (quantum) de um nó
 */
void
bk_sched_set_request(BK_RB_SCHED_TREE *tree, BK_RB_SCHED_NODE *node, BK_TIME request)
{
    sched_spin_lock(&tree->t_lock);
    node->n_request = request;
    sched_spin_unlock(&tree->t_lock);
}

/**
 * Obtém estatísticas de justiça
 */
struct bk_sched_justice
bk_sched_get_justice(BK_RB_SCHED_TREE *tree, BK_RB_SCHED_NODE *node)
{
    struct bk_sched_justice justice;
    
    sched_spin_lock(&tree->t_lock);
    justice = node->n_justice;
    sched_spin_unlock(&tree->t_lock);
    
    return justice;
}

/**
 * Verifica se a justiça foi alcançada (vi - S ≈ 0)
 */
BK_BOOL
bk_sched_is_just(BK_RB_SCHED_TREE *tree, BK_RB_SCHED_NODE *node)
{
    bk_fixed_t error;
    BK_BOOL result;
    
    sched_spin_lock(&tree->t_lock);
    error = node->n_virtual_time - tree->t_system_time;
    result = (fixed_abs(error) < tree->t_epsilon) ? BK_TRUE : BK_FALSE;
    sched_spin_unlock(&tree->t_lock);
    
    return result;
}

/* ============================================================================
 * FUNÇÕES DE DEBUG (apenas se _BK_SCHED_DEBUG definido)
 * ============================================================================ */

#ifdef _BK_SCHED_DEBUG

void
bk_sched_dump_node(BK_RB_SCHED_NODE *node)
{
    printl("Nó [%p]:\n", node);
    printl("  VE: %llu, Request: %llu, Weight: 0x%x\n", 
              node->n_ve, node->n_request, node->n_weight);
    printl("  vi: 0x%x, LagIce: 0x%x, Lag: 0x%x\n",
              node->n_virtual_time, node->n_lag_ice, node->n_lag);
    printl("  VDi: %llu, VTi: 0x%x, Mi: 0x%x\n",
              node->n_vd, node->n_vtl, node->n_mercy);
    printl("  VFi: 0x%x, RFi: 0x%x, Custi: 0x%x\n",
              node->n_virtual_fire, node->n_real_fire, node->n_cost);
    printl("  Execuções: %llu, Tempo total: %llu\n",
              node->n_executions, node->n_total_time);
}

void
bk_sched_dump_tree(BK_RB_SCHED_TREE *tree)
{
   printl("Árvore Rubro-Negra [%p]:\n", tree);
    printl("  Nós ativos: %u\n", tree->t_count);
    printl("  S (tempo sistema): 0x%x\n", tree->t_system_time);
    printl("  Total execuções: %llu\n", tree->t_total_execs);
    printl("  Inserções: %llu, Remoções: %llu, Rotações: %llu, Lookups: %llu\n",
              tree->t_insertions, tree->t_deletions, 
              tree->t_rotations, tree->t_lookups);
}

#endif /* _BK_SCHED_DEBUG */

__BK_END_DECLS

#endif /* !_BIBLE_TREE_SCHED_H_ */
