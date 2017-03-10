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
  void print_match(iter_t cur);
  size_t end(iter_t cur);

  bool match_ = true;

  size_t start_;
  state_set const &set_;
  string const& str;
  iter_t end_;
};

}


#endif //REGEX_SYNTAX_MATCH_H
