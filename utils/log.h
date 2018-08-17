#include <stdio.h>
#define LOGV(x, ...) fprintf(stderr, "[V] "LOG_TAG": " x, ##__VA_ARGS__)
#define LOGI(x, ...) fprintf(stderr, "[I] "LOG_TAG": " x, ##__VA_ARGS__)
#define LOGW(x, ...) fprintf(stderr, "[W] "LOG_TAG": " x, ##__VA_ARGS__)
#define LOGE(x, ...) fprintf(stderr, "[E] "LOG_TAG": " x, ##__VA_ARGS__)