#ifndef _BIBLE_BIBLEFS_H_
#define _BIBLE_BIBLEFS_H_

/*
 * biblefs.h - Bible File System para RAM
 * 
 * RELAÇÃO COM A FILOSOFIA:
 * ┌─────────────────────────────────────────────────────────────┐
 * │                                                             │
 * │  KERNEL (Tudo é Registro)                                   │
 * │  └── Processos, Tasks, Threads, Atividades, Capacidades    │
 * │                                                             │
 * │         ↓ (software usuário despeja dados)                 │
 * │                                                             │
 * │  RAM (BIBLEFS)                                              │
 * │  ├── Funciona como "C:\ da RAM"                            │
 * │  ├── Sistemas de Arquivos na RAM (voláteis)                │
 * │  ├── Usado junto com FS de disco                            │
 * │  └── Usa as syscalls de arquivo (BIBLE_OPEN, BIBLE_READ...)│
 * │                                                             │
 * │         ↓ (persistência)                                    │
 * │                                                             │
 * │  DISCO (FS tradicional)                                     │
 * │  └── Sistema de arquivos persistente                        │
 * │                                                             │
 * └─────────────────────────────────────────────────────────────┘
 * 
 * NOTA: Diferente do TMPFS, BIBLEFS é um sistema de arquivos
 * completo na RAM, usado para performance, não apenas para
 * dados temporários. Mas SÓ existe porque o software usuário
 * despejou dados na RAM - não é a filosofia padrão do kernel!
 * 
 * NÃO TEM INODES! A estrutura é simplificada para performance
 * máxima na RAM.
 */

#include <bible/cdefs.h>
#include <bible/types.h>
#include <bible/queue.h>
#include <bible/regsister.h>
#include <bible/syscalls.h>

/*
 * ============================================================================
 * CONSTANTES DO BIBLEFS
 * ============================================================================
 */

/* Tamanhos padrão */
#define BIBLEFS_BLOCK_SIZE		4096	/* 4KB blocks */
#define BIBLEFS_NAME_MAX		255	/* Tamanho máximo de nome */
#define BIBLEFS_PATH_MAX		4096	/* Tamanho máximo de path */
#define BIBLEFS_MAX_FILES		65536	/* Máximo de arquivos por FS */

/* Tipos de entrada no BIBLEFS */
#define BIBLEFS_TYPE_FILE		0x01	/* Arquivo regular */
#define BIBLEFS_TYPE_DIR		0x02	/* Diretório */
#define BIBLEFS_TYPE_SYMLINK		0x03	/* Link simbólico */
#define BIBLEFS_TYPE_DEVICE		0x04	/* Dispositivo */
#define BIBLEFS_TYPE_PIPE		0x05	/* Pipe */
#define BIBLEFS_TYPE_SOCKET		0x06	/* Socket */

/* Flags de abertura */
#define BIBLEFS_O_READ			0x0001	/* Leitura */
#define BIBLEFS_O_WRITE			0x0002	/* Escrita */
#define BIBLEFS_O_EXEC			0x0004	/* Execução */
#define BIBLEFS_O_CREATE		0x0008	/* Criar se não existir */
#define BIBLEFS_O_EXCL			0x0010	/* Exclusivo (com CREATE) */
#define BIBLEFS_O_TRUNC			0x0020	/* Truncar */
#define BIBLEFS_O_APPEND		0x0040	/* Append */
#define BIBLEFS_O_SYNC			0x0080	/* Escrita síncrona */
#define BIBLEFS_O_DIRECT		0x0100	/* I/O direto (sem cache) */
#define BIBLEFS_O_TMPFILE		0x0200	/* Arquivo temporário */

/* Flags de busca */
#define BIBLEFS_SEEK_SET		0	/* Início do arquivo */
#define BIBLEFS_SEEK_CUR		1	/* Posição atual */
#define BIBLEFS_SEEK_END		2	/* Final do arquivo */

