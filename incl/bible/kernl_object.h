#ifndef _BIBLE_KERNL_OBJECT_H_
#define _BIBLE_KERNL_OBJECT_H_

#include <bible/cdefs.h>
#include <bible/types.h>
#include <bible/queue.h>

/*
 * SPDX-License-Identifier: MIT License
 * see src/LICENSE
 *
 * Copyright (c) 2024 Bible System Developers
 * Todos os direitos reservados.
 *
 * Sistema de Objetos do Kernel Bible
 * 
 * Este sistema implementa um gerenciamento de objetos orientado a objetos
 * para o kernel, permitindo herança, polimorfismo e gerenciamento de
 * ciclo de vida automático.
 */

/*
 * Tipos de objetos do kernel
 */
typedef enum {
	BK_OBJ_TYPE_INVALID	= 0,	/* Tipo inválido/reservado */
	
	/* Objetos fundamentais */
	BK_OBJ_TYPE_OBJECT	= 1,	/* Objeto base (abstrato) */
	BK_OBJ_TYPE_REFCOUNTED	= 2,	/* Objeto com contagem de referências */
	BK_OBJ_TYPE_LOCKABLE	= 3,	/* Objeto que pode ser bloqueado */
	BK_OBJ_TYPE_WAITABLE	= 4,	/* Objeto que pode ser esperado */
	
	/* Objetos de sistema */
	BK_OBJ_TYPE_PROCESS	= 100,	/* Processo */
	BK_OBJ_TYPE_THREAD	= 101,	/* Thread */
	BK_OBJ_TYPE_TASK	= 102,	/* Task */
	BK_OBJ_TYPE_ACTIVITY	= 103,	/* Atividade */
	
	/* Objetos de memória */
	BK_OBJ_TYPE_VM_ADDRSPACE = 200,	/* Espaço de endereço virtual */
	BK_OBJ_TYPE_VM_REGION	= 201,	/* Região de memória virtual */
	BK_OBJ_TYPE_PM_PAGE	= 202,	/* Página de memória física */
	BK_OBJ_TYPE_PM_ZONE	= 203,	/* Zona de memória física */
	
	/* Objetos de IPC */
	BK_OBJ_TYPE_IPC_SPACE	= 300,	/* Espaço IPC */
	BK_OBJ_TYPE_IPC_PORT	= 301,	/* Porta IPC */
	BK_OBJ_TYPE_IPC_CHANNEL	= 302,	/* Canal IPC */
	BK_OBJ_TYPE_IPC_MESSAGE	= 303,	/* Mensagem IPC */
	BK_OBJ_TYPE_IPC_SHM	= 304,	/* Memória compartilhada */
	BK_OBJ_TYPE_IPC_SEM	= 305,	/* Semáforo IPC */
	BK_OBJ_TYPE_IPC_MUTEX	= 306,	/* Mutex IPC */
	BK_OBJ_TYPE_IPC_COND	= 307,	/* Variável de condição IPC */
	
	/* Objetos de arquivos/sistema de arquivos */
	BK_OBJ_TYPE_VNODE	= 400,	/* Vnode (nó virtual do filesystem) */
	BK_OBJ_TYPE_FILE	= 401,	/* Arquivo aberto */
	BK_OBJ_TYPE_DIR		= 402,	/* Diretório */
	BK_OBJ_TYPE_DEVICE	= 403,	/* Dispositivo */
	BK_OBJ_TYPE_PIPE	= 404,	/* Pipe */
	BK_OBJ_TYPE_SOCKET	= 405,	/* Socket */
	BK_OBJ_TYPE_MOUNT	= 406,	/* Ponto de montagem */
	
	/* Objetos de driver/dispositivo */
	BK_OBJ_TYPE_DRIVER	= 500,	/* Driver */
	BK_OBJ_TYPE_DEVICE_INST	= 501,	/* Instância de dispositivo */
	BK_OBJ_TYPE_BUS		= 502,	/* Barramento */
	BK_OBJ_TYPE_INTERRUPT	= 503,	/* Interrupção */
	BK_OBJ_TYPE_DMA_CHANNEL	= 504,	/* Canal DMA */
	
	/* Objetos de rede */
	BK_OBJ_TYPE_NETIF	= 600,	/* Interface de rede */
	BK_OBJ_TYPE_NET_SOCKET	= 601,	/* Socket de rede */
	BK_OBJ_TYPE_NET_CONN	= 602,	/* Conexão de rede */
	BK_OBJ_TYPE_NET_BUFFER	= 603,	/* Buffer de rede */
	
	/* Objetos de segurança */
	BK_OBJ_TYPE_CAPABILITY	= 700,	/* Capability */
	BK_OBJ_TYPE_TOKEN	= 701,	/* Token de segurança */
	BK_OBJ_TYPE_ACL		= 702,	/* Lista de controle de acesso */
	BK_OBJ_TYPE_SESSION	= 703,	/* Sessão de segurança */
	
	/* Objetos de tempo */
	BK_OBJ_TYPE_TIMER	= 800,	/* Timer */
	BK_OBJ_TYPE_CLOCK	= 801,	/* Relógio */
	BK_OBJ_TYPE_EVENT	= 802,	/* Evento temporal */
	
	/* Objetos do usuário (definidos pelo usuário) */
	BK_OBJ_TYPE_USER	= 1000,	/* Base para objetos do usuário */
	
	BK_OBJ_TYPE_MAX		= 0xFFFF	/* Máximo valor de tipo */
} BK_OBJ_TYPE;

