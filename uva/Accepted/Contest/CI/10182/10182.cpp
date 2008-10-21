#include <iostream>

int main()
{
	int x[110000]={0,0,-1,-1,0,1,1},y[110000]={0,1,1,0,-1,-1,0};
	int n,male;
	
	male=7;
	for( int i = 1; male < 100001; i++ )
	{
		x[male] = i;
		y[male] = 1;
		male++;
		for( int j = 0; j < i; j++ )
		{
			x[male] = x[male-1] - 1;
			y[male] = y[male-1] + 1;
			male++;
		}
		for( int j = 0; j < i + 1; j++ )
		{
			x[male] = x[male-1] - 1;
			y[male] = y[male-1];
			male++;
		}
		for( int j = 0; j < i + 1; j++ )
		{
			x[male] = x[male-1];
			y[male] = y[male-1] - 1;
			male++;
		}
		for( int j = 0; j < i + 1; j++ )
		{
			x[male] = x[male-1] + 1;
			y[male] = y[male-1] - 1;
			male++;
		}
		for( int j = 0; j < i + 1; j++ )
		{
			x[male] = x[male-1] + 1;
			y[male] = y[male-1];
			male++;
		}
		for( int j = 0; j < i + 1; j++)
		{
			x[male] = x[male-1];
			y[male] = y[male-1] + 1;
			male++;
		}
	}
	while( std::cin >> n )
		std::cout << x[n-1] << ' ' << y[n-1] << '\n';
	
	return 0;
}
