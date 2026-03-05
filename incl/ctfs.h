#ifndef _BIBLE_CTFS_H_
#define _BIBLE_CTFS_H_

/*
 * ctfs.h - Configurable Table File System
 * 
 * CTFS é o sistema central de metadados usado tanto pelo BIBLEFS (RAM)
 * quanto pelo BOFS (Disco). Funciona como uma tabela configurável que
 * armazena referências para todos os arquivos no sistema.
 * 
 * ┌─────────────────────────────────────────────────────────────┐
 * │                    CTFS - CONCEITO                          │
 * ├─────────────────────────────────────────────────────────────┤
 * │                                                              │
 * │  [VOLUME]                                                   │
 * │    └── CTFS (Tabela Central)                                │
 * │         ├── [FMR ID 1] → Arquivo A (metadados + dados)     │
 * │         ├── [FMR ID 2] → Arquivo B (metadados) → Blocos    │
 * │         ├── [FMR ID 3] → Arquivo C (link p/ FMR ID 1)      │
 * │         └── [FMR ID 4] → Diretório (lista de FMR IDs)      │
 * │                                                              │
 * │  EQUIVALE A:                                                 │
 * │  • MFT do Windows (Master File Table)                       │
 * │  • Inodes do Unix                                            │
 * │  • Mas é CONFIGURÁVEL e CENTRALIZADO no volume              │
 * │                                                              │
 * └─────────────────────────────────────────────────────────────┘
 * 
 * CARACTERÍSTICAS:
 * • File Metadata Reference (FMR) ID - identificador único
 * • CTFS centralizado no volume (tabela única)
 * • Nome guardado no REGISTRO (bk_register), não na CTFS
 * • Arquivos pequenos: dados podem estar dentro da CTFS
 * • Arquivos grandes: dados fora, CTFS aponta para blocos
 * • Hard Links: suportados, apontam para mesma FMR ID
 */

#include <bible/cdefs.h>
#include <bible/types.h>
#include <bible/regsister.h>
#include <bible/queue.h>

/*
 * ============================================================================
 * CONSTANTES DO CTFS
 * ============================================================================
 */

/* Tamanhos */
#define CTFS_BLOCK_SIZE		4096	/* 4KB blocks */
#define CTFS_NAME_MAX		255	/* Tamanho máximo de nome */
#define CTFS_FMR_SIZE_MAX	2048	/* Tamanho máximo para dados inline */

/* Limites da CTFS */
#define CTFS_MAX_FILES		0xFFFFFFFF	/* Até 4 bilhões de arquivos */
#define CTFS_MAX_VOLUMES	256		/* Máximo de volumes */

/* Flags de FMR */
#define CTFS_FMR_FLAG_INLINE	0x0001	/* Dados estão inline na CTFS */
#define CTFS_FMR_FLAG_COMPRESSED 0x0002	/* Dados comprimidos */
#define CTFS_FMR_FLAG_ENCRYPTED	0x0004	/* Dados criptografados */
#define CTFS_FMR_FLAG_SPARSE	0x0008	/* Arquivo esparso */
#define CTFS_FMR_FLAG_DIRTY	0x0010	/* Modificado não sincronizado */
#define CTFS_FMR_FLAG_SYSTEM	0x0020	/* Arquivo do sistema */
#define CTFS_FMR_FLAG_HIDDEN	0x0040	/* Oculto */
#define CTFS_FMR_FLAG_READONLY	0x0080	/* Somente leitura */

/* Tipos de FMR */
#define CTFS_FMR_TYPE_UNKNOWN	0x00
#define CTFS_FMR_TYPE_FILE	0x01	/* Arquivo regular */
#define CTFS_FMR_TYPE_DIR	0x02	/* Diretório */
#define CTFS_FMR_TYPE_SYMLINK	0x03	/* Link simbólico */
#define CTFS_FMR_TYPE_HARDLINK	0x04	/* Hard link (aponta para outra FMR) */
#define CTFS_FMR_TYPE_DEVICE	0x05	/* Dispositivo */
#define CTFS_FMR_TYPE_PIPE	0x06	/* Pipe */
#define CTFS_FMR_TYPE_SOCKET	0x07	/* Socket */
#define CTFS_FMR_TYPE_VOLUME	0x08	/* Volume/partição */

/*
 * ============================================================================
 * ESTRUTURAS DA CTFS
 * ============================================================================
 */

/* File Metadata Reference ID (identificador único no volume) */
typedef BK_UINT32 CTFS_FMR_ID;

/* FMR ID inválido */
#define CTFS_FMR_ID_NULL	((CTFS_FMR_ID)0)
#define CTFS_FMR_ID_ROOT	((CTFS_FMR_ID)1)	/* Root directory */