/*
 * Flags de objeto
 */
#define BK_OBJ_FLAG_NONE		0x00000000
#define BK_OBJ_FLAG_PERSISTENT		0x00000001	/* Objeto persistente */
#define BK_OBJ_FLAG_VOLATILE		0x00000002	/* Objeto volátil */
#define BK_OBJ_FLAG_SHARED		0x00000004	/* Objeto compartilhado */
#define BK_OBJ_FLAG_PRIVATE		0x00000008	/* Objeto privado */
#define BK_OBJ_FLAG_KERNEL		0x00000010	/* Objeto do kernel */
#define BK_OBJ_FLAG_USER		0x00000020	/* Objeto do usuário */
#define BK_OBJ_FLAG_SYSTEM		0x00000040	/* Objeto do sistema */
#define BK_OBJ_FLAG_DYNAMIC		0x00000080	/* Objeto alocado dinamicamente */
#define BK_OBJ_FLAG_STATIC		0x00000100	/* Objeto alocado estaticamente */
#define BK_OBJ_FLAG_INHERITABLE		0x00000200	/* Herdável por filhos */
#define BK_OBJ_FLAG_TRANSFERABLE	0x00000400	/* Transferível entre processos */
#define BK_OBJ_FLAG_SECURE		0x00000800	/* Objeto seguro (criptografado) */
#define BK_OBJ_FLAG_VALIDATED		0x00001000	/* Objeto validado */
#define BK_OBJ_FLAG_LOCKED		0x00002000	/* Objeto bloqueado */
#define BK_OBJ_FLAG_DELETED		0x00004000	/* Objeto marcado para deleção */
#define BK_OBJ_FLAG_ZOMBIE		0x00008000	/* Objeto zombie */

/*
 * Permissões de objeto
 */
#define BK_OBJ_PERM_NONE		0x00
#define BK_OBJ_PERM_READ		0x01	/* Leitura */
#define BK_OBJ_PERM_WRITE		0x02	/* Escrita */
#define BK_OBJ_PERM_EXECUTE		0x04	/* Execução */
#define BK_OBJ_PERM_DELETE		0x08	/* Deleção */
#define BK_OBJ_PERM_CHMOD		0x10	/* Alterar permissões */
#define BK_OBJ_PERM_TAKE_OWNERSHIP	0x20	/* Tomar posse */
#define BK_OBJ_PERM_ALL			(BK_OBJ_PERM_READ | BK_OBJ_PERM_WRITE | \
					 BK_OBJ_PERM_EXECUTE | BK_OBJ_PERM_DELETE | \
					 BK_OBJ_PERM_CHMOD | BK_OBJ_PERM_TAKE_OWNERSHIP)

