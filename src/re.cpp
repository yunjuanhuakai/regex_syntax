//
// Created by makai on 16-8-16.
//

#include <re.h>
#include <parser.h>
#include <iostream>

namespace regex {

re::re(string r) {
  parser p{lexer(r)};
  ast a;
  try {
    p.regex(a);
  } catch (base_error& e) {
    std::cerr << e.what();
  }
  n = std::move(nfa(a));
}

re& re::operator=(string r) {
  *this = re(r);
  return *this;
}

bool re::operator()(string str) {
  return match{n, str};
}

}