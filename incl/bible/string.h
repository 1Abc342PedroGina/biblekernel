#ifndef _BIBLE_STRING_H_
#define _BIBLE_STRING_H_

/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2024 Bible System Developers
 *
 * string.h - Funções de manipulação de strings para kernel mode (Ring 0)
 * 
 * Este cabeçalho fornece funções de string seguras para uso no kernel,
 * sem dependências de userland. Todas as funções operam em memória do kernel
 * e não fazem chamadas de sistema.
 */

#include <bible/cdefs.h>
#include <bible/types.h>
#include <bible/ktypes.h>
#include <bible/regsister.h>
#include <bible/task.h>
#include <bible/vm.h>
#include <bible/pmap.h>
#include <bible/ipc.h>

__BK_BEGIN_DECLS

/* ============================================================================
 * MACROS DE UTILIDADE
 * ============================================================================ */

/* Obtém o tamanho de um array estático */
#define BK_ARRAY_SIZE(arr)     (sizeof(arr) / sizeof((arr)[0]))

/* Verifica se um ponteiro é válido no espaço do kernel */
#define BK_IS_KERNEL_ADDR(ptr) \
    (((BK_VM_OFFSET)(ptr) >= BK_VM_KERNEL_BASE) && \
     ((BK_VM_OFFSET)(ptr) <= BK_VM_KERNEL_END))

/* ============================================================================
 * FUNÇÕES DE MEMÓRIA (declaradas como inline para performance)
 * ============================================================================ */

/**
 * bk_memset - Preenche uma região de memória com um valor constante
 * @s: Ponteiro para a região de memória
 * @c: Valor a ser escrito (convertido para unsigned char)
 * @n: Número de bytes a serem preenchidos
 * 
 * Retorna: O ponteiro @s
 * 
 * Uso: Apenas para memória do kernel, sem verificação de limites
 */
static __BK_ALWAYS_INLINE void *
bk_memset(void *s, BK_I32 c, BK_SIZE n)
{
    BK_UI8 *p = (BK_UI8 *)s;
    BK_UI8 uc = (BK_UI8)c;
    
    while (n--) {
        *p++ = uc;
    }
    
    return s;
}

/**
 * bk_memcpy - Copia uma região de memória
 * @dest: Ponteiro de destino
 * @src: Ponteiro de origem
 * @n: Número de bytes a copiar
 * 
 * Retorna: O ponteiro @dest
 * 
 * Uso: As regiões não devem se sobrepor. Para sobreposição, use bk_memmove.
 */
static __BK_ALWAYS_INLINE void *
bk_memcpy(void *dest, const void *src, BK_SIZE n)
{
    BK_UI8 *d = (BK_UI8 *)dest;
    const BK_UI8 *s = (const BK_UI8 *)src;
    
    while (n--) {
        *d++ = *s++;
    }
    
    return dest;
}

/**
 * bk_memmove - Copia uma região de memória (suporta sobreposição)
 * @dest: Ponteiro de destino
 * @src: Ponteiro de origem
 * @n: Número de bytes a copiar
 * 
 * Retorna: O ponteiro @dest
 */
static __BK_ALWAYS_INLINE void *
bk_memmove(void *dest, const void *src, BK_SIZE n)
{
    BK_UI8 *d = (BK_UI8 *)dest;
    const BK_UI8 *s = (const BK_UI8 *)src;
    
    if (d < s) {
        /* Copia da esquerda para a direita */
        while (n--) {
            *d++ = *s++;
        }
    } else if (d > s) {
        /* Copia da direita para a esquerda */
        d += n;
        s += n;
        while (n--) {
            *--d = *--s;
        }
    }
    
    return dest;
}

/**
 * bk_memcmp - Compara duas regiões de memória
 * @s1: Primeira região
 * @s2: Segunda região
 * @n: Número de bytes a comparar
 * 
 * Retorna: 0 se iguais, <0 se s1 < s2, >0 se s1 > s2
 */
