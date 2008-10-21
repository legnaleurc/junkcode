#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

int main()
{
	int k, temp;
	string line;
	vector<int> p, q;
	istringstream iss;
	
	while(cin >> k)
	{
		cin.ignore();
		getline(cin, line);
		iss.str(line);
		while(iss >> temp)
		{
			p.push_back(temp);
			iss.ignore();
		}
		
		if(p.size() == 1)
		{
			cout << "q(x): " << endl;
			cout << "r = " << p.back() << endl;
			cout << endl;
		}
		else
		{
			q.push_back(p[0]);
			cout << "q(x): " << q[0];
			for(int i = 1; i < p.size()-1; i++)
			{
				q.push_back( q[i-1] * k + p[i] );
				cout << " " << q[i];
			}
			cout << endl;
			cout << "r = " << q.back() * k + p.back() << endl;
			cout << endl;
		}
		
		p.clear();
		q.clear();
		iss.clear();
	}
	
	return 0;
}
