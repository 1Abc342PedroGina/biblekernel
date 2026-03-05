#ifndef _BIBLE_BOFS_H_
#define _BIBLE_BOFS_H_

/*
 * bofs.h - Bible Original File System (FS de Disco)
 * 
 * FILOSOFIA DE NOMES:
 * ┌─────────────────────────────────────────────────────────────┐
 * │                                                             │
 * │  ~   = Meu Disco (disco principal)                          │
 * │  ()  = Indica que é um disco (ex: ~, Â, ^_^, ?)            │
 * │  /.\ = Separador de diretório                               │
 * │  /;\ = Separador de arquivo                                 │
 * │  /p  = Separador de arquivo (alternativo)                  │
 * │  ~   no início do nome = oculto do usuário                 │
 * │                                                             │
 * │  EXEMPLOS:                                                  │
 * │  ~(~)/.\User                 → Disco ~, diretório User     │
 * │  ~(~)/.\~Sys                 → Disco ~, diretório oculto   │
 * │  ~(~)/.\Soft/;\program.exe   → Arquivo no diretório Soft   │
 * │  Â(Â)/.\PASTA                → Disco Â, diretório PASTA    │
 * │                                                             │
 * └─────────────────────────────────────────────────────────────┘
 * 
 * ESTRUTURA DE DIRETÓRIOS OBRIGATÓRIA:
 * ┌─────────────────────────────────────────────────────────────┐
 * │  ~(~)/                        (Raiz do disco principal)    │
 * │  ├── /.\~Sys                  (Sistema - oculto)           │
 * │  ├── /.\~SysSoft              (Softwares padrão)           │
 * │  ├── /.\User                  (Usuários)                   │
 * │  ├── /.\bsr                   (Bible Software Resource)    │
 * │  └── /.\Soft                  (Softwares instalados)       │
 * └─────────────────────────────────────────────────────────────┘
 * 
 * REGRAS DE IGUALDADE DE ARQUIVOS:
 * Para dois arquivos serem considerados IGUAIS, TODOS devem ser iguais:
 * 1. Peso da pasta (metadata)
 * 2. Quantidade de arquivos dentro
 * 3. Nome dos arquivos
 * 4. Conteúdo dos arquivos
 * 5. Nome da pasta/arquivo (case-sensitive, ASCII vs ASCii são diferentes!)
 */

#include <bible/cdefs.h>
#include <bible/types.h>
#include <bible/queue.h>
#include <bible/regsister.h>

/*
 * ============================================================================
 * CONSTANTES DO BOFS
 * ============================================================================
 */

/* Tamanhos */
#define BOFS_BLOCK_SIZE		4096	/* 4KB blocks */
#define BOFS_NAME_MAX		255	/* Tamanho máximo de nome */
#define BOFS_PATH_MAX		4096	/* Tamanho máximo de path */
#define BOFS_DISK_NAME_MAX	32	/* Tamanho máximo nome do disco */

/* Separadores */
#define BOFS_DISK_START		'('
#define BOFS_DISK_END		')'
#define BOFS_DIR_SEP		"/.\\"	/* Separador de diretório */
#define BOFS_FILE_SEP1		"/;\\"	/* Separador de arquivo (opção 1) */
#define BOFS_FILE_SEP2		"/p"	/* Separador de arquivo (opção 2) */
#define BOFS_HIDDEN_PREFIX	'~'	/* Prefixo para itens ocultos */

/* Tipos de entrada */
#define BOFS_TYPE_FILE		0x01	/* Arquivo regular */
#define BOFS_TYPE_DIR		0x02	/* Diretório */
#define BOFS_TYPE_SYMLINK	0x03	/* Link simbólico */
#define BOFS_TYPE_DEVICE	0x04	/* Dispositivo */
#define BOFS_TYPE_PIPE		0x05	/* Pipe */
#define BOFS_TYPE_SOCKET	0x06	/* Socket */

/* Flags de abertura */
#define BOFS_O_READ		0x0001
#define BOFS_O_WRITE		0x0002
#define BOFS_O_EXEC		0x0004
#define BOFS_O_CREATE		0x0008
#define BOFS_O_EXCL		0x0010
#define BOFS_O_TRUNC		0x0020
#define BOFS_O_APPEND		0x0040

/*
 * ============================================================================
 * ESTRUTURAS DO BOFS
 * ============================================================================
 */

