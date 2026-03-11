#ifndef _OSD_ERRNO_H
#define _OSD_ERRNO_H

/*
 * OSDErrno.h - Códigos de erro do OSDKit (Operational System Drivers Kit)
 * 
 * "Erros são Registros" - Até as falhas seguem a filosofia do sistema
 * 
 * Os códigos de erro no OSDKit seguem um padrão específico:
 * - Positivos: Erros leves, advertências, condições recuperáveis
 * - Negativos: Erros graves, falhas de segurança, condições irreversíveis
 * 
 * Todos os erros relacionados a pirataria e violação de segurança são NEGATIVOS.
 * Todos os erros de I/O e operacionais normais são POSITIVOS.
 * 
 * Retorno padrão das funções OSD: OSD_X (onde X é o código de erro)
 */

/*
 * ============================================================================
 * CÓDIGOS DE ERRO ZERO (SUCESSO)
 * ============================================================================
 */

#define OSD_SUCCESS                      0  /* Operação concluída com sucesso */
#define OSD_OK                            0  /* Sinônimo de sucesso */
#define OSD_OPERATION_COMPLETE            0  /* Operação finalizada */

/*
 * ============================================================================
 * ERROS POSITIVOS (LEVES, ADVERTÊNCIAS, RECUPERÁVEIS)
 * Faixa: 1 a 999
 * ============================================================================
 */

/* Erros de I/O e operacionais (1-99) */
#define OSD_IO_PENDING                    1  /* Operação I/O pendente */
#define OSD_IO_INCOMPLETE                  2  /* I/O incompleto, mais dados pendentes */
#define OSD_IO_PARTIAL                     3  /* Transferência parcial */
#define OSD_IO_RETRY                       4  /* Tentar novamente */
#define OSD_IO_BUSY                        5  /* Dispositivo ocupado */
#define OSD_IO_TIMEOUT                      6  /* Timeout de I/O */
#define OSD_IO_INTERRUPTED                  7  /* I/O interrompido por sinal */
#define OSD_IO_ABORTED                      8  /* I/O abortado */

/* Erros de recurso (100-199) */
#define OSD_RESOURCE_UNAVAILABLE          100  /* Recurso temporariamente indisponível */
#define OSD_RESOURCE_BUSY                  101  /* Recurso ocupado */
#define OSD_RESOURCE_STALLED               102  /* Recurso estagnado */
#define OSD_RESOURCE_LIMITED               103  /* Recurso limitado, operação lenta */
#define OSD_MEMORY_LOW                      104  /* Pouca memória, mas ainda funciona */
#define OSD_MEMORY_FRAGMENTED              105  /* Memória fragmentada */
#define OSD_CACHE_COLD                      106  /* Cache frio, performance reduzida */

/* Advertências de configuração (200-299) */
#define OSD_CONFIG_SUGGEST                 200  /* Configuração sugerida diferente */
#define OSD_CONFIG_LEGACY                   201  /* Usando modo legado */
#define OSD_CONFIG_COMPAT                   202  /* Modo compatibilidade ativo */
#define OSD_CONFIG_FALLBACK                 203  /* Fallback para configuração padrão */
#define OSD_CONFIG_PARTIAL                  204  /* Configuração parcial aplicada */

/* Advertências de hardware (300-399) */
#define OSD_HARDWARE_THROTTLED             300  /* Hardware throttled por temperatura */
#define OSD_HARDWARE_AGING                  301  /* Hardware envelhecido, performance reduzida */
#define OSD_HARDWARE_REFRESH_NEEDED         302  /* Hardware precisa de recalibração */
#define OSD_HARDWARE_CALIBRATING            303  /* Hardware em calibração */
#define OSD_HARDWARE_NOISY                   304  /* Sinal ruidoso, mas legível */
#define OSD_HARDWARE_DEGRADED               305  /* Hardware degradado, mas funcional */

/* Advertências de driver (400-499) */
#define OSD_DRIVER_COMPAT                   400  /* Driver em modo compatibilidade */
#define OSD_DRIVER_LEGACY                    401  /* Driver legado */
#define OSD_DRIVER_STUB                      402  /* Driver stub (funcionalidade limitada) */
#define OSD_DRIVER_EMULATION                 403  /* Driver em emulação */
#define OSD_DRIVER_FALLBACK                  404  /* Fallback para driver genérico */

/* Erros de sistema de arquivos (500-599) */
#define OSD_FS_FRAGMENTED                   500  /* FS fragmentado, optimize recomendado */
#define OSD_FS_DIRTY                        501  /* FS sujo, recovery na próxima montagem */
#define OSD_FS_JOURNAL_FULL                  502  /* Journal cheio, checkpoint necessário */
#define OSD_FS_QUOTA_WARNING                 503  /* Quota próximo do limite */
#define OSD_FS_CASE_INSENSITIVE              504  /* Operação case-insensitive aplicada */

