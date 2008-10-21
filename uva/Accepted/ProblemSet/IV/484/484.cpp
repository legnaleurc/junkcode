#include <iostream>
#include <vector>
using namespace std;

int main()
{
	vector<long long int> a,b;
	long long int temp;
	bool match;
	
	while(cin >> temp)
	{
		match = false;
		for(int i = 0; i < a.size(); i++)
		{
			if(a[i] == temp)
			{
				match = true;
				b[i]++;
				break;
			}
		}
		if(!match)
		{
			a.push_back(temp);
			b.push_back(1);
			match = false;
		}
	}
	for(int i = 0; i < a.size(); i++)
	{
		cout << a[i] << " " << b[i] << endl;
	}
	
	return 0;
}
