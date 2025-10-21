#ifndef LAVA_LOG_H
#define LAVA_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

void lava_log(const unsigned char *message);
void lava_logf(const unsigned char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* LAVA_LOG_H */
