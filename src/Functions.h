#include "main.h"

using namespace std;

class Functions
{
  public:
	string getRaw(string reply, string what)
	{
		int counter = 0;
		istringstream iss(reply);
		do
		{
			string sub;
			iss >> sub;
			if(counter == 0)
			{
				if(what.compare("nickname") == 0)
				{
					sub.erase (0, 1);
					int pos = sub.find("!");
					string string1 = sub.substr(0, pos);
					return string1;
				}
				else if(what.compare("host") == 0)
				{
					sub.erase (0, 1);
					int pos = sub.find("!");
					string string1 = sub.substr(0, pos);
					string string2 = sub.substr(pos);
					return string2;
				}
				else if(what.compare("rawuser") == 0)
				{
					sub.erase (0, 1);
					return sub;
				}
			}
			else if(counter == 1 && what.compare("event") == 0)
			{
				return sub;
			}
			else if(counter == 2 && what.compare("channel") == 0)
			{
				return sub;
			}
			else if(counter == 3 && what.compare("message") == 0)
			{
				int pos = reply.find(sub);
				reply = reply.substr(pos);
				return reply;
			}
			++counter;
		} while (iss);
	}

	string getRawUser(string reply) { return getRaw(reply, "rawuser"); }
	string getNickname(string reply) { return getRaw(reply, "nickname"); }
	string getHost(string reply) { return getRaw(reply, "host"); }
	string getEvent(string reply) { return getRaw(reply, "event"); }
	string getChannel(string reply) { return getRaw(reply, "channel"); }
	string getMessage(string reply) { return getRaw(reply, "message"); }
	string usageMessage(string reply) { return getRaw(reply, "message"); }

	string getCommandArgs(string message, string command)
	{
		//hacky way,  but works.
		message.erase (0, command.length() + 1);
		message.erase(message.end() - 1, message.end());
		return message;
	}
};
