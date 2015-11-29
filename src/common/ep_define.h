#ifndef EP_DEFINE
#define EP_DEFINE
#include <stdint.h>
#include <memory.h>

#pragma pack(1)

#define EP_SAFE_DELETE_ARR(p) { if (p) { delete [] (p); p = nullptr; } }
#define EP_SAFE_DELETE(p) { if (p) { delete (p); p = nullptr; } }

const char EP_UNCOMPRESSED_FALG = '0';
const char EP_COMPRESSED_FLAG = '1';

#define EP_PACK_MODE_WRITE		"wb"
#define EP_PACK_MODE_APPEND		"ab+"

#define EP_SIGN_LENGTH		4
#define EP_VERSION_LENGTH	12
const char EP_PACKAGE_SIGN[EP_SIGN_LENGTH] = "EPK";
const char EP_VERSION[EP_VERSION_LENGTH] = "000.000.000";

// _MAX_PATH = 260
#define  EP_MAX_PATH 260

typedef struct EPHeader
{
	unsigned char	compressed;					// 1-compressed 0-uncompressed

	EPHeader()
	{
		compressed = 1;
	}
}EPHeader;

typedef struct EPFileEntity
{
	uint32_t	offset;
	uint16_t	relative_path_size;
	uint32_t	source_data_size;
	uint32_t	compressed_data_size;

	EPFileEntity() 
	{
		offset = 0;
		relative_path_size = 0;
		source_data_size = 0;
		compressed_data_size = 0;
	}
}EPFileEntity;

typedef struct EPFileEntityEx : public EPFileEntity
{
	char		relative_path[EP_MAX_PATH];
	bool		b_delete_flag;
	
	EPFileEntityEx() : EPFileEntity()
	{
		memset(relative_path, 0, EP_MAX_PATH);
		b_delete_flag = false;
	}
}EPFileEntityEx;

#pragma pack()

struct IEP_READER 
{
	virtual bool file_exist(const char* file_path) = 0;

	virtual uint32_t get_file_data(const char* file_path, char** buf) = 0;

	virtual int export_package(const char* export_dir) = 0;
};

struct IEP_WRITER
{
	virtual int package_dir(const char* pkg_dir, const char* file_dir, const char mode) = 0;
};

#endif