/* Permissões (estilo Unix) */
#define BIBLEFS_IRUSR			0x100	/* Dono pode ler */
#define BIBLEFS_IWUSR			0x080	/* Dono pode escrever */
#define BIBLEFS_IXUSR			0x040	/* Dono pode executar */
#define BIBLEFS_IRGRP			0x020	/* Grupo pode ler */
#define BIBLEFS_IWGRP			0x010	/* Grupo pode escrever */
#define BIBLEFS_IXGRP			0x008	/* Grupo pode executar */
#define BIBLEFS_IROTH			0x004	/* Outros podem ler */
#define BIBLEFS_IWOTH			0x002	/* Outros podem escrever */
#define BIBLEFS_IXOTH			0x001	/* Outros podem executar */

#define BIBLEFS_DEFAULT_PERMS		0x1FF	/* 0777 */

/*
 * ============================================================================
 * ESTRUTURAS DO BIBLEFS (SEM INODES!)
 * ============================================================================
 */

/* Estatísticas de arquivo (compatível com syscalls) */
struct biblefs_stat {
	BK_DEV		st_dev;		/* Dispositivo */
	BK_UI64		st_ino;		/* Número de arquivo (não é inode!) */
	BK_UINT32	st_mode;	/* Tipo + permissões */
	BK_UINT32	st_nlink;	/* Número de links */
	BK_UID		st_uid;		/* User ID */
	BK_GID		st_gid;		/* Group ID */
	BK_DEV		st_rdev;	/* Device ID (se for device) */
	BK_OFF		st_size;	/* Tamanho em bytes */
	BK_TIME		st_atime;	/* Último acesso */
	BK_TIME		st_mtime;	/* Última modificação */
	BK_TIME		st_ctime;	/* Última mudança de status */
	BK_BLKSIZE	st_blksize;	/* Tamanho de bloco preferencial */
	BK_BLKCNT	st_blocks;	/* Número de blocos alocados */
};

/* Entrada de diretório */
struct biblefs_dirent {
	BK_UI64		d_ino;		/* Número do arquivo */
	BK_UINT16	d_reclen;	/* Tamanho deste registro */
	BK_UINT8	d_type;		/* Tipo do arquivo */
	BK_UINT8	d_namlen;	/* Tamanho do nome */
	char		d_name[BIBLEFS_NAME_MAX + 1];	/* Nome */
};

/* Estatísticas do sistema de arquivos */
struct biblefs_statfs {
	BK_I32		f_type;		/* Tipo do FS */
	BK_I32		f_flags;	/* Flags */
	BK_UI64		f_bsize;	/* Tamanho de bloco */
	BK_UI64		f_blocks;	/* Total de blocos */
	BK_UI64		f_bfree;	/* Blocos livres */
	BK_UI64		f_bavail;	/* Blocos disponíveis */
	BK_UI64		f_files;	/* Total de arquivos */
	BK_UI64		f_ffree;	/* Arquivos livres */
	BK_UI64		f_fsid;		/* ID do FS */
	BK_I32		f_namelen;	/* Tamanho máximo de nome */
};

/*
 * ============================================================================
 * ESTRUTURAS INTERNAS (APENAS PARA O KERNEL)
 * ============================================================================
 */

#ifdef _BK_KERNEL

/* Bloco de dados do BIBLEFS */
struct biblefs_block {
	BK_UI64			b_number;	/* Número do bloco */
	BK_UINT32		b_refcount;	/* Contagem de referências */
	BK_UINT8		b_data[BIBLEFS_BLOCK_SIZE]; /* Dados */
	BK_LIST_ENTRY(biblefs_block) b_list;	/* Lista de blocos */
};

/* Arquivo no BIBLEFS (sem inode!) */
struct biblefs_file {
	BK_UI64			f_number;	/* Número único do arquivo */
	BK_UINT32		f_type;		/* Tipo (arquivo, dir, etc) */
	BK_UINT32		f_mode;		/* Permissões */
	BK_UINT32		f_flags;	/* Flags internas */
	
	/* Dono e grupo */
	BK_UID			f_uid;
	BK_GID			f_gid;
	
	/* Links */
	BK_UINT32		f_nlink;	/* Número de links */
	
	/* Tamanho e blocos */
	BK_OFF			f_size;		/* Tamanho em bytes */
	BK_UINT32		f_block_count;	/* Número de blocos */
	BK_LIST_HEAD(, biblefs_block) f_blocks;	/* Lista de blocos */
	
