//
// Created by makai on 16-8-15.
//

#include <iostream>
#include "match.h"

namespace regex {

inline bool id_is_end(state_id id) {
  return (id ^ std::numeric_limits<state_id>::max()) == 0;
}

match::match(nfa const &n, string const &s)
    : set_(n.set()), end_(s.end()) {
  _dfs(s.begin(), 0);
}


void match::_dfs(iter_t cur, state_id id) {
  if (id_is_end(id))
    return;

  switch (set_[id].op_) {
    case opcode::Null:
      match_ = true;
    case opcode::Concat:
      _dfs(cur, set_[id].next_);
      break;
    case opcode::Or:
      _dfs(cur, set_[id].next_);
      if (!match_)
        _dfs(cur, set_[id].alt_);
      break;
    case opcode::Repeat:
      if (id_is_end(set_[id].next_)) {
        if (cur == end_)
          _dfs(cur, set_[id].next_);
      } else {
        _dfs(cur, set_[id].next_);
      }

      if (!match_)
        _dfs(cur, set_[id].alt_);
      break;
    case opcode::Match:
      if (set_[id].func_(*cur)) {
        match_ = true;
        state_id n_id = set_[id].next_;
        if (id_is_end(n_id) && set_[n_id].op_ == opcode::Repeat)
          _dfs(cur, n_id);
        else if (cur == end_)
          match_ = false;
        else
          _dfs(cur + 1, n_id);
      } else
        match_ = false;
  }
}

match::operator bool() {
  return match_;
}

}