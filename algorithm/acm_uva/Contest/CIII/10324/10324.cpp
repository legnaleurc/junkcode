#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

int main()
{
	string line;
	vector<unsigned int> sum;
	unsigned int cases, a, b, temp, count = 0;
	
	while(getline(cin, line))
	{
		cin >> cases;
		cout << "Case " << ++count << ":" << endl;
		
		sum.resize(line.length() + 1, 0);
		for(unsigned int i = 1; i <= line.length(); i++)
			sum[i] = ( sum[i-1] + line[i-1] - '0' );
		
		while(cases--)
		{
			cin >> a >> b;
			if(a > b)
				swap(a, b);
			
			temp = sum[b+1] - sum[a];
			if( temp == 0 || temp == (b - a + 1) )
				cout << "Yes" << endl;
			else
				cout << "No" << endl;
		}
		
		sum.clear();
		cin.ignore();
	}
	
	return 0;
}