	/* Tempos */
	BK_TIME			f_atime;	/* Último acesso */
	BK_TIME			f_mtime;	/* Última modificação */
	BK_TIME			f_ctime;	/* Última mudança de status */
	
	/* Para diretórios */
	struct biblefs_dir	*f_dir;		/* Se for diretório */
	
	/* Para links simbólicos */
	char			*f_symlink;	/* Target do link */
	
	/* Listas */
	BK_TAILQ_ENTRY(biblefs_file) f_list;	/* Lista global */
	BK_HASH_ENTRY(biblefs_file) f_hash;	/* Tabela hash por número */
};

/* Diretório no BIBLEFS */
struct biblefs_dir {
	BK_RWLOCK		d_lock;		/* Lock do diretório */
	BK_UINT32		d_count;	/* Número de entradas */
	
	/* Entradas do diretório (nome → arquivo) */
	struct bk_hash_table	*d_entries;
};

/* Sistema de arquivos BIBLEFS montado */
struct biblefs_mount {
	char			m_path[BIBLEFS_PATH_MAX]; /* Ponto de montagem */
	
	/* Estatísticas */
	BK_UI64			m_total_blocks;	/* Total de blocos */
	BK_UI64			m_free_blocks;	/* Blocos livres */
	BK_UI64			m_total_files;	/* Total de arquivos */
	BK_UI64			m_free_files;	/* Arquivos livres */
	
	/* Próximo número de arquivo */
	BK_UI64			m_next_file_number;
	
	/* Tabela de arquivos */
	BK_TABLE_HEAD(, biblefs_file) m_files;
	BK_HASH_TABLE		m_file_hash;	/* Hash por número */
	
	/* Blocos livres */
	BK_LIST_HEAD(, biblefs_block) m_free_blocks;
	BK_SPINLOCK		m_blocks_lock;
	
	/* Root do sistema */
	struct biblefs_file	*m_root;
	
	/* Lista de montagens */
	BK_LIST_ENTRY(biblefs_mount) m_list;
};

/*
 * ============================================================================
 * FUNÇÕES PÚBLICAS DO BIBLEFS (CHAMADAS PELAS SYSCALLS)
 * ============================================================================
 */

/* Inicialização do BIBLEFS */
BK_I32 biblefs_init(void);
void biblefs_shutdown(void);

/* Montagem e desmontagem */
BK_I32 biblefs_mount(const char *path, BK_UI64 total_blocks, BK_UI64 total_files);
BK_I32 biblefs_umount(const char *path);

/* Operações em arquivos (usadas pelas syscalls BIBLE_OPEN, etc) */
BK_I32 biblefs_open(const char *path, BK_I32 flags, BK_MODE mode, BK_I32 *fd);
BK_I32 biblefs_close(BK_I32 fd);
BK_SSIZE biblefs_read(BK_I32 fd, void *buf, BK_SIZE count);
BK_SSIZE biblefs_write(BK_I32 fd, const void *buf, BK_SIZE count);
BK_OFF biblefs_lseek(BK_I32 fd, BK_OFF offset, BK_I32 whence);
BK_I32 biblefs_stat(const char *path, struct biblefs_stat *st);
BK_I32 biblefs_fstat(BK_I32 fd, struct biblefs_stat *st);
BK_I32 biblefs_unlink(const char *path);
BK_I32 biblefs_mkdir(const char *path, BK_MODE mode);
BK_I32 biblefs_rmdir(const char *path);
BK_I32 biblefs_rename(const char *oldpath, const char *newpath);
BK_I32 biblefs_truncate(const char *path, BK_OFF length);
BK_I32 biblefs_ftruncate(BK_I32 fd, BK_OFF length);
BK_I32 biblefs_getdents(BK_I32 fd, struct biblefs_dirent *dirp, BK_UINT32 count);
BK_I32 biblefs_chmod(const char *path, BK_MODE mode);
BK_I32 biblefs_chown(const char *path, BK_UID uid, BK_GID gid);
BK_I32 biblefs_access(const char *path, BK_I32 mode);
BK_I32 biblefs_statfs(const char *path, struct biblefs_statfs *st);

