//
// Created by makai on 16-7-5.
//

#include "nfa.h"
#include <list>
#include <set>

namespace regex {

struct char_match {
  explicit char_match(char c)
      : c_(c) {}

  bool operator()(char c) const noexcept {
    return c == c_;
  }

  char c_;
};

struct range_match {
  range_match(char start, char end)
      : start_(start), end_(end) {}

  bool operator()(char c) const noexcept {
    return c <= end_ && c >= start_;
  }

  char start_;
  char end_;
};

struct alt_match {
  alt_match() = default;

  void push(match_func &&func) {
    func_set.push_back(func);
  }

  void _not() noexcept { res_ = false; }

  bool operator()(bool c) const noexcept {
    for (auto &func : func_set)
      if (func(c))
        return res_;
    return ~res_;
  }

  bool res_ = true;
  std::vector<match_func> func_set;
};

struct set_match {
  set_match() = default;

  void push(char c) {
    set_.insert(c);
  }

  bool operator()(bool c) const noexcept {
    return set_.find(c) != set_.end();
  }

  std::set<char> set_;
};

struct any_match {
  bool operator()(bool c) const noexcept {
    return true;
  }
};

struct escape_match {
  explicit escape_match(eTAG e) {
    switch (e) {
      case eTAG::NWORD:
        _not = true;
      case eTAG::WORD:
        init_word();
        break;
      case eTAG::NNUMBER:
        _not = true;
      case eTAG::NUMBER:
        init_number();
        break;
      case eTAG::NSPACE:
        _not = true;
      case eTAG::SPACE:
        init_space();
        break;
      default:
        throw;
    }
  }

  bool operator()(char c) const noexcept {
    return _not ^ std::binary_search(set_.begin(), set_.end(), c);
  }

 private:
  void init_word() noexcept {
    using namespace std;
    set_.reserve(26 * 2 + 11);
    for (char i = '0'; i <= '9'; ++i)
      set_.push_back(i);
    for (char i = 'A'; i <= 'Z'; ++i)
      set_.push_back(i);
    set_.push_back('_');
    for (char i = 'a'; i <= 'z'; ++i)
      set_.push_back(i);
  }

  void init_number() noexcept {
    set_.reserve(10);
    for (char i = '0'; i <= '9'; ++i)
      set_.push_back(i);
  }

  void init_space() noexcept {
    set_.push_back('\t');
    set_.push_back(' ');
  }

  bool _not = false;
  std::vector<char> set_;
};

}

namespace regex {

nfa::nfa(ast const &a) {
  distribution(a);
}

state_set const &nfa::set() const {
  return set_;
}

state_id nfa::distribution(ast const &a) {
  switch (a.get_token().stat()) {
    case TAG_Stat::M:
      switch (to_tag<mTAG>(a.get_token())) {
        case mTAG::Or:
        case mTAG::IsMay:
          return push_or(a);
        case mTAG::ORepeat: // M + -> M . M *
        case mTAG::CONCAT:
          return push_concat(a);
        case mTAG::ZRepeat:
          return push_repeat(a);
        default:
          return push_match(a);
      }
    default:
      return push_match(a);
  }
}

state_id nfa::push_state(state &&s) {
  set_.push_back(std::move(s));
  return set_.size() - 1;
}

// match无法获得next和alt信息, 所以需要一个辅助类&函数, 参考regex_automaton.h中的_StateSeq类
state_id nfa::push_match(ast const &a) {
  state res{opcode::Match};

  switch (a.get_token().stat()) {
    case TAG_Stat::E:
      res.func_ = escape_match(to_tag<eTAG>(a.get_token()));
      break;
    case TAG_Stat::M_C:
      res.func_ = char_match(a.get_token().c());
      break;
    case TAG_Stat::M: {
      alt_match afunc;
      set_match sfunc;
      switch (to_tag<mTAG>(a.get_token())) {
        case mTAG::NOLBRACKET:
          afunc._not();
        case mTAG::OLBRACKET:
          for (auto const &c_ast : a) {
            if (c_ast.get_token().stat() == TAG_Stat::O_C) {
              sfunc.push(c_ast.get_token().c());
            } else if (c_ast.get_token().is(oTAG::RANGE)) {
              char start = c_ast[0].get_token().c();
              char end = c_ast[1].get_token().c();
              afunc.push(range_match(start, end));
            } else if (c_ast.get_token().stat() == TAG_Stat::E) {
              afunc.push(escape_match(to_tag<eTAG>(c_ast.get_token())));
            } else {
              assert(false);
            }
          }
          afunc.push(std::move(sfunc));
          res.func_ = std::move(afunc);
          break;
        case mTAG::ALL:
          res.func_ = any_match();
          break;
        default:
          assert(false);
      }
    }
      break;
    default:
      assert(false);
  }
  return push_state(std::move(res));
}

// concat的链接在函数内进行
state_id nfa::push_concat(ast const &a) {
  if (a.empty())
    return push_null();

  state_id id = push_state(state(opcode::Concat));
  state_id cur = base_join(id, distribution(a[0]));

  if (a.get_token().is(mTAG::CONCAT)) {
    for (state_id i = 1; i != a.size(); ++i)
      cur = _join(cur, distribution(a[i]));
  } else {
    cur = _join(cur, push_repeat(a));
  }
  set_[id].end_ = cur;

  return id;
}

state_id nfa::push_or(ast const &a) {
  state_id id = push_state(state(opcode::Or));

  if (a.get_token().is(mTAG::IsMay)) {
    set_[id].alt_0 = distribution(a[0]);
    set_[id].alt_ = push_null();
  } else {
    set_[id].alt_0 = distribution(a[0]);
    set_[id].alt_ = distribution(a[1]);
  }

  return id;
}

state_id nfa::push_repeat(ast const &a) {
  state_id id = push_state(state(opcode::Repeat));

  set_[id].alt_ = distribution(a[0]);
  set_[set_[id].alt_].next_ = id;

  return id;
}

state_id nfa::push_null() {
  return push_state(state(opcode::Null));
}

state_id nfa::_join(state_id s, state_id next) {
  switch (set_[s].op_) {
    case opcode::Or:
      _join(set_[s].alt_0, next);
      _join(set_[s].alt_, next);
      break;
    case opcode::Concat:
      _join(set_[s].end_, next);
      break;
    default:
      base_join(s, next);
  }
  return next;
}

state_id nfa::base_join(state_id s, state_id next) {
  set_[s].next_ = next;
  return next;
}

void nfa::print(ostream &os) {
  for (int i = 0; i != set_.size(); ++i)
    os << i << ": " << set_[i].to_string() << '\n';
}

}

namespace regex {


state::state(state &&rhs)
    : op_(rhs.op_), alt_(rhs.alt_), next_(rhs.next_), func_(std::move(rhs.func_)) {}

state::state(opcode o)
    : op_(o) {}

string state::to_string() const {
  ostringstream os;
  os << opcode_str.at(op_)
     << '{'
     << (op_ == opcode::Or ? "alt_0" : "next")
     << ": " << next_
     << ", "
     << (op_ == opcode::Concat ? "end" : "alt")
     << ": "
     << (op_ == opcode::Or || op_ == opcode::Concat || op_ == opcode::Repeat ? alt_ : 0)
     << '}';
  return os.str();
}

}
