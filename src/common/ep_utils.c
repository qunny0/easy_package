#include "ep_utils.h"
#include <sys/stat.h>
#include <stdarg.h>
#include <errno.h>

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#else
#include <sys/stat.h>
#endif

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
	int ret = ACCESS(dir, mode);
	if (ret == -1)
	{
		return -1;
	}
	return 0;
}

int ep_mk_dir(const char* dir)
{
	// 
	uint32_t len = strlen(dir);
	char* psz_dir = malloc(len+2);
	strcpy(psz_dir, dir);

	if (psz_dir[len-1] != '/' && psz_dir[len-2] != '\\')
	{
		psz_dir[len] = '/';
		psz_dir[++len] = '\0';
	}

	for (uint32_t i = 0; i < len; i++)
	{
		if (psz_dir[i] == '\\' || psz_dir[i] == '/')
		{
			psz_dir[i] = '\0';
			int ret = dir_valid(psz_dir, 0);
			if (ret != 0)
			{
				ret = MKDIR(psz_dir);
				if (ret != 0)
				{
					return -1;
				}
			}
			psz_dir[i] = '/';
		}
	}

	free(psz_dir);

	return 0;
}

int ep_read(const char* path, unsigned long offset, unsigned long size, char* out_buf)
{
	FILE* file = fopen(path, "rb");
	if (!file)  {
		goto EP_ERROR;
	}

	int ret = fseek(file, offset, SEEK_SET);
	if (ret != 0) {
		goto EP_ERROR;
	}

	int numread = fread(out_buf, sizeof(char), size, file);
	if (numread != size) {
		goto EP_ERROR;
	}

	fclose(file);
	return 0;

EP_ERROR:
	{	
		if (file) 
			fclose(file);
		return -1;
	}
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

int ep_set_file_length(const char* path, uint32_t len)
{
#ifdef _WIN32
	FILE* file = fopen(path, "ab");
	if (file)
	{
		fseek(file, len, SEEK_SET);
		int fd = _fileno(file);
		HANDLE hfile = (HANDLE)_get_osfhandle(fd);
		int ret = SetEndOfFile(hfile);
		fclose(file);
		return ret;
	}
#endif

	return -1;
}