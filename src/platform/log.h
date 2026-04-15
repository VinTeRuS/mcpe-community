#ifndef LOG_H__
#define LOG_H__

#ifdef __cplusplus
	#include <cstdio>
#else
	#include <stdio.h>
#endif

#define __LOG_PUBLISH(...) do { __VA_ARGS__; } while(0)

#ifdef ANDROID
	#include <android/log.h>
	#ifdef ANDROID_PUBLISH
		#define LOGV(fmt, ...) __LOG_PUBLISH(__VA_ARGS__)
		#define LOGI(fmt, ...) __LOG_PUBLISH(__VA_ARGS__)
		#define LOGW(fmt, ...) __LOG_PUBLISH(__VA_ARGS__)
		#define LOGE(fmt, ...) __LOG_PUBLISH(__VA_ARGS__)
	#else
		// @todo @fix; Obiously the tag shouldn't be hardcoded in here..
		#define LOGV(...) ((void)__android_log_print( ANDROID_LOG_VERBOSE, "MinecraftPE", __VA_ARGS__ ))
		#define LOGI(...) ((void)__android_log_print( ANDROID_LOG_INFO,  "MinecraftPE", __VA_ARGS__ ))
		#define LOGW(...) ((void)__android_log_print( ANDROID_LOG_WARN,  "MinecraftPE", __VA_ARGS__ ))
		#define LOGE(...) ((void)__android_log_print( ANDROID_LOG_ERROR, "MinecraftPE", __VA_ARGS__ ))
		#define printf LOGI
	#endif
#else
#ifdef PUBLISH
    #define LOGV(fmt, ...) __LOG_PUBLISH(__VA_ARGS__)
    #define LOGI(fmt, ...) __LOG_PUBLISH(__VA_ARGS__)
    #define LOGW(fmt, ...) __LOG_PUBLISH(__VA_ARGS__)
    #define LOGE(fmt, ...) __LOG_PUBLISH(__VA_ARGS__)
#else
	#ifdef DEBUG_BUILD
		#include <cstring>
		#ifdef LINUX_LOG_FILE
			extern FILE* g_logFile;
			#define LOGV(fmt, ...) do { if(g_logFile) { fprintf(g_logFile, "[VERBOSE] "); fprintf(g_logFile, fmt "\n", ##__VA_ARGS__); fflush(g_logFile); } } while(0)
			#define LOGI(fmt, ...) do { if(g_logFile) { fprintf(g_logFile, "[INFO] "); fprintf(g_logFile, fmt "\n", ##__VA_ARGS__); fflush(g_logFile); } } while(0)
			#define LOGW(fmt, ...) do { if(g_logFile) { fprintf(g_logFile, "[WARN] "); fprintf(g_logFile, fmt "\n", ##__VA_ARGS__); fflush(g_logFile); } } while(0)
			#define LOGE(fmt, ...) do { if(g_logFile) { fprintf(g_logFile, "[ERROR] "); fprintf(g_logFile, fmt "\n", ##__VA_ARGS__); fflush(g_logFile); } } while(0)
		#else
			#define LOGV(fmt, ...) do { fprintf(stderr, "[VERBOSE] "); fprintf(stderr, fmt "\n", ##__VA_ARGS__); } while(0)
			#define LOGI(fmt, ...) do { fprintf(stderr, "[INFO] "); fprintf(stderr, fmt "\n", ##__VA_ARGS__); } while(0)
			#define LOGW(fmt, ...) do { fprintf(stderr, "\033[1;33m[WARN]\033[0m "); fprintf(stderr, fmt "\n", ##__VA_ARGS__); } while(0)
			#define LOGE(fmt, ...) do { fprintf(stderr, "\033[1;31m[ERROR]\033[0m "); fprintf(stderr, fmt "\n", ##__VA_ARGS__); } while(0)
		#endif
	#else
		#define LOGV(...) ((void)0)
		#define LOGI(...) ((void)0)
		#define LOGW(...) ((void)0)
		#define LOGE(...) ((void)0)
	#endif
#endif
#endif

#if defined(_DEBUG) || defined(DEBUG_BUILD)
	#define LOGVV LOGV
#else
	#define LOGVV(fmt, ...) __LOG_PUBLISH(__VA_ARGS__)
#endif

#endif /*LOG_H__*/