/*
 * Estrutura base de objeto do kernel
 */
struct __bk_kernl_object {
	/* Cabeçalho do objeto (deve ser o primeiro campo) */
	BK_OBJ_TYPE		ko_type;	/* Tipo do objeto */
	BK_UINT32		ko_flags;	/* Flags do objeto */
	BK_REFCOUNT		ko_refcount;	/* Contador de referências */
	BK_SIZE			ko_size;	/* Tamanho total do objeto */
	
	/* Identificação */
	BK_ID			ko_id;		/* ID único do objeto */
	BK_ID			ko_parent_id;	/* ID do objeto pai */
	const char		*ko_name;	/* Nome do objeto (opcional) */
	
	/* Metadados */
	BK_TIME			ko_creation_time;	/* Tempo de criação */
	BK_TIME			ko_modification_time;	/* Tempo de modificação */
	BK_TIME			ko_access_time;		/* Tempo de último acesso */
	
	/* Permissões */
	BK_UINT32		ko_perms;	/* Permissões do objeto */
	BK_UID			ko_owner;	/* Dono do objeto */
	BK_GID			ko_group;	/* Grupo do objeto */
	
	/* Ponteiro para a classe do objeto */
	const struct __bk_object_class *ko_class;
	
	/* Dados específicos do tipo (alinhado) */
	union {
		void		*ko_data;	/* Dados genéricos */
		BK_UL3264	ko_align;	/* Para alinhamento */
	};
	
	/* Listas */
	BK_LIST_ENTRY(__bk_kernl_object) ko_global_link;	/* Link na lista global */
	BK_LIST_ENTRY(__bk_kernl_object) ko_parent_link;	/* Link na lista do pai */
	BK_LIST_ENTRY(__bk_kernl_object) ko_type_link;	/* Link na lista por tipo */
	
	/* Lock para sincronização */
	BK_SPINLOCK		ko_lock;	/* Lock do objeto */
	
	/* Callbacks (opcionais) */
	void			(*ko_destructor)(struct __bk_kernl_object *);
	void			(*ko_validator)(struct __bk_kernl_object *);
	void			(*ko_cloner)(struct __bk_kernl_object *, 
					     struct __bk_kernl_object *);
};

typedef struct __bk_kernl_object BK_KERNL_OBJECT;

/* Definição do tipo de handle para objetos */
typedef BK_KERNL_OBJECT *BK_OBJECT_HANDLE;

/*
 * Classe de objeto (vtable-like structure)
 */
struct __bk_object_class {
	const char			*oc_name;		/* Nome da classe */
	BK_OBJ_TYPE			oc_type;		/* Tipo da classe */
	BK_SIZE			oc_instance_size;	/* Tamanho da instância */
	
	/* Operações da classe (métodos virtuais) */
	BK_KERNL_OBJECT*	(*oc_create)(BK_SIZE size, BK_UINT32 flags);
	BK_I32			(*oc_destroy)(BK_KERNL_OBJECT *obj);
	BK_I32			(*oc_clone)(BK_KERNL_OBJECT *src, 
					    BK_KERNL_OBJECT **dst);
	BK_I32			(*oc_validate)(BK_KERNL_OBJECT *obj);
	BK_I32			(*oc_compare)(BK_KERNL_OBJECT *obj1, 
					      BK_KERNL_OBJECT *obj2);
	
	/* Operações de I/O (se aplicável) */
	BK_SIZE			(*oc_read)(BK_KERNL_OBJECT *obj, void *buf, 
					   BK_SIZE size, BK_OFF offset);
	BK_SIZE			(*oc_write)(BK_KERNL_OBJECT *obj, const void *buf, 
					    BK_SIZE size, BK_OFF offset);
	
