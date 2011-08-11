// classes example
#include "main.h"
#include "Colors.h"
using namespace std;

class Message
{
  public:
	void sendMessage(Socket& sock, string channel, string message)
	{
		sock << "PRIVMSG " + channel + " " + message + "\r\n";
	}
	void pm(Socket& sock, string channel, string message)
	{
		sendMessage(sock, channel, message);
	}

	void sendUsage(Socket& sock, string channel, string command, string message)
	{
		Colors color;
		sendMessage(sock, channel, color.red() + color.bold("[" + color.ColorIt(color.usage, "WRONG USAGE") + color.ColorIt(color.red(), "] " + command + " ") + color.ColorIt(color.teal(), message)));
	}
};
