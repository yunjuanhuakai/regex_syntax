#include <iostream>
#include <re.h>

int main() {
  using namespace regex;
  re r(R"(\([^()]*\)abc)");
  if (r("(afasd(fas)abcfdas)abca(bca)abcsdf"))
    std::cout << "match ok\n";
}