#ifndef __DEBUG_H__
#define __DEBUG_H__

#define DEBUG_LOG(fmt, ...); \
	do { \
		printf("%s@%d: "fmt"\n", __func__, __LINE__, ##__VA_ARGS__); \
	} while (0)

#define DEBUG_LOG_SK(fmt, ...); \
	do { \
		printf("%s@%d: sk #%d: "fmt"\n", __func__, __LINE__, sk, ##__VA_ARGS__); \
	} while (0)

#endif /* __DEBUG_H__ */
