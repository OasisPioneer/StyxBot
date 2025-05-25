#include <iostream>
#include "LoggingSystem.HPP"

int main(int argc, char* argv[])
{
	LoggingSystem LOG("System Log.txt");

	if (argc < 2)
	{
		LOG.Log(LoggingSystem::ERROR, "No Command Provided!");
		LOG.Log(LoggingSystem::INFO, "Please Add Command --H to Get Help!");
	}

	for (int i = 1; i < argc; i++)
	{
		std::string Command = argv[i];
		if (Command == "--H" || Command == "--HELP")
		{
			std::string HELPINFO;
			HELPINFO = "⇐========================================⇒\n";
			HELPINFO += "‖ [How to Use] " + std::string(argv[0]) + " --Command\n";
			HELPINFO += "‖ --H Or --HELP [Get Help]\n";
			HELPINFO += "‖ --T Or --TOKEN [Set Telegram Bot Token]\n";
			HELPINFO += "‖ --A Or --ADMIN [Set Telegram Bot Admin]\n";
			HELPINFO += "⇐========================================⇒";
			std::cout << HELPINFO << std::endl;
		}
	}

	return 0;
}