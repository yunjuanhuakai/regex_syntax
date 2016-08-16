#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <re.h>
using namespace std;

BOOST_AUTO_TEST_SUITE(regex_test)

BOOST_AUTO_TEST_CASE(test_1)
{
  using namespace regex;
  re r(R"([a-z]+abcd)");

  BOOST_CHECK(r("abdaeabcd"));
  BOOST_CHECK(r("......abcd"));
  // BOOST_CHECK(r("abcd"));

  r = R"([a-z]*abcd)";

  BOOST_CHECK(r("abcd"));

  r = R"(\(.*\)abcd)";
  BOOST_CHECK(r("(asfdaw\\(e))abcd)abcd"));

  r = R"(\d)";
  for (int i = 0; i != 10; ++i)
    BOOST_CHECK(r(to_string(i)));

  r = R"(\d+\.\d+\.\d+\.\d+(:\d+)?)";
  BOOST_CHECK(r("1.2.3.4"));
  BOOST_CHECK(r("127.0.0.1"));
  BOOST_CHECK(r("123.206.57.222"));
  BOOST_CHECK(r("123.206.57.222:80"));

  r = R"(data|value)";
  BOOST_CHECK(r("data"));
  BOOST_CHECK(r("value"));
}


BOOST_AUTO_TEST_SUITE_END()
