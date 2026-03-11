#ifndef _OSD_MSG_H
#define _OSD_MSG_H

/*
 * OSDMsg.h - Sistema de Mensagens do OSDKit (Operational System Drivers Kit)
 * 
 * "Mensagens são Registros" - Extensão da filosofia do BibleKernel para IPC
 * 
 * Este arquivo define o sistema de comunicação entre drivers, entre drivers e
 * kernel, e entre drivers e userland. Todo driver no OSDKit se comunica através
 * de mensagens que seguem o padrão de registros do sistema.
 * 
 * O sistema de mensagens do OSDKit é construído sobre o IPC do kernel, mas
 * adiciona camadas específicas para comunicação entre drivers:
 * - Message Routing: Encaminhamento de mensagens entre drivers
 * - Driver Discovery: Descoberta de serviços oferecidos por drivers
 * - Protocol Negotiation: Negociação de protocolos entre drivers
 * - Zero-Copy Transfers: Transferência eficiente de dados
 */

#include <bible/types.h>
#include <bible/cdefs.h>
#include <bible/regsister.h>
#include <bible/pmap.h>
#include <bible/vm.h>
#include <bible/task.h>
#include <bible/kernl_object.h>
#include <bible/ipc.h>
#include <OSDKit/OSDtypes.h>

/*
 * ============================================================================
 * HANDLES E IDENTIFICADORES DO SISTEMA DE MENSAGENS
 * ============================================================================
 */

/* Handles específicos para mensagens */
typedef BK_REGISTER_HANDLE OSD_MSG_HANDLE;      /* Handle para mensagem */
typedef BK_REGISTER_HANDLE OSD_CHANNEL_HANDLE;  /* Handle para canal de comunicação */
typedef BK_REGISTER_HANDLE OSD_ENDPOINT_HANDLE; /* Handle para endpoint */
typedef BK_REGISTER_HANDLE OSD_SERVICE_HANDLE;  /* Handle para serviço */

/* Handles inválidos */
#define OSD_MSG_HANDLE_NULL      BK_REGISTER_NULL
#define OSD_MSG_HANDLE_INVALID   BK_REGISTER_INVALID

/*
 * ============================================================================
 * TIPOS DE MENSAGEM DO OSDKIT
 * ============================================================================
 */

/* Tipos de mensagem (faixa reservada: 0x2000-0x2FFF para OSDKit) */
enum __OSD_MSG_TYPE {
    /* Mensagens de controle (0x2000-0x20FF) */
    OSD_MSG_TYPE_NULL           = 0x2000,
    OSD_MSG_TYPE_HELLO          = 0x2001,  /* Handshake inicial */
    OSD_MSG_TYPE_ACK            = 0x2002,  /* Confirmação */
    OSD_MSG_TYPE_NACK           = 0x2003,  /* Confirmação negativa */
    OSD_MSG_TYPE_PING           = 0x2004,  /* Verificação de presença */
    OSD_MSG_TYPE_PONG           = 0x2005,  /* Resposta ao ping */
    OSD_MSG_TYPE_QUIT           = 0x2006,  /* Finalização */
    OSD_MSG_TYPE_ABORT          = 0x2007,  /* Abortar operação */
    OSD_MSG_TYPE_CANCEL         = 0x2008,  /* Cancelar mensagem pendente */
    
    /* Mensagens de descoberta (0x2100-0x21FF) */
    OSD_MSG_TYPE_WHO_ARE_YOU    = 0x2100,  /* Identifique-se */
    OSD_MSG_TYPE_I_AM           = 0x2101,  /* Resposta de identificação */
    OSD_MSG_TYPE_LIST_SERVICES  = 0x2102,  /* Liste serviços oferecidos */
    OSD_MSG_TYPE_SERVICE_LIST   = 0x2103,  /* Lista de serviços */
    OSD_MSG_TYPE_FIND_SERVICE   = 0x2104,  /* Encontrar serviço */
    OSD_MSG_TYPE_SERVICE_FOUND  = 0x2105,  /* Serviço encontrado */
    OSD_MSG_TYPE_SERVICE_LOST   = 0x2106,  /* Serviço perdido */
    
    /* Mensagens de configuração (0x2200-0x22FF) */
    OSD_MSG_TYPE_GET_PARAM      = 0x2200,  /* Obter parâmetro */
    OSD_MSG_TYPE_SET_PARAM      = 0x2201,  /* Configurar parâmetro */
    OSD_MSG_TYPE_PARAM_VALUE    = 0x2202,  /* Valor do parâmetro */
    OSD_MSG_TYPE_GET_STATUS     = 0x2203,  /* Obter status */
    OSD_MSG_TYPE_STATUS         = 0x2204,  /* Status atual */
    OSD_MSG_TYPE_GET_INFO       = 0x2205,  /* Obter informação */
    OSD_MSG_TYPE_INFO           = 0x2206,  /* Informação */
    
