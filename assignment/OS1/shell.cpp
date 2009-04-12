/**
 * @author 潘韋成(494410006)
 * @encode UTF-8
 */
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>

using std::cin;
using std::cout;
using std::cerr;
using std::string;
using std::istringstream;
using std::vector;
using std::stack;

void start( stack< vector<string> >& commands, bool background );
void pipeline( stack< vector<string> >& commands );
char * * setup( const vector<string> & v );

int main() {
	stack< vector<string> > commands;
	vector< string > command;	//command and arguments
	string line;	//line unit
	string token;	//token unit

	cout << "shell >>> ";
	while( getline( cin, line ) ) {
		istringstream iss( line );
		while( iss >> token ) {
			if( token == ";" ) {
				commands.push( command );
				command.clear();
				start( commands, false );
			} else if( token == "&" ) {
				commands.push( command );
				command.clear();
				start( commands, true );
			} else if( token == "|" ) {
				commands.push( command );
				command.clear();
			} else {
				command.push_back( token );
			}
		}
		if( !command.empty() ) {
			commands.push( command );
			command.clear();
		}
		if( !commands.empty() ) {
			start( commands, false );
		}
		cout << "shell >>> ";
	}

	cout << '\n';
	return EXIT_SUCCESS;
}

void start( stack< vector< string > >& commands, bool background ) {
	while( !commands.empty() ) {
		if( commands.top()[0] == "cd" ) {	//Execute my own cd
			if( commands.top().size() > 1 ) {
				if( commands.top()[1] == "~" ) {
					chdir( getenv( "HOME" ) );
				} else {
					chdir( commands.top()[1].c_str() );
				}
			} else {
				chdir( getenv( "HOME" ) );
			}
			commands.pop();
		} else if( commands.top()[0] == "exit" ) {	//Execute my own exit
			exit( EXIT_SUCCESS );
		} else {	//Start forking child process to run shell
			pid_t pid = fork();
			if( pid < 0 ) {
				cerr << "Fork error.\n";
			} else if( pid > 0 ) {	//Parent process should kill all argument list
				while( !commands.empty() ) {
					commands.pop();
				}
				if( !background ) {	//Background or not
					wait( NULL );
				}
			} else {	//Child process starts pipe
				pipeline( commands );
			}
		}
	}
}

void pipeline( stack< vector< string > >& commands ) {
	pid_t pid;
	int fd[2];
	char * * argv = NULL;
	vector< string > command = commands.top();
	commands.pop();

	if( !commands.empty() ) {
		if( pipe( fd ) < 0 ) {	//create pipe
			cerr << "Pipe error.\n";
		} else if( ( pid = fork() ) < 0 ) {	//fork process
			cerr << "Fork error.\n";
		} else if( pid > 0 ) {	//parent process
			wait( NULL );
			argv = setup( command );

			close( fd[1] );
			dup2( fd[0], fileno( stdin ) );
			execvp( argv[0], argv );
		} else {	//child process
			close( fd[0] );
			dup2( fd[1], fileno( stdout ) );
			pipeline( commands );
		}
	} else {
		argv = setup( command );

		execvp( argv[0], argv );
	}
}

char * * setup( const vector< string > & v ) {
	char * * argv;
	int j = 0;

	argv = new char*[ v.size() + 1 ];
	for( int i = 0; i < v.size(); ++i ) {
		if( v[i] == "<" ) {
			dup2( open( v[++i].c_str(), O_RDONLY ), fileno( stdin ) );
			if( errno == EBADF ) {
				cerr << "shell: " << argv[0] << ": No such file or directory.\n";
				exit( EBADF );
			}
			++i;
		} else if( v[i] == ">" ) {
			dup2( open( v[++i].c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ), fileno( stdout ) );
			++i;
		} else {
			argv[j] = new char[ v[i].length() + 1 ];
			strcpy( argv[j], v[i].c_str() );
			++j;
		}
	}
	argv[j] = NULL;
	if( errno == EBADF ) {
		cerr << "shell: " << argv[0] << ": No such file or directory.\n";
		exit( EBADF );
	}
	
	return argv;
}
