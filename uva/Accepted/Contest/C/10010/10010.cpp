//http://acm.uva.es/p/v100/10010.html
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

typedef unsigned int uint;

struct Coordinate
{
	uint row;
	uint col;
};	//Struct which carry row and column.

struct Matrix
{
	char **matrix;
	uint rows;
	uint cols;
};	//Matrix whith max rows and max columns.

struct Array
{
	string *data;
	uint size;
};	//Array of string.

Coordinate match( Matrix mat, string pattern );	//Decide which compare function should be invoked in #mat, using #pattern.
bool east( Matrix mat, string pattern, Coordinate point );	//Check whether #pattern is in #mat, start at #point and span to East.
bool southeast( Matrix mat, string pattern, Coordinate point );	//Check whether #pattern is in #mat, start at #point and span to SouthEast.
bool south( Matrix mat, string pattern, Coordinate point );	//Check whether #pattern is in #mat, start at #point and span to South.
bool southwest( Matrix mat, string pattern, Coordinate point );	//Check whether #pattern is in #mat, start at #point and span to SouthWest.
bool west( Matrix mat, string pattern, Coordinate point );	//Check whether #pattern is in #mat, start at #point and span to West.
bool northwest( Matrix mat, string pattern, Coordinate point );	//Check whether #pattern is in #mat, start at #point and span to NorthWest.
bool north( Matrix mat, string pattern, Coordinate point );	//Check whether #pattern is in #mat, start at #point and span to North.
bool northeast( Matrix mat, string pattern, Coordinate point );	//Check whether #pattern is in #mat, start at #point and span to NorthEast.

int main()
{
	uint cases;	//Number of cases.
	Matrix mat;	//Letter matrix.
	Array pattern;	//Pattern array.
	Coordinate temp;	//Tempary point.
	
	cin >> cases;
	while( cases-- )	//Execute #cases times.
	{
		cin >> mat.rows >> mat.cols;	//Decide width and height.
		cin.ignore();	//A white space: '\n'
		mat.matrix = new char*[mat.rows];	//Create rows.
		for( uint row = 0; row < mat.rows; row++ )	//Create cols and put characters in.
		{
			mat.matrix[row] = new char[mat.cols];
			for( uint col = 0; col < mat.cols; col++ )
			{
				mat.matrix[row][col] = tolower( cin.get() );
			}
			cin.ignore();	//Still ingore '\n'.
		}
		
		cin >> pattern.size;	//Decide array size.
		cin.ignore();	//Don't forget '\n'
		pattern.data = new string[pattern.size];
		for( uint i = 0; i < pattern.size; i++ )	//Assign patterns.
		{
			getline( cin, pattern.data[i] );
			uint length = pattern.data[i].length();
			for( uint j = 0; j < length; j++ )
			{
				pattern.data[i][j] = tolower( pattern.data[i][j] );
			}
		}
		
		for( uint i = 0; i < pattern.size; i++ )	//For each pattern, find match point.
		{
			temp = match( mat, pattern.data[i] );
			cout << temp.row + 1 << " " << temp.col + 1 << endl;
		}
		if( cases > 0 )	//Last case shouldn't have blank line.
			cout << endl;
		
		for( uint i = 0; i < mat.rows; i++ )	//Always remember to recycle Dynamic Arrays.
			delete [] mat.matrix[i];
		delete [] mat.matrix;
		mat.matrix = NULL;
		delete [] pattern.data;
		pattern.data = NULL;
	}
	
	return 0;
}

Coordinate match( Matrix mat, string pattern )
{
	Coordinate result;
	
	for( uint row = 0; row < mat.rows; row++ )
	{
		for( uint col = 0; col < mat.cols; col++ )
		{
			if( pattern[0] == mat.matrix[row][col] )
			{
				result.row = row;	//Assume this point is the result.
				result.col = col;
				if( row == 0 && col == 0 )	//Check top-left point.
				{
					if( east( mat, pattern, result ) || southeast( mat, pattern, result ) || south( mat, pattern, result ) )
					{
						return result;
					}
				}
				else if( row == 0 && col == mat.cols - 1 )	//Check top-right point.
				{
					if( south( mat, pattern, result ) || southwest( mat, pattern, result ) || west( mat, pattern, result ) )
					{
						return result;
					}
				}
				else if( row == mat.rows - 1 && col == 0 )	//Check bottom-left point.
				{
					if( north( mat, pattern, result ) || northeast( mat, pattern, result ) || east( mat, pattern, result ) )
					{
						return result;
					}
				}
				else if( row == mat.rows - 1 && col == mat.cols - 1 )	//Check bottom-right point.
				{
					if( west( mat, pattern, result ) || northwest( mat, pattern, result ) || north( mat, pattern, result ) )
					{
						return result;
					}
				}
				else if( row == 0 )	//Check top points.
				{
					if( east( mat, pattern, result ) || southeast( mat, pattern, result ) || south( mat, pattern, result ) || southwest( mat, pattern, result ) || west( mat, pattern, result ) )
					{
						return result;
					}
				}
				else if( row == mat.rows - 1 )	//Check bottom points.
				{
					if( west( mat, pattern, result ) || northwest( mat, pattern, result ) || north( mat, pattern, result ) || northeast( mat, pattern, result ) || east( mat, pattern, result ) )
					{
						return result;
					}
				}
				else if( col == 0 )	//Check right points.
				{
					if( north( mat, pattern, result ) || northeast( mat, pattern, result ) || east( mat, pattern, result ) || southeast( mat, pattern, result ) || south( mat, pattern, result ) )
					{
						return result;
					}
				}
				else if( col == mat.cols - 1 )	//Check left points.
				{
					if( south( mat, pattern, result ) || southwest( mat, pattern, result ) || west( mat, pattern, result ) || northwest( mat, pattern, result ) || north( mat, pattern, result ) )
					{
						return result;
					}
				}
				else	//Otherwise.
				{
					if( east( mat, pattern, result ) || southeast( mat, pattern, result ) || south( mat, pattern, result ) || southwest( mat, pattern, result ) || west( mat, pattern, result ) || northwest( mat, pattern, result ) || north( mat, pattern, result ) || northeast( mat, pattern, result ) )
					{
						return result;
					}
				}
			}
		}
	}
}

