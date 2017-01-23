#ifndef GLOBAL_HPP_
#define GLOBAL_HPP_


#include "global.hpp"

#include <boost/program_options.hpp>


namespace s5p {

typedef boost::program_options::options_description Options;
typedef boost::program_options::variables_map OptionMap;


class Application::Private {
public:
    Private(int argc, char ** argv);

    Options createOptions() const;
    OptionMap parseOptions(const Options & options) const;

    void onSystemSignal(const ErrorCode & ec, int signal_number);

    IOLoop loop;
    int argc;
    char ** argv;
};

}


#endif
