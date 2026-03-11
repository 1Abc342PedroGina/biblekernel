#ifndef _OSD_TYPES_H
#define _OSD_TYPES_H

/*
 * OSDTypes.h - Tipos Fundamentais do OSDKit (Operational System Drivers Kit)
 * 
 * "Drivers são Registros" - Extensão da filosofia do BibleKernel para o OSDKit
 * 
 * Este arquivo define os tipos base para todos os drivers do sistema.
 * 
 * O OSDKit é HÍBRIDO - sua configuração é feita via #ifdef no momento da
 * compilação, não por enums ou constantes em tempo de execução.
 * 
 * Flavors (sabores) são definidos por diretivas do pré-processador:
 *   - #ifdef OSD_DEBUG          : Debug, todas verificações off
 *   - #ifdef OSD_DEVELOPMENT     : Development, algumas verificações
 *   - #ifdef OSD_RELEASE_GENERIC : Release Generic, verificações padrão
 *   - #ifdef OSD_RELEASE_BIBLE   : Release Bible, verificações máximas
 *   - #ifdef OSD_PROJECTED       : Projetado (fins especiais)
 */

#include <bible/types.h>
#include <bible/cdefs.h>
#include <bible/regsister.h>

/*
 * ============================================================================
 * HANDLES ESPECÍFICOS DO OSDKIT
 * ============================================================================
 */

/* Handles para componentes do OSDKit */
typedef BK_REGISTER_HANDLE OSD_DRIVER_HANDLE;      /* Handle para driver */
typedef BK_REGISTER_HANDLE OSD_DEVICE_HANDLE;      /* Handle para dispositivo */
typedef BK_REGISTER_HANDLE OSD_INTERFACE_HANDLE;   /* Handle para interface */
typedef BK_REGISTER_HANDLE OSD_PROTOCOL_HANDLE;    /* Handle para protocolo */
typedef BK_REGISTER_HANDLE OSD_FILESYSTEM_HANDLE;  /* Handle para FS */
typedef BK_REGISTER_HANDLE OSD_SECURITY_HANDLE;    /* Handle para módulo de segurança */
typedef BK_REGISTER_HANDLE OSD_CHIP_HANDLE;        /* Handle para chip físico */
typedef BK_REGISTER_HANDLE OSD_BKEY_HANDLE;        /* Handle para chave BKEY */

/* Handles para operações */
typedef BK_REGISTER_HANDLE OSD_REQUEST_HANDLE;     /* Handle para requisição de driver */
typedef BK_REGISTER_HANDLE OSD_RESPONSE_HANDLE;    /* Handle para resposta */
typedef BK_REGISTER_HANDLE OSD_TRANSACTION_HANDLE; /* Handle para transação */

/* Handle inválido */
#define OSD_HANDLE_NULL      BK_REGISTER_NULL
#define OSD_HANDLE_INVALID   BK_REGISTER_INVALID

/*
 * ============================================================================
 * SISTEMAS DE ARQUIVOS
 * ============================================================================
 */

/* 
 * HALFS (Hardware Abstraction Layer File System)
 * - Camada de abstração que unifica todos os sistemas de arquivos
 * - Presente em todas as configurações do sistema
 */
#define OSD_FS_HALFS        0x48414C46  /* 'HALF' - HALFS (base) */

/*
 * Sistemas de arquivos HALFS (específicos do BibleOS)
 */
#ifdef OSD_HALFS_DISK
#define OSD_FS_BOFS         0x424F4653  /* 'BOFS' - BibleOS File System (Disco) */
/* BOFS: Sistema de arquivos principal em disco, com suporte a journaling e selos */
#endif

#ifdef OSD_HALFS_RAM
#define OSD_FS_BIBLEFS      0x42494246  /* 'BIBF' - Bible File System (RAM) */
/* BibleFS: Sistema de arquivos em RAM para dados temporários e caches */
#endif

/*
 * Sistemas de arquivos HALFS CTFS (Core Transaction File System)
 */
#ifdef OSD_CTFS
#define OSD_FS_CTFS         0x43544653  /* 'CTFS' - Core Transaction File System */
#define OSD_FS_FSCOREDAT    0x46534344  /* 'FSCD' - FSCoreDat (dados do core) */
/* 
 * CTFS: Sistema de arquivos transacional para operações críticas
 * FSCoreDat: Extensão do CTFS para dados do núcleo do sistema
 */
#endif

/*
 * Sistemas de arquivos tradicionais (suporte via HALFS)
 */
