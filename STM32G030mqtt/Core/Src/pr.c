#include <stdio.h>
#include "pr.h"

static char pr__get_crc(char *buff, int len);

pr__event_t pr__make_msg(char *buff, pr__cmd_t cmd, pr__data_t *data)
{
    if (buff == NULL)
        return PR__EVENT_PARAMS_NA;

    int idx = 0;
    int len = 0;
    buff[idx] = HEAD;
    idx += 2;
    switch (cmd) {
    case PR__CMD_DATA:
        buff[idx++] = PR__CMD_DATA;
        buff[idx++] = PR__TAG_TEMP;
        buff[idx++] = data->temperature & 0xFF;
        buff[idx++] = data->temperature >> 8;
        buff[idx++] = data->temperature >> 16;
        buff[idx++] = data->temperature >> 24;
        buff[idx++] = PR__TAG_PRES;
        buff[idx++] = data->pressure & 0xFF;
        buff[idx++] = data->pressure >> 8;
        buff[idx++] = data->pressure >> 16;
        buff[idx++] = data->pressure >> 24;
        break;
    case PR__CMD_MSG:
        buff[idx++] = PR__CMD_MSG;
        break;
    default:
        break;
    }
    len = idx -  HEAD_LEN - 1;
    buff[1] = len;
    buff[idx++] = pr__get_crc(buff + HEAD_LEN, len);
    buff[idx++] = TAIL;

    return PR__EVENT_OK;
}

pr__event_t pr__parse_answer(char *buff, pr__data_t *data)
{
    if (buff == NULL)
        return PR__EVENT_PARAMS_NA;
    int len = 0;
    len = buff[1];
    uint8_t crc = buff[HEAD_LEN + len + 1];
    if (pr__get_crc(buff + HEAD_LEN, len) != crc) {
        return PR__EVENT_PARSING_ERROR;
    }
    pr__cmd_t cmd = buff[HEAD_LEN];
    int idx = HEAD_LEN + 2;
    switch (cmd) {
    case PR__CMD_DATA:
            data->temperature = buff[idx++];
            data->temperature |= buff[idx++] << 8;
            data->temperature |= buff[idx++] << 16;
            data->temperature |= buff[idx++] << 24;
            idx++;
            data->pressure = buff[idx++];
            data->pressure |= buff[idx++] << 8;
            data->pressure |= buff[idx++] << 16;
            data->pressure |= buff[idx++] << 24;
        break;
    case PR__CMD_MSG:
        break;
    }

    return PR__EVENT_OK;
}

static char pr__get_crc(char *buff, int len)
{
    int res = 0;
    for (int i = 0; i < len; ++i) {
        res += buff[i];
    }
    return (char)(~res & 0xff);
}