    /* Mensagens de operação (0x2300-0x23FF) */
    OSD_MSG_TYPE_OPEN           = 0x2300,  /* Abrir dispositivo/recurso */
    OSD_MSG_TYPE_CLOSE          = 0x2301,  /* Fechar dispositivo/recurso */
    OSD_MSG_TYPE_READ           = 0x2302,  /* Leitura */
    OSD_MSG_TYPE_WRITE          = 0x2303,  /* Escrita */
    OSD_MSG_TYPE_IOCTL          = 0x2304,  /* Controle de I/O */
    OSD_MSG_TYPE_MMAP           = 0x2305,  /* Mapear memória */
    OSD_MSG_TYPE_MUNMAP         = 0x2306,  /* Desmapear memória */
    OSD_MSG_TYPE_SYNC           = 0x2307,  /* Sincronizar */
    OSD_MSG_TYPE_FLUSH          = 0x2308,  /* Descarregar buffers */
    OSD_MSG_TYPE_TRUNCATE       = 0x2309,  /* Truncar */
    
    /* Mensagens de dados (0x2400-0x24FF) */
    OSD_MSG_TYPE_DATA           = 0x2400,  /* Dados genéricos */
    OSD_MSG_TYPE_DATA_START     = 0x2401,  /* Início de transmissão de dados */
    OSD_MSG_TYPE_DATA_CHUNK     = 0x2402,  /* Fragmento de dados */
    OSD_MSG_TYPE_DATA_END       = 0x2403,  /* Fim de transmissão */
    OSD_MSG_TYPE_DATA_READY     = 0x2404,  /* Dados prontos para leitura */
    OSD_MSG_TYPE_DATA_WANTED    = 0x2405,  /* Dados solicitados */
    
    /* Mensagens de evento (0x2500-0x25FF) */
    OSD_MSG_TYPE_EVENT          = 0x2500,  /* Evento genérico */
    OSD_MSG_TYPE_INTERRUPT      = 0x2501,  /* Interrupção de hardware */
    OSD_MSG_TYPE_NOTIFY         = 0x2502,  /* Notificação */
    OSD_MSG_TYPE_ALERT          = 0x2503,  /* Alerta */
    OSD_MSG_TYPE_TIMEOUT        = 0x2504,  /* Timeout */
    OSD_MSG_TYPE_ERROR          = 0x2505,  /* Erro */
    
    /* Mensagens de driver específico (0x2600-0x26FF) */
    OSD_MSG_TYPE_DRIVER_BASE    = 0x2600,  /* Base para drivers específicos */
    
    /* Mensagens de segurança (0x2700-0x27FF) */
    OSD_MSG_TYPE_AUTH           = 0x2700,  /* Autenticação */
    OSD_MSG_TYPE_AUTH_RESPONSE  = 0x2701,  /* Resposta de autenticação */
    OSD_MSG_TYPE_CHALLENGE      = 0x2702,  /* Desafio criptográfico */
    OSD_MSG_TYPE_CHALLENGE_RESP = 0x2703,  /* Resposta ao desafio */
    OSD_MSG_TYPE_CAPABILITY     = 0x2704,  /* Capacidade */
    OSD_MSG_TYPE_BKEY_REQUEST   = 0x2705,  /* Requisição de chave BKEY */
    OSD_MSG_TYPE_BKEY_RESPONSE  = 0x2706,  /* Resposta com chave BKEY */
    
    /* Mensagens de sistema de arquivos (0x2800-0x28FF) */
    OSD_MSG_TYPE_FS_MOUNT       = 0x2800,  /* Montar FS */
    OSD_MSG_TYPE_FS_UNMOUNT     = 0x2801,  /* Desmontar FS */
    OSD_MSG_TYPE_FS_FORMAT      = 0x2802,  /* Formatar FS */
    OSD_MSG_TYPE_FS_CHECK       = 0x2803,  /* Verificar FS */
    OSD_MSG_TYPE_FS_REPAIR      = 0x2804,  /* Reparar FS */
};

/*
 * ============================================================================
 * PRIORIDADES DE MENSAGEM
 * ============================================================================
 */

#define OSD_MSG_PRIO_LOW        0x00  /* Baixa prioridade */
#define OSD_MSG_PRIO_NORMAL     0x01  /* Prioridade normal */
#define OSD_MSG_PRIO_HIGH       0x02  /* Alta prioridade */
#define OSD_MSG_PRIO_CRITICAL   0x03  /* Crítica (não pode ser ignorada) */
#define OSD_MSG_PRIO_INTERRUPT  0x04  /* Nível de interrupção */

