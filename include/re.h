//
// Created by makai on 16-8-16.
//

#ifndef REGEX_SYNTAX_REGEX_H
#define REGEX_SYNTAX_REGEX_H

#include <match.h>

namespace regex {

struct re {
  explicit re(string r);
  re &operator=(string r);
  bool operator()(string str);
  nfa n;
};

}

#endif //REGEX_SYNTAX_REGEX_H