bool east( Matrix mat, string pattern, Coordinate point )
{
	bool flag = true;
	if( mat.cols - point.col < pattern.length() )	//Distance of #point to right edge should be longer than #pattern's length.
		return false;
	else
	{
		for( uint i = 1; i < pattern.length(); i++ )
		{
			if( pattern[i] != mat.matrix[point.row][point.col + i] )
			{
				flag = false;
				break;
			}
		}
		return flag;
	}
}

bool southeast( Matrix mat, string pattern, Coordinate point )
{
	bool flag = true;
	if( min( mat.rows - point.row, mat.cols - point.col ) < pattern.length() )	//Distance of #point to lesser edge should be longer than pattern's length.
		return false;
	else
	{
		for( uint i = 1; i < pattern.length(); i++ )
		{
			if( pattern[i] != mat.matrix[point.row + i][point.col + i] )
			{
				flag = false;
				break;
			}
		}
		return flag;
	}
}

bool south( Matrix mat, string pattern, Coordinate point )
{
	bool flag = true;
	if( mat.rows - point.row < pattern.length() )	//Distance of #point to bottom edge should be longer than #pattern's length.
		return false;
	else
	{
		for( uint i = 1; i < pattern.length(); i++ )
		{
			if( pattern[i] != mat.matrix[point.row + i][point.col] )
			{
				flag = false;
				break;
			}
		}
		return flag;
	}
}

bool southwest( Matrix mat, string pattern, Coordinate point )
{
	bool flag = true;
	if( min( mat.rows - point.row, point.col + 1 ) < pattern.length() )	//Distance of #point to lesser edge should be longer than pattern's length.
		return false;
	else
	{
		for( uint i = 1; i < pattern.length(); i++ )
		{
			if( pattern[i] != mat.matrix[point.row + i][point.col - i] )
			{
				flag = false;
				break;
			}
		}
		return flag;
	}
}

bool west( Matrix mat, string pattern, Coordinate point )
{
	bool flag = true;
	if( point.col + 1 < pattern.length() )	//Distance of #point to left edge should be longer than #pattern's length.
		return false;
	else
	{
		for( uint i = 1; i < pattern.length(); i++ )
		{
			if( pattern[i] != mat.matrix[point.row][point.col - i] )
			{
				flag = false;
				break;
			}
		}
		return flag;
	}
}

bool northwest( Matrix mat, string pattern, Coordinate point )
{
	bool flag = true;
	if( min( point.row + 1, point.col + 1 ) < pattern.length() )	//Distance of #point to lesser edge should be longer than pattern's length.
		return false;
	else
	{
		for( uint i = 1; i < pattern.length(); i++ )
		{
			if( pattern[i] != mat.matrix[point.row - i][point.col - i] )
			{
				flag = false;
				break;
			}
		}
		return flag;
	}
}

bool north( Matrix mat, string pattern, Coordinate point )
{
	bool flag = true;
	if( point.row + 1 < pattern.length() )	//Distance of #point to top edge should be longer than #pattern's length.
		return false;
	else
	{
		for( uint i = 1; i < pattern.length(); i++ )
		{
			if( pattern[i] != mat.matrix[point.row - i][point.col] )
			{
				flag = false;
				break;
			}
		}
		return flag;
	}
}

bool northeast( Matrix mat, string pattern, Coordinate point )
{
	bool flag = true;
	if( min( point.row + 1, mat.cols - point.col ) < pattern.length() )	//Distance of #point to lesser edge should be longer than pattern's length.
		return false;
	else
	{
		for( uint i = 1; i < pattern.length(); i++ )
		{
			if( pattern[i] != mat.matrix[point.row - i][point.col + i] )
			{
				flag = false;
				break;
			}
		}
		return flag;
	}
}