/*
 * ============================================================================
 * FLAGS DE MENSAGEM
 * ============================================================================
 */

#define OSD_MSG_FLAG_NONE       0x00000000  /* Sem flags */
#define OSD_MSG_FLAG_SYNC       0x00000001  /* Mensagem síncrona (espera resposta) */
#define OSD_MSG_FLAG_ASYNC      0x00000002  /* Mensagem assíncrona (não espera) */
#define OSD_MSG_FLAG_BLOCK      0x00000004  /* Bloqueante */
#define OSD_MSG_FLAG_NONBLOCK   0x00000008  /* Não bloqueante */
#define OSD_MSG_FLAG_BROADCAST  0x00000010  /* Broadcast para todos */
#define OSD_MSG_FLAG_MULTICAST  0x00000020  /* Multicast para grupo */
#define OSD_MSG_FLAG_UNICAST    0x00000040  /* Unicast para um destino */
#define OSD_MSG_FLAG_ENCRYPTED  0x00000080  /* Mensagem criptografada */
#define OSD_MSG_FLAG_SIGNED     0x00000100  /* Mensagem assinada */
#define OSD_MSG_FLAG_COMPRESSED 0x00000200  /* Mensagem comprimida */
#define OSD_MSG_FLAG_URGENT     0x00000400  /* Mensagem urgente */
#define OSD_MSG_FLAG_NO_REPLY   0x00000800  /* Não requer resposta */
#define OSD_MSG_FLAG_EXPECT_REPLY 0x00001000 /* Aguarda resposta */
#define OSD_MSG_FLAG_PERSISTENT 0x00002000  /* Mensagem persistente */
#define OSD_MSG_FLAG_PRIVATE    0x00004000  /* Mensagem privada */
#define OSD_MSG_FLAG_SECURE     0x00008000  /* Canal seguro */
#define OSD_MSG_FLAG_FASTPATH   0x00010000  /* Caminho rápido (bypass) */

/*
 * ============================================================================
 * ESTADOS DE MENSAGEM
 * ============================================================================
 */

typedef enum __OSD_MSG_STATE {
    OSD_MSG_STATE_CREATED       = 0,  /* Mensagem criada */
    OSD_MSG_STATE_QUEUED        = 1,  /* Na fila para envio */
    OSD_MSG_STATE_SENT          = 2,  /* Enviada */
    OSD_MSG_STATE_DELIVERED     = 3,  /* Entregue ao destino */
    OSD_MSG_STATE_RECEIVED      = 4,  /* Recebida pelo destino */
    OSD_MSG_STATE_PROCESSING    = 5,  /* Em processamento */
    OSD_MSG_STATE_PROCESSED     = 6,  /* Processada */
    OSD_MSG_STATE_REPLIED       = 7,  /* Resposta enviada */
    OSD_MSG_STATE_REPLY_RCVD    = 8,  /* Resposta recebida */
    OSD_MSG_STATE_TIMEOUT       = 9,  /* Timeout */
    OSD_MSG_STATE_CANCELLED     = 10, /* Cancelada */
    OSD_MSG_STATE_ERROR         = 11, /* Erro no processamento */
} OSD_MSG_STATE;

/*
 * ============================================================================
 * ESTRUTURA DA MENSAGEM
 * ============================================================================
 */

/* Cabeçalho da mensagem (sempre presente) */
struct osd_msg_header {
    BK_UINT32           msg_magic;      /* Número mágico: 'OSDM' */
    BK_UINT32           msg_version;    /* Versão do protocolo */
    BK_UINT32           msg_type;       /* Tipo da mensagem (OSD_MSG_TYPE_*) */
    BK_UINT32           msg_flags;      /* Flags da mensagem */
    BK_UINT32           msg_priority;   /* Prioridade */
    BK_UINT32           msg_state;      /* Estado atual */
    
    BK_UINT64           msg_id;         /* ID único da mensagem */
    BK_UINT64           msg_reply_to;   /* ID da mensagem original (para respostas) */
    BK_UINT64           msg_correlation_id; /* ID de correlação */
    
    BK_TIME             msg_created;    /* Timestamp de criação */
    BK_TIME             msg_sent;       /* Timestamp de envio */
    BK_TIME             msg_received;   /* Timestamp de recebimento */
    BK_TIME             msg_expiry;     /* Timestamp de expiração (0 = nunca) */
    
