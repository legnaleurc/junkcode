//http://acm.uva.es/p/v110/11044.html
#include <iostream>
using namespace std;

int main()
{
	int cases, row, col;

	cin >> cases;
	while( cases-- )
	{
		cin >> row >> col;

		while( row % 3 != 0 )
			row--;
		while( col % 3 != 0 )
			col--;

		cout << row * col / 9 << endl;
	}

	return 0;
}