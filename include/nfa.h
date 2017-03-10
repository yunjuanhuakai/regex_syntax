//
// Created by makai on 16-7-5.
//

#ifndef REGEX_SYNTAX_NFA_H
#define REGEX_SYNTAX_NFA_H
#include <ast.h>
#include <unordered_map>
#include <limits>
#include <regex>

namespace regex {

using state_id = size_t;
using match_func = std::function<bool(char)>;


enum class opcode {
  Null,
  Repeat,
  Concat,
  Or,
  Match
};

#define TOSTR(op) {opcode::op, #op}
const std::map<opcode, string> opcode_str{
    TOSTR(Null),
    TOSTR(Repeat),
    TOSTR(Concat),
    TOSTR(Or),
    TOSTR(Match)
};

struct state {
  opcode op_;
  union {
    state_id alt_0 = std::numeric_limits<state_id>::max();
    state_id next_;
  };
  union {
    state_id alt_ = std::numeric_limits<state_id>::max();
    state_id end_;
  };
  match_func func_;

  state(opcode);

  state(state &) = delete;
  state &operator=(state &) = delete;

  state(state &&);

  string to_string() const;
};

using state_set = vector<state>;

// using _StatsId = size_t;
// using _StatsSet = std::array<char, std::numeric_limits<char>::max()>;
// using _StatsSet = std::pair<char, vector<_StatsId>>;
// using _RegexMap = std::unordered_map<_StatsId, _StatsSet>;

// using _Row = std::array<char, std::numeric_limits<char>::max()>;
// using _Mat = std::array<_Row , std::numeric_limits<char>::max()>;

struct nfa {
  // using push_func = std::function<state_id(ast const&)>;
  nfa() = default;
  explicit nfa(ast const &);

  state_set const &set() const;
  // _RegexMap map_;
  // _Mat mat_;
  void print(ostream &) const;
 private:
  state_id distribution(ast const &);

  state_id push_state(state &&s);
  state_id push_concat(ast const &);
  state_id push_or(ast const &);
  // state_id push_escape(ast const&);
  state_id push_repeat(ast const &);
  state_id push_match(ast const &);
  state_id push_null();
  // match无法获得next和alt信息, 所以需要一个辅助类&函数, 参考regex_automaton.h中的_StateSeq类
  // state_id push_ast(ast const&);

  state_id _join(state_id s, state_id next);
  state_id base_join(state_id s, state_id next);

  state_set set_;
};

}

#endif //REGEX_SYNTAX_NFA_H