static __BK_ALWAYS_INLINE BK_I32
bk_memcmp(const void *s1, const void *s2, BK_SIZE n)
{
    const BK_UI8 *p1 = (const BK_UI8 *)s1;
    const BK_UI8 *p2 = (const BK_UI8 *)s2;
    
    while (n--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    
    return 0;
}

/**
 * bk_memchr - Procura um caractere em uma região de memória
 * @s: Região de memória
 * @c: Caractere a procurar
 * @n: Número de bytes a verificar
 * 
 * Retorna: Ponteiro para a primeira ocorrência ou NULL se não encontrado
 */
static __BK_ALWAYS_INLINE void *
bk_memchr(const void *s, BK_I32 c, BK_SIZE n)
{
    const BK_UI8 *p = (const BK_UI8 *)s;
    BK_UI8 uc = (BK_UI8)c;
    
    while (n--) {
        if (*p == uc) {
            return (void *)p;
        }
        p++;
    }
    
    return NULL;
}

/**
 * bk_memrchr - Procura um caractere do fim para o início
 * @s: Região de memória
 * @c: Caractere a procurar
 * @n: Número de bytes a verificar
 * 
 * Retorna: Ponteiro para a última ocorrência ou NULL se não encontrado
 */
static __BK_ALWAYS_INLINE void *
bk_memrchr(const void *s, BK_I32 c, BK_SIZE n)
{
    const BK_UI8 *p = (const BK_UI8 *)s + n - 1;
    BK_UI8 uc = (BK_UI8)c;
    
    while (n--) {
        if (*p == uc) {
            return (void *)p;
        }
        p--;
    }
    
    return NULL;
}

/* ============================================================================
 * FUNÇÕES DE STRING (null-terminated)
 * ============================================================================ */

/**
 * bk_strlen - Calcula o comprimento de uma string
 * @s: String null-terminated
 * 
 * Retorna: Número de caracteres antes do '\\0'
 */
static __BK_ALWAYS_INLINE BK_SIZE
bk_strlen(const char *s)
{
    const char *p = s;
    
    while (*p) {
        p++;
    }
    
    return (BK_SIZE)(p - s);
}

/**
 * bk_strnlen - Calcula o comprimento limitado de uma string
 * @s: String
 * @maxlen: Tamanho máximo a verificar
 * 
 * Retorna: Mínimo entre o comprimento real e maxlen
 */
static __BK_ALWAYS_INLINE BK_SIZE
bk_strnlen(const char *s, BK_SIZE maxlen)
{
    const char *p = s;
    BK_SIZE n = 0;
    
    while (n < maxlen && *p) {
        p++;
        n++;
    }
    
    return n;
}

/**
 * bk_strcpy - Copia uma string
 * @dest: Buffer de destino
 * @src: String de origem
 * 
 * Retorna: O ponteiro @dest
 * 
 * ATENÇÃO: Não verifica overflow. Use bk_strlcpy para segurança.
 */
static __BK_ALWAYS_INLINE char *
bk_strcpy(char *dest, const char *src)
{
    char *d = dest;
    
    while ((*d++ = *src++)) {
        /* vazio */
    }
    
    return dest;
}

/**
 * bk_strncpy - Copia uma string com limite
 * @dest: Buffer de destino
 * @src: String de origem
 * @n: Tamanho máximo do buffer
 * 
 * Retorna: O ponteiro @dest
 * 
 * ATENÇÃO: Se o comprimento de src >= n, a string resultante NÃO será
 * null-terminated. Use bk_strlcpy para garantir terminação.
 */
static __BK_ALWAYS_INLINE char *
bk_strncpy(char *dest, const char *src, BK_SIZE n)
{
    char *d = dest;
    BK_SIZE i;
    
    for (i = 0; i < n && src[i]; i++) {
        d[i] = src[i];
    }
    for (; i < n; i++) {
        d[i] = '\0';
    }
    
    return dest;
}

/**
 * bk_strlcpy - Copia string com segurança (garante terminação)
 * @dest: Buffer de destino
 * @src: String de origem
 * @size: Tamanho do buffer de destino
 * 
 * Retorna: Comprimento de src (permite verificar truncamento)
 * 
 * Se o retorno >= size, a string foi truncada.
 */
static __BK_ALWAYS_INLINE BK_SIZE
bk_strlcpy(char *dest, const char *src, BK_SIZE size)
{
    BK_SIZE srclen = bk_strlen(src);
    BK_SIZE copylen = (srclen < size - 1) ? srclen : size - 1;
    
    if (size > 0) {
        bk_memcpy(dest, src, copylen);
        dest[copylen] = '\0';
    }
    
    return srclen;
}

/**
 * bk_strcat - Concatena strings
 * @dest: Buffer de destino (já deve conter uma string)
 * @src: String a ser concatenada
 * 
 * Retorna: O ponteiro @dest
 * 
 * ATENÇÃO: Não verifica overflow. Use bk_strlcat para segurança.
 */
static __BK_ALWAYS_INLINE char *
bk_strcat(char *dest, const char *src)
{
    char *d = dest;
    
    /* Vai até o final de dest */
    while (*d) {
        d++;
    }
    
    /* Copia src para o final */
    while ((*d++ = *src++)) {
        /* vazio */
    }
    
    return dest;
}

/**
 * bk_strncat - Concatena strings com limite
 * @dest: Buffer de destino
 * @src: String a ser concatenada
 * @n: Número máximo de caracteres a copiar de src
 * 
 * Retorna: O ponteiro @dest
 */
static __BK_ALWAYS_INLINE char *
bk_strncat(char *dest, const char *src, BK_SIZE n)
{
    char *d = dest;
    BK_SIZE i;
    
    /* Vai até o final de dest */
    while (*d) {
        d++;
    }
    
    /* Copia no máximo n caracteres de src */
    for (i = 0; i < n && src[i]; i++) {
        d[i] = src[i];
    }
    d[i] = '\0';
    
    return dest;
}

/**
 * bk_strlcat - Concatena strings com segurança
 * @dest: Buffer de destino
 * @src: String a ser concatenada
 * @size: Tamanho do buffer de destino
 * 
 * Retorna: Comprimento total que tentou criar (len(dest) + len(src))
 * 
 * Se o retorno >= size, ocorreu truncamento.
 */
static __BK_ALWAYS_INLINE BK_SIZE
bk_strlcat(char *dest, const char *src, BK_SIZE size)
{
    BK_SIZE destlen = bk_strnlen(dest, size);
    BK_SIZE srclen = bk_strlen(src);
    BK_SIZE copylen;
    
    if (destlen >= size) {
        return size + srclen;
    }
    
    copylen = (srclen < size - destlen - 1) ? srclen : size - destlen - 1;
    bk_memcpy(dest + destlen, src, copylen);
    dest[destlen + copylen] = '\0';
    
    return destlen + srclen;
}

/**
 * bk_strcmp - Compara duas strings
 * @s1: Primeira string
 * @s2: Segunda string
 * 
 * Retorna: 0 se iguais, <0 se s1 < s2, >0 se s1 > s2
 */
static __BK_ALWAYS_INLINE BK_I32
bk_strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    
    return (BK_UI8)*s1 - (BK_UI8)*s2;
}

