#include "ep_utils.h"
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <stdarg.h>

int is_dir(const char* path)
{
	struct stat buf;
	int ret = stat(path, &buf);
	if (!ret && (buf.st_mode & S_IFDIR))
		return 0;
	return -1;
}

int dir_valid(const char* dir, int mode)
{
	if (int ret = _access(dir, mode) == -1)
	{
		return -1;
	}
	return 0;

// 	SetEndOfFile()
}

int ep_output_to_console(const char* format, va_list arglist)
{
	return 0;
}

void ep_output(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	ep_output_to_console(format, args);
	va_end(args);
}

int ep_read(const char* path, int offset, int size, char* out_buf)
{
	FILE* file = fopen(path, "rb");
	if (!file) goto ERROR;

	int ret = fseek(file, offset, SEEK_SET);
	if (ret != 0) goto ERROR;

	int numread = fread(out_buf, sizeof(char), size, file);
	if (numread != size) goto ERROR;

	fclose(file);
	return 0;

ERROR:
	if (file) fclose(file);
	return -1;
}

extern int ep_write(const char* path, const char* mode, long offset, int size, const char* in_buf)
{
	FILE* file = fopen(path, mode);
	if (!file)
		goto ERROR;

	int ret = fseek(file, offset, SEEK_SET);
	if (ret != 0)
		goto ERROR;

	unsigned int numwrite = fwrite(in_buf, 1, size, file);
	if (numwrite != size)
		goto ERROR;

	fclose(file);
	return 0;

ERROR:
	if (file)
	{
		fclose(file);
	}
	return -1;
}

extern int ep_clear_file(const char* path)
{
	FILE* file = fopen(path, "w");
	if (!file)
	{
		return -1;
	}
	fclose(file);
	return 0;
}