/* Estados de transição (600-699) */
#define OSD_STATE_CHANGING                  600  /* Estado em transição */
#define OSD_STATE_PENDING                    601  /* Mudança de estado pendente */
#define OSD_STATE_RECONFIG                   602  /* Reconfiguração em andamento */
#define OSD_STATE_RECOVERY                   603  /* Modo recovery ativo */
#define OSD_STATE_DEGRADED                   604  /* Estado degradado */

/*
 * ============================================================================
 * ERROS NEGATIVOS (GRAVES, SEGURANÇA, IRRECUPERÁVEIS)
 * Faixa: -1 a -999
 * ============================================================================
 */

/* Erros genéricos graves (-1 a -99) */
#define OSD_ERROR                           -1  /* Erro genérico */
#define OSD_FAILED                           -2  /* Falha genérica */
#define OSD_INVALID                         -3  /* Inválido */
#define OSD_NOT_SUPPORTED                   -4  /* Não suportado */
#define OSD_NOT_IMPLEMENTED                 -5  /* Não implementado */
#define OSD_NOT_ALLOWED                     -6  /* Não permitido */
#define OSD_DENIED                           -7  /* Acesso negado */
#define OSD_FORBIDDEN                        -8  /* Proibido */
#define OSD_DISABLED                         -9  /* Desabilitado */

/* Erros de parâmetro (-100 a -199) */
#define OSD_INVALID_PARAM                   -100 /* Parâmetro inválido */
#define OSD_INVALID_HANDLE                   -101 /* Handle inválido */
#define OSD_INVALID_TYPE                     -102 /* Tipo inválido */
#define OSD_INVALID_SIZE                     -103 /* Tamanho inválido */
#define OSD_INVALID_ADDRESS                  -104 /* Endereço inválido */
#define OSD_INVALID_REQUEST                  -105 /* Requisição inválida */
#define OSD_INVALID_STATE                    -106 /* Estado inválido */
#define OSD_NULL_POINTER                     -107 /* Ponteiro nulo */
#define OSD_OUT_OF_RANGE                     -108 /* Fora do intervalo */

/* Erros de recurso (-200 a -299) */
#define OSD_NO_MEMORY                        -200 /* Sem memória */
#define OSD_NO_RESOURCE                      -201 /* Sem recurso disponível */
#define OSD_NO_DEVICE                        -202 /* Dispositivo não encontrado */
#define OSD_NO_DRIVER                        -203 /* Driver não encontrado */
#define OSD_NO_INTERFACE                     -204 /* Interface não encontrada */
#define OSD_NO_FILESYSTEM                    -205 /* Sistema de arquivos não encontrado */
#define OSD_NO_SPACE                         -206 /* Sem espaço */
#define OSD_NO_PERMISSION                    -207 /* Sem permissão */
#define OSD_EXHAUSTED                         -208 /* Recursos exauridos */

/* Erros de I/O críticos (-300 a -399) */
#define OSD_IO_ERROR                         -300 /* Erro de I/O */
#define OSD_IO_CORRUPTED                     -301 /* Dados corrompidos */
#define OSD_IO_BROKEN                        -302 /* Link/pipe quebrado */
#define OSD_IO_INVALID                       -303 /* Dados inválidos */
#define OSD_IO_MISMATCH                      -304 /* Incompatibilidade de dados */
#define OSD_IO_CHECKSUM                      -305 /* Checksum inválido */
#define OSD_IO_CRC                           -306 /* CRC error */
#define OSD_IO_BAD_SECTOR                    -307 /* Setor danificado */
#define OSD_IO_MEDIA                          -308 /* Erro de mídia */

/* Erros de hardware (-400 a -499) */
#define OSD_HARDWARE_ERROR                    -400 /* Erro de hardware */
#define OSD_HARDWARE_MISSING                  -401 /* Hardware ausente */
#define OSD_HARDWARE_FAILURE                  -402 /* Falha de hardware */
#define OSD_HARDWARE_DISCONNECTED             -403 /* Hardware desconectado */
#define OSD_HARDWARE_UNKNOWN                   -404 /* Hardware desconhecido */
#define OSD_HARDWARE_UNSUPPORTED              -405 /* Hardware não suportado */
#define OSD_HARDWARE_INCOMPATIBLE             -406 /* Hardware incompatível */
#define OSD_HARDWARE_FUSED                    -407 /* Hardware fused (bloqueado) */
#define OSD_HARDWARE_BRICKED                  -408 /* Hardware brickado */