/* Identificador de disco */
struct bofs_disk_id {
	char		name[BOFS_DISK_NAME_MAX];	/* Nome do disco (ex: "~", "Â", "^_^") */
	BK_UINT64	serial;				/* Número serial único */
	BK_UINT64	sector_count;			/* Total de setores */
	BK_UINT64	sector_size;			/* Tamanho do setor */
};

/* Identificador único de arquivo (baseado em peso + conteúdo) */
struct bofs_file_id {
	BK_UINT64	weight;			/* Peso da pasta/arquivo */
	BK_UINT64	content_hash;		/* Hash do conteúdo */
	BK_UINT64	metadata_hash;		/* Hash dos metadados */
	BK_UINT32	file_count;		/* Quantidade de arquivos (se dir) */
};

/* Estatísticas de arquivo */
struct bofs_stat {
	struct bofs_disk_id	st_disk;	/* Disco */
	struct bofs_file_id	st_id;		/* ID único do arquivo */
	BK_UINT32		st_type;	/* Tipo */
	BK_UINT32		st_mode;	/* Permissões */
	BK_UINT32		st_nlink;	/* Número de links */
	BK_UID			st_uid;		/* User ID */
	BK_GID			st_gid;		/* Group ID */
	BK_OFF			st_size;	/* Tamanho em bytes */
	BK_TIME			st_atime;	/* Último acesso */
	BK_TIME			st_mtime;	/* Última modificação */
	BK_TIME			st_ctime;	/* Última mudança */
	BK_BLKSIZE		st_blksize;	/* Tamanho de bloco */
	BK_BLKCNT		st_blocks;	/* Número de blocos */
	BK_BOOL			st_hidden;	/* Se é oculto (começa com ~) */
};

/* Entrada de diretório */
struct bofs_dirent {
	struct bofs_file_id	d_id;		/* ID do arquivo */
	BK_UINT16		d_reclen;	/* Tamanho deste registro */
	BK_UINT8		d_type;		/* Tipo */
	BK_UINT16		d_namelen;	/* Tamanho do nome */
	BK_BOOL			d_hidden;	/* Se é oculto */
	char			d_name[BOFS_NAME_MAX + 1];	/* Nome */
};

/*
 * ============================================================================
 * ESTRUTURAS INTERNAS (APENAS PARA O KERNEL)
 * ============================================================================
 */

#ifdef _BK_KERNEL

/* Bloco no disco */
struct bofs_block {
	BK_UINT64		b_number;	/* Número do bloco */
	BK_UINT64		b_disk_sector;	/* Setor no disco */
	BK_UINT32		b_refcount;	/* Contagem de referências */
	BK_UINT8		*b_data;	/* Dados em cache */
	BK_LIST_ENTRY(bofs_block) b_list;	/* Lista de blocos */
};

/* Arquivo no BOFS */
struct bofs_file {
	struct bofs_file_id	f_id;		/* ID único */
	struct bofs_disk_id	f_disk;		/* Disco onde está */
	
	BK_UINT32		f_type;		/* Tipo */
	BK_UINT32		f_mode;		/* Permissões */
	BK_BOOL			f_hidden;	/* Se é oculto */
	
	/* Dono e grupo */
	BK_UID			f_uid;
	BK_GID			f_gid;
	
	/* Links */
	BK_UINT32		f_nlink;
	
	/* Tamanho e blocos */
	BK_OFF			f_size;
	BK_UINT32		f_block_count;
	BK_LIST_HEAD(, bofs_block) f_blocks;
	
	/* Pesos e hashes */
	BK_UINT64		f_weight;	/* Peso calculado */
	BK_UINT64		f_content_hash;	/* Hash do conteúdo */
	BK_UINT64		f_metadata_hash;	/* Hash dos metadados */
	
	/* Tempos */
	BK_TIME			f_atime;
	BK_TIME			f_mtime;
	BK_TIME			f_ctime;
	
	/* Para diretórios */
	struct bofs_dir		*f_dir;
	BK_UINT32		f_child_count;	/* Número de filhos */
	
	/* Para links */
	char			*f_symlink_target;
	
	/* Listas */
	BK_TAILQ_ENTRY(bofs_file) f_list;
	BK_HASH_ENTRY(bofs_file) f_hash;
};

/* Diretório no BOFS */
struct bofs_dir {
	BK_RWLOCK		d_lock;
	BK_UINT32		d_count;	/* Número de entradas */
	