#ifdef OSD_FS_FAT
#define OSD_FS_FAT12        0x46413132  /* 'FA12' - FAT12 */
#define OSD_FS_FAT16        0x46413136  /* 'FA16' - FAT16 */
#define OSD_FS_FAT32        0x46413332  /* 'FA32' - FAT32 */
#endif

#ifdef OSD_FS_EXFAT
#define OSD_FS_EXFAT        0x45584641  /* 'EXFA' - exFAT */
#endif

#ifdef OSD_FS_CDFS
#define OSD_FS_CDFS         0x43444653  /* 'CDFS' - CD File System (ISO9660) */
#endif

/*
 * ============================================================================
 * TIPOS DE DRIVER POR CATEGORIA
 * ============================================================================
 */

/* 
 * Categorias de drivers (usadas em structs, não em #ifdefs)
 * Estas são constantes para preenchimento de campos, não para controle de compilação
 */

/* Drivers fundamentais */
#define OSD_DRIVER_BUS          0x0001  /* Driver de barramento */
#define OSD_DRIVER_CHIP         0x0002  /* Driver de chip físico */
#define OSD_DRIVER_CPU          0x0003  /* Driver de CPU */
#define OSD_DRIVER_MEMORY       0x0004  /* Driver de memória */
#define OSD_DRIVER_INTERRUPT    0x0005  /* Driver de IRQ */
#define OSD_DRIVER_DMA          0x0006  /* Driver de DMA */
#define OSD_DRIVER_CLOCK        0x0007  /* Driver de clock */
#define OSD_DRIVER_POWER        0x0008  /* Driver de energia */

/* Drivers de dispositivos */
#define OSD_DRIVER_STORAGE      0x0100  /* Driver de armazenamento */
#define OSD_DRIVER_NETWORK      0x0101  /* Driver de rede */
#define OSD_DRIVER_DISPLAY      0x0102  /* Driver de vídeo */
#define OSD_DRIVER_AUDIO        0x0103  /* Driver de áudio */
#define OSD_DRIVER_INPUT        0x0104  /* Driver de entrada */
#define OSD_DRIVER_SERIAL       0x0105  /* Driver serial */
#define OSD_DRIVER_USB          0x0106  /* Driver USB */
#define OSD_DRIVER_SENSOR       0x0107  /* Driver de sensor */

/* Drivers de barramento específico */
#define OSD_DRIVER_PCI          0x0200  /* Driver PCI */
#define OSD_DRIVER_PCIE         0x0201  /* Driver PCIe */
#define OSD_DRIVER_USB_HOST     0x0202  /* Driver USB host */
#define OSD_DRIVER_SATA         0x0203  /* Driver SATA */
#define OSD_DRIVER_NVME         0x0204  /* Driver NVMe */
#define OSD_DRIVER_I2C          0x0205  /* Driver I2C */
#define OSD_DRIVER_SPI          0x0206  /* Driver SPI */
#define OSD_DRIVER_GPIO         0x0207  /* Driver GPIO */

/* Drivers de sistema de arquivos (mapeiam para os OSD_FS_*) */
#define OSD_DRIVER_FS_BASE      0x0300  /* Base para drivers de FS */
#define OSD_DRIVER_FS_HALFS     (OSD_DRIVER_FS_BASE + 0x01)  /* HALFS */
#define OSD_DRIVER_FS_BOFS      (OSD_DRIVER_FS_BASE + 0x02)  /* BOFS */
#define OSD_DRIVER_FS_BIBLEFS   (OSD_DRIVER_FS_BASE + 0x03)  /* BibleFS */
#define OSD_DRIVER_FS_CTFS      (OSD_DRIVER_FS_BASE + 0x04)  /* CTFS */
#define OSD_DRIVER_FS_FSCOREDAT (OSD_DRIVER_FS_BASE + 0x05)  /* FSCoreDat */
#define OSD_DRIVER_FS_FAT       (OSD_DRIVER_FS_BASE + 0x10)  /* FAT (genérico) */
#define OSD_DRIVER_FS_EXFAT     (OSD_DRIVER_FS_BASE + 0x11)  /* exFAT */
#define OSD_DRIVER_FS_CDFS      (OSD_DRIVER_FS_BASE + 0x12)  /* CDFS */

/* Drivers de segurança */
#define OSD_DRIVER_SECURITY     0x0400  /* Driver de segurança genérico */
#define OSD_DRIVER_CRYPTO       0x0401  /* Driver criptográfico */
#define OSD_DRIVER_ATTESTATION  0x0402  /* Driver de atestação */
#define OSD_DRIVER_BKEY         0x0403  /* Driver de chaves BKEY */

/*
 * ============================================================================
 * ESTRUTURAS BASE DO OSDKIT
 * ============================================================================
 */