	/* Operações de controle */
	BK_I32			(*oc_ioctl)(BK_KERNL_OBJECT *obj, BK_UINT32 cmd, 
					    void *arg);
	BK_I32			(*oc_mmap)(BK_KERNL_OBJECT *obj, void *addr, 
					   BK_SIZE size, BK_UINT32 prot, 
					   BK_UINT32 flags, BK_OFF offset);
	
	/* Operações de sincronização (se aplicável) */
	BK_I32			(*oc_lock)(BK_KERNL_OBJECT *obj, BK_TIME timeout);
	BK_I32			(*oc_unlock)(BK_KERNL_OBJECT *obj);
	BK_I32			(*oc_wait)(BK_KERNL_OBJECT *obj, BK_TIME timeout);
	BK_I32			(*oc_signal)(BK_KERNL_OBJECT *obj);
	
	/* Operações de listagem/enumeração (se aplicável) */
	BK_I32			(*oc_get_children)(BK_KERNL_OBJECT *obj, 
						   BK_KERNL_OBJECT ***children, 
						   BK_UI32 *count);
	BK_I32			(*oc_add_child)(BK_KERNL_OBJECT *parent, 
						BK_KERNL_OBJECT *child);
	BK_I32			(*oc_remove_child)(BK_KERNL_OBJECT *parent, 
						   BK_KERNL_OBJECT *child);
	
	/* Informações de debug */
	void			(*oc_dump)(BK_KERNL_OBJECT *obj);
	const char*		(*oc_to_string)(BK_KERNL_OBJECT *obj);
};

typedef struct __bk_object_class BK_OBJECT_CLASS;

/*
 * Estrutura para objetos com contagem de referências
 */
struct __bk_refcounted_object {
	BK_KERNL_OBJECT		ro_base;	/* Objeto base */
	BK_REFCOUNT		ro_refcount;	/* Contador de referências */
	void			(*ro_cleanup)(struct __bk_refcounted_object *);
	BK_LIST_HEAD(, BK_KERNL_OBJECT) ro_dependents;	/* Objetos dependentes */
};

typedef struct __bk_refcounted_object BK_REFCOUNTED_OBJECT;

/*
 * Estrutura para objetos bloqueáveis
 */
struct __bk_lockable_object {
	BK_KERNL_OBJECT		lo_base;	/* Objeto base */
	BK_MUTEX		lo_mutex;	/* Mutex para exclusão mútua */
	BK_UINT32		lo_lock_depth;	/* Profundidade do lock */
	BK_THREAD		*lo_owner;	/* Thread proprietária */
	BK_TIME			lo_lock_time;	/* Tempo do lock */
};

typedef struct __bk_lockable_object BK_LOCKABLE_OBJECT;

/*
 * Estrutura para objetos esperáveis
 */
struct __bk_waitable_object {
	BK_KERNL_OBJECT		wo_base;	/* Objeto base */
	BK_CONDVAR		wo_condvar;	/* Variável de condição */
	BK_BOOLEAN		wo_signaled;	/* Estado sinalizado */
	BK_THREAD_LIST		wo_waiters;	/* Threads esperando */
	BK_UINT32		wo_wait_count;	/* Número de waiters */
};

typedef struct __bk_waitable_object BK_WAITABLE_OBJECT;

/*
 * Manager de objetos do kernel
 */
struct __bk_object_manager {
	/* Tabela de hash para busca rápida por ID */
	struct __bk_object_hash_table	*om_hash_table;
	BK_UI32				om_hash_size;
	
	/* Listas por tipo */
	struct __bk_object_type_list {
		BK_LIST_HEAD(, BK_KERNL_OBJECT) otl_list;
		BK_UI32			otl_count;
		BK_SPINLOCK		otl_lock;
	} om_type_lists[BK_OBJ_TYPE_MAX + 1];
	