    BK_UINT32           msg_src_type;   /* Tipo da fonte (driver, processo, etc) */
    BK_UINT32           msg_dst_type;   /* Tipo do destino */
    
    BK_REGISTER_HANDLE  msg_src;        /* Handle da fonte */
    BK_REGISTER_HANDLE  msg_dst;        /* Handle do destino */
    BK_REGISTER_HANDLE  msg_reply_to_handle; /* Handle para resposta */
    
    BK_UINT32           msg_data_size;  /* Tamanho dos dados */
    BK_UINT32           msg_data_max;   /* Capacidade máxima dos dados */
    BK_UINT32           msg_data_used;  /* Dados usados atualmente */
    
    BK_UINT32           msg_checksum;   /* Checksum da mensagem */
    BK_UINT32           msg_reserved[4]; /* Reservado para uso futuro */
};

/* Estrutura principal da mensagem */
struct osd_message {
    struct osd_msg_header   msg_header;     /* Cabeçalho */
    BK_UINT8                *msg_data;      /* Dados da mensagem */
    
    /* Metadados adicionais (opcionais) */
    struct osd_msg_metadata *msg_metadata;  /* Metadados */
    
    /* Callbacks */
    void (*msg_on_sent)(struct osd_message *msg, void *context);
    void (*msg_on_delivered)(struct osd_message *msg, void *context);
    void (*msg_on_reply)(struct osd_message *msg, struct osd_message *reply, void *context);
    void (*msg_on_timeout)(struct osd_message *msg, void *context);
    void (*msg_on_error)(struct osd_message *msg, BK_I32 error, void *context);
    void                    *msg_callback_context;
    
    /* Privado (para uso interno do sistema) */
    void                    *msg_private;
};

typedef struct osd_message OSD_MESSAGE;

/* Metadados da mensagem (extensível) */
struct osd_msg_metadata {
    BK_UINT32           md_type;        /* Tipo de metadado */
    BK_UINT32           md_size;        /* Tamanho */
    BK_UINT8            *md_data;       /* Dados do metadado */
    struct osd_msg_metadata *md_next;   /* Próximo metadado */
};

/*
 * ============================================================================
 * CANAIS DE COMUNICAÇÃO
 * ============================================================================
 */

/* Tipos de canal */
typedef enum __OSD_CHANNEL_TYPE {
    OSD_CHANNEL_TYPE_P2P        = 0,  /* Ponto-a-ponto */
    OSD_CHANNEL_TYPE_BROADCAST  = 1,  /* Broadcast */
    OSD_CHANNEL_TYPE_MULTICAST  = 2,  /* Multicast */
    OSD_CHANNEL_TYPE_PUBSUB     = 3,  /* Publicação/assinatura */
    OSD_CHANNEL_TYPE_REQREP     = 4,  /* Requisição/resposta */
    OSD_CHANNEL_TYPE_STREAM     = 5,  /* Stream de dados */
    OSD_CHANNEL_TYPE_DATAGRAM   = 6,  /* Datagrama */
} OSD_CHANNEL_TYPE;

/* Modos de canal */
typedef enum __OSD_CHANNEL_MODE {
    OSD_CHANNEL_MODE_SYNC       = 0,  /* Síncrono */
    OSD_CHANNEL_MODE_ASYNC      = 1,  /* Assíncrono */
    OSD_CHANNEL_MODE_BLOCKING   = 2,  /* Bloqueante */
    OSD_CHANNEL_MODE_NONBLOCK   = 3,  /* Não bloqueante */
} OSD_CHANNEL_MODE;

/* Estrutura do canal */
struct osd_channel {
    BK_UINT32           chan_id;            /* ID do canal */
    OSD_CHANNEL_TYPE    chan_type;          /* Tipo do canal */
    OSD_CHANNEL_MODE    chan_mode;          /* Modo do canal */
    BK_UINT32           chan_flags;         /* Flags do canal */
    
    BK_UINT32           chan_max_msgs;      /* Máximo de mensagens na fila */
    BK_UINT32           chan_cur_msgs;      /* Mensagens atuais na fila */
    BK_UINT32           chan_max_size;      /* Tamanho máximo por mensagem */
    
    BK_UINT32           chan_src_count;     /* Número de fontes */
    BK_UINT32           chan_dst_count;     /* Número de destinos */
    
    BK_REGISTER_HANDLE  *chan_srcs;         /* Array de fontes */
    BK_REGISTER_HANDLE  *chan_dsts;         /* Array de destinos */
    
    BK_QUEUE            *chan_queue;        /* Fila de mensagens */
    BK_SPINLOCK         chan_lock;          /* Lock do canal */
    