/* Informações de versão do driver */
struct osd_driver_version {
    BK_UI16      major;           /* Versão major */
    BK_UI16      minor;           /* Versão minor */
    BK_UI16      patch;           /* Patch level */
    BK_UI16      interface;       /* Versão da interface */
    BK_CHAR      vendor[32];      /* Vendor string */
    BK_CHAR      model[32];       /* Modelo do driver */
#ifdef OSD_DEBUG
    BK_CHAR      build_date[20];  /* Data de compilação (debug) */
    BK_CHAR      build_time[20];  /* Hora de compilação (debug) */
#endif
};

/* Identificação de dispositivo */
struct osd_device_id {
    BK_UI16      vendor_id;       /* ID do vendor */
    BK_UI16      device_id;       /* ID do dispositivo */
    BK_UI16      subsystem_vendor;/* Subsystem vendor */
    BK_UI16      subsystem_device;/* Subsystem device */
    BK_UI8       class_code;      /* Classe do dispositivo */
    BK_UI8       subclass;        /* Subclasse */
    BK_UI8       prog_if;         /* Programming interface */
    BK_UI8       revision;        /* Revisão */
};

/* Recurso de hardware (I/O, memória, IRQ, DMA) */
struct osd_hardware_resource {
    BK_UINT      type;            /* Tipo do recurso */
#define OSD_RESOURCE_IO           0x01  /* Porta I/O */
#define OSD_RESOURCE_MEMORY       0x02  /* Região de memória */
#define OSD_RESOURCE_IRQ          0x03  /* IRQ */
#define OSD_RESOURCE_DMA          0x04  /* Canal DMA */
#define OSD_RESOURCE_BAR          0x05  /* PCI BAR */
    
    BK_UINT64    start;           /* Endereço/base inicial */
    BK_UINT64    end;             /* Endereço/base final */
    BK_UINT64    flags;           /* Flags do recurso */
#define OSD_RESOURCE_PREFETCHABLE 0x01  /* Memória prefetchable */
#define OSD_RESOURCE_READ_ONLY    0x02  /* Apenas leitura */
#define OSD_RESOURCE_WRITE_ONLY   0x04  /* Apenas escrita */
#define OSD_RESOURCE_VOLATILE     0x08  /* Volátil */
    
    BK_CHAR      description[64]; /* Descrição do recurso */
};

/*
 * ============================================================================
 * TIPOS PARA OPERAÇÕES COM CHIP FÍSICO
 * ============================================================================
 */

/* Desafio para chip (usado nas verificações de hardware) */
struct osd_chip_challenge {
    BK_UI64       nonce;           /* Número aleatório */
    BK_UI64       timestamp;       /* Timestamp */
    BK_UI8        challenge_data[64]; /* Dados do desafio */
#ifdef OSD_RELEASE_BIBLE
    BK_UI8        hmac[32];        /* HMAC do desafio (Bible apenas) */
#endif
};

/* Resposta do chip */
struct osd_chip_response {
    BK_UI64       response_nonce;  /* Nonce da resposta */
    BK_UI64       chip_id;         /* ID do chip */
    BK_UI8        signature[256];  /* Assinatura da resposta */
    BK_UI8        response_data[64]; /* Dados da resposta */
};

/* Chave BKEY */
struct osd_bkey {
    BK_UI8        key_id[16];      /* ID da chave */
    BK_UI8        key_data[64];    /* Dados da chave (criptografados) */
    BK_UI64       valid_from;      /* Válida a partir de */
    BK_UI64       valid_until;     /* Válida até */
    BK_UI32       key_flags;       /* Flags da chave */
#define OSD_BKEY_ACTIVE           0x0001  /* Chave ativa */
#define OSD_BKEY_REVOKED          0x0002  /* Chave revogada */
#define OSD_BKEY_EXPIRED          0x0004  /* Chave expirada */
#define OSD_BKEY_HARDWARE         0x0008  /* Chave de hardware */
#define OSD_BKEY_SOFTWARE         0x0010  /* Chave de software */
};

/*
 * ============================================================================
 * MACROS DE VERIFICAÇÃO CONDICIONAL
 * ============================================================================
 */

/* Macros que se comportam diferentemente conforme o flavor */

#ifdef OSD_DEBUG
/* Debug: sem verificações, apenas logging */
#define OSD_VERIFY_CHIP(chip, req)      do { \
    if (chip) osd_log("Chip verification skipped (DEBUG)"); \
} while (0)

#define OSD_VERIFY_BKEY(key, data)      do { \
    osd_log("BKEY verification skipped (DEBUG)"); \
} while (0)

#define OSD_VERIFY_SIGNATURE(sig, data)  (BK_TRUE)

