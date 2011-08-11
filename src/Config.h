// classes example
#include "main.h"
using namespace std;

class Config
{
  public:
    string server;
    string port;
    string mainchannel;
    string channels;
    string nick;
    string username;
    string nickservpassword;
    string owner_nick;
	void loadConfig() {
	    ConfigFile config( "config.txt" );
	    config.readInto( server, "server" );
	    config.readInto( port, "port" );
	    config.readInto( mainchannel, "mainchannel" );
	    string temp = "#";
	    config.mainchannel = temp.append(mainchannel);
	    config.readInto( channels, "channels" );
	    config.readInto( nick, "nick" );
	    config.readInto( username, "username" );
	    config.readInto( nickservpassword, "nickservpassword" );
	    config.readInto( owner_nick, "owner_nick" );
	}
};
