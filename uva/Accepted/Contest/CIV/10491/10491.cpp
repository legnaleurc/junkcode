#include <iostream>
using std::cin;
using std::cout;
using std::ios;

int main()
{
	double ncows, ncars, nshow;

	while( cin >> ncows >> ncars >> nshow )
	{
		cout.precision( 5 );
		cout.setf( ios::fixed, ios::floatfield );
		cout << ncows / ( ncows + ncars ) * ncars / ( ncows + ncars - nshow - 1 ) + ncars / ( ncows + ncars ) * ( ncars - 1 ) / ( ncows + ncars - nshow - 1 ) << '\n';
	}
	return 0;
}
