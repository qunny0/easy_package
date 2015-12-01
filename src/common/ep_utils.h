#ifndef EP_UTILS
#define EP_UTILS

#include <stdint.h>

#define EP_WRITE(dir, mode, offset, size, data) {	\
	if (ep_write(dir, mode, offset, size, data) != 0)	\
		return -1; }


extern int is_dir(const char* path);

// F_OK 0	R_OK 2	W_OK 4	X_OK 6
extern int dir_valid(const char* dir, int mode);

extern int ep_mk_dir(const char* dir);

extern int ep_read(const char* path, unsigned long offset, unsigned long size, char* out_buf);

// mode : "a" "w"
extern int ep_write(const char* path, const char* mode, unsigned long offset, unsigned long size, const char* in_buf);

extern int ep_clear_file(const char* path);

extern void ep_output(const char* format, ...);

// extern void ep_compress(char* dest_buf, unsigned long* dest_size, const char* src_buf, unsigned long src_len);

extern uint64_t ep_bkdr_hash(const char* key, uint32_t seed);

// to-do
// 1. 可变参数，获取各个参数 va_list

#endif
