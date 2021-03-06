#include <boost/log/core.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/make_shared.hpp>

#include <fstream>

int main (int argc, char * argv[]) {
  namespace bl = boost::log;

  auto core = bl::core::get();

  auto backend = boost::make_shared<bl::sinks::text_ostream_backend>();
  auto fout = boost::make_shared<std::ofstream>("/tmp/logging.txt");
  backend->add_stream(fout);
  backend->auto_flush(true);

  typedef bl::sinks::synchronous_sink<bl::sinks::text_ostream_backend> SinkType;
  auto sink = boost::make_shared<SinkType>(backend);

  core->add_sink(sink);

  bl::sources::logger l;
  auto r = l.open_record();
  if (r) {
    bl::record_ostream s(r);
    s << "asdfsadf";
    s.flush();
    l.push_record(boost::move(r));
  }

  return 0;
}
