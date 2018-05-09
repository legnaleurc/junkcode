#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

int main()
{
	long long int a,b,temp;
	vector<unsigned int> v;
	
	while(cin >> a >> b)
	{
		temp = -1;
		v.erase(v.begin(), v.end());
		while(temp != 0)
		{
			v.push_back(static_cast<unsigned int>(floor(a / b)));
			temp = a % b;
			a = b;
			b = temp;
		}
		
		cout << "[" << v[0] << ";" << v[1];
		for(int i = 2; i < v.size(); i++)
			cout << "," << v[i];
		cout << "]" << endl;
	}
	
	return 0;
}