	/* Entradas (nome → arquivo) - Nomes são CASE-SENSITIVE! */
	struct bk_hash_table	*d_entries;
	
	/* Cache de pesos */
	BK_UINT64		d_total_weight;	/* Peso total do diretório */
};

/* Disco montado */
struct bofs_mount {
	struct bofs_disk_id	m_disk;		/* Identificação do disco */
	char			m_mount_point[BOFS_PATH_MAX];	/* Ponto de montagem */
	
	/* Estatísticas do disco */
	BK_UINT64		m_total_sectors;
	BK_UINT64		m_free_sectors;
	BK_UINT64		m_total_files;
	BK_UINT64		m_free_files;
	
	/* Tabela de arquivos */
	BK_TABLE_HEAD(, bofs_file) m_files;
	BK_HASH_TABLE		m_file_hash;	/* Hash por ID */
	
	/* Blocos livres */
	BK_LIST_HEAD(, bofs_block) m_free_blocks;
	BK_SPINLOCK		m_blocks_lock;
	
	/* Root */
	struct bofs_file	*m_root;
	
	/* Lista de montagens */
	BK_LIST_ENTRY(bofs_mount) m_list;
};

/*
 * ============================================================================
 * FUNÇÕES DE PARSING DE PATH
 * ============================================================================
 */

/* Extrai o nome do disco de um path */
static inline BK_BOOL bofs_parse_disk(const char *path, struct bofs_disk_id *disk) {
	char disk_name[BOFS_DISK_NAME_MAX];
	const char *p = path;
	int i = 0;
	
	/* Formato: NOME(disco)/... */
	while (*p && *p != BOFS_DISK_START && i < BOFS_DISK_NAME_MAX - 1) {
		disk_name[i++] = *p++;
	}
	disk_name[i] = '\0';
	
	if (*p != BOFS_DISK_START)
		return BK_FALSE;
	p++; /* Pula '(' */
	
	/* Extrai nome do disco */
	i = 0;
	while (*p && *p != BOFS_DISK_END && i < BOFS_DISK_NAME_MAX - 1) {
		disk_name[i++] = *p++;
	}
	disk_name[i] = '\0';
	
	if (*p != BOFS_DISK_END)
		return BK_FALSE;
	
	/* Copia nome do disco */
	bk_strlcpy(disk->name, disk_name, BOFS_DISK_NAME_MAX);
	
	return BK_TRUE;
}

/* Verifica se um nome é oculto (começa com ~) */
static inline BK_BOOL bofs_is_hidden(const char *name) {
	return (name && name[0] == BOFS_HIDDEN_PREFIX);
}

/* Compara dois nomes considerando case-sensitivity */
static inline BK_BOOL bofs_name_equal(const char *name1, const char *name2) {
	/* ASCII é diferente de ASCii! Comparação exata */
	return (bk_strcmp(name1, name2) == 0);
}

/*
 * ============================================================================
 * FUNÇÕES DE CÁLCULO DE PESO
 * ============================================================================
 */

/* Calcula o peso de um arquivo */
static inline BK_UINT64 bofs_calculate_file_weight(struct bofs_file *file) {
	BK_UINT64 weight = 0;
	
	/* Peso base */
	weight += file->f_size;
	weight += file->f_mode;
	weight += file->f_uid;
	weight += file->f_gid;
	
	/* Hash do conteúdo */
	weight ^= file->f_content_hash;
	
	return weight;
}

/* Calcula o peso de um diretório (inclui filhos) */
BK_UINT64 bofs_calculate_dir_weight(struct bofs_file *dir) {
	struct bofs_dir *d = dir->f_dir;
	BK_UINT64 weight = 0;
	
	if (!d)
		return 0;
	
	/* Peso base do diretório */
	weight += dir->f_size;
	weight += d->d_count * 1000;	/* Cada arquivo aumenta o peso */
	
	/* Soma pesos dos filhos */
	/* (implementação real iteraria sobre os filhos) */
	
	return weight;
}