/**
 * bk_strncmp - Compara duas strings com limite
 * @s1: Primeira string
 * @s2: Segunda string
 * @n: Número máximo de caracteres a comparar
 * 
 * Retorna: 0 se iguais, <0 se s1 < s2, >0 se s1 > s2
 */
static __BK_ALWAYS_INLINE BK_I32
bk_strncmp(const char *s1, const char *s2, BK_SIZE n)
{
    if (n == 0) {
        return 0;
    }
    
    while (--n && *s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    
    return (BK_UI8)*s1 - (BK_UI8)*s2;
}

/**
 * bk_strchr - Encontra a primeira ocorrência de um caractere
 * @s: String
 * @c: Caractere a procurar
 * 
 * Retorna: Ponteiro para o caractere ou NULL se não encontrado
 */
static __BK_ALWAYS_INLINE char *
bk_strchr(const char *s, BK_I32 c)
{
    char ch = (char)c;
    
    while (*s && *s != ch) {
        s++;
    }
    
    return (*s == ch) ? (char *)s : NULL;
}

/**
 * bk_strrchr - Encontra a última ocorrência de um caractere
 * @s: String
 * @c: Caractere a procurar
 * 
 * Retorna: Ponteiro para o caractere ou NULL se não encontrado
 */
static __BK_ALWAYS_INLINE char *
bk_strrchr(const char *s, BK_I32 c)
{
    const char *last = NULL;
    char ch = (char)c;
    
    while (*s) {
        if (*s == ch) {
            last = s;
        }
        s++;
    }
    
    return (char *)last;
}

/**
 * bk_strstr - Encontra a primeira ocorrência de uma substring
 * @haystack: String a ser pesquisada
 * @needle: Substring a procurar
 * 
 * Retorna: Ponteiro para o início da substring ou NULL se não encontrado
 */
static __BK_ALWAYS_INLINE char *
bk_strstr(const char *haystack, const char *needle)
{
    BK_SIZE needle_len = bk_strlen(needle);
    
    if (needle_len == 0) {
        return (char *)haystack;
    }
    
    while (*haystack) {
        if (bk_memcmp(haystack, needle, needle_len) == 0) {
            return (char *)haystack;
        }
        haystack++;
    }
    
    return NULL;
}

/**
 * bk_strdup - Duplica uma string (aloca memória)
 * @s: String a ser duplicada
 * 
 * Retorna: Nova string alocada com bk_vm_alloc ou NULL se falhar
 * 
 * ATENÇÃO: A memória deve ser liberada com bk_vm_free quando não for mais usada.
 */
static __BK_ALWAYS_INLINE char *
bk_strdup(const char *s)
{
    BK_SIZE len = bk_strlen(s) + 1;
    char *new_str;
    
    /* Usa bk_vm_alloc do vm.h */
   new_str = (char *)bk_vm_alloc(NULL, len, 
                               BK_VM_PROT_READ | BK_VM_PROT_WRITE,
                               BK_VM_MAP_ANON);
    
    if (new_str) {
        bk_memcpy(new_str, s, len);
    }
    
    return new_str;
}

/**
 * bk_strndup - Duplica uma string com limite
 * @s: String a ser duplicada
 * @n: Número máximo de caracteres a copiar
 * 
 * Retorna: Nova string alocada ou NULL se falhar
 */
static __BK_ALWAYS_INLINE char *
bk_strndup(const char *s, BK_SIZE n)
{
    BK_SIZE len = bk_strnlen(s, n);
    char *new_str;  /* <--- DECLARAÇÃO FALTANDO */
    
    new_str = (char *)bk_vm_alloc(NULL, len + 1,
                                   BK_VM_PROT_READ | BK_VM_PROT_WRITE,
                                   BK_VM_MAP_ANON);
    
    if (new_str) {
        bk_memcpy(new_str, s, len);
        new_str[len] = '\0';
    }
    
    return new_str;
}

/* ============================================================================
 * FUNÇÕES DE CONVERSÃO
 * ============================================================================ */

/**
 * bk_atoi - Converte string para inteiro
 * @s: String contendo número
 * 
 * Retorna: Valor inteiro convertido
 */
static __BK_ALWAYS_INLINE BK_I32
bk_atoi(const char *s)
{
    BK_I32 result = 0;
    BK_I32 sign = 1;
    
    /* Pula espaços em branco */
    while (*s == ' ' || *s == '\t' || *s == '\n') {
        s++;
    }
    
    /* Verifica sinal */
    if (*s == '-') {
        sign = -1;
        s++;
    } else if (*s == '+') {
        s++;
    }
    
    /* Converte dígitos */
    while (*s >= '0' && *s <= '9') {
        result = result * 10 + (*s - '0');
        s++;
    }
    
    return sign * result;
}

/**
 * bk_atol - Converte string para long
 */
static __BK_ALWAYS_INLINE BK_L3264
bk_atol(const char *s)
{
    BK_L3264 result = 0;
    BK_I32 sign = 1;
    
    while (*s == ' ' || *s == '\t' || *s == '\n') s++;
    
    if (*s == '-') {
        sign = -1;
        s++;
    } else if (*s == '+') {
        s++;
    }
    
    while (*s >= '0' && *s <= '9') {
        result = result * 10 + (*s - '0');
        s++;
    }
    
    return sign * result;
}

/**
 * bk_itoa - Converte inteiro para string (decimal)
 * @value: Valor a converter
 * @str: Buffer de saída
 * @base: Base (10, 16, etc) - apenas 10 implementado
 * 
 * Retorna: Ponteiro para o início da string
 */
static __BK_ALWAYS_INLINE char *
bk_itoa(BK_I32 value, char *str, BK_I32 base)
{
    char *p = str;
    BK_I32 num = value;
    BK_I32 i = 0;
    char tmp[32];
    
    if (base != 10) {
        /* Apenas base 10 implementada */
        *str = '\0';
        return str;
    }
    
    if (num < 0) {
        *p++ = '-';
        num = -num;
    }
    
    do {
        tmp[i++] = '0' + (num % 10);
        num /= 10;
    } while (num > 0);
    
    while (i > 0) {
        *p++ = tmp[--i];
    }
    
    *p = '\0';
    return str;
}

/**
 * bk_utoa - Converte unsigned para string hexadecimal
 * @value: Valor a converter
 * @str: Buffer de saída
 * @uppercase: 1 para maiúsculas, 0 para minúsculas
 * 
 * Retorna: Ponteiro para o início da string
 */
static __BK_ALWAYS_INLINE char *
bk_utoa_hex(BK_UL3264 value, char *str, BK_BOOL uppercase)
{
    char *p = str;
    BK_I32 i;
    char hexchars[] = "0123456789abcdef";
    char hexchars_up[] = "0123456789ABCDEF";
    char *chars = uppercase ? hexchars_up : hexchars;
    
    if (value == 0) {
        *p++ = '0';
        *p = '\0';
        return str;
    }
    
    /* Encontra o primeiro dígito não-zero */
    for (i = 60; i >= 0; i -= 4) {
        BK_UI8 digit = (value >> i) & 0xF;
        if (digit != 0 || p != str) {
            *p++ = chars[digit];
        }
    }
    
    *p = '\0';
    return str;
}

/* ============================================================================
 * FUNÇÕES DE VALIDAÇÃO DE CARACTERES (ctype simplificado)
 * ============================================================================ */

#define bk_isdigit(c)  ((c) >= '0' && (c) <= '9')
#define bk_isxdigit(c) (bk_isdigit(c) || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))
#define bk_isalpha(c)  (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
#define bk_isalnum(c)  (bk_isalpha(c) || bk_isdigit(c))
#define bk_isspace(c)  ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r')
#define bk_isupper(c)  ((c) >= 'A' && (c) <= 'Z')
#define bk_islower(c)  ((c) >= 'a' && (c) <= 'z')
#define bk_toupper(c)  (bk_islower(c) ? (c) - 'a' + 'A' : (c))
#define bk_tolower(c)  (bk_isupper(c) ? (c) - 'A' + 'a' : (c))

/* ============================================================================
 * FUNÇÕES DE TOKENIZAÇÃO
 * ============================================================================ */

static __BK_ALWAYS_INLINE BK_SIZE
bk_strspn(const char *s, const char *accept)
{
    const char *p;
    BK_SIZE count = 0;
    
    while (*s) {
        for (p = accept; *p; p++) {
            if (*s == *p) {
                break;
            }
        }
        if (*p == '\0') {
            break;
        }
        count++;
        s++;
    }
    
    return count;
}

/**
 * bk_strcspn - Obtém comprimento do segmento inicial que NÃO contém caracteres de reject
 */
static __BK_ALWAYS_INLINE BK_SIZE
bk_strcspn(const char *s, const char *reject)
{
    const char *p;
    BK_SIZE count = 0;
    
    while (*s) {
        for (p = reject; *p; p++) {
            if (*s == *p) {
                return count;
            }
        }
        count++;
        s++;
    }
    
    return count;
}

/**
 * bk_strpbrk - Encontra a primeira ocorrência de qualquer caractere de accept
 */
static __BK_ALWAYS_INLINE char *
bk_strpbrk(const char *s, const char *accept)
{
    while (*s) {
        const char *a = accept;
        while (*a) {
            if (*s == *a) {
                return (char *)s;
            }
            a++;
        }
        s++;
    }
    
    return NULL;
}

/**
 * bk_strtok_r - Tokeniza string (versão reentrante)
 * @str: String a tokenizar (NULL para continuar)
 * @delim: Conjunto de delimitadores
 * @saveptr: Ponteiro para salvar contexto
 * 
 * Retorna: Próximo token ou NULL se não houver mais
 */
static __BK_ALWAYS_INLINE char *
bk_strtok_r(char *str, const char *delim, char **saveptr)
{
    char *token;
    
    if (str == NULL) {
        str = *saveptr;
    }
    
    /* Pula delimitadores iniciais */
    str += bk_strspn(str, delim);
    if (*str == '\0') {
        *saveptr = str;
        return NULL;
    }
    
    token = str;
    str = bk_strpbrk(token, delim);
    if (str == NULL) {
        *saveptr = token + bk_strlen(token);
    } else {
        *str = '\0';
        *saveptr = str + 1;
    }
    
    return token;
}

/**
 * bk_strtok - Tokeniza string (não reentrante)
 * @str: String a tokenizar (NULL para continuar)
 * @delim: Conjunto de delimitadores
 * 
 * ATENÇÃO: Modifica a string original. Não é thread-safe.
 * Use bk_strtok_r para versão reentrante.
 */
static __BK_ALWAYS_INLINE char *
bk_strtok(char *str, const char *delim)
{
    static char *last = NULL;
    return bk_strtok_r(str, delim, &last);
}
/* ============================================================================
 * FUNÇÕES DE DEBUG
 * ============================================================================ */

#ifdef _BK_STRING_DEBUG

/**
 * bk_strdump - Imprime uma string de forma segura (para debug)
 * @s: String a imprimir
 * @max: Número máximo de caracteres a imprimir
 */
static __BK_ALWAYS_INLINE void
bk_strdump(const char *s, BK_SIZE max)
{
    BK_SIZE i;
    
    bk_printf("String[%p]: \"", s);
    for (i = 0; i < max && s[i]; i++) {
        if (s[i] >= 32 && s[i] <= 126) {
            bk_printf("%c", s[i]);
        } else {
            bk_printf("\\x%02x", (BK_UI8)s[i]);
        }
    }
    if (s[i]) {
        bk_printf("...");
    }
    bk_printf("\"\n");
}

#endif /* _BK_STRING_DEBUG */

__BK_END_DECLS

#endif /* !_BIBLE_STRING_H_ */
