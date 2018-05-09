#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
using namespace std;

int main()
{
	int n, m, count = 0;
	string temp;
	vector<string> sol, ans;
	bool flag;
	
	while( true )
	{
		cin >> n;
		cin.ignore();
		if( n == 0 )
			break;
		
		for( int i = 0; i < n; i++ )
		{
			getline( cin, temp );
			sol.push_back( temp );
		}
		cin >> m;
		cin.ignore();
		for( int i = 0; i < m; i++ )
		{
			getline( cin, temp );
			ans.push_back( temp );
		}
		
		cout << "Run #" << ++count << ": ";
		
		flag = true;
		for( int i = 0; i < min( m , n ); i++ )
		{
			if( sol[i] != ans[i] )
			{
				flag = false;
				break;
			}
		}
		if( n == m && flag )
			cout << "Accepted" << endl;
		else
		{
			temp = "";
			for( int i = 0; i < n; i++ )
			{
				for( int j = 0; j < sol[i].length(); j++ )
				{
					if( isdigit( sol[i][j] ) )
						temp.push_back( sol[i][j] );
				}
			}
			sol[0] = temp;
			
			temp = "";
			for( int i = 0; i < m; i++ )
			{
				for( int j = 0; j < ans[i].length(); j++ )
				{
					if( isdigit( ans[i][j] ) )
						temp.push_back( ans[i][j] );
				}
			}
			ans[0] = temp;
			
			if( sol[0] == ans[0] )
				cout << "Presentation Error" << endl;
			else
				cout << "Wrong Answer" << endl;
		}
		
		sol.clear();
		ans.clear();
	}
	
	return 0;
}
