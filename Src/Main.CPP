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


	return 0;
}