#elif defined(OSD_DEVELOPMENT)
/* Development: verificações leves, não bloqueantes */
#define OSD_VERIFY_CHIP(chip, req)      osd_verify_chip_light(chip, req)
#define OSD_VERIFY_BKEY(key, data)      osd_verify_bkey_light(key, data)
#define OSD_VERIFY_SIGNATURE(sig, data)  osd_verify_signature_light(sig, data)

#elif defined(OSD_RELEASE_GENERIC)
/* Release Generic: verificações padrão */
#define OSD_VERIFY_CHIP(chip, req)      osd_verify_chip_full(chip, req)
#define OSD_VERIFY_BKEY(key, data)      osd_verify_bkey_full(key, data)
#define OSD_VERIFY_SIGNATURE(sig, data)  osd_verify_signature_full(sig, data)

#elif defined(OSD_RELEASE_BIBLE)
/* Release Bible: verificações máximas */
#define OSD_VERIFY_CHIP(chip, req)      osd_verify_chip_paranoid(chip, req)
#define OSD_VERIFY_BKEY(key, data)      osd_verify_bkey_paranoid(key, data)
#define OSD_VERIFY_SIGNATURE(sig, data)  osd_verify_signature_paranoid(sig, data)

#elif defined(OSD_PROJECTED)
/* Projetado: verificações especiais + contadores */
#define OSD_VERIFY_CHIP(chip, req)      osd_verify_chip_projected(chip, req)
#define OSD_VERIFY_BKEY(key, data)      osd_verify_bkey_projected(key, data)
#define OSD_VERIFY_SIGNATURE(sig, data)  osd_verify_signature_projected(sig, data)

#else
/* Padrão (seguro): pelo menos verificações genéricas */
#define OSD_VERIFY_CHIP(chip, req)      osd_verify_chip_generic(chip, req)
#define OSD_VERIFY_BKEY(key, data)      osd_verify_bkey_generic(key, data)
#define OSD_VERIFY_SIGNATURE(sig, data)  osd_verify_signature_generic(sig, data)
#endif

/*
 * ============================================================================
 * FUNÇÕES PÚBLICAS DO OSDKIT
 * ============================================================================
 */

/* Inicialização e finalização */
OSD_DRIVER_HANDLE osd_driver_register(BK_UINT16 driver_type, const char *name);
BK_I32 osd_driver_unregister(OSD_DRIVER_HANDLE driver);

/* Gerenciamento de dispositivos */
OSD_DEVICE_HANDLE osd_device_attach(OSD_DRIVER_HANDLE driver, struct osd_device_id *id);
BK_I32 osd_device_detach(OSD_DEVICE_HANDLE device);
BK_I32 osd_device_add_resource(OSD_DEVICE_HANDLE device, struct osd_hardware_resource *resource);

/* Operações com chip */
BK_I32 osd_chip_challenge(OSD_CHIP_HANDLE chip, struct osd_chip_challenge *challenge);
BK_I32 osd_chip_verify(OSD_CHIP_HANDLE chip, struct osd_chip_response *response);
OSD_BKEY_HANDLE osd_bkey_get(BK_UI8 *key_id);
BK_I32 osd_bkey_revoke(OSD_BKEY_HANDLE key);

/* Sistema de arquivos */
OSD_FILESYSTEM_HANDLE osd_fs_mount(BK_UINT32 fs_type, OSD_DEVICE_HANDLE device, BK_UINT32 flags);
BK_I32 osd_fs_unmount(OSD_FILESYSTEM_HANDLE fs);
BK_I32 osd_fs_format(OSD_DEVICE_HANDLE device, BK_UINT32 fs_type, BK_UINT32 flags);

/*
 * ============================================================================
 * ASSERTIVAS DE COMPILAÇÃO
 * ============================================================================
 */

/* Garante que pelo menos um flavor está definido */
#if !defined(OSD_DEBUG) && !defined(OSD_DEVELOPMENT) && \
    !defined(OSD_RELEASE_GENERIC) && !defined(OSD_RELEASE_BIBLE) && \
    !defined(OSD_PROJECTED)
#warning "Nenhum flavor OSD definido. Usando configuração padrão de segurança."
#endif

/* Garante consistência dos sistemas de arquivos */
#ifdef OSD_HALFS_DISK
#ifndef OSD_FS_BOFS
#error "HALFS Disk requer BOFS"
#endif
#endif

#ifdef OSD_HALFS_RAM
#ifndef OSD_FS_BIBLEFS
#error "HALFS RAM requer BibleFS"
#endif
#endif

#endif /* _OSD_TYPES_H */
