#ifndef _BIBLE_EXEC_FORMAT_H_
#define _BIBLE_EXEC_FORMAT_H_

/*
 * exec_format.h - Formato de Executável do BibleKernel
 * 
 * O BibleKernel SÓ executa arquivos neste formato nativo.
 * Extensões suportadas: .cae, .cbe, .bc, ou sem extensão
 * 
 * RELAÇÃO COM O KERNEL:
 * ┌─────────────────────────────────────────────────────────────┐
 * │                                                              │
 * │  [Arquivo .cae/.cbe/.bc]                                    │
 * │         ↓                                                   │
 * │  [Validador de Formato]                                     │
 * │         ↓                                                   │
 * │  [Verificação de Assinatura e Permissões]                  │
 * │         ↓                                                   │
 * │  [Carregador do Kernel]                                     │
 * │         ↓                                                   │
 * │  [Mapeamento de Syscalls (devem existir exatamente)]       │
 * │         ↓                                                   │
 * │  [Criação de Processo/Task/Thread/Atividade]               │
 * │         ↓                                                   │
 * │  [Execução]                                                 │
 * │                                                              │
 * └─────────────────────────────────────────────────────────────┘
 * 
 * O executável DEVE conter TODAS as syscalls que serão usadas,
 * exatamente como o kernel espera. Não há compatibilidade
 * automática - as syscalls são verificadas na carga.
 */

#include <bible/cdefs.h>
#include <bible/types.h>
#include <bible/regsister.h>
#include <bible/syscalls.h>
#include <bible/ctfs.h>

/*
 * ============================================================================
 * CONSTANTES DO FORMATO EXECUTÁVEL
 * ============================================================================
 */

/* Magic numbers do formato Bible Executable */
#define BEXEC_MAGIC0		0x42  /* 'B' */
#define BEXEC_MAGIC1		0x45  /* 'E' */
#define BEXEC_MAGIC2		0x58  /* 'X' */
#define BEXEC_MAGIC3		0x43  /* 'C' */

#define BEXEC_MAGIC		{ BEXEC_MAGIC0, BEXEC_MAGIC1, BEXEC_MAGIC2, BEXEC_MAGIC3 }

/* Extensões suportadas */
#define BEXEC_EXT_CAE		".cae"  /* Compiled Bible Executable */
#define BEXEC_EXT_CBE		".cbe"  /* Compiled Bible Executable (alternate) */
#define BEXEC_EXT_BC		".bc"   /* Bible Code */

/* Classes de arquivo */
enum bexec_class {
	BEXEC_CLASS_NONE	= 0,
	BEXEC_CLASS_16		= 1,	/* 16-bit */
	BEXEC_CLASS_32		= 2,	/* 32-bit */
	BEXEC_CLASS_64		= 3,	/* 64-bit */
	BEXEC_CLASS_128		= 4,	/* 128-bit (futuro) */
	BEXEC_CLASS_VM		= 5,	/* Máquina Virtual */
};

/* Endianness */
enum bexec_data {
	BEXEC_DATA_NONE		= 0,
	BEXEC_DATA_LSB		= 1,	/* Little-endian */
	BEXEC_DATA_MSB		= 2,	/* Big-endian */
};

/* Versões do formato */
enum bexec_version {
	BEXEC_VERSION_NONE	= 0,
	BEXEC_VERSION_1_0	= 1,	/* BibleOS 1.0 */
	BEXEC_VERSION_1_1	= 2,	/* BibleOS 1.1 (com segurança) */
	BEXEC_VERSION_1_2	= 3,	/* BibleOS 1.2 (com syscall table) */
	BEXEC_VERSION_CURRENT	= BEXEC_VERSION_1_2,
};

/* Tipos de arquivo executável */
enum bexec_type {
	BEXEC_TYPE_NONE		= 0,
	BEXEC_TYPE_EXEC		= 1,	/* Executável normal */
	BEXEC_TYPE_DYN		= 2,	/* Biblioteca dinâmica */
	BEXEC_TYPE_DRIVER	= 3,	/* Driver de dispositivo */
	BEXEC_TYPE_SERVICE	= 4,	/* Serviço de sistema */
	BEXEC_TYPE_BOOT		= 5,	/* Bootloader */
	BEXEC_TYPE_FIRMWARE	= 6,	/* Firmware */
	BEXEC_TYPE_KERNEL	= 7,	/* Módulo do kernel */
};

