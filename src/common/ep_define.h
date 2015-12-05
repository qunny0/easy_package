#ifndef EP_DEFINE
#define EP_DEFINE
#include <stdint.h>
#include <memory.h>
#include <map>

#define EP_SAFE_DELETE_ARR(p) { if (p) { delete [] (p); p = nullptr; } }
#define EP_SAFE_DELETE(p) { if (p) { delete (p); p = nullptr; } }

const char EP_UNCOMPRESSED_FALG = '0';
const char EP_COMPRESSED_FLAG = '1';

#define EP_PACK_MODE_WRITE		"wb"
#define EP_PACK_MODE_APPEND		"ab+"

#define EP_HASH_SEED		31		// 131 13131

#define EP_SIGN_LENGTH		4
#define EP_VERSION_LENGTH	12
const char EP_PACKAGE_SIGN[EP_SIGN_LENGTH] = "EPK";
const char EP_VERSION[EP_VERSION_LENGTH] = "1.0.12";

// _MAX_PATH = 260
#define  EP_MAX_PATH 260

#define EP_WRITE(dir, mode, offset, size, data) {	\
if (ep_write(dir, mode, offset, size, data) != 0)	\
	return -1; }

#pragma pack(1)

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
	uint64_t	relative_path_hash;
	uint16_t	relative_path_size;
	uint32_t	compress_relative_path_size;
	uint32_t	source_data_size;
	uint32_t	compressed_data_size;
	uint64_t	crc32_source_data;

	EPFileEntity() 
	{
		offset = 0;
		relative_path_hash = 0;
		relative_path_size = 0;
		compress_relative_path_size = 0;
		source_data_size = 0;
		compressed_data_size = 0;
		crc32_source_data = 0;
	}
}EPFileEntity;

typedef struct EPFileEntityEx : public EPFileEntity
{
	char		relative_path[EP_MAX_PATH];
	uint16_t	n_hash_collision_flag;
	bool		b_delete_flag;
	
	EPFileEntityEx() : EPFileEntity()
	{
		memset(relative_path, 0, EP_MAX_PATH);
		n_hash_collision_flag = 0;
		b_delete_flag = false;
	}
}EPFileEntityEx;

#pragma pack()

struct IEP_READER 
{
	virtual bool file_exist(const char* file_path) = 0;

	virtual uint32_t get_file_data_from_package(const char* file_path, char** buf) = 0;

	virtual int export_package(const char* export_dir) = 0;
};

struct IEP_WRITER
{
	virtual int package_dir(const char* pkg_dir, const char* file_dir, const char mode) = 0;
};

// map<relative_path_hash, ep_file_entity_ex>
typedef std::pair<uint64_t, EPFileEntityEx>					MAP_EP_FILE_ENTITY_EX_PAIR;
typedef std::map<uint64_t, EPFileEntityEx>					MAP_EP_FILE_ENTITY_EX;
typedef std::map<uint64_t, EPFileEntityEx>::iterator		MAP_EP_FILE_ENTITY_EX_ITERATOR;
typedef std::map<uint64_t, EPFileEntityEx>::const_iterator	MAP_EP_FILE_ENTITY_EX_CONST_ITERATOR;

#endif