	/* Estatísticas */
	struct {
		BK_UI64		total_objects;
		BK_UI64		active_objects;
		BK_UI64		peak_objects;
		BK_UI64		allocated_memory;
		BK_UI64		freed_memory;
		BK_UI64		creation_count[BK_OBJ_TYPE_MAX + 1];
		BK_UI64		destruction_count[BK_OBJ_TYPE_MAX + 1];
	} om_stats;
	
	/* Classes registradas */
	BK_OBJECT_CLASS		*om_classes[BK_OBJ_TYPE_MAX + 1];
	
	/* Sincronização */
	BK_SPINLOCK		om_lock;	/* Lock global */
	BK_MUTEX		om_mutex;	/* Mutex para operações longas */
	
	/* Callbacks */
	void			(*om_low_memory_cb)(void);
	void			(*om_object_created_cb)(BK_KERNL_OBJECT *obj);
	void			(*om_object_destroyed_cb)(BK_KERNL_OBJECT *obj);
};

typedef struct __bk_object_manager BK_OBJECT_MANAGER;

/*
 * Macros para definição de classes de objeto
 */
#define BK_DECLARE_OBJECT_CLASS(name, type, size)			\
	static BK_OBJECT_CLASS name ## _class = {			\
		.oc_name = #name,					\
		.oc_type = (type),					\
		.oc_instance_size = (size),				\
	}

#define BK_DEFINE_OBJECT_CLASS(name, create_fn, destroy_fn, ...)	\
	BK_OBJECT_CLASS name ## _class = {				\
		.oc_name = #name,					\
		.oc_create = (create_fn),				\
		.oc_destroy = (destroy_fn),				\
		__VA_ARGS__						\
	}

#define BK_REGISTER_OBJECT_CLASS(type, class)				\
	bk_object_register_class((type), (class))

/*
 * Macros para acesso a objetos
 */
#define BK_OBJECT_CAST(type, obj)					\
	((type *)(obj))

#define BK_OBJECT_CONTAINER(ptr, type, member)				\
	((type *)((char *)(ptr) - __BK_OFFSETOF(type, member)))

#define BK_OBJECT_IS_TYPE(obj, type)					\
	((obj) && (obj)->ko_type == (type))

#define BK_OBJECT_HAS_FLAG(obj, flag)					\
	((obj) && ((obj)->ko_flags & (flag)))

#define BK_OBJECT_SET_FLAG(obj, flag)					\
	do { if (obj) (obj)->ko_flags |= (flag); } while (0)

#define BK_OBJECT_CLEAR_FLAG(obj, flag)				\
	do { if (obj) (obj)->ko_flags &= ~(flag); } while (0)

#define BK_OBJECT_CHECK_PERM(obj, perm)				\
	((obj) && ((obj)->ko_perms & (perm)))

/*
 * Funções de gerenciamento de objetos
 */

/* Inicialização do sistema de objetos */
BK_I32 bk_object_system_init(void);
void bk_object_system_shutdown(void);

/* Criação e destruição de objetos */
BK_KERNL_OBJECT *bk_object_create(BK_OBJ_TYPE type, BK_SIZE size, 
				 BK_UINT32 flags, const char *name);
BK_I32 bk_object_destroy(BK_KERNL_OBJECT *obj);
BK_I32 bk_object_destroy_by_id(BK_ID obj_id);

/* Referências */
BK_KERNL_OBJECT *bk_object_ref(BK_KERNL_OBJECT *obj);
BK_I32 bk_object_unref(BK_KERNL_OBJECT *obj);
BK_REFCOUNT bk_object_refcount(BK_KERNL_OBJECT *obj);

/* Busca de objetos */
BK_KERNL_OBJECT *bk_object_find_by_id(BK_ID obj_id);
BK_KERNL_OBJECT *bk_object_find_by_name(const char *name, BK_OBJ_TYPE type);
BK_I32 bk_object_find_by_type(BK_OBJ_TYPE type, BK_KERNL_OBJECT ***objs, 
			     BK_UI32 *count);