/* Referência a bloco de dados */
struct ctfs_block_ref {
	BK_UINT64		block_number;	/* Número do bloco no dispositivo */
	BK_UINT32		block_size;	/* Tamanho real (pode ser menor) */
	BK_UINT32		checksum;	/* Checksum do bloco */
};

/* Entrada na CTFS - File Metadata Reference */
struct ctfs_fmr {
	/* Identificação */
	CTFS_FMR_ID		fmr_id;		/* ID único (índice na tabela) */
	BK_UINT8		fmr_type;	/* Tipo de FMR */
	BK_UINT16		fmr_flags;	/* Flags */
	
	/* Contagem de referências (para hard links) */
	BK_UINT32		fmr_link_count;	/* Número de nomes apontando p/ este FMR */
	
	/* Dono e permissões (copiados do registro) */
	BK_UID			fmr_uid;
	BK_GID			fmr_gid;
	BK_UINT16		fmr_mode;	/* Permissões */
	
	/* Tamanho */
	BK_OFF			fmr_size;	/* Tamanho total em bytes */
	BK_UINT32		fmr_blocks;	/* Número de blocos usados */
	
	/* Tempos */
	BK_TIME			fmr_ctime;	/* Criação */
	BK_TIME			fmr_mtime;	/* Modificação */
	BK_TIME			fmr_atime;	/* Acesso */
	
	/* Dados/Blocos */
	union {
		/* Para arquivos pequenos (dados inline) */
		struct {
			BK_UINT8	data[CTFS_FMR_SIZE_MAX];
			BK_UINT16	data_len;
		} inline_data;
		
		/* Para arquivos grandes (lista de blocos) */
		struct {
			BK_UINT32	block_count;
			struct ctfs_block_ref *blocks;	/* Array de blocos */
		} block_list;
		
		/* Para hard links */
		struct {
			CTFS_FMR_ID	target_fmr;	/* FMR alvo */
		} hardlink;
		
		/* Para diretórios */
		struct {
			BK_UINT32	entry_count;
			/* As entradas do diretório ficam nos blocos */
		} dir;
		
		/* Para links simbólicos */
		struct {
			char		*target_path;	/* Caminho do alvo */
		} symlink;
	} fmr_data;
	
	/* Checksum da própria FMR (para integridade) */
	BK_UINT32		fmr_checksum;
};

/* Cabeçalho da CTFS no volume */
struct ctfs_header {
	char			magic[8];	/* "BIBLECTFS" */
	BK_UINT32		version;	/* Versão do formato */
	
	/* Localização da CTFS */
	BK_UINT64		table_start;	/* Início da tabela (setores) */
	BK_UINT32		entry_size;	/* Tamanho de cada entrada */
	BK_UINT32		max_entries;	/* Capacidade máxima */
	BK_UINT32		used_entries;	/* Entradas usadas */
	
	/* Próximo ID disponível */
	CTFS_FMR_ID		next_fmr_id;
	
	/* Estatísticas */
	BK_UINT64		total_blocks;	/* Total de blocos no volume */
	BK_UINT64		free_blocks;	/* Blocos livres */
	
	/* Root directory */
	CTFS_FMR_ID		root_fmr_id;
	
	/* Checksum do header */
	BK_UINT32		header_checksum;
};

/* Informação sobre o volume CTFS */
struct ctfs_volume_info {
	char			volume_name[256];
	BK_UINT64		total_space;
	BK_UINT64		free_space;
	BK_UINT32		total_files;
	BK_UINT32		total_dirs;
	BK_UINT32		fragmentation;
	BK_UINT32		open_files;
};

/*
 * ============================================================================
 * ESTRUTURAS INTERNAS (APENAS PARA O KERNEL)
 * ============================================================================
 */

#ifdef _BK_KERNEL

/* Volume CTFS montado */
struct ctfs_volume {
	char			vol_mount_point[256];
	
	/* Cabeçalho */
	struct ctfs_header	vol_header;
	
	/* Dispositivo subjacente (disco ou RAM) */
	void			*vol_device;
	BK_UINT32		vol_sector_size;
	BK_UINT64		vol_total_sectors;
	
	/* Cache de FMRs */
	BK_HASH_TABLE		vol_fmr_cache;	/* ID → FMR */
	BK_SPINLOCK		vol_cache_lock;
	
	/* Tabela de FMRs (se completamente carregada) */
	struct ctfs_fmr		*vol_fmr_table;
	BK_SPINLOCK		vol_table_lock;
	
	/* Gerenciamento de blocos livres */
	BK_BITMAP		vol_block_bitmap;
	BK_SPINLOCK		vol_bitmap_lock;
	
