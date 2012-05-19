#ifndef BI_ERROR_HPP
#define BI_ERROR_HPP

#include <exception>
#include <memory>
#include <string>

namespace bi {

	class ValueError : public std::exception {
	public:
		ValueError( const std::string & msg );
		virtual ~ValueError() throw();

		virtual const char * what() const throw();

	private:
		class Private;
		std::shared_ptr< Private > p_;
	};

}

#endif
