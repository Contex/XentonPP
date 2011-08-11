// classes example
#include "main.h"
using namespace std;

class Notice
{
  public:
	void run (const Socket& sock, string message)
	{
		Functions functions;
		string nickname = functions.getNickname(message);
		if(nickname.compare("Nickserv") == 0)
		{
			sock << "PRIVMSG #Xenton++ test o.o";
			//string message = getMessage(reply);
			//if(message.find("STATUS 3") != string::npos) { user.identified = true; }
			//if(message.find("STATUS 0") != string::npos) { user.identified = false; }
		}
	}
};