    /* Estatísticas */
    BK_UINT64           chan_msgs_sent;     /* Mensagens enviadas */
    BK_UINT64           chan_msgs_received; /* Mensagens recebidas */
    BK_UINT64           chan_bytes_sent;    /* Bytes enviados */
    BK_UINT64           chan_bytes_received; /* Bytes recebidos */
    BK_UINT32           chan_peak_msgs;     /* Pico de mensagens na fila */
    
    /* Callbacks */
    void (*chan_on_message)(struct osd_channel *chan, OSD_MESSAGE *msg, void *context);
    void (*chan_on_error)(struct osd_channel *chan, BK_I32 error, void *context);
    void                    *chan_callback_context;
    
    void                    *chan_private;
};

typedef struct osd_channel OSD_CHANNEL;

/*
 * ============================================================================
 * ENDPOINTS E SERVIÇOS
 * ============================================================================
 */

/* Endpoint de comunicação */
struct osd_endpoint {
    BK_UINT32           ep_id;              /* ID do endpoint */
    BK_REGISTER_HANDLE  ep_owner;           /* Dono do endpoint */
    BK_UINT32           ep_type;            /* Tipo do endpoint */
    BK_UINT32           ep_flags;           /* Flags */
    
    char                ep_name[64];        /* Nome do endpoint */
    char                ep_description[256]; /* Descrição */
    
    BK_UINT32           ep_protocol_version; /* Versão do protocolo */
    BK_UINT32           ep_capabilities;    /* Capacidades do endpoint */
    
    OSD_CHANNEL_HANDLE  ep_channel;         /* Canal associado */
    
    /* Fila de mensagens recebidas */
    BK_QUEUE            *ep_recv_queue;
    BK_SPINLOCK         ep_recv_lock;
    BK_SEMAPHORE        ep_recv_sem;
    
    /* Callbacks */
    BK_I32 (*ep_msg_handler)(struct osd_endpoint *ep, OSD_MESSAGE *msg);
    void   (*ep_event_handler)(struct osd_endpoint *ep, BK_UINT32 event, void *data);
    void                    *ep_handler_context;
    
    void                    *ep_private;
};

typedef struct osd_endpoint OSD_ENDPOINT;

/* Serviço oferecido por um driver */
struct osd_service {
    BK_UINT32           svc_id;             /* ID do serviço */
    BK_REGISTER_HANDLE  svc_provider;       /* Driver que provê o serviço */
    BK_UINT32           svc_type;           /* Tipo de serviço */
    
    char                svc_name[64];       /* Nome do serviço */
    char                svc_version[32];    /* Versão */
    BK_UINT32           svc_caps;           /* Capacidades */
    
    OSD_ENDPOINT_HANDLE svc_endpoint;       /* Endpoint para acesso */
    
    BK_LIST_ENTRY(osd_service) svc_list;    /* Link na lista de serviços */
};

typedef struct osd_service OSD_SERVICE;

/*
 * ============================================================================
 * FUNÇÕES DE CRIAÇÃO E DESTRUIÇÃO
 * ============================================================================
 */

/* Cria uma nova mensagem */
OSD_MESSAGE *osd_msg_create(BK_UINT32 type, BK_UINT32 flags, BK_UINT32 data_size);
OSD_MESSAGE *osd_msg_create_reply(OSD_MESSAGE *original, BK_UINT32 flags, BK_UINT32 data_size);

/* Destrói uma mensagem */
BK_I32 osd_msg_destroy(OSD_MESSAGE *msg);

/* Inicializa uma mensagem a partir de buffer existente */
BK_I32 osd_msg_init(OSD_MESSAGE *msg, void *buffer, BK_UINT32 buffer_size);

/* Limpa uma mensagem */
void osd_msg_clear(OSD_MESSAGE *msg);

/*
 * ============================================================================
 * FUNÇÕES DE MANIPULAÇÃO DE DADOS
 * ============================================================================
 */

/* Adiciona dados à mensagem */
BK_I32 osd_msg_append_data(OSD_MESSAGE *msg, const void *data, BK_UINT32 size);
BK_I32 osd_msg_set_data(OSD_MESSAGE *msg, BK_UINT32 offset, const void *data, BK_UINT32 size);

/* Obtém dados da mensagem */
void *osd_msg_get_data(OSD_MESSAGE *msg);
BK_UINT32 osd_msg_get_data_size(OSD_MESSAGE *msg);
BK_UINT32 osd_msg_get_remaining_space(OSD_MESSAGE *msg);

/* Adiciona metadados */
BK_I32 osd_msg_add_metadata(OSD_MESSAGE *msg, BK_UINT32 type, const void *data, BK_UINT32 size);
void *osd_msg_get_metadata(OSD_MESSAGE *msg, BK_UINT32 type, BK_UINT32 *size);