/* Erros de driver (-500 a -599) */
#define OSD_DRIVER_ERROR                      -500 /* Erro de driver */
#define OSD_DRIVER_MISSING                    -501 /* Driver ausente */
#define OSD_DRIVER_CORRUPTED                  -502 /* Driver corrompido */
#define OSD_DRIVER_INCOMPATIBLE               -503 /* Driver incompatível */
#define OSD_DRIVER_VERSION                    -504 /* Versão de driver incorreta */
#define OSD_DRIVER_EXPIRED                     -505 /* Driver expirado */
#define OSD_DRIVER_BLACKLISTED                 -506 /* Driver na blacklist */
#define OSD_DRIVER_REVOKED                     -507 /* Driver revogado */

/* ============================================================================
 * ERROS DE SEGURANÇA E PIRATARIA (-600 a -799)
 * ============================================================================
 */

/* Erros de chip e BKEY (-600 a -649) */
#define OSD_CHIP_ERROR                        -600 /* Erro genérico de chip */
#define OSD_CHIP_NOT_FOUND                    -601 /* Chip não encontrado */
#define OSD_CHIP_INVALID                      -602 /* Chip inválido */
#define OSD_CHIP_UNAUTHORIZED                  -603 /* Chip não autorizado */
#define OSD_CHIP_BLACKLISTED                   -604 /* Chip na blacklist */
#define OSD_CHIP_REVOKED                       -605 /* Chip revogado */
#define OSD_CHIP_EXPIRED                       -606 /* Chip expirado */
#define OSD_CHIP_FUSED                         -607 /* Chip fused (bloqueado) */
#define OSD_CHIP_COUNTER_MAX                   -608 /* Contador de chip estourado */
#define OSD_CHIP_TAMPERED                      -609 /* Chip violado fisicamente */

/* Erros de BKEY (-650 a -679) */
#define OSD_BKEY_ERROR                        -650 /* Erro genérico de BKEY */
#define OSD_BKEY_NOT_FOUND                    -651 /* BKEY não encontrada */
#define OSD_BKEY_INVALID                      -652 /* BKEY inválida */
#define OSD_BKEY_EXPIRED                       -653 /* BKEY expirada */
#define OSD_BKEY_REVOKED                       -654 /* BKEY revogada */
#define OSD_BKEY_BLACKLISTED                   -655 /* BKEY na blacklist */
#define OSD_BKEY_MISMATCH                      -656 /* BKEY não corresponde */
#define OSD_BKEY_CORRUPTED                     -657 /* BKEY corrompida */
#define OSD_BKEY_UNAUTHORIZED                  -658 /* BKEY não autorizada */
#define OSD_BKEY_HARDWARE                      -659 /* BKEY de hardware inválida */
#define OSD_BKEY_SOFTWARE                      -660 /* BKEY de software inválida */

/* Erros de verificação de integridade (-680 a -699) */
#define OSD_VERIFY_ERROR                      -680 /* Erro de verificação */
#define OSD_VERIFY_FAILED                     -681 /* Verificação falhou */
#define OSD_SIGNATURE_INVALID                 -682 /* Assinatura inválida */
#define OSD_SIGNATURE_EXPIRED                  -683 /* Assinatura expirada */
#define OSD_SIGNATURE_REVOKED                  -684 /* Assinatura revogada */
#define OSD_HASH_MISMATCH                      -685 /* Hash não corresponde */
#define OSD_HASH_INVALID                       -686 /* Hash inválido */
#define OSD_CERTIFICATE_INVALID                -687 /* Certificado inválido */
#define OSD_CERTIFICATE_EXPIRED                -688 /* Certificado expirado */
#define OSD_CERTIFICATE_REVOKED                -689 /* Certificado revogado */

/* Erros de atestação (-700 a -719) */
#define OSD_ATTESTATION_ERROR                  -700 /* Erro de atestação */
#define OSD_ATTESTATION_FAILED                 -701 /* Atestação falhou */
#define OSD_ATTESTATION_INVALID                -702 /* Atestação inválida */
#define OSD_ATTESTATION_EXPIRED                -703 /* Atestação expirada */
#define OSD_ATTESTATION_REVOKED                -704 /* Atestação revogada */
#define OSD_ATTESTATION_MISMATCH               -705 /* Atestação não corresponde */