/* Clonagem de objetos */
BK_KERNL_OBJECT *bk_object_clone(BK_KERNL_OBJECT *src, BK_UINT32 flags);

/* Classes de objeto */
BK_I32 bk_object_register_class(BK_OBJ_TYPE type, BK_OBJECT_CLASS *cls);
BK_OBJECT_CLASS *bk_object_get_class(BK_OBJ_TYPE type);
BK_I32 bk_object_unregister_class(BK_OBJ_TYPE type);

/* Operações em objetos */
BK_I32 bk_object_lock(BK_KERNL_OBJECT *obj, BK_TIME timeout);
BK_I32 bk_object_unlock(BK_KERNL_OBJECT *obj);
BK_I32 bk_object_wait(BK_KERNL_OBJECT *obj, BK_TIME timeout);
BK_I32 bk_object_signal(BK_KERNL_OBJECT *obj);

/* I/O em objetos */
BK_SIZE bk_object_read(BK_KERNL_OBJECT *obj, void *buffer, BK_SIZE size, 
		      BK_OFF offset);
BK_SIZE bk_object_write(BK_KERNL_OBJECT *obj, const void *buffer, BK_SIZE size, 
		       BK_OFF offset);
BK_I32 bk_object_ioctl(BK_KERNL_OBJECT *obj, BK_UINT32 cmd, void *arg);
BK_I32 bk_object_mmap(BK_KERNL_OBJECT *obj, void *addr, BK_SIZE size, 
		     BK_UINT32 prot, BK_UINT32 flags, BK_OFF offset);

/* Hierarquia de objetos */
BK_I32 bk_object_set_parent(BK_KERNL_OBJECT *obj, BK_KERNL_OBJECT *parent);
BK_KERNL_OBJECT *bk_object_get_parent(BK_KERNL_OBJECT *obj);
BK_I32 bk_object_get_children(BK_KERNL_OBJECT *obj, BK_KERNL_OBJECT ***children, 
			     BK_UI32 *count);
BK_I32 bk_object_add_child(BK_KERNL_OBJECT *parent, BK_KERNL_OBJECT *child);
BK_I32 bk_object_remove_child(BK_KERNL_OBJECT *parent, BK_KERNL_OBJECT *child);

/* Permissões e segurança */
BK_I32 bk_object_set_perms(BK_KERNL_OBJECT *obj, BK_UINT32 perms);
BK_I32 bk_object_get_perms(BK_KERNL_OBJECT *obj, BK_UINT32 *perms);
BK_I32 bk_object_set_owner(BK_KERNL_OBJECT *obj, BK_UID uid, BK_GID gid);
BK_I32 bk_object_get_owner(BK_KERNL_OBJECT *obj, BK_UID *uid, BK_GID *gid);
BK_I32 bk_object_check_access(BK_KERNL_OBJECT *obj, BK_UINT32 required_perms);

/* Validação e integridade */
BK_I32 bk_object_validate(BK_KERNL_OBJECT *obj);
BK_I32 bk_object_validate_all(void);
BK_I32 bk_object_mark_valid(BK_KERNL_OBJECT *obj);
BK_I32 bk_object_mark_invalid(BK_KERNL_OBJECT *obj);

/* Estatísticas e monitoramento */
struct __bk_object_stats {
	BK_UI64		total_objects;
	BK_UI64		active_objects;
	BK_UI64		peak_objects;
	BK_UI64		total_memory;
	BK_UI64		by_type[BK_OBJ_TYPE_MAX + 1];
	BK_TIME		uptime;
};

BK_I32 bk_object_get_stats(struct __bk_object_stats *stats);
BK_I32 bk_object_reset_stats(void);

