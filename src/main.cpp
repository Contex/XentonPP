/* irc_client.cpp */

#include "Socket.h"
#include "SocketException.h"
#include "ConfigFile.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <algorithm>
#include <iterator>
#include <vector>
#include <sstream>
#include <fstream>
#include <sstream>

using namespace std;

vector<string> split(const string& strValue, char separator)
{
    vector<string> vecstrResult;
    int startpos=0;
    int endpos=0;

    endpos = strValue.find_first_of(separator, startpos);
    while (endpos != -1)
    {       
        vecstrResult.push_back(strValue.substr(startpos, endpos-startpos)); // add to vector
        startpos = endpos+1; //jump past sep
        endpos = strValue.find_first_of(separator, startpos); // find next
        if(endpos==-1)
        {
            //lastone, so no 2nd param required to go to end of string
            vecstrResult.push_back(strValue.substr(startpos));
        }
    }

    return vecstrResult;
}

string make_two_digits(int x){
  if (x < 10){
    stringstream dd_ss;
    dd_ss << "0" << x;
    string dd_time = dd_ss.str();
    return dd_time;
  }else{
    stringstream sd_ss;
    sd_ss << x;
    string sd_time = sd_ss.str();
    return sd_time;
  }
}

string make_pass(){
  //password
  int p1,p2,p3,p4,p5;
  srand(time(NULL));
  p1 = rand()%10; 
  p2 = rand()%10;
  p3 = rand()%10;
  p4 = rand()%10;
  p5 = rand()%10;
  stringstream pass_ss;
  pass_ss << p1 << p2 << p3 << p4 << p5;
  string pass_str = pass_ss.str();
  return pass_str;
}

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

string getCommandArgs(string message, string command) 
{
	//hacky way,  but works.
	message.erase (0, command.length() + 2);
	message.erase(message.end() - 2, message.end());
	return message;
}

string getRawUser(string reply) { return getRaw(reply, "rawuser"); } 
string getNickname(string reply) { return getRaw(reply, "nickname"); } 
string getHost(string reply) { return getRaw(reply, "host"); } 
string getEvent(string reply) { return getRaw(reply, "event"); } 
string getChannel(string reply) { return getRaw(reply, "channel"); } 
string getMessage(string reply) { return getRaw(reply, "message"); } 

