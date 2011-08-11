// classes example
#include "main.h"
using namespace std;

class Command
{
  public:
	bool isCommand(string reply, string command)
	{
		Functions functions;
		if(reply.find(command) != string::npos)
		{
			string message = functions.getMessage(reply);
			int counter = 0;
			istringstream iss(message);
			do
			{
				string sub;
				iss >> sub;
				if(counter == 0)
				{
					int pos = sub.find(command);
					sub = sub.substr(pos);
					if(command.compare(sub) == 0)
					{
						return true;
					}
					return false;
				}
				++counter;
			} while (iss);
		}
	}
};