/* Gerenciamento de memória */
BK_I32 bk_object_gc(void);	/* Garbage collection */
BK_I32 bk_object_purge(void);	/* Purge de objetos não referenciados */
BK_I32 bk_object_compact(void);	/* Compactação de memória */

/* Debug e diagnóstico */
#ifdef _BK_OBJECT_DEBUG
void bk_object_dump(BK_KERNL_OBJECT *obj);
void bk_object_dump_all(void);
void bk_object_dump_by_type(BK_OBJ_TYPE type);
void bk_object_dump_stats(void);
BK_I32 bk_object_check_integrity(void);
BK_I32 bk_object_validate_references(void);
#endif

/* Funções de utilidade */
static __BK_ALWAYS_INLINE BK_BOOLEAN
bk_object_is_valid(BK_KERNL_OBJECT *obj)
{
	return (obj != NULL && 
		obj->ko_type > BK_OBJ_TYPE_INVALID && 
		obj->ko_type <= BK_OBJ_TYPE_MAX &&
		!(obj->ko_flags & BK_OBJ_FLAG_DELETED));
}

static __BK_ALWAYS_INLINE BK_BOOLEAN
bk_object_is_alive(BK_KERNL_OBJECT *obj)
{
	return (bk_object_is_valid(obj) && 
		!(obj->ko_flags & (BK_OBJ_FLAG_ZOMBIE | BK_OBJ_FLAG_DELETED)));
}

static __BK_ALWAYS_INLINE BK_ID
bk_object_get_id(BK_KERNL_OBJECT *obj)
{
	return obj ? obj->ko_id : BK_ID_INVALID;
}

static __BK_ALWAYS_INLINE const char *
bk_object_get_name(BK_KERNL_OBJECT *obj)
{
	return obj ? obj->ko_name : NULL;
}

static __BK_ALWAYS_INLINE BK_OBJ_TYPE
bk_object_get_type(BK_KERNL_OBJECT *obj)
{
	return obj ? obj->ko_type : BK_OBJ_TYPE_INVALID;
}

static __BK_ALWAYS_INLINE BK_SIZE
bk_object_get_size(BK_KERNL_OBJECT *obj)
{
	return obj ? obj->ko_size : 0;
}

static __BK_ALWAYS_INLINE BK_TIME
bk_object_get_creation_time(BK_KERNL_OBJECT *obj)
{
	return obj ? obj->ko_creation_time : 0;
}

/* Macros para criação de tipos específicos */
#define BK_CREATE_OBJECT(type, name, size, flags)			\
	bk_object_create((type), (size), (flags), (name))

#define BK_DESTROY_OBJECT(obj)		bk_object_destroy((obj))

#define BK_REF_OBJECT(obj)		bk_object_ref((obj))
#define BK_UNREF_OBJECT(obj)		bk_object_unref((obj))

#define BK_LOCK_OBJECT(obj)		bk_object_lock((obj), BK_TIME_INFINITE)
#define BK_UNLOCK_OBJECT(obj)		bk_object_unlock((obj))

/*
 * Funções para tipos específicos de objetos
 */

/* Objetos com contagem de referências */
BK_REFCOUNTED_OBJECT *bk_refcounted_object_create(BK_SIZE size, BK_UINT32 flags);
BK_I32 bk_refcounted_object_add_ref(BK_REFCOUNTED_OBJECT *obj);
BK_I32 bk_refcounted_object_release(BK_REFCOUNTED_OBJECT *obj);
BK_I32 bk_refcounted_object_add_dependent(BK_REFCOUNTED_OBJECT *obj, 
					 BK_KERNL_OBJECT *dependent);
BK_I32 bk_refcounted_object_remove_dependent(BK_REFCOUNTED_OBJECT *obj, 
					    BK_KERNL_OBJECT *dependent);

