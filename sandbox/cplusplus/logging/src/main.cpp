#include <boost/log/core.hpp>

int main (int argc, char * argv[]) {
  namespace bl = boost::log;

  auto core = bl::core::get();

  return 0;
}