/* Erros de "Pirataria Detectada" (-720 a -749) */
#define OSD_PIRACY_DETECTED                    -720 /* Pirataria detectada */
#define OSD_PIRACY_HARDWARE                    -721 /* Hardware não original */
#define OSD_PIRACY_FIRMWARE                    -722 /* Firmware não original */
#define OSD_PIRACY_BOOT                         -723 /* Bootloader não original */
#define OSD_PIRACY_KERNEL                      -724 /* Kernel modificado */
#define OSD_PIRACY_DRIVER                      -725 /* Driver não autorizado */
#define OSD_PIRACY_FILE                        -726 /* Arquivo do sistema modificado */
#define OSD_PIRACY_BOOTLEG                      -727 /* Cópia bootleg detectada */
#define OSD_PIRACY_EMULATOR                     -728 /* Executando em emulador */
#define OSD_PIRACY_VM                          -729 /* Executando em VM não autorizada */

/* Erros de Karma (-750 a -769) */
#define OSD_KARMA_ERROR                        -750 /* Erro de karma */
#define OSD_KARMA_VIOLATION                    -751 /* Violação de karma */
#define OSD_KARMA_COUNTER                      -752 /* Contador de karma excedido */
#define OSD_KARMA_LIMIT                        -753 /* Limite de karma atingido */
#define OSD_KARMA_BLOCKED                      -754 /* Bloqueado por karma */
#define OSD_KARMA_PERMANENT                    -755 /* Karma permanente */
#define OSD_KARMA_IRREVERSIBLE                 -756 /* Karma irreversível */

/* Erros de intervenção (-770 a -789) */
#define OSD_INTERVENTION_NEEDED                 -770 /* Intervenção da Bible Foundation necessária */
#define OSD_INTERVENTION_PENDING               -771 /* Intervenção pendente */
#define OSD_INTERVENTION_REQUIRED              -772 /* Intervenção obrigatória */
#define OSD_INTERVENTION_BLOCKED               -773 /* Bloqueado até intervenção */

/* ============================================================================
 * ERROS ESPECÍFICOS DE SISTEMAS DE ARQUIVOS (-800 a -899)
 * ============================================================================
 */

/* Erros HALFS (-800 a -819) */
#define OSD_HALFS_ERROR                       -800 /* Erro HALFS */
#define OSD_HALFS_CORRUPTED                   -801 /* HALFS corrompido */
#define OSD_HALFS_INCOMPATIBLE                -802 /* HALFS incompatível */
#define OSD_HALFS_VERSION                     -803 /* Versão HALFS incorreta */
#define OSD_HALFS_LAYER                       -804 /* Erro na camada HALFS */

/* Erros BOFS (-820 a -839) */
#define OSD_BOFS_ERROR                        -820 /* Erro BOFS */
#define OSD_BOFS_CORRUPTED                    -821 /* BOFS corrompido */
#define OSD_BOFS_JOURNAL                      -822 /* Erro no journal BOFS */
#define OSD_BOFS_SUPERBLOCK                    -823 /* Superbloco BOFS inválido */
#define OSD_BOFS_INODE                         -824 /* Inode BOFS inválido */
#define OSD_BOFS_SEAL                          -825 /* Selo BOFS violado */

/* Erros BibleFS (-840 a -859) */
#define OSD_BIBLEFS_ERROR                     -840 /* Erro BibleFS */
#define OSD_BIBLEFS_CORRUPTED                 -841 /* BibleFS corrompido */
#define OSD_BIBLEFS_INVALID                    -842 /* BibleFS inválido */
#define OSD_BIBLEFS_NO_RAM                     -843 /* RAM insuficiente para BibleFS */

/* Erros CTFS (-860 a -879) */
#define OSD_CTFS_ERROR                        -860 /* Erro CTFS */
#define OSD_CTFS_CORRUPTED                    -861 /* CTFS corrompido */
#define OSD_CTFS_TRANSACTION                   -862 /* Erro em transação CTFS */
#define OSD_CTFS_COMMIT                        -863 /* Erro ao commitar CTFS */
#define OSD_CTFS_ROLLBACK                      -864 /* Erro ao fazer rollback */

/* Erros FSCoreDat (-880 a -889) */
#define OSD_FSCOREDAT_ERROR                   -880 /* Erro FSCoreDat */
#define OSD_FSCOREDAT_CORRUPTED               -881 /* FSCoreDat corrompido */
#define OSD_FSCOREDAT_INVALID                  -882 /* FSCoreDat inválido */

/* Erros de sistemas de arquivos tradicionais (-890 a -899) */
#define OSD_FAT_ERROR                         -890 /* Erro FAT */
#define OSD_FAT_CORRUPTED                     -891 /* FAT corrompido */
#define OSD_EXFAT_ERROR                       -892 /* Erro exFAT */
#define OSD_EXFAT_CORRUPTED                   -893 /* exFAT corrompido */
#define OSD_CDFS_ERROR                        -894 /* Erro CDFS */
#define OSD_CDFS_CORRUPTED                    -895 /* CDFS corrompido */

