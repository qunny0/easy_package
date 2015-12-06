#include "ep_manager.h"

#include <map>
#include <time.h>

#define COMMAND_PACK	"pack"
#define COMMAND_PARSE	"parse"
#define COMMAND_EXPORT	"export"

// pack C:\Users\Administrator\Desktop\ep_test.ep E:\Work\FuBeiProject\Parkour\Run\client\iPhone\armatures w
// pack C:\Users\Administrator\Desktop\ep_test.ep C:\Users\Administrator\Desktop\test w
// parse C:\Users\Administrator\Desktop\ep_test.ep
// export C:\Users\Administrator\Desktop\ep_test.ep C:\Users\Administrator\Desktop\export

std::map<int, int> map_test;

void init()
{
	for (int i = 0; i < 3000; i++)
	{
		map_test[i] = i + 1;
	}
}

const std::map<int, int>& get()
{
	return map_test;
}

void test()
{
	time_t pre_time, aft_time;

	time(&pre_time);

	for (int i = 0; i < 2000; i++)
	{
		printf("test %d\n", i);

		// cost 非调试模式下:14s  调试模式下:227s
// 		std::map<int, int> mm = get();

		// cost 非调试模式下:0/1s  调试模式下:0/1s
		const std::map<int, int>& mm = get();
	}

	time(&aft_time);
	uint32_t dif_time = uint32_t(aft_time - pre_time);
	printf("test cost %d s.\n", dif_time);
}

int main(int argc, char* argv[])
{
	ep_manager manager;

	if (argc == 1)
	{
		manager.show_tips();
	}

	const char* command = argv[1];
	int ret = 0;
	if (strcmp(command, COMMAND_PACK) == 0)
	{
		ret = manager.package(argv[2], argv[3], argv[4]);
	}
	else if (strcmp(command, COMMAND_PARSE) == 0)
	{
		ret = manager.parse_package(argv[2]);
	}
	else if (strcmp(command, COMMAND_EXPORT) == 0)
	{
		ret = manager.package_export(argv[2], argv[3]);
	}
	else
	{
		manager.show_tips();
	}

	if (ret != 0)
	{
		printf("success!");
	}

	system("pause");
	return 0;
}