/* Arquiteturas suportadas */
enum bexec_machine {
	BEXEC_MACHINE_NONE	= 0,
	BEXEC_MACHINE_X86	= 1,	/* x86 32-bit */
	BEXEC_MACHINE_X64	= 2,	/* x86-64 */
	BEXEC_MACHINE_ARM32	= 3,	/* ARM 32-bit */
	BEXEC_MACHINE_ARM64	= 4,	/* ARM 64-bit */
	BEXEC_MACHINE_RISCV32	= 5,	/* RISC-V 32-bit */
	BEXEC_MACHINE_RISCV64	= 6,	/* RISC-V 64-bit */
	BEXEC_MACHINE_PPC32	= 7,	/* PowerPC 32-bit */
	BEXEC_MACHINE_PPC64	= 8,	/* PowerPC 64-bit */
	BEXEC_MACHINE_VM	= 254,	/* Máquina Virtual */
	BEXEC_MACHINE_ANY	= 255,	/* Qualquer arquitetura */
};

/* Flags de segurança */
enum bexec_security_flags {
	BEXEC_SEC_NONE		= 0x00000000,
	BEXEC_SEC_NX		= 0x00000001,	/* No-Execute */
	BEXEC_SEC_ASLR		= 0x00000002,	/* ASLR habilitado */
	BEXEC_SEC_STACK_PROT	= 0x00000004,	/* Proteção de stack */
	BEXEC_SEC_HEAP_PROT	= 0x00000008,	/* Proteção de heap */
	BEXEC_SEC_CFI		= 0x00000010,	/* Control Flow Integrity */
	BEXEC_SEC_SIGNED	= 0x00000020,	/* Assinado digitalmente */
	BEXEC_SEC_ENCRYPTED	= 0x00000040,	/* Criptografado */
	BEXEC_SEC_VERIFIED	= 0x00000080,	/* Verificado */
	BEXEC_SEC_SANDBOX	= 0x00000100,	/* Executar em sandbox */
	BEXEC_SEC_JIT		= 0x00000200,	/* Permite JIT */
	BEXEC_SEC_WX		= 0x00000400,	/* Write XOR Execute */
};

/* Flags de segmento */
enum bexec_segment_flags {
	BEXEC_SEG_READ		= 0x00000001,	/* Legível */
	BEXEC_SEG_WRITE		= 0x00000002,	/* Gravável */
	BEXEC_SEG_EXEC		= 0x00000004,	/* Executável */
	BEXEC_SEG_ALLOC		= 0x00000008,	/* Alocar na memória */
	BEXEC_SEG_LOAD		= 0x00000010,	/* Carregar na carga */
	BEXEC_SEG_ZERO		= 0x00000020,	/* Zerar (BSS) */
	BEXEC_SEG_COMPRESSED	= 0x00000040,	/* Comprimido */
	BEXEC_SEG_ENCRYPTED	= 0x00000080,	/* Criptografado */
	BEXEC_SEG_TLS		= 0x00000100,	/* Thread Local Storage */
	BEXEC_SEG_STACK		= 0x00000200,	/* Stack */
	BEXEC_SEG_HEAP		= 0x00000400,	/* Heap */
};

