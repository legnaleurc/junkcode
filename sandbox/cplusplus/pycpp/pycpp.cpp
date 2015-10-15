#include "Base.hpp"

#include <boost/python.hpp>

BOOST_PYTHON_MODULE(pycpp) {
	using namespace boost::python;
	class_< Base, boost::shared_ptr< Base >, boost::noncopyable >( "Base", no_init )
	.def( "create", &Base::create ).staticmethod( "create" )
	.def( "signin", &Base::signin ).staticmethod( "signin" )
	.def( "getName", &Base::getName )
	.def( "setName", &Base::setName )
	;
}
