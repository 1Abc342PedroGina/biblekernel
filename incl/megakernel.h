/* megakernel.h - Núcleo Meta-Gerenciável em C puro */
#ifndef MEGAKERNEL_H
#define MEGAKERNEL_H

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
   1. SISTEMA META-GERENCIÁVEL
   ============================================================================ */

/* Tipos de hardware suportados */
typedef enum {
    HW_CPU = 0,
    HW_GPU,
    HW_TPU,
    HW_NPU,
    HW_FPGA,
    HW_MAX_TYPES
} hardware_type_t;

/* Estados do kernel */
typedef enum {
    KS_IDLE = 0,
    KS_ACTIVE,
    KS_RECONFIGURING,
    KS_ERROR,
    KS_SLEEPING
} kernel_state_t;

/* Estrutura para métricas de desempenho */
typedef struct {
    double utilization;
    double power_consumption;
    uint64_t instructions_per_sec;
    uint64_t memory_usage;
    double temperature;
    uint64_t timestamp;
} perf_metrics_t;

/* Estrutura do sub-kernel especializado */
typedef struct subkernel {
    uint32_t id;
    hardware_type_t hw_type;
    kernel_state_t state;
    void* (*execute)(void* data);
    void* (*reconfigure)(struct subkernel* self, perf_metrics_t metrics);
    perf_metrics_t metrics;
    void* hardware_context;
    struct subkernel* next;
} subkernel_t;

/* Estrutura principal do Meta-Kernel */
typedef struct {
    uint32_t kernel_id;
    kernel_state_t state;
    subkernel_t* subkernels[HW_MAX_TYPES];
    uint32_t num_subkernels;
    
    /* Funções de gerenciamento */
    bool (*add_subkernel)(struct megakernel* mk, subkernel_t* sk);
    bool (*remove_subkernel)(struct megakernel* mk, uint32_t sk_id);
    void (*reconfigure_dynamic)(struct megakernel* mk, perf_metrics_t* sys_metrics);
    void (*balance_load)(struct megakernel* mk);
    
    /* Contadores e métricas */
    perf_metrics_t system_metrics;
    uint64_t total_operations;
    uint32_t reconfig_count;
} megakernel_t;

/* ============================================================================
   2. PARALELISMO HIERÁRQUICO
   ============================================================================ */

/* Níveis de granularidade */
typedef enum {
    GRAN_FINE = 0,      /* Thread/Task nível */
    GRAN_MEDIUM,        /* Bloco/Processo nível */
    GRAN_COARSE,        /* Aplicação/Subsistema nível */
    GRAN_SYSTEM         /* Sistema completo */
} granularity_level_t;

/* Estrutura de tarefa */
typedef struct {
    uint64_t task_id;
    granularity_level_t granularity;
    hardware_type_t preferred_hw;
    void* (*task_function)(void*);
    void* task_data;
    uint32_t priority;
    uint64_t deadline_us;
} mega_task_t;

/* Estrutura do escalonador hierárquico */
typedef struct {
    mega_task_t** task_queues[HW_MAX_TYPES];
    uint32_t queue_sizes[HW_MAX_TYPES];
    uint32_t queue_capacities[HW_MAX_TYPES];
    
    /* Políticas de escalonamento */
    bool (*schedule_task)(struct scheduler* sched, mega_task_t* task);
    mega_task_t* (*get_next_task)(struct scheduler* sched, hardware_type_t hw_type);
    void (*migrate_task)(mega_task_t* task, hardware_type_t from, hardware_type_t to);
    
    /* Estatísticas */
    uint64_t tasks_completed[HW_MAX_TYPES];
    uint64_t tasks_migrated;
} hierarchical_scheduler_t;

/* ============================================================================
   3. AUTONOMIA ADAPTATIVA
   ============================================================================ */

/* Estrutura para modelo de ML simplificado (regressão linear) */
typedef struct {
    double weights[10];  /* Pesos para features */
    double bias;
    double learning_rate;
    uint32_t feature_count;
} ml_model_t;

/* Sistema de auto-diagnóstico */
typedef struct {
    ml_model_t performance_model;
    ml_model_t fault_model;
    
    /* Histórico de métricas para análise */
    perf_metrics_t history[1000];
    uint32_t history_index;
    uint32_t history_size;
    
    /* Detecção de anomalias */
    double anomaly_threshold;
    uint32_t anomaly_count;
    
    /* Funções de diagnóstico */
    bool (*detect_anomaly)(struct autonomy_system* as, perf_metrics_t current);
    void (*update_model)(struct autonomy_system* as, perf_metrics_t* samples, uint32_t count);
    void (*self_correct)(struct megakernel* mk, uint32_t error_code);
} autonomy_system_t;

/* ============================================================================
   ESTRUTURA PRINCIPAL COMPLETA
   ============================================================================ */

/* Contexto completo do MegaKernel */
typedef struct {
    megakernel_t meta_kernel;
    hierarchical_scheduler_t scheduler;
    autonomy_system_t autonomy;
    
    /* Controles do sistema */
    pthread_mutex_t lock;
    pthread_cond_t cond_var;
    bool shutdown_requested;
    
    /* Configuração */
    uint32_t max_subkernels;
    double power_budget;
    double thermal_limit;
} megakernel_context_t;

/* ============================================================================
   PROTÓTIPOS DE FUNÇÕES PRINCIPAIS
   ============================================================================ */

/* Inicialização e destruição */
megakernel_context_t* megakernel_init(uint32_t max_sk, double power_budget, double thermal_limit);
void megakernel_destroy(megakernel_context_t* ctx);

/* Gerenciamento de subkernels */
bool megakernel_add_subkernel(megakernel_context_t* ctx, subkernel_t* subkernel);
bool megakernel_remove_subkernel(megakernel_context_t* ctx, uint32_t sk_id);

/* Execução de tarefas */
uint64_t megakernel_submit_task(megakernel_context_t* ctx, mega_task_t* task);
bool megakernel_execute_tasks(megakernel_context_t* ctx, uint32_t max_tasks);

/* Controle adaptativo */
void megakernel_reconfigure(megakernel_context_t* ctx);
void megakernel_optimize_resources(megakernel_context_t* ctx);

/* Monitoramento */
perf_metrics_t megakernel_get_metrics(megakernel_context_t* ctx);
void megakernel_print_status(megakernel_context_t* ctx);

/* Auto-diagnóstico */
void megakernel_run_diagnostics(megakernel_context_t* ctx);
bool megakernel_handle_fault(megakernel_context_t* ctx, uint32_t error_code);

/* ============================================================================
   CONSTANTES E CONFIGURAÇÕES
   ============================================================================ */

#define MAX_SUBKERNELS_PER_TYPE   64
#define TASK_QUEUE_CAPACITY       1024
#define METRICS_HISTORY_SIZE      1000
#define ANOMALY_THRESHOLD         3.0    /* Desvios padrão */
#define DEFAULT_LEARNING_RATE     0.01

/* Códigos de erro */
#define MK_SUCCESS                0
#define MK_ERR_INVALID_PARAM      1
#define MK_ERR_RESOURCE_BUSY      2
#define MK_ERR_HW_NOT_AVAILABLE   3
#define MK_ERR_OVERLOAD           4
#define MK_ERR_THERMAL            5
#define MK_ERR_POWER              6

#endif /* MEGAKERNEL_H */