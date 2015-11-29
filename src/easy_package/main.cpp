#include "ep_manager.h"

#define COMMAND_PACK	"pack"
#define COMMAND_PARSE	"parse"
#define COMMAND_EXPORT	"export"

// pack C:\Users\Administrator\Desktop\ep_test.ep E:\Work\FuBeiProject\Parkour\Run\client\iPhone\armatures w
// parse C:\Users\Administrator\Desktop\ep_test.ep
// export C:\Users\Administrator\Desktop\ep_test.ep C:\Users\Administrator\Desktop\export

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