	/* Lista de volumes montados */
	BK_LIST_ENTRY(ctfs_volume) vol_list;
};

/*
 * ============================================================================
 * FUNÇÕES DO CTFS
 * ============================================================================
 */

/* Inicialização global */
BK_I32 ctfs_init(void);
void ctfs_shutdown(void);

/* Formatação de volume */
BK_I32 ctfs_format(void *device, BK_UINT64 total_sectors, BK_UINT32 sector_size);
BK_I32 ctfs_quick_format(void *device);	/* Reusa estrutura existente */

/* Montagem/desmontagem */
BK_I32 ctfs_mount(void *device, const char *mount_point, struct ctfs_volume **vol);
BK_I32 ctfs_umount(struct ctfs_volume *vol);
BK_I32 ctfs_check(struct ctfs_volume *vol);	/* Verifica integridade */
BK_I32 ctfs_repair(struct ctfs_volume *vol);	/* Repara danos */

/* Operações com FMRs */
CTFS_FMR_ID ctfs_alloc_fmr(struct ctfs_volume *vol, BK_UINT8 type);
BK_I32 ctfs_free_fmr(struct ctfs_volume *vol, CTFS_FMR_ID fmr_id);
struct ctfs_fmr *ctfs_get_fmr(struct ctfs_volume *vol, CTFS_FMR_ID fmr_id);
BK_I32 ctfs_put_fmr(struct ctfs_volume *vol, struct ctfs_fmr *fmr);
BK_I32 ctfs_update_fmr(struct ctfs_volume *vol, struct ctfs_fmr *fmr);

/* Operações com dados */
BK_I32 ctfs_read_data(struct ctfs_volume *vol, CTFS_FMR_ID fmr_id, 
		      BK_OFF offset, void *buffer, BK_SIZE size);
BK_I32 ctfs_write_data(struct ctfs_volume *vol, CTFS_FMR_ID fmr_id,
		       BK_OFF offset, const void *buffer, BK_SIZE size);
BK_I32 ctfs_truncate(struct ctfs_volume *vol, CTFS_FMR_ID fmr_id, BK_OFF new_size);
BK_I32 ctfs_allocate_blocks(struct ctfs_volume *vol, CTFS_FMR_ID fmr_id, BK_UINT32 count);

/* Hard links */
BK_I32 ctfs_create_hardlink(struct ctfs_volume *vol, CTFS_FMR_ID target_fmr, 
			    CTFS_FMR_ID *new_fmr);
BK_I32 ctfs_unlink_hardlink(struct ctfs_volume *vol, CTFS_FMR_ID fmr_id);

/* Diretórios */
BK_I32 ctfs_dir_add_entry(struct ctfs_volume *vol, CTFS_FMR_ID dir_fmr,
			  const char *name, CTFS_FMR_ID file_fmr);
BK_I32 ctfs_dir_remove_entry(struct ctfs_volume *vol, CTFS_FMR_ID dir_fmr,
			     const char *name);
BK_I32 ctfs_dir_find_entry(struct ctfs_volume *vol, CTFS_FMR_ID dir_fmr,
			   const char *name, CTFS_FMR_ID *file_fmr);
BK_I32 ctfs_dir_list(struct ctfs_volume *vol, CTFS_FMR_ID dir_fmr,
		     char **names, BK_UINT32 *count);

/* Nomes (guardados no REGISTRO, não na CTFS) */
BK_I32 ctfs_register_name(struct ctfs_volume *vol, CTFS_FMR_ID fmr_id,
			  const char *name, BK_REGISTER_HANDLE *name_reg);
const char *ctfs_get_name_from_register(BK_REGISTER_HANDLE name_reg);

/* Estatísticas */
BK_I32 ctfs_get_volume_info(struct ctfs_volume *vol, struct ctfs_volume_info *info);
BK_I32 ctfs_get_fmr_info(struct ctfs_volume *vol, CTFS_FMR_ID fmr_id,
			 struct ctfs_fmr *info);

/*
 * ============================================================================
 * FUNÇÕES DE CACHE E OTIMIZAÇÃO
 * ============================================================================
 */

/* Cache de FMRs */
struct ctfs_fmr *ctfs_cache_lookup(struct ctfs_volume *vol, CTFS_FMR_ID fmr_id);
void ctfs_cache_insert(struct ctfs_volume *vol, struct ctfs_fmr *fmr);
void ctfs_cache_invalidate(struct ctfs_volume *vol, CTFS_FMR_ID fmr_id);
void ctfs_cache_flush(struct ctfs_volume *vol);

/* Leitura/escrita otimizada */
BK_I32 ctfs_readahead(struct ctfs_volume *vol, CTFS_FMR_ID fmr_id,
		      BK_OFF offset, BK_SIZE size);
