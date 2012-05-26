#ifndef CONTROLSERVER_HPP
#define CONTROLSERVER_HPP

#include <memory>

class ControlServer {
public:
	static void initialize();
	static ControlServer & instance();

private:
	ControlServer();
	~ControlServer();
	ControlServer( const ControlServer & );
	ControlServer & operator =( const ControlServer & );
	class Private;
	friend class Private;
	std::shared_ptr< Private > p_;
};

#endif