/* Funções para gerenciamento de memória */
BK_I32 biblefs_sync(void);	/* Sincroniza dados (no caso da RAM, só marca) */
void *biblefs_map_file(BK_I32 fd, BK_OFF offset, BK_SIZE size);
BK_I32 biblefs_unmap_file(void *addr, BK_SIZE size);

/* Estatísticas */
BK_UI64 biblefs_get_total_blocks(void);
BK_UI64 biblefs_get_free_blocks(void);
BK_UI64 biblefs_get_total_files(void);
BK_UI64 biblefs_get_free_files(void);

/*
 * ============================================================================
 * FUNÇÕES INTERNAS (APENAS PARA O KERNEL)
 * ============================================================================
 */

/* Gerenciamento de blocos */
static inline struct biblefs_block *biblefs_alloc_block(void) {
	struct biblefs_block *block;
	
	block = bk_alloc(sizeof(struct biblefs_block));
	if (!block)
		return NULL;
	
	block->b_number = (BK_UI64)block;  /* Número baseado no endereço */
	block->b_refcount = 0;
	bk_memset(block->b_data, 0, BIBLEFS_BLOCK_SIZE);
	
	return block;
}

static inline void biblefs_free_block(struct biblefs_block *block) {
	if (!block)
		return;
	
	bk_free(block);
}

/* Conversão de permissões */
static inline BK_UINT32 biblefs_mode_to_prot(BK_UINT32 mode) {
	BK_UINT32 prot = 0;
	
	if (mode & BIBLEFS_IRUSR) prot |= BIBLEFS_O_READ;
	if (mode & BIBLEFS_IWUSR) prot |= BIBLEFS_O_WRITE;
	if (mode & BIBLEFS_IXUSR) prot |= BIBLEFS_O_EXEC;
	
	return prot;
}

/* Cache de blocos (opcional) */
#ifdef BIBLEFS_USE_CACHE
struct biblefs_cache {
	BK_HASH_TABLE	cache_hash;
	BK_SPINLOCK	cache_lock;
	BK_UINT32	cache_hits;
	BK_UINT32	cache_misses;
};

BK_I32 biblefs_cache_init(void);
void biblefs_cache_flush(void);
struct biblefs_block *biblefs_cache_lookup(BK_UI64 block_num);
void biblefs_cache_insert(struct biblefs_block *block);
#endif /* BIBLEFS_USE_CACHE */

#endif /* _BK_KERNEL */

/*
 * ============================================================================
 * MACROS DE CONVENIÊNCIA PARA USUÁRIO
 * ============================================================================
 */

/* Verificação de tipo de arquivo */
#define S_ISREG(m)	(((m) & 0xF000) == 0x8000)  /* Arquivo regular */
#define S_ISDIR(m)	(((m) & 0xF000) == 0x4000)  /* Diretório */
#define S_ISLNK(m)	(((m) & 0xF000) == 0xA000)  /* Link simbólico */
#define S_ISDEV(m)	(((m) & 0xF000) == 0x2000)  /* Dispositivo */

/* Extrair permissões */
#define S_IRWXU		(BIBLEFS_IRUSR|BIBLEFS_IWUSR|BIBLEFS_IXUSR)
#define S_IRUSR		BIBLEFS_IRUSR
#define S_IWUSR		BIBLEFS_IWUSR
#define S_IXUSR		BIBLEFS_IXUSR
#define S_IRWXG		(BIBLEFS_IRGRP|BIBLEFS_IWGRP|BIBLEFS_IXGRP)
#define S_IRGRP		BIBLEFS_IRGRP
#define S_IWGRP		BIBLEFS_IWGRP
#define S_IXGRP		BIBLEFS_IXGRP
#define S_IRWXO		(BIBLEFS_IROTH|BIBLEFS_IWOTH|BIBLEFS_IXOTH)
#define S_IROTH		BIBLEFS_IROTH
#define S_IWOTH		BIBLEFS_IWOTH
#define S_IXOTH		BIBLEFS_IXOTH

#endif /* !_BIBLE_BIBLEFS_H_ */
