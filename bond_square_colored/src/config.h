#ifndef CONFIG_H
#define CONFIG_H

/* https://stackoverflow.com/questions/7063303/macro-unix-not-defined-in-macos-x */
#if defined(__unix__) || defined(__unix) || defined(unix)
#define PP_UNIX
#endif

/* CPU specific */
#define THREAD_COUNT (8-1)

/* nr of samples */
#define PERC_MAX_LVL 1500

/* quality */
#define WORLD_WIDTH 1920
#define WORLD_HEIGHT 1080

/* no impact on performance */
/*#define DSUF_HALVE
*/

/* recommended due to color smoothness */
#define DSUF_RANK

#define PRIV_PERC_STRING(expr) #expr
#define PERC_STRING(expr) PRIV_PERC_STRING(expr)

/* https://stackoverflow.com/questions/8302547/temp-failure-retry-and-use-gnu */
#define CALL_RETRY(expression) { \
    int retvar; \
    do { \
        retvar = (expression); \
    } while (retvar == -1 && errno == EINTR); \
}

#define CALL_RETRY_RET(ret, expression) do { \
    ret = (expression); \
} while (ret == -1 && errno == EINTR)

#endif
