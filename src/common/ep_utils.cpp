#include "ep_utils.h"
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <string>
#include <stdarg.h>
#include <direct.h>
#include <errno.h>

#include "zlib.h"

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

int ep_mk_dir(const char* dir)
{
	// to-do: optimize
	std::string str_dir = dir;

	// dir exist
	if (is_dir(str_dir.c_str()) == 0)
	{
		return 0;
	}
	else
	{
		std::string str_parent_dir = str_dir.substr(0, str_dir.rfind('\\'));
		ep_mk_dir(str_parent_dir.c_str());
		if (_mkdir(str_dir.c_str()) != 0)
		{
			if (errno == EEXIST)
				return 0;
			else if (errno == ENOENT)
				return 1;
		}
	}

	return 0;
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

int ep_read(const char* path, unsigned long offset, unsigned long size, char* out_buf)
{
	FILE* file = fopen(path, "rb");
	if (!file) goto EP_ERROR;

	int ret = fseek(file, offset, SEEK_SET);
	if (ret != 0) goto EP_ERROR;

	int numread = fread(out_buf, sizeof(char), size, file);
	if (numread != size) goto EP_ERROR;

	fclose(file);
	return 0;

EP_ERROR:
	if (file) fclose(file);
	return -1;
}

int ep_write(const char* path, const char* mode, unsigned long offset, unsigned long size, const char* in_buf)
{
	FILE* file = fopen(path, mode);
	if (!file)
		goto EP_ERROR;

	int ret = fseek(file, offset, SEEK_SET);
	if (ret != 0)
		goto EP_ERROR;

	unsigned int numwrite = fwrite(in_buf, 1, size, file);
	if (numwrite != size)
		goto EP_ERROR;

	fclose(file);
	return 0;

EP_ERROR:
	if (file)
	{
		fclose(file);
	}
	return -1;
}

int ep_clear_file(const char* path)
{
	FILE* file = fopen(path, "w");
	if (!file)
	{
		return -1;
	}
	fclose(file);
	return 0;
}

uint64_t ep_bkdr_hash(const char* key, uint32_t seed)
{
	uint64_t out = 0;
	while (*key)
	{
		char ch = *(key++);
		if (ch == ('\\'))
		{
			ch = ('/');
		}

		out = out * seed + ch;
	}

	return out;
}
