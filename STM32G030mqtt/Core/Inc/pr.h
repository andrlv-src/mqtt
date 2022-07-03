/*
 * protocol API
 */
#include <inttypes.h>

#define HEAD        0x10
#define TAIL        0x16
#define HEAD_LEN    2
#define PR_BUFF_LEN 64

typedef enum {
    PR__EVENT_PARAMS_NA     = -3,
    PR__EVENT_PARSING_ERROR = -2,
    PR__EVENT_ERROR         = -1,
    PR__EVENT_OK            = 0,
} pr__event_t;

typedef enum {
    PR__CMD_DATA = 0x48,
    PR__CMD_MSG  = 0x49,
} pr__cmd_t;

typedef enum {
    PR__TAG_TEMP = 0x1D,
    PR__TAG_PRES = 0x2D,
} pr__tag_t;

typedef struct {
    int temperature;
    int pressure;
} pr__data_t;

pr__event_t pr__make_msg(char *buff, pr__cmd_t cmd, pr__data_t *data);
pr__event_t pr__parse_answer(char *buff, pr__data_t *data);

