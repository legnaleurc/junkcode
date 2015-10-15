#include <iostream>

class Ghost {
public:
	void run() {
		std::cout << "YOLO" << std::endl;
	}
};

template< typename T >
class Lock {
public:
	Lock( T * t ): t( t ) {
		std::cout << "lock" << std::endl;
	}
	~Lock() {
		std::cout << "unlock" << std::endl;
	}
	T * operator ->() {
		return t;
	}
	T * t;
};

class Shell {
public:
	Lock< Ghost > operator ->() {
		return Lock< Ghost >( &g );
	}
	Ghost g;
};

int main() {
	Shell s;
	s->run();
	return 0;
}