/* Serializa/deserializa */
BK_I32 osd_msg_serialize(OSD_MESSAGE *msg, void *buffer, BK_UINT32 *size);
BK_I32 osd_msg_deserialize(OSD_MESSAGE *msg, const void *buffer, BK_UINT32 size);

/*
 * ============================================================================
 * FUNÇÕES DE ENVIO E RECEBIMENTO
 * ============================================================================
 */

/* Envia mensagem */
BK_I32 osd_msg_send(OSD_MESSAGE *msg, BK_REGISTER_HANDLE dst);
BK_I32 osd_msg_send_to_channel(OSD_MESSAGE *msg, OSD_CHANNEL_HANDLE channel);
BK_I32 osd_msg_send_to_endpoint(OSD_MESSAGE *msg, OSD_ENDPOINT_HANDLE endpoint);
BK_I32 osd_msg_send_to_service(OSD_MESSAGE *msg, const char *service_name);

/* Envia e aguarda resposta (síncrono) */
BK_I32 osd_msg_call(OSD_MESSAGE *msg, BK_REGISTER_HANDLE dst, 
                   OSD_MESSAGE **reply, BK_TIME timeout);

/* Envia mensagem urgente (alta prioridade) */
BK_I32 osd_msg_send_urgent(OSD_MESSAGE *msg, BK_REGISTER_HANDLE dst);

/* Recebe mensagem */
OSD_MESSAGE *osd_msg_receive(BK_REGISTER_HANDLE src, BK_UINT32 type_mask, BK_TIME timeout);
BK_I32 osd_msg_receive_into(OSD_MESSAGE **msg, BK_REGISTER_HANDLE src, 
                           BK_UINT32 type_mask, BK_TIME timeout);

/* Responde a uma mensagem */
BK_I32 osd_msg_reply(OSD_MESSAGE *original, OSD_MESSAGE *reply);

/*
 * ============================================================================
 * FUNÇÕES DE CANAL
 * ============================================================================
 */

/* Cria canal */
OSD_CHANNEL_HANDLE osd_channel_create(OSD_CHANNEL_TYPE type, OSD_CHANNEL_MODE mode,
                                     BK_UINT32 max_msgs, BK_UINT32 max_msg_size);

/* Destrói canal */
BK_I32 osd_channel_destroy(OSD_CHANNEL_HANDLE channel);

/* Adiciona/remove participantes */
BK_I32 osd_channel_add_source(OSD_CHANNEL_HANDLE channel, BK_REGISTER_HANDLE src);
BK_I32 osd_channel_remove_source(OSD_CHANNEL_HANDLE channel, BK_REGISTER_HANDLE src);
BK_I32 osd_channel_add_destination(OSD_CHANNEL_HANDLE channel, BK_REGISTER_HANDLE dst);
BK_I32 osd_channel_remove_destination(OSD_CHANNEL_HANDLE channel, BK_REGISTER_HANDLE dst);

/* Abre/fecha canal para comunicação */
BK_I32 osd_channel_open(OSD_CHANNEL_HANDLE channel);
BK_I32 osd_channel_close(OSD_CHANNEL_HANDLE channel);

/* Obtém estatísticas do canal */
BK_I32 osd_channel_get_stats(OSD_CHANNEL_HANDLE channel, struct osd_channel_stats *stats);

struct osd_channel_stats {
    BK_UINT32       cs_msgs_sent;
    BK_UINT32       cs_msgs_received;
    BK_UINT32       cs_bytes_sent;
    BK_UINT32       cs_bytes_received;
    BK_UINT32       cs_current_msgs;
    BK_UINT32       cs_peak_msgs;
    BK_UINT32       cs_dropped_msgs;
    BK_UINT32       cs_timeout_msgs;
};

/*
 * ============================================================================
 * FUNÇÕES DE ENDPOINT
 * ============================================================================
 */

/* Cria endpoint */
OSD_ENDPOINT_HANDLE osd_endpoint_create(const char *name, BK_REGISTER_HANDLE owner);

/* Destrói endpoint */
BK_I32 osd_endpoint_destroy(OSD_ENDPOINT_HANDLE endpoint);

/* Registra handler de mensagens */
BK_I32 osd_endpoint_register_handler(OSD_ENDPOINT_HANDLE endpoint,
                                     BK_I32 (*handler)(OSD_ENDPOINT *, OSD_MESSAGE *),
                                     void *context);

/* Escuta por mensagens no endpoint */
BK_I32 osd_endpoint_listen(OSD_ENDPOINT_HANDLE endpoint);

