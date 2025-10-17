#ifndef LAVA_LOG_H
#define LAVA_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

void lava_log(const char *message);
void lava_logf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* LAVA_LOG_H */