/* Tipos de segmento */
enum bexec_segment_type {
	BEXEC_SEG_NULL		= 0,
	BEXEC_SEG_CODE		= 1,	/* Código executável */
	BEXEC_SEG_DATA		= 2,	/* Dados inicializados */
	BEXEC_SEG_BSS		= 3,	/* Dados não inicializados */
	BEXEC_SEG_RODATA	= 4,	/* Dados somente leitura */
	BEXEC_SEG_STACK		= 5,	/* Stack */
	BEXEC_SEG_HEAP		= 6,	/* Heap */
	BEXEC_SEG_TLS		= 7,	/* Thread Local Storage */
	BEXEC_SEG_SYSCALL_TABLE	= 8,	/* Tabela de syscalls */
	BEXEC_SEG_IMPORT_TABLE	= 9,	/* Tabela de importações */
	BEXEC_SEG_EXPORT_TABLE	= 10,	/* Tabela de exportações */
	BEXEC_SEG_RELOC_TABLE	= 11,	/* Tabela de relocações */
	BEXEC_SEG_SYMBOL_TABLE	= 12,	/* Tabela de símbolos */
	BEXEC_SEG_STRING_TABLE	= 13,	/* Tabela de strings */
	BEXEC_SEG_RESOURCE	= 14,	/* Recursos */
	BEXEC_SEG_DEBUG		= 15,	/* Informação de debug */
	BEXEC_SEG_COMMENT	= 16,	/* Comentários */
	BEXEC_SEG_NOTE		= 17,	/* Notas */
	BEXEC_SEG_SECURITY	= 18,	/* Informações de segurança */
};

/*
 * ============================================================================
 * ESTRUTURAS DO FORMATO EXECUTÁVEL (32-bit)
 * ============================================================================
 */

struct bexec32_ehdr {
	/* Identificação */
	BK_UINT8		e_ident[16];	/* Magic + info */
	
	/* Cabeçalho principal */
	BK_UINT16		e_type;		/* Tipo de executável */
	BK_UINT16		e_machine;	/* Arquitetura */
	BK_UINT32		e_version;	/* Versão do formato */
	BK_UINT32		e_entry;	/* Ponto de entrada */
	BK_UINT32		e_phoff;	/* Offset do program header */
	BK_UINT32		e_shoff;	/* Offset do section header */
	BK_UINT32		e_flags;	/* Flags do arquivo */
	BK_UINT16		e_ehsize;	/* Tamanho do ehdr */
	BK_UINT16	_e_phentsize;	/* Tamanho program header entry */
	BK_UINT16		e_phnum;	/* Número program headers */
	BK_UINT16	_e_shentsize;	/* Tamanho section header entry */
	BK_UINT16		e_shnum;	/* Número section headers */
	BK_UINT16		e_shstrndx;	/* Índice string table */
	
	/* Informações específicas BibleOS */
	BK_UINT32		e_syscall_off;	/* Offset da tabela de syscalls */
	BK_UINT16	_e_syscall_entsize;	/* Tamanho da entrada syscall */
	BK_UINT16		e_syscall_num;	/* Número de syscalls */
	BK_UINT32		e_security_off;	/* Offset do header de segurança */
	BK_UINT16	_e_security_size;	/* Tamanho do header de segurança */
	BK_UINT16		e_reserved[3];	/* Reservado */
};

/*
 * ============================================================================
 * ESTRUTURAS DO FORMATO EXECUTÁVEL (64-bit)
 * ============================================================================
 */

struct bexec64_ehdr {
	/* Identificação */
	BK_UINT8		e_ident[16];	/* Magic + info */
	
	/* Cabeçalho principal */
	BK_UINT16		e_type;		/* Tipo de executável */
	BK_UINT16		e_machine;	/* Arquitetura */
	BK_UINT32		e_version;	/* Versão do formato */
	BK_UINT64		e_entry;	/* Ponto de entrada */
	BK_UINT64	_e_phoff;	/* Offset do program header */
	BK_UINT64		e_shoff;	/* Offset do section header */
	BK_UINT32		e_flags;	/* Flags do arquivo */
	BK_UINT16		e_ehsize;	/* Tamanho do ehdr */
	BK_UINT16	_e_phentsize;	/* Tamanho program header entry */
	BK_UINT16		e_phnum;	/* Número program headers */
	BK_UINT16	_e_shentsize;	/* Tamanho section header entry */
	BK_UINT16		e_shnum;	/* Número section headers */
	BK_UINT16		e_shstrndx;	/* Índice string table */
	
