#include <iostream>
#include <string>
using namespace std;

int main()
{
	int cases,times,height;
	string s[9] = { "1\n", "1\n22\n1\n", "1\n22\n333\n22\n1\n", "1\n22\n333\n4444\n333\n22\n1\n", "1\n22\n333\n4444\n55555\n4444\n333\n22\n1\n", "1\n22\n333\n4444\n55555\n666666\n55555\n4444\n333\n22\n1\n", "1\n22\n333\n4444\n55555\n666666\n7777777\n666666\n55555\n4444\n333\n22\n1\n", "1\n22\n333\n4444\n55555\n666666\n7777777\n88888888\n7777777\n666666\n55555\n4444\n333\n22\n1\n", "1\n22\n333\n4444\n55555\n666666\n7777777\n88888888\n999999999\n88888888\n7777777\n666666\n55555\n4444\n333\n22\n1\n" };
	
	cin >> cases;
	while(cases--)
	{
		cin >> height >> times;
		for( int i = 0; i < times; i++ )
		{
			cout << s[height-1];
			if( cases > 0 )
			{
				cout << endl;
			}
		}
	}
	
	return 0;
}