/* Objetos bloqueáveis */
BK_LOCKABLE_OBJECT *bk_lockable_object_create(BK_SIZE size, BK_UINT32 flags);
BK_I32 bk_lockable_object_lock(BK_LOCKABLE_OBJECT *obj, BK_TIME timeout);
BK_I32 bk_lockable_object_unlock(BK_LOCKABLE_OBJECT *obj);
BK_BOOLEAN bk_lockable_object_is_locked(BK_LOCKABLE_OBJECT *obj);
BK_THREAD *bk_lockable_object_get_owner(BK_LOCKABLE_OBJECT *obj);

/* Objetos esperáveis */
BK_WAITABLE_OBJECT *bk_waitable_object_create(BK_SIZE size, BK_UINT32 flags);
BK_I32 bk_waitable_object_wait(BK_WAITABLE_OBJECT *obj, BK_TIME timeout);
BK_I32 bk_waitable_object_signal(BK_WAITABLE_OBJECT *obj);
BK_I32 bk_waitable_object_broadcast(BK_WAITABLE_OBJECT *obj);
BK_BOOLEAN bk_waitable_object_is_signaled(BK_WAITABLE_OBJECT *obj);

/*
 * Callbacks para eventos de objeto
 */
typedef void (*BK_OBJECT_CREATED_CB)(BK_KERNL_OBJECT *obj, void *user_data);
typedef void (*BK_OBJECT_DESTROYED_CB)(BK_KERNL_OBJECT *obj, void *user_data);
typedef void (*BK_OBJECT_MODIFIED_CB)(BK_KERNL_OBJECT *obj, void *user_data);
typedef void (*BK_OBJECT_ACCESSED_CB)(BK_KERNL_OBJECT *obj, void *user_data);

BK_I32 bk_object_register_callback(BK_OBJ_TYPE type, BK_UINT32 event_mask,
				  void (*callback)(BK_KERNL_OBJECT *, void *),
				  void *user_data);
BK_I32 bk_object_unregister_callback(BK_OBJ_TYPE type, BK_UINT32 event_mask,
				    void (*callback)(BK_KERNL_OBJECT *, void *));

/*
 * Constantes e definições
 */
#define BK_OBJECT_ID_INVALID		((BK_ID)0)
#define BK_OBJECT_NAME_MAX		256
#define BK_OBJECT_DEFAULT_SIZE		sizeof(BK_KERNL_OBJECT)

/* Eventos de objeto */
#define BK_OBJECT_EVENT_CREATED		0x0001
#define BK_OBJECT_EVENT_DESTROYED	0x0002
#define BK_OBJECT_EVENT_MODIFIED	0x0004
#define BK_OBJECT_EVENT_ACCESSED	0x0008
#define BK_OBJECT_EVENT_REFCOUNT_CHANGED	0x0010
#define BK_OBJECT_EVENT_PERMS_CHANGED	0x0020
#define BK_OBJECT_EVENT_OWNER_CHANGED	0x0040
#define BK_OBJECT_EVENT_ALL_EVENTS	0xFFFF

/* Erros específicos de objetos */
#define BK_OBJECT_SUCCESS		0
#define BK_OBJECT_EINVAL		1	/* Objeto inválido */
#define BK_OBJECT_ENOMEM		2	/* Sem memória */
#define BK_OBJECT_ENOENT		3	/* Objeto não encontrado */
#define BK_OBJECT_EEXIST		4	/* Objeto já existe */
#define BK_OBJECT_EACCES		5	/* Acesso negado */
#define BK_OBJECT_EPERM			6	/* Permissão negada */
#define BK_OBJECT_EBUSY			7	/* Objeto ocupado */
#define BK_OBJECT_ETIMEDOUT		8	/* Timeout */
#define BK_OBJECT_EAGAIN		9	/* Tente novamente */
#define BK_OBJECT_ENOTSUP		10	/* Não suportado */
#define BK_OBJECT_EINVALIDTYPE		11	/* Tipo inválido */
#define BK_OBJECT_ECORRUPT		12	/* Objeto corrompido */


#endif /* !_BIBLE_KERNL_OBJECT_H_ */