	/* Informações específicas BibleOS */
	BK_UINT64		e_syscall_off;	/* Offset da tabela de syscalls */
	BK_UINT16	_e_syscall_entsize;	/* Tamanho da entrada syscall */
	BK_UINT16		e_syscall_num;	/* Número de syscalls */
	BK_UINT64		e_security_off;	/* Offset do header de segurança */
	BK_UINT16		e_security_size;	/* Tamanho do header de segurança */
	BK_UINT16		e_reserved[5];	/* Reservado */
};

/*
 * ============================================================================
 * PROGRAM HEADER (Segmentos)
 * ============================================================================
 */

struct bexec32_phdr {
	BK_UINT32		p_type;		/* Tipo de segmento */
	BK_UINT32		p_offset;	/* Offset no arquivo */
	BK_UINT32		p_vaddr;	/* Endereço virtual */
	BK_UINT32	_p_paddr;	/* Endereço físico (opcional) */
	BK_UINT32		p_filesz;	/* Tamanho no arquivo */
	BK_UINT32		p_memsz;	/* Tamanho na memória */
	BK_UINT32		p_flags;	/* Flags do segmento */
	BK_UINT32		p_align;	/* Alinhamento */
};

struct bexec64_phdr {
	BK_UINT32		p_type;		/* Tipo de segmento */
	BK_UINT32		p_flags;	/* Flags do segmento */
	BK_UINT64		p_offset;	/* Offset no arquivo */
	BK_UINT64		p_vaddr;	/* Endereço virtual */
	BK_UINT64	_p_paddr;	/* Endereço físico (opcional) */
	BK_UINT64		p_filesz;	/* Tamanho no arquivo */
	BK_UINT64		p_memsz;	/* Tamanho na memória */
	BK_UINT64	_p_align;	/* Alinhamento */
};

/*
 * ============================================================================
 * SECTION HEADER (Seções)
 * ============================================================================
 */

struct bexec32_shdr {
	BK_UINT32		sh_name;	/* Índice do nome */
	BK_UINT32		sh_type;	/* Tipo de seção */
	BK_UINT32		sh_flags;	/* Flags da seção */
	BK_UINT32		sh_addr;	/* Endereço virtual */
	BK_UINT32		sh_offset;	/* Offset no arquivo */
	BK_UINT32		sh_size;	/* Tamanho da seção */
	BK_UINT32		sh_link;	/* Link para outra seção */
	BK_UINT32		sh_info;	/* Informação adicional */
	BK_UINT32		sh_addralign;	/* Alinhamento */
	BK_UINT32		sh_entsize;	/* Tamanho da entrada */
};

struct bexec64_shdr {
	BK_UINT32		sh_name;	/* Índice do nome */
	BK_UINT32		sh_type;	/* Tipo de seção */
	BK_UINT64		sh_flags;	/* Flags da seção */
	BK_UINT64		sh_addr;	/* Endereço virtual */
	BK_UINT64		sh_offset;	/* Offset no arquivo */
	BK_UINT64		sh_size;	/* Tamanho da seção */
	BK_UINT32		sh_link;	/* Link para outra seção */
	BK_UINT32		sh_info;	/* Informação adicional */
	BK_UINT64		sh_addralign;	/* Alinhamento */
	BK_UINT64		sh_entsize;	/* Tamanho da entrada */
};

/*
 * ============================================================================
 * TABELA DE SYSCALLS (CRÍTICO - DEVE EXISTIR EXATAMENTE)
 * ============================================================================
 */

/* Entrada da tabela de syscalls */
struct bexec_syscall_entry {
	BK_UINT32		syscall_number;	/* Número da syscall (10000-10700) */
	BK_UINT64		syscall_addr;	/* Endereço da implementação */
	BK_UINT32		syscall_flags;	/* Flags da syscall */
	BK_UINT16		syscall_args;	/* Número de argumentos (0-6) */
	BK_UINT16		syscall_reserved;
};

/* Flags de syscall */
#define BEXEC_SYSCALL_REQUIRED	0x0001	/* Obrigatória para execução */
#define BEXEC_SYSCALL_OPTIONAL	0x0002	/* Opcional (fallback) */
#define BEXEC_SYSCALL_PRIVILEGED 0x0004	/* Requer privilégio */
#define BEXEC_SYSCALL_DEPRECATED 0x0008	/* Depreciada */
#define BEXEC_SYSCALL_INTERNAL	0x0010	/* Uso interno */

