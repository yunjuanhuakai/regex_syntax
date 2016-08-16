//
// Created by makai on 16-8-15.
//

#ifndef REGEX_SYNTAX_MATCH_H
#define REGEX_SYNTAX_MATCH_H
#include <nfa.h>

namespace regex {

struct match {
  using iter_t = string::const_iterator;

  match(nfa const &, string const&);

  match(match const&) = delete;
  match &operator=(match const&) = delete;

  operator bool();

 private:
  void _dfs(iter_t cur, state_id id);

  bool match_ = true;

  state_set const &set_;
  iter_t end_;
};

}


#endif //REGEX_SYNTAX_MATCH_H
