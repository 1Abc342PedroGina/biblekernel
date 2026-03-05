#ifndef _BIBLE_PRINTS_H
#define _BIBLE_PRINTS_H

#include <bible/types.h>

/* Níveis de log */
typedef enum {
    LOG_EMERG   = 0,
    LOG_ALERT   = 1,
    LOG_CRIT    = 2,
    LOG_ERR     = 3,
    LOG_WARNING = 4,
    LOG_NOTICE  = 5,
    LOG_INFO    = 6,
    LOG_DEBUG   = 7
} log_level_t;

/* Estrutura para URLs */
typedef struct {
    __bible_size_t id;
    __bible_size_t type;
    const __BK_C8* url;
    const __BK_C8* description;
} kernel_url_t;

/* Inicialização */
void prints_init(void);
void prints_cleanup(void);

/* Funções de log kernel */
void prints_log(log_level_t level, const __BK_C8* format, ...);
void prints_emerg(const __BK_C8* format, ...);
void prints_alert(const __BK_C8* format, ...);
void prints_crit(const __BK_C8* format, ...);
void prints_err(const __BK_C8* format, ...);
void prints_warning(const __BK_C8* format, ...);
void prints_notice(const __BK_C8* format, ...);
void prints_info(const __BK_C8* format, ...);
void prints_debug(const __BK_C8* format, ...);

/* Funções específicas para Psalms */
void prints_show_psalms(void);
void prints_show_creeds(void);
void prints_show_prayers(void);
void prints_show_channels(void);
void prints_show_all_resources(void);

/* Funções auxiliares (kernel-mode) */
void prints_putchar(__BK_C8 c);
void prints_puts(const __BK_C8* str);
void prints_puthex(__BK_UL3264 val);
void prints_putdec(__BK_UL3264 val);


  /* USO DE EXEMPLO:
   * static const print_url_t g_print_urls[] = {
   * 
   * {1, PRINT_TYPE_CHANNEL, "https://www.youtube.com/channel/UCkWWYwj7Ppjo7SBXQmVO09A", "Canal Evangélico 1"},
   * {2, PRINT_TYPE_CHANNEL, "https://www.youtube.com/channel/UCXn58a-MauD5jvF5Ie7YEbw", "Canal Evangélico 2"},
   * {3, PRINT_TYPE_CHANNEL, "https://www.youtube.com/@classicosdafemusic", "Clássicos da Fé Music"},
   * {4, PRINT_TYPE_CHANNEL, "https://www.youtube.com/channel/UCUddFvhwvAoIc-VClBbh04g", "Canal Evangélico 4"},
   * {5, PRINT_TYPE_CHANNEL, "https://www.youtube.com/channel/UCj0LhRmp7stN1XdAEKs4uyw", "Canal Evangélico 5"},
   * {6, PRINT_TYPE_CHANNEL, "https://www.youtube.com/@MariaVirginia-p1l/shorts", "Maria Virgínia - Shorts"},
   * {7, PRINT_TYPE_CHANNEL, "https://www.youtube.com/channel/UClYrsT4fwl2th_2A_HREL0A", "Canal Evangélico 7"},
   * {8, PRINT_TYPE_CHANNEL, "https://www.harpacrista.org", "Harpa Cristã"},
   * {9, PRINT_TYPE_CHANNEL, "https://www.youtube.com/channel/UC3urAVGAr46-su1LgClaxUg", "Canal Evangélico 9"},
   * {10, PRINT_TYPE_CHANNEL, "https://www.youtube.com/@EulaCrisOficiall", "Eula Cris Oficial"},
   * {11, PRINT_TYPE_CHANNEL, "https://www.youtube.com/channel/UCb08zJgXapidkizIaEN5EfQ", "Canal Evangélico 11"},
   * {12, PRINT_TYPE_CHANNEL, "https://smilinguido.com.br/", "Smilinguido"},
   * {13, PRINT_TYPE_CHANNEL, "https://www.youtube.com/osoliveirinhas", "Os Oliveirinhas"},
   * {14, PRINT_TYPE_CHANNEL, "https://open.spotify.com/intl-pt/artist/5kRvFIoKAioJsrl8dt5zAq", "Artista no Spotify"},
   * {15, PRINT_TYPE_CHANNEL, "https://www.youtube.com/@motivalogos/shorts", "Motiva Logos - Shorts"},
   *
   * {16, PRINT_TYPE_CREED, "https://pt.wikisource.org/wiki/Credo_de_Atan%C3%A1sio", "Credo de Atanásio"},
   * 
   * {17, PRINT_TYPE_PRAYER, "https://www.catolicoorante.com.br/oracao.php?id=16", "Oração Católica/Protestante conhecida como 'Oração da Trindade'"},
   *
   * {18, PRINT_TYPE_PSALM, "https://www.bibliaonline.com.br/acf/sl/1", "Salmo 1"},
   * {19, PRINT_TYPE_PSALM, "https://www.bibliaonline.com.br/acf/sl/2", "Salmo 2"},
   * {20, PRINT_TYPE_PSALM, "https://www.bibliaonline.com.br/acf/sl/3", "Salmo 3"},
   * {21, PRINT_TYPE_PSALM, "https://www.bibliaonline.com.br/acf/sl/5", "Salmo 5"},
   * {22, PRINT_TYPE_PSALM, "https://www.bibliaonline.com.br/acf/sl/10", "Salmo 10"},
   * {23, PRINT_TYPE_PSALM, "https://www.bibliaonline.com.br/acf/sl/23", "Salmo 23"},
   * {24, PRINT_TYPE_PSALM, "https://www.bibliaonline.com.br/acf/sl/51", "Salmo 51"},
   * {25, PRINT_TYPE_PSALM, "https://www.bibliaonline.com.br/acf/sl/91", "Salmo 91"},
   * {26, PRINT_TYPE_PSALM, "https://www.bibliaonline.com.br/acf/sl/110", "Salmo 110"},
   * {27, PRINT_TYPE_PSALM, "https://www.bibliaonline.com.br/acf/sl/109", "Salmo 109"},
   * {28, PRINT_TYPE_PSALM, "https://www.bibliaonline.com.br/acf/sl/149", "Salmo 149"},
   * {29, PRINT_TYPE_PSALM, "https://www.bibliaonline.com.br/acf/sl/150", "Salmo 150"}
   * };
   */
#endif /* _BIBLE_PRINTS_H */