/* Tabela de syscalls completa */
struct bexec_syscall_table {
	BK_UINT32		st_magic;	/* Magic "SYSC" */
	BK_UINT16		st_version;	/* Versão da tabela */
	BK_UINT16		st_count;	/* Número de syscalls */
	BK_UINT32		st_flags;	/* Flags da tabela */
	BK_UINT64		st_checksum;	/* Checksum da tabela */
	
	/* Array de syscall entries */
	struct bexec_syscall_entry	st_entries[];
};

/* Magic da tabela de syscalls */
#define BEXEC_SYSCALL_MAGIC	0x43535953	/* "SYSC" em little-endian */

/*
 * ============================================================================
 * HEADER DE SEGURANÇA
 * ============================================================================
 */

struct bexec_security_hdr {
	BK_UINT32		sec_magic;	/* Magic "SECU" */
	BK_UINT8		sec_ring;	/* Ring level (0-7) */
	BK_UINT8		sec_reserved1;
	BK_UINT16		sec_flags;	/* Security flags */
	BK_UINT32		sec_permissions;	/* Permission mask */
	
	/* Assinatura digital */
	BK_UINT64		sec_sig_offset;	/* Offset da assinatura */
	BK_UINT32		sec_sig_size;	/* Tamanho da assinatura */
	BK_UINT32		sec_sig_type;	/* Tipo de assinatura */
	
	/* Hash do executável */
	BK_UINT32		sec_hash_type;	/* Tipo de hash */
	BK_UINT8		sec_hash[64];	/* Hash (até 512 bits) */
	
	/* Certificado */
	BK_UINT64		sec_cert_offset;	/* Offset do certificado */
	BK_UINT32		sec_cert_size;	/* Tamanho do certificado */
	
	BK_UINT32		sec_reserved2[8];
};

/* Magic do header de segurança */
#define BEXEC_SECURITY_MAGIC	0x55434553	/* "SECU" */

/*
 * ============================================================================
 * ESTRUTURA DE SÍMBOLO
 * ============================================================================
 */

struct bexec_symbol {
	BK_UINT32		st_name;	/* Índice do nome */
	BK_UINT8		st_info;	/* Tipo e binding */
	BK_UINT8		st_other;	/* Visibilidade */
	BK_UINT16		st_shndx;	/* Índice da seção */
	BK_UINT64		st_value;	/* Valor/endereço */
	BK_UINT64		st_size;	/* Tamanho */
};

/* Macros para st_info */
#define BEXEC_ST_BIND(info)	((info) >> 4)
#define BEXEC_ST_TYPE(info)	((info) & 0xF)
#define BEXEC_ST_INFO(bind, type)	(((bind) << 4) | ((type) & 0xF))

/* Bindings de símbolo */
#define BEXEC_STB_LOCAL		0
#define BEXEC_STB_GLOBAL	1
#define BEXEC_STB_WEAK		2

/* Tipos de símbolo */
#define BEXEC_STT_NOTYPE	0
#define BEXEC_STT_OBJECT	1
#define BEXEC_STT_FUNC		2
#define BEXEC_STT_SECTION	3
#define BEXEC_STT_FILE		4

/*
 * ============================================================================
 * ESTRUTURA DE RELOCAÇÃO
 * ============================================================================
 */

struct bexec_reloc {
	BK_UINT64		r_offset;	/* Offset para relocar */
	BK_UINT64	_r_info;	/* Tipo e índice do símbolo */
	BK_INT64		r_addend;	/* Addend */
};

/* Macros para r_info */
#define BEXEC_R_SYM(info)	((info) >> 32)
#define BEXEC_R_TYPE(info)	((info) & 0xFFFFFFFF)
#define BEXEC_R_INFO(sym, type)	(((sym) << 32) | (type))

