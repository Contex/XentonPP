// classes example
#include "main.h"
using namespace std;

class Command
{
  public:
	void joinChannel(Socket& sock, string channel)
	{
		sock << "JOIN " + channel + "\r\n";
	}
};