/* Verifica se dois arquivos são iguais (TODOS os critérios) */
static inline BK_BOOL bofs_files_equal(struct bofs_file *f1, struct bofs_file *f2) {
	if (!f1 || !f2)
		return BK_FALSE;
	
	/* 1. Peso deve ser igual */
	if (f1->f_weight != f2->f_weight)
		return BK_FALSE;
	
	/* 2. Quantidade de arquivos (se diretório) */
	if (f1->f_type == BOFS_TYPE_DIR && f2->f_type == BOFS_TYPE_DIR) {
		if (f1->f_child_count != f2->f_child_count)
			return BK_FALSE;
	}
	
	/* 3. Nome (já verificado por quem chama) */
	/* 4. Conteúdo (hash) */
	if (f1->f_content_hash != f2->f_content_hash)
		return BK_FALSE;
	
	/* 5. Metadados */
	if (f1->f_metadata_hash != f2->f_metadata_hash)
		return BK_FALSE;
	
	return BK_TRUE;
}

/*
 * ============================================================================
 * FUNÇÕES PÚBLICAS DO BOFS
 * ============================================================================
 */

/* Inicialização */
BK_I32 bofs_init(void);
void bofs_shutdown(void);

/* Montagem de discos */
BK_I32 bofs_mount(const char *disk_name, const char *mount_point);
BK_I32 bofs_umount(const char *mount_point);

/* Criação da estrutura obrigatória */
BK_I32 bofs_create_system_structure(const char *disk_name);

/* Operações de arquivo */
BK_I32 bofs_open(const char *path, BK_I32 flags, BK_MODE mode, BK_I32 *fd);
BK_I32 bofs_close(BK_I32 fd);
BK_SSIZE bofs_read(BK_I32 fd, void *buf, BK_SIZE count);
BK_SSIZE bofs_write(BK_I32 fd, const void *buf, BK_SIZE count);
BK_OFF bofs_lseek(BK_I32 fd, BK_OFF offset, BK_I32 whence);
BK_I32 bofs_stat(const char *path, struct bofs_stat *st);
BK_I32 bofs_fstat(BK_I32 fd, struct bofs_stat *st);
BK_I32 bofs_unlink(const char *path);
BK_I32 bofs_mkdir(const char *path, BK_MODE mode);
BK_I32 bofs_rmdir(const char *path);
BK_I32 bofs_rename(const char *oldpath, const char *newpath);
BK_I32 bofs_getdents(BK_I32 fd, struct bofs_dirent *dirp, BK_UINT32 count);

/* Funções específicas do BOFS */
BK_I32 bofs_get_disk_info(const char *path, struct bofs_disk_id *disk);
BK_I32 bofs_get_file_id(const char *path, struct bofs_file_id *id);
BK_I32 bofs_verify_integrity(const char *path);	/* Verifica se arquivo corrompeu */

#endif /* _BK_KERNEL */

/*
 * ============================================================================
 * MACROS DE CONVENIÊNCIA
 * ============================================================================
 */

/* Monta um path no formato BOFS */
#define BOFS_PATH(disk, type, name) \
	disk "(" disk ")" type name

#define BOFS_PATH_DIR(disk, dir) \
	BOFS_PATH(disk, "/.\\", dir)

#define BOFS_PATH_FILE(disk, dir, file) \
	BOFS_PATH(disk, "/.\\", dir) "/;\\" file

#define BOFS_PATH_HIDDEN(disk, type, name) \
	disk "(" disk ")" type "~" name

/* Discos padrão */
#define BOFS_DISK_MAIN		"~"		/* Disco principal */
#define BOFS_DISK_EXAMPLE1	"Â"		/* Disco exemplo */
#define BOFS_DISK_EXAMPLE2	"^_^"		/* Disco exemplo */
#define BOFS_DISK_EXAMPLE3	"?"		/* Disco exemplo */

/* Diretórios obrigatórios no disco principal */
#define BOFS_MAIN_ROOT		"~(~)"					/* Raiz */
#define BOFS_SYSTEM_DIR		"~(~)/.\\~Sys"				/* Sistema */
#define BOFS_SYSSOFT_DIR	"~(~)/.\\~SysSoft"			/* Softwares padrão */
#define BOFS_USER_DIR		"~(~)/.\\User"				/* Usuários */
#define BOFS_BSR_DIR		"~(~)/.\\bsr"				/* Bible Software Resource */
#define BOFS_SOFT_DIR		"~(~)/.\\Soft"				/* Softwares instalados */

/* Exemplos de arquivos */
#define BOFS_SYSTEM_CONFIG	"~(~)/.\\~Sys/;\\kconf.conf"
#define BOFS_USER_HOME		"~(~)/.\\User/;\\"

#endif /* !_BIBLE_BOFS_H_ */