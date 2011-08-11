// classes example
using namespace std;

class Colors
{
	public:
		string white() { return "0"; }
		string black() { return "1"; }
		string navy() { return "2"; }
		string green() { return "3"; }
		string red() { return "4"; }
		string brown() { return "5"; }
		string purple() { return "6"; }
		string orange() { return "7"; }
		string yellow() { return "8"; }
		string light_green() { return "9"; }
		string teal() { return "10"; }
		string light_cyan() { return "11"; }
		string light_blue() { return "12"; }
		string pink() { return "13"; }
		string grey() { return "14"; }
		string light_grey() { return "15"; }

		string error() { return red; }
		string success() { return light_green; }
		string usage() { return orange; }
		string defaultcolor() { return black; }

		string bold(string message)
		{
			return "" + message + "";
		}

		string ColorIt(string color, string message)
		{
			return color + message + defaultcolor();
		}
};
