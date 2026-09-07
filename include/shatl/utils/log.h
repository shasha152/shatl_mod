#pragma once

#include <android/log.h>

#define FORMAT_FRONT_STRING(str) "[" __FILE__ ":%d] " str

#define LOGI(str, ...)                                                         \
    __android_log_print(ANDROID_LOG_INFO, "shatl", FORMAT_FRONT_STRING(str),   \
                        __LINE__, ##__VA_ARGS__)
#define LOGD(str, ...)                                                         \
    __android_log_print(ANDROID_LOG_DEBUG, "shatl", FORMAT_FRONT_STRING(str),  \
                        __LINE__, ##__VA_ARGS__)
#define LOGW(str, ...)                                                         \
    __android_log_print(ANDROID_LOG_WARN, "shatl", FORMAT_FRONT_STRING(str),   \
                        __LINE__, ##__VA_ARGS__)
#define LOGE(str, ...)                                                         \
    __android_log_print(ANDROID_LOG_ERROR, "shatl", FORMAT_FRONT_STRING(str),  \
                        __LINE__, ##__VA_ARGS__)