/* Para de escutar */
BK_I32 osd_endpoint_stop(OSD_ENDPOINT_HANDLE endpoint);

/* Aguarda por mensagem no endpoint */
OSD_MESSAGE *osd_endpoint_wait(OSD_ENDPOINT_HANDLE endpoint, BK_TIME timeout);

/*
 * ============================================================================
 * FUNÇÕES DE SERVIÇO
 * ============================================================================
 */

/* Registra um serviço */
BK_I32 osd_service_register(const char *name, OSD_ENDPOINT_HANDLE endpoint,
                           BK_UINT32 caps, OSD_SERVICE_HANDLE *service);

/* Cancela registro de serviço */
BK_I32 osd_service_unregister(OSD_SERVICE_HANDLE service);

/* Encontra um serviço pelo nome */
OSD_SERVICE_HANDLE osd_service_find(const char *name);

/* Obtém endpoint de um serviço */
OSD_ENDPOINT_HANDLE osd_service_get_endpoint(OSD_SERVICE_HANDLE service);

/*
 * ============================================================================
 * FUNÇÕES DE UTILIDADE
 * ============================================================================
 */

/* Duplica uma mensagem */
OSD_MESSAGE *osd_msg_duplicate(OSD_MESSAGE *msg);

/* Verifica se mensagem é de resposta */
static __BK_ALWAYS_INLINE BK_BOOLEAN
osd_msg_is_reply(OSD_MESSAGE *msg)
{
    return (msg->msg_header.msg_reply_to != 0);
}

/* Obtém tipo da mensagem */
static __BK_ALWAYS_INLINE BK_UINT32
osd_msg_get_type(OSD_MESSAGE *msg)
{
    return msg->msg_header.msg_type;
}

/* Obtém ID da mensagem */
static __BK_ALWAYS_INLINE BK_UINT64
osd_msg_get_id(OSD_MESSAGE *msg)
{
    return msg->msg_header.msg_id;
}

/* Obtém flags da mensagem */
static __BK_ALWAYS_INLINE BK_UINT32
osd_msg_get_flags(OSD_MESSAGE *msg)
{
    return msg->msg_header.msg_flags;
}

/* Configura callback de resposta */
static __BK_ALWAYS_INLINE void
osd_msg_set_reply_callback(OSD_MESSAGE *msg,
                          void (*callback)(OSD_MESSAGE *, OSD_MESSAGE *, void *),
                          void *context)
{
    msg->msg_on_reply = callback;
    msg->msg_callback_context = context;
}

/* Calcula checksum da mensagem */
static __BK_ALWAYS_INLINE BK_UINT32
osd_msg_calculate_checksum(OSD_MESSAGE *msg)
{
    BK_UINT32 sum = 0;
    BK_UINT32 i;
    
    /* Checksum do cabeçalho (excluindo o próprio campo checksum) */
    BK_UINT8 *bytes = (BK_UINT8 *)&msg->msg_header;
    for (i = 0; i < sizeof(struct osd_msg_header) - sizeof(BK_UINT32); i++) {
        sum += bytes[i];
        sum = (sum << 1) | (sum >> 31);
    }
    
    /* Checksum dos dados */
    if (msg->msg_data && msg->msg_header.msg_data_size > 0) {
        for (i = 0; i < msg->msg_header.msg_data_size; i++) {
            sum += msg->msg_data[i];
            sum = (sum << 1) | (sum >> 31);
        }
    }
    
    return sum;
}

/*
 * ============================================================================
 * MACROS DE CONVENIÊNCIA
 * ============================================================================
 */

/* Cria e envia mensagem em um passo */
#define OSD_MSG_SEND(type, dst, data, size, flags) \
    do { \
        OSD_MESSAGE *__msg = osd_msg_create((type), (flags), (size)); \
        if (__msg) { \
            if ((data) && (size) > 0) \
                osd_msg_append_data(__msg, (data), (size)); \
            osd_msg_send(__msg, (dst)); \
        } \
    } while (0)

/* Cria, envia e aguarda resposta */
#define OSD_MSG_CALL(type, dst, data, size, reply, timeout) \
    ({ \
        OSD_MESSAGE *__msg = osd_msg_create((type), OSD_MSG_FLAG_EXPECT_REPLY, (size)); \
        BK_I32 __ret = BK_ERROR; \
        if (__msg) { \
            if ((data) && (size) > 0) \
                osd_msg_append_data(__msg, (data), (size)); \
            __ret = osd_msg_call(__msg, (dst), (reply), (timeout)); \
        } \
        __ret; \
    })