BK_I32 ctfs_write_async(struct ctfs_volume *vol, CTFS_FMR_ID fmr_id,
			BK_OFF offset, const void *buffer, BK_SIZE size);

/*
 * ============================================================================
 * FUNÇÕES DE RECUPERAÇÃO E MANUTENÇÃO
 * ============================================================================
 */

/* Backup da CTFS */
BK_I32 ctfs_backup(struct ctfs_volume *vol, void *backup_device);
BK_I32 ctfs_restore(struct ctfs_volume *vol, void *backup_device);

/* Desfragmentação */
BK_I32 ctfs_defragment(struct ctfs_volume *vol);
BK_I32 ctfs_defragment_file(struct ctfs_volume *vol, CTFS_FMR_ID fmr_id);

/* Verificação de integridade */
BK_I32 ctfs_verify_fmr(struct ctfs_volume *vol, CTFS_FMR_ID fmr_id);
BK_I32 ctfs_rebuild_checksums(struct ctfs_volume *vol);

/*
 * ============================================================================
 * FUNÇÕES AUXILIARES
 * ============================================================================
 */

/* Conversão de permissões */
static inline BK_UINT16 ctfs_mode_from_register(BK_REGISTER_HANDLE reg) {
	/* Extrai permissões do registro */
	BK_REGISTER_HEADER *hdr = bk_register_get_header(reg);
	return hdr->r_permissions;
}

/* Cálculo de checksum */
static inline BK_UINT32 ctfs_calculate_checksum(const void *data, BK_UINT32 len) {
	BK_UINT32 sum = 0;
	const BK_UINT8 *bytes = (const BK_UINT8 *)data;
	
	for (BK_UINT32 i = 0; i < len; i++) {
		sum += bytes[i];
		sum = (sum << 1) | (sum >> 31);	/* Rotação */
	}
	
	return sum;
}

/* Verifica se FMR é inline */
static inline BK_BOOL ctfs_fmr_is_inline(struct ctfs_fmr *fmr) {
	return (fmr->fmr_flags & CTFS_FMR_FLAG_INLINE) != 0;
}

/* Obtém o ID do FMR alvo (para hard links) */
static inline CTFS_FMR_ID ctfs_fmr_get_target(struct ctfs_fmr *fmr) {
	if (fmr->fmr_type == CTFS_FMR_TYPE_HARDLINK)
		return fmr->fmr_data.hardlink.target_fmr;
	return fmr->fmr_id;
}

/*
 * ============================================================================
 * MACROS DE CONVENIÊNCIA
 * ============================================================================
 */

/* Itera sobre todas as FMRs em um volume */
#define CTFS_FMR_FOREACH(vol, fmr_id, fmr) \
	for (fmr_id = CTFS_FMR_ID_ROOT; \
	     fmr_id < (vol)->vol_header.used_entries; \
	     fmr_id++) \
		if ((fmr = ctfs_get_fmr(vol, fmr_id)) != NULL)

/* Itera sobre todos os blocos de um FMR */
#define CTFS_BLOCK_FOREACH(fmr, block, i) \
	for (i = 0, block = (fmr)->fmr_data.block_list.blocks; \
	     i < (fmr)->fmr_data.block_list.block_count; \
	     i++, block++)

#endif /* _BK_KERNEL */

/*
 * ============================================================================
 * ESTRUTURAS PARA USUÁRIO (VIA SYSCALLS)
 * ============================================================================
 */

/* Informações de FMR para usuário */
struct ctfs_user_fmr_info {
	CTFS_FMR_ID		fmr_id;
	BK_UINT8		fmr_type;
	BK_UINT16		fmr_flags;
	BK_UINT32		fmr_link_count;
	BK_OFF			fmr_size;
	BK_TIME			fmr_ctime;
	BK_TIME			fmr_mtime;
	BK_TIME			fmr_atime;
	BK_UINT32		fmr_blocks;
};

/* Informações do volume para usuário */
struct ctfs_user_volume_info {
	char			volume_name[256];
	char			mount_point[256];
	BK_UINT64		total_space;
	BK_UINT64		free_space;
	BK_UINT32		total_files;
	BK_UINT32		total_dirs;
};

/* Syscalls relacionadas ao CTFS (implementadas em outro lugar) */
BK_I32 ctfs_sys_get_fmr_info(CTFS_FMR_ID fmr_id, struct ctfs_user_fmr_info *info);
BK_I32 ctfs_sys_get_volume_info(const char *path, struct ctfs_user_volume_info *info);
BK_I32 ctfs_sys_defragment(const char *path);
BK_I32 ctfs_sys_verify(const char *path);

#endif /* !_BIBLE_CTFS_H_ */