bool isCommand(string reply, string command) 
{
	if(reply.find(command) != string::npos)
	{
		string message = getMessage(reply);
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

struct Config
{
    string server;
    string port;
    string mainchannel;
    string channels;
    string nick;
    string username;
    string nickservpassword;
    string owner_nick;
};

void loadConfig(Config& config) {
    ConfigFile theconfig( "config.txt" );
    theconfig.readInto( config.server, "server" );
    theconfig.readInto( config.port, "port" );
    theconfig.readInto( config.mainchannel, "mainchannel" );
    string temp = "#";
    config.mainchannel = temp.append(config.mainchannel);
    theconfig.readInto( config.channels, "channels" );
    theconfig.readInto( config.nick, "nick" );
    theconfig.readInto( config.username, "username" );
    theconfig.readInto( config.nickservpassword, "nickservpassword" );
    theconfig.readInto( config.owner_nick, "owner_nick" );
}

int main ()
{
	Config config;
	loadConfig(config);

	const string welcome_msg = config.nick+" has connected. Type !time to see the time.";
	const string kick_msg = "KICK "+config.mainchannel+" "+config.nick;
	//353 is a code the server returns whenever you join a channel
	const string chnl_join_rsl = "353 "+config.nick;
	//433 replies that the nick is taken
	const string nick_taken_rsl = "433 * "+config.nick+" :Nickname is already in use.";
	const string rejoin_msg = "NOTICE "+config.nick+" :!rejoin";
	//001 is just the generic welcome message saying you have logged in to the server.
	const string server_welcome = "005 "+config.nick;
	const string con_closed_nick = "[Registration timeout]";
	const bool rejoin_on_kick = true;
	const string password = make_pass();
	const string quit_req = "NOTICE "+config.nick+" :!quit "+password;

	bool in_channel = false;
  
  try
  {
    //Make the socket used to connect to the server
    Socket sock(config.server, config.port);
    //Incase there is no connection, say so
    if ( !sock.get_address() ) throw SocketException ( "Could not find irc server." );
    if ( !sock.connect() ) throw SocketException ( "Could not connect to irc server." );

    string reply;

    //Accept some server replies after connecting
    sock >> reply;
    //Set the username and nick
    sock << "USER "+config.username+" * 8 :"+config.username+"\r\n";
    sock << "NICK "+config.nick+"\r\n";
    /* Lordofsraam fix here
    int nt = reply.find(nick_taken_rsl);
    while (true){
    sock >> reply;
    if (nt != string::npos && !in_channel){
	sock << "NICK "+nick+"_\r\n";;
    }else{break;}
    }
    */
    //Reply to the ping from the server
    string::size_type loc = reply.find( "PING :", 0 );
    if( loc != string::npos ){
      string pong = "PONG :" + reply.substr(6,-1);
      sock << pong;
      }
    sock >> reply;
    /* Lordofsraam fix here
    int nt = reply.find(nick_taken_rsl);
    if (nt != string::npos && !in_channel){
	sock << "NICK "+nick+"_\r\n";;
    }
    */
	
	//Nickserv add
	sock << "PRIVMSG NICKSERV IDENTIFY " + config.nickservpassword + "\r\n";
	
    while (true){ //infi loop to stay connected
      
      sock >> reply; //keep showing what the server says
      
      //join the channel if we are not already in it.
      int J = reply.find(server_welcome);
      if(J != string::npos) {
	sock << "JOIN " + config.mainchannel + "\r\n";
      }
      //Another ping reply
      string::size_type loc = reply.find( "PING :", 0 );
      if( loc != string::npos ){
	string pong = "PONG :" + reply.substr(6,-1);
	sock << pong;
      }
      //if the nick is not avalable
      int nt = reply.find(nick_taken_rsl);
      if (nt != string::npos){
	//sock << "NICK "+nick+"_\r\n"; <-- needs work along with lines 99 - 105
	cout << "Nickname is taken. Please compile with a new nickname. Exiting";
	break;
	exit(1);
      }    
      int reg_timeout = reply.find(con_closed_nick);
      if (reg_timeout != string::npos){
	cout << "Nickname is taken. Please compile with a new nickname. Exiting.\r\n";
	break;
	exit(1);
      }
      //MESSAGES TO CHECK FOR
      int i = reply.find(chnl_join_rsl);
      int b = reply.find(config.owner_nick);
      int event_kick = reply.find(kick_msg);
	  //int event_whois = reply.find(whois_msg);
      int rj = reply.find(rejoin_msg);
      int q = reply.find(quit_req);
      
      /*Find the channel join code (353)
      If found it makes in_channel true.
      This is so you can make things happen only if you know you are in 
      a channel.
      */       
	  
    if(i != string::npos) 
	{
		cout << "\033[22;31mChannel join confirmation\033[22;30m... \033[22;32mCHECK\033[22;36m\r\n";
		cout << "\033[22;31mSending password to owner\033[22;30m... \033[22;32mCHECK\033[22;36m\r\n";
		cout << "\033[22;34mSession Quit Password: \033[01;32m"+password+"\033[22;36m\r\n";
		sock << "NOTICE " + config.owner_nick + " The quit password is: "+password+"\r\n";
		sock << "NOTICE " + config.owner_nick + " The config password is: "+password+"\r\n";
		in_channel = true;
    }      
      
    if (isCommand(reply, "!calc") && in_channel)
	{
		sock << "PRIVMSG " + config.mainchannel + " " + getMessage(reply) + "\r\n";
    }     
    else if (isCommand(reply, "!join") && in_channel)
	{
		sock << "PRIVMSG " + config.mainchannel + " " + getMessage(reply) + "\r\n";
    }   
    else if (isCommand(reply, "!restart") && in_channel)
	{
		sock << "PRIVMSG " + config.mainchannel + " Restarting (recompiling) the bot..\r\n";
		system("python restart.py");
		exit(1);
    } 
	else if (isCommand(reply, "!whois") && in_channel)
	{
		sock << "WHOIS " + getCommandArgs(getMessage(reply), "!whois") + "\r\n";
    }   
	else if (isCommand(reply, "!shell") && in_channel)
	{
		FILE *in;
		char buff[512];
		string args = getCommandArgs(getMessage(reply), "!shell");
		sock << "PRIVMSG " + config.mainchannel + " User: ,"+getNickname(reply)+",\r\n";
		if(args.empty() || args.length() == 1)
		{
			sock << "PRIVMSG " + config.mainchannel + " Command usage: !shell COMMAND\r\n";
		}
		else
		{
			const char *temp;
			temp=args.c_str();
			if(!(in = popen(temp, "r")))
			{
				sock << "PRIVMSG " + config.mainchannel + " Failed executing Shell command: " + args + "\r\n";
			}
			else
			{
				sock << "PRIVMSG " + config.mainchannel + " Executing Shell command: " + args + "\r\n";
				while(fgets(buff, sizeof(buff), in)!=NULL)
				{
					cout << buff;
					sock << "PRIVMSG " + config.mainchannel + " " + buff + "..\r\n";
				}
				pclose(in);
			}
		}
    }   
	  
    if (event_kick != string::npos && in_channel)
	{
		sock << "PRIVMSG " + config.mainchannel + " Someone kicked me from: " + getChannel(reply) + "!\r\n";
		if(rejoin_on_kick) 
		{
			sock << "JOIN " + config.mainchannel + "\r\n";
			sock << "PRIVMSG " + config.mainchannel + " I have rejoined channel: " + getChannel(reply) + "!\r\n";
		}
		in_channel = false;
    }
	
    if (rj != string::npos && !in_channel)
	{
		sock << "JOIN " + config.mainchannel + "\r\n";
		sock << "PRIVMSG " + config.mainchannel + " " + welcome_msg + "\r\n";
    }
    
	if (q != string::npos)
	{
		sock << "QUIT Requested. Pass:\00320 "+password+"\017\r\n";
		cout << "\033[01;31mRequested Quit. Pass: "+password+" \033[22;37m\r\n";
		exit(1);
    }
      
    } 
    
    
  }
  catch ( SocketException& e )
  {
    cout << "Exception was caught: " << e.description() << "\n";
  }
  return 0;
}
