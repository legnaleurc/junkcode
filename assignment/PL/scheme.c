#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int token;
int pos = 0;

void parse();
void getToken();
void command();
void expr();
void operator();
void operands();
void oprand();
void number();
void digit();
void error();
void match( char c );

int main()
{
	parse();
	return 0;
}

void parse()
{
	command();
}

void getToken()
{
	token = getchar();
	pos++;
	while( token == ' ' )
	{
		token = getchar();
		pos++;
	}
}

void command()
{
	getToken();
	expr();
	match( '\n' );
	pos = 0;
}

void expr()
{
	match( '(' );
	operator();
	operands();
	match( ')' );
}

void operator()
{
	if( token != '+' && token != '*' )
		error();
	else
		getToken();
}

void operands()
{
	oprand();
	while( token == '(' || isdigit( token ) )
		oprand();
}

void oprand()
{
	if( isdigit( token ) )
		number();
	else if( token == '(' )
		expr();
	else
		error();
}

void number()
{
	digit();
	while( isdigit( token ) )
		digit();
}

void digit()
{
	if( isdigit( token ) )
		match( token );
	else
		error();
}

void error()
{
	printf( "parse error: position %d: character %d\n", pos, token );
	exit( 1 );
}

void match( char c )
{
	if( token == c )
		getToken();
	else
		error();
}
