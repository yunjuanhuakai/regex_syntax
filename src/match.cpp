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
      : set_(n.set()), end_(s.end()), str(s), start_(0) {
    n.print(std::cout);
    std::cout << "match start\n";
    for (iter_t cur = s.begin(); cur != end_; ++cur) {
      _dfs(cur, 0);
      if (match_)
        break;
      ++start_;
    }
    std::cout << "matche end\n";
  }

  void match::_dfs(iter_t cur, state_id id) {
    if (id_is_end(id))
      return;
    print_match(cur);

    switch (set_[id].op_) {
      case opcode::Null:
        match_ = true;
      case opcode::Concat:
        _dfs(cur, set_[id].next_);
        break;
      case opcode::Or:
        _dfs(cur, set_[id].alt_);
        if (!match_)
          _dfs(cur, set_[id].next_);
        break;
      case opcode::Repeat:
        _dfs(cur, set_[id].alt_);

        if (!match_) {
          if (id_is_end(set_[id].next_)) {
            if (cur == end_)
              _dfs(cur, set_[id].next_);
          } else {
            _dfs(cur, set_[id].next_);
          }
        }
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
        } else {
          match_ = false;
        }
    }
  }

  void match::print_match(iter_t cur) {
    size_t len = end(cur);
    if (len >= str.size())
      return;
    std::cout << string(str.begin(), str.begin() + start_);
    std::cout << "\033[7m" << string(str.begin() + start_, cur + 1) << "\033[0m";
    std::cout << string(cur + 1, str.end()) << '\n';
  }

  size_t match::end(iter_t cur) {
    return static_cast<size_t>(cur - str.begin());
  }

  match::operator bool() {
    return match_;
  }

}