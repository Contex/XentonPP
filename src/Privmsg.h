// classes example
#include "main.h"
#include "Command.h"
#include "Config.h"
using namespace std;

class Privmsg
{
  public:
	void run (const Socket& sock, string reply)
	{
		bool in_channel = true;
		//Functions functions;
		Command command;
		Message message;
		Config configClass;
		struct config;
		config = configClass.ConfigF;
		if(reply.find("!") == 8)
		{
			//loadUser(user, reply);
			if (command.isCommand(reply, "!calc") && in_channel)
			{
				message.sendMessage(sock, config.mainchannel, getMessage(reply));
			}
			else if (command.isCommand(reply, "!join") && in_channel)
			{
				message.sendMessage(sock, config.mainchannel, getMessage(reply));
			}
			else if (command.isCommand(reply, "!restart") && in_channel)
			{
				if(user.botadmin)
				{
					message.sendMessage(sock, config.mainchannel, "Restarting (recompiling) the bot..\r\n");

					exit(1);
					system("python restart.py");
				}
			}
			else if (command.isCommand(reply, "!whois") && in_channel)
			{
				sock << "WHOIS " + getCommandArgs(getMessage(reply), "!whois") + "\r\n";
			}
			else if (command.isCommand(reply, "!shell") && in_channel)
			{
				FILE *in;
				char buff[512];
				string args = getCommandArgs(getMessage(reply), "!shell");
				if(args.empty() || args.length() == 1)
				{
					message.sendUsage(sock, config.mainchannel, "!shell", "COMMAND");
				}
				else
				{
					const char *temp;
					temp=args.c_str();
					if(!(in = popen(temp, "r")))
					{
						sock << "PRIVMSG " + config.mainchannel + " " + botColor.error + "Failed executing Shell command: " + args + "\r\n";
					}
					else
					{
						sock << "PRIVMSG " + config.mainchannel +  " " + botColor.success + "Executing Shell command: " + args + "\r\n";
						while(fgets(buff, sizeof(buff), in)!=NULL)
						{
							cout << buff;
							sock << "PRIVMSG " + config.mainchannel + " " + buff + "..\r\n";
						}
						pclose(in);
					}
				}
			}
		}
	}
};