/* Tipos de relocação */
#define BEXEC_R_NONE		0
#define BEXEC_R_64		1
#define BEXEC_R_32		2
#define BEXEC_R_16		3
#define BEXEC_R_PC64		4
#define BEXEC_R_PC32		5
#define BEXEC_R_PC16		6
#define BEXEC_R_GOT64		7
#define BEXEC_R_GOT32		8
#define BEXEC_R_PLT64		9
#define BEXEC_R_PLT32		10
#define BEXEC_R_COPY		11
#define BEXEC_R_GLOB_DAT	12
#define BEXEC_R_JUMP_SLOT	13
#define BEXEC_R_RELATIVE	14

/*
 * ============================================================================
 * FUNÇÕES DO KERNEL PARA CARREGAR EXECUTÁVEIS
 * ============================================================================
 */

#ifdef _BK_KERNEL

/* Resultado da verificação de executável */
struct bexec_validation_result {
	BK_BOOL			valid;		/* Se é válido */
	BK_UINT8		class;		/* Classe (32/64 bits) */
	BK_UINT16		machine;	/* Arquitetura */
	BK_UINT16		type;		/* Tipo de executável */
	BK_UINT64		entry;		/* Ponto de entrada */
	BK_UINT32		security_flags;	/* Flags de segurança */
	BK_UINT8		ring;		/* Ring level requerido */
	BK_UINT32		syscall_count;	/* Número de syscalls */
	BK_UINT32		missing_syscalls; /* Syscalls faltando */
};

/* Informações do executável para criar processo */
struct bexec_exec_info {
	/* Pontos de entrada */
	BK_UINT64		entry_point;
	
	/* Segmentos de memória */
	struct {
		BK_UINT64	addr;
		BK_UINT64	size;
		BK_UINT32	flags;
		const BK_UINT8	*data;
	} segments[16];
	BK_UINT32		segment_count;
	
	/* Syscalls implementadas */
	struct {
		BK_UINT32	number;
		BK_UINT64	addr;
		BK_UINT16	args;
	} syscalls[256];
	BK_UINT32		syscall_count;
	
	/* TLS (Thread Local Storage) */
	BK_UINT64		tls_size;
	BK_UINT64		tls_align;
	const BK_UINT8		*tls_data;
	
	/* Stack e heap */
	BK_UINT64		stack_size;
	BK_UINT64		heap_size;
	
	/* Segurança */
	BK_UINT8		ring_level;
	BK_UINT32		permissions;
};

/* Verifica se um arquivo é um executável Bible válido */
BK_BOOL bexec_check_magic(const BK_UINT8 *data, BK_SIZE size);

/* Valida um executável Bible */
BK_I32 bexec_validate(const BK_UINT8 *data, BK_SIZE size,
		      struct bexec_validation_result *result);

/* Carrega um executável na memória */
BK_I32 bexec_load(const BK_UINT8 *data, BK_SIZE size,
		  struct bexec_exec_info *info);

/* Verifica se todas as syscalls necessárias existem */
BK_BOOL bexec_check_syscalls(const struct bexec_syscall_table *table,
			     BK_UINT32 *missing);

/* Cria um processo a partir de um executável */
BK_PROCESS *bexec_create_process(const BK_UINT8 *data, BK_SIZE size,
				 const char *name,
				 BK_UID uid, BK_GID gid,
				 BK_UINT32 flags);

/* Extrai informações de segurança */
BK_I32 bexextract_security_info(const BK_UINT8 *data, BK_SIZE size,
				  struct bexec_security_hdr *sec);

/* Verifica assinatura digital (se houver) */
BK_BOOL bexec_verify_signature(const BK_UINT8 *data, BK_SIZE size,
			       const BK_UINT8 *key, BK_SIZE key_size);

/*
 * ============================================================================
 * FUNÇÕES AUXILIARES
 * ============================================================================
 */

/* Obtém o offset da tabela de syscalls */
static inline BK_UINT64 bexec_get_syscall_offset(const struct bexec64_ehdr *ehdr) {
	if (ehdr->e_ident[4] == BEXEC_CLASS_64)
		return ((const struct bexec64_ehdr *)ehdr)->e_syscall_off;
	else
		return ((const struct bexec32_ehdr *)ehdr)->e_syscall_off;
}