/* Itera sobre todas as mensagens em uma fila */
#define OSD_MSG_QUEUE_FOREACH(queue, msg) \
    for (msg = (OSD_MESSAGE *)bk_queue_dequeue((queue), 0); \
         msg != NULL; \
         msg = (OSD_MESSAGE *)bk_queue_dequeue((queue), 0))

/*
 * ============================================================================
 * CONSTANTES E DEFINIÇÕES
 * ============================================================================
 */

/* Número mágico para mensagens OSD */
#define OSD_MSG_MAGIC           0x4F53444D  /* 'OSDM' */

/* Tamanhos máximos */
#define OSD_MSG_MAX_SIZE         (1024 * 1024)  /* 1MB */
#define OSD_MSG_MAX_DATA_SIZE    (1024 * 1024 - sizeof(struct osd_msg_header))
#define OSD_MSG_MAX_METADATA_SIZE 65536          /* 64KB */

/* Timeouts especiais */
#define OSD_MSG_TIMEOUT_IMMEDIATE    0
#define OSD_MSG_TIMEOUT_INFINITE     ((BK_TIME)-1)
#define OSD_MSG_TIMEOUT_DEFAULT      5000  /* 5 segundos */

/* Número mágico do cabeçalho */
#define OSD_MSG_HEADER_MAGIC    0x4F534448  /* 'OSDH' */

/*
 * ============================================================================
 * INICIALIZAÇÃO DO SISTEMA
 * ============================================================================
 */

/* Inicializa o sistema de mensagens do OSDKit */
BK_I32 osd_msg_system_init(void);

/* Finaliza o sistema de mensagens */
void osd_msg_system_shutdown(void);

/* Obtém estatísticas globais */
BK_I32 osd_msg_system_get_stats(struct osd_system_msg_stats *stats);

struct osd_system_msg_stats {
    BK_UINT64       sms_total_messages;      /* Total de mensagens criadas */
    BK_UINT64       sms_active_messages;     /* Mensagens ativas */
    BK_UINT64       sms_sent_messages;       /* Mensagens enviadas */
    BK_UINT64       sms_received_messages;   /* Mensagens recebidas */
    BK_UINT64       sms_dropped_messages;    /* Mensagens descartadas */
    BK_UINT64       sms_timeout_messages;    /* Mensagens com timeout */
    BK_UINT64       sms_error_messages;      /* Mensagens com erro */
    BK_UINT32       sms_active_channels;     /* Canais ativos */
    BK_UINT32       sms_active_endpoints;    /* Endpoints ativos */
    BK_UINT32       sms_active_services;     /* Serviços ativos */
    BK_UINT64       sms_total_bytes;          /* Total de bytes trafegados */
};

/*
 * ============================================================================
 * CÓDIGOS DE ERRO ESPECÍFICOS
 * ============================================================================
 */

#define OSD_MSG_SUCCESS         0   /* Sucesso */
#define OSD_MSG_EINVAL          -2000  /* Parâmetro inválido */
#define OSD_MSG_ENOMEM          -2001  /* Sem memória */
#define OSD_MSG_EMSGSIZE        -2002  /* Tamanho de mensagem inválido */
#define OSD_MSG_ENOMSG          -2003  /* Sem mensagem */
#define OSD_MSG_ETIMEDOUT       -2004  /* Timeout */
#define OSD_MSG_ECANCELED       -2005  /* Cancelado */
#define OSD_MSG_ENOTCONN        -2006  /* Não conectado */
#define OSD_MSG_ECONNREFUSED    -2007  /* Conexão recusada */
#define OSD_MSG_EPIPE           -2008  /* Pipe/Canal quebrado */
#define OSD_MSG_EBADMSG         -2009  /* Mensagem corrompida */
#define OSD_MSG_EBADHANDLE      -2010  /* Handle inválido */
#define OSD_MSG_EBADCHANNEL     -2011  /* Canal inválido */
#define OSD_MSG_EBADENDPOINT    -2012  /* Endpoint inválido */
#define OSD_MSG_EBADSERVICE     -2013  /* Serviço inválido */
#define OSD_MSG_EACCESS         -2014  /* Acesso negado */
#define OSD_MSG_EPERM           -2015  /* Permissão negada */
#define OSD_MSG_EBUSY           -2016  /* Ocupado */
#define OSD_MSG_EAGAIN          -2017  /* Tente novamente */
#define OSD_MSG_EEXIST          -2018  /* Já existe */
#define OSD_MSG_ENOENT          -2019  /* Não existe */
#define OSD_MSG_ENOSPC          -2020  /* Sem espaço na fila */

#endif /* _OSD_MSG_H */
