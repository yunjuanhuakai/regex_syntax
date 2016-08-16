//
// Created by makai on 16-6-4.
//

#ifndef REGEX_SYNTAX_TOKEN_H
#define REGEX_SYNTAX_TOKEN_H

#include <base.h>
#include <iosfwd>
#include <cassert>
#include <unordered_map>

namespace regex {


// 元字符
enum class mTAG: char {
  END = 0,
  CHAT = 'c',
  ALL = '.',
  IsMay = '?',
  Or = '|',
  ESCAPE = '\\',
  ZRepeat = '*',
  ORepeat = '+',
  LBRACKET = '(', RBRACKET = ')',
  OLBRACKET = '[', ORBRACKET = ']',

  NOLBRACKET = 2,// 代表'[^'
  CONCAT = 1
  // LBRACE = '{', RBRACE = '}' // 这里以后再说
};

// 转义字符
enum class eTAG: char {
  WORD = 'w',
  NWORD = 'W',
  NUMBER = 'd',
  NNUMBER = 'D',
  SPACE = 's',
  NSPACE = 'S',
  MOMATE
};

// []中的元字符
enum class oTAG: char {
  CHAT = 'o',
  RANGE = '-',
  ORBRACKET = ']',
  ESCAPE = '\\'
};

enum class TAG_Stat {
  M,
  M_C,
  O,
  O_C,
  E
};

}

namespace regex {

TAG_Stat tag_to_stat(mTAG);
TAG_Stat tag_to_stat(oTAG);
TAG_Stat tag_to_stat(eTAG);

template<typename EnumT>
bool match_tag(TAG_Stat) { return false; }


template<> bool match_tag<eTAG>(TAG_Stat stat);
template<> bool match_tag<oTAG>(TAG_Stat stat);
template<> bool match_tag<mTAG>(TAG_Stat stat);

template<typename EnumT>
struct TAG_hash {
  std::size_t operator()(EnumT e) const {
    return toUType(e);
  }
};

extern const
std::unordered_map<mTAG, string, TAG_hash<mTAG>>
    mTAG_string;

extern const
std::unordered_map<eTAG, string, TAG_hash<eTAG>>
    eTAG_string;

extern const
std::unordered_map<oTAG, string, TAG_hash<oTAG>>
    oTAG_string;


}

namespace regex {

struct token {
  class error { };

  template<typename EnumT>
  explicit token(EnumT e) noexcept
      : stat_(tag_to_stat(e)), c_(toUType(e)) { }

  token() = default;
  token(token const &) = default;
  token(token &&) = default;
  token &operator=(token const &) = default;

  // 保证部分被转义元字符不会解析出错
  template<typename EnumT>
  token(EnumT e, char c) noexcept
      : stat_(tag_to_stat(e)), c_(c) { }

  TAG_Stat stat() const noexcept;
  char c() const noexcept;

  template<typename EnumT>
  bool is(EnumT e) const noexcept {
    auto tag = tag_to_stat(e);
    switch (tag) {
      case TAG_Stat::M_C:
      case TAG_Stat::O_C:
        return tag == stat_;
      default:
        return toUType(e) == c_
            && tag == stat_;
    }
  }

  bool is_item() const noexcept {
    return
        !(stat_ == TAG_Stat::M || stat_ == TAG_Stat::O);
  }

  bool is_unit() const noexcept {
    return is(mTAG::LBRACKET) || is(mTAG::OLBRACKET) || is(mTAG::NOLBRACKET);
  }

  bool is_suffix() const noexcept {
    return is(mTAG::IsMay) || is(mTAG::ORepeat) || is(mTAG::ZRepeat);
  }

 private:

  char c_;
  TAG_Stat stat_;
};


template<typename EnumT>
EnumT to_tag(token const &t) noexcept {
  assert(match_tag<EnumT>(t.stat()));

  return EnumT(t.c());
}

string token_to_string(token const &);
std::ostream &operator<<(std::ostream &, token const &);
bool operator==(token const &, token const &);

}


#endif //REGEX_SYNTAX_TOKEN_H