/* Obtém o número de syscalls */
static inline BK_UINT16 bexec_get_syscall_count(const struct bexec64_ehdr *ehdr) {
	if (ehdr->e_ident[4] == BEXEC_CLASS_64)
		return ((const struct bexec64_ehdr *)ehdr)->e_syscall_num;
	else
		return ((const struct bexec32_ehdr *)ehdr)->e_syscall_num;
}

/* Verifica a classe do executável */
static inline BK_UINT8 bexec_get_class(const BK_UINT8 *ident) {
	return ident[4];
}

/* Verifica a endianness */
static inline BK_UINT8 bexec_get_data_encoding(const BK_UINT8 *ident) {
	return ident[5];
}

/* Verifica a versão */
static inline BK_UINT8 bexec_get_version(const BK_UINT8 *ident) {
	return ident[6];
}

/* Converte endianness se necessário */
static inline BK_UINT32 bexec_to_cpu32(BK_UINT32 val, BK_UINT8 data_encoding) {
	if (data_encoding == BEXEC_DATA_LSB) {
		/* Little-endian para CPU (assumindo little) */
		return val;
	} else {
		/* Big-endian para CPU */
		return ((val & 0xFF000000) >> 24) |
		       ((val & 0x00FF0000) >> 8) |
		       ((val & 0x0000FF00) << 8) |
		       ((val & 0x000000FF) << 24);
	}
}

#endif /* _BK_KERNEL */

/*
 * ============================================================================
 * MACROS DE VERIFICAÇÃO DE EXTENSÃO
 * ============================================================================
 */

/* Verifica se a extensão é de executável Bible */
#define BEXEC_IS_VALID_EXTENSION(ext) \
	(bk_strcmp(ext, BEXEC_EXT_CAE) == 0 || \
	 bk_strcmp(ext, BEXEC_EXT_CBE) == 0 || \
	 bk_strcmp(ext, BEXEC_EXT_BC) == 0 || \
	 ext[0] == '\0')  /* Sem extensão também é válido */

/*
 * ============================================================================
 * CONSTANTES ADICIONAIS
 * ============================================================================
 */

/* Tamanhos padrão */
#define BEXEC_DEFAULT_STACK_SIZE	(2 * 1024 * 1024)	/* 2MB */
#define BEXEC_DEFAULT_HEAP_SIZE		(8 * 1024 * 1024)	/* 8MB */
#define BEXEC_MIN_STACK_SIZE		(4 * 1024)		/* 4KB */
#define BEXEC_MIN_HEAP_SIZE		(4 * 1024)		/* 4KB */
#define BEXEC_MAX_STACK_SIZE		(64 * 1024 * 1024)	/* 64MB */
#define BEXEC_MAX_HEAP_SIZE		(1024 * 1024 * 1024)	/* 1GB */

/* Códigos de erro */
#define BEXEC_ERR_NONE			0
#define BEXEC_ERR_MAGIC			-1	/* Magic number inválido */
#define BEXEC_ERR_CLASS			-2	/* Classe não suportada */
#define BEXEC_ERR_VERSION		-3	/* Versão não suportada */
#define BEXEC_ERR_MACHINE		-4	/* Arquitetura não suportada */
#define BEXEC_ERR_CORRUPT		-5	/* Arquivo corrompido */
#define BEXEC_ERR_NOSYSCALL		-6	/* Tabela de syscalls ausente */
#define BEXEC_ERR_MISSING_SYSCALL	-7	/* Syscall obrigatória faltando */
#define BEXEC_ERR_SECURITY		-8	/* Falha de segurança */
#define BEXEC_ERR_SIGNATURE		-9	/* Assinatura inválida */
#define BEXEC_ERR_PERMISSION		-10	/* Permissão negada */
#define BEXEC_ERR_RING			-11	/* Ring level incompatível */
#define BEXEC_ERR_SIZE			-12	/* Tamanho inválido */

#endif /* !_BIBLE_EXEC_FORMAT_H_ */