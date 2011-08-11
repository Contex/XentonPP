/* irc_client.cpp */

#include "Socket.h"
#include "SocketException.h"
#include "Functions.h"
#include "ConfigFile.h"
#include "Config.h"
#include "Message.h"
#include "Privmsg.h"
#include "Notice.h"
#include "Command.h"
#include "main.h"

using namespace std;

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

bool isIdentified(string reply)
{
	string message = getMessage(reply);
}

struct User
{
    string nickname;
    string host;
	string channels;
	string registred;
	int idle;
	bool identified;
	bool botadmin;
	bool ssl;
};

void loadUser(User& user, string reply) {
	//if("NOTICE")
	//{
	//	string byuser;
	//}
	//else
	//{
		string nickname = getNickname(reply);
		string host = getHost(reply);
		string channels = "";
		//bool identified = getNickname(reply);
		user.nickname = getNickname(reply);
		user.host = getHost(reply);
		user.channels = "";
		user.identified = false;
		user.botadmin = false;
		user.registred = "";
	//}
}

int main ()
{
	Config config;
	Colors color;
	BotColors botColor;
	User user;
	loadConfig(config);
	loadColors(color);
	loadBotColors(botColor, color);
	
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
	const string quit_req = "PRIVMSG "+config.mainchannel+" :!quit "+password;

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
    if( loc != string::npos )
	{
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
		string event = getEvent(reply);
		//Notice notice;
		//notice.run(sock);
		cout << event;
		if(event.compare("NOTICE") != 0)
		{
			Notice notice;
			notice.run(sock, reply);
		}
		else if(event.compare("PRIVMSG") != 0)
		{
			Privmsg privmsg;
			privmsg.run(sock, reply);
		}
	  
	  
      //join the channel if we are not already in it.
    int J = reply.find(server_welcome);
    if(J != string::npos) 
	{
    	Join join;
		join.joinChannel(sock, config.mainchannel);
    }
      //Another ping reply
      string::size_type loc = reply.find( "PING :", 0 );
      if( loc != string::npos )
      {
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
      int event_quit_command = reply.find(quit_req);
      
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
		in_channel = true;
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

	if (event_quit_command != string::npos)
	{
		sock << "PRIVMSG " + config.mainchannel + " 4QUIT Requested. Pass:\00320 " + password + "\017 by user " + getNickname(reply) + "\r\n";
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