/* ============================================================================
 * ERROS CRÍTICOS DO SISTEMA (-900 a -999)
 * ============================================================================
 */

#define OSD_CRITICAL_ERROR                    -900 /* Erro crítico genérico */
#define OSD_KERNEL_PANIC                      -901 /* Kernel panic iminente */
#define OSD_SYSTEM_HALT                       -902 /* Sistema vai parar */
#define OSD_SYSTEM_REBOOT                     -903 /* Sistema vai reiniciar */
#define OSD_SYSTEM_SHUTDOWN                   -904 /* Sistema vai desligar */
#define OSD_SYSTEM_FREEZE                     -905 /* Sistema congelado */

#define OSD_SECURITY_BREACH                   -910 /* Brecha de segurança */
#define OSD_SECURITY_VIOLATION                -911 /* Violação de segurança */
#define OSD_SECURITY_COMPROMISED              -912 /* Segurança comprometida */

#define OSD_FORMAT_SCHEDULED                  -920 /* Formatação agendada (após 4 erros) */
#define OSD_FORMAT_IMMINENT                    -921 /* Formatação iminente */
#define OSD_FORMAT_IN_PROGRESS                 -922 /* Formatação em andamento */
#define OSD_FORMAT_COMPLETE                    -923 /* Formatação completa */

#define OSD_FIRMWARE_FORMAT_SCHEDULED          -930 /* Formatação de firmware agendada (10 erros) */
#define OSD_FIRMWARE_FORMAT_IMMINENT           -931 /* Formatação de firmware iminente */
#define OSD_FIRMWARE_FORMAT_IN_PROGRESS        -932 /* Formatação de firmware em andamento */
#define OSD_FIRMWARE_FORMAT_COMPLETE           -933 /* Formatação de firmware completa */

#define OSD_LDT_DISABLED                       -940 /* LDT desabilitado (modo projetado) */
#define OSD_GDT_FLATTENED                      -941 /* GDT flattened (sem separação ring 3/0) */

#define OSD_COUNTER_1                          -950 /* 1º erro de contador (advertência) */
#define OSD_COUNTER_2                          -951 /* 2º erro de contador (último aviso) */
#define OSD_COUNTER_3                          -952 /* 3º erro de contador (BLOQUEIO) */

/*
 * ============================================================================
 * MACROS DE VERIFICAÇÃO DE ERRO
 * ============================================================================
 */

/* Verifica se é sucesso */
#define OSD_IS_SUCCESS(x)      ((x) == OSD_SUCCESS)

/* Verifica se é erro (qualquer código diferente de 0) */
#define OSD_IS_ERROR(x)        ((x) != 0)

/* Verifica se é erro positivo (advertência) */
#define OSD_IS_WARNING(x)      ((x) > 0)

/* Verifica se é erro negativo (grave) */
#define OSD_IS_CRITICAL(x)     ((x) < 0)

/* Verifica se é erro de segurança (faixa -600 a -799) */
#define OSD_IS_SECURITY_ERROR(x)  ((x) <= -600 && (x) >= -799)

/* Verifica se é erro de pirataria (faixa -720 a -749) */
#define OSD_IS_PIRACY_ERROR(x)    ((x) <= -720 && (x) >= -749)

/* Verifica se é erro de karma (faixa -750 a -769) */
#define OSD_IS_KARMA_ERROR(x)     ((x) <= -750 && (x) >= -769)

/* Verifica se requer intervenção (faixa -770 a -789) */
#define OSD_NEEDS_INTERVENTION(x) ((x) <= -770 && (x) >= -789)

/* Verifica se é erro de formatação (faixa -920 a -933) */
#define OSD_IS_FORMAT_ERROR(x)    ((x) <= -920 && (x) >= -933)

/*
 * ============================================================================
 * FUNÇÕES DE TRATAMENTO DE ERRO
 * ============================================================================
 */

/* Retorna string descritiva do erro */
const char *osd_strerror(OSD_X error);

/* Retorna o nome do erro (para logging) */
const char *osd_errname(OSD_X error);

/* Retorna a severidade do erro */
int osd_errseverity(OSD_X error);

/* Retorna true se o erro é fatal para a operação atual */
int osd_errfatal(OSD_X error);

/* Retorna true se o erro é fatal para o sistema */
int osd_errsystemfatal(OSD_X error);

#endif /* _OSD_ERRNO_H */
