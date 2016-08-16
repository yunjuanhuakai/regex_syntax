//
// Created by makai on 16-6-4.
//


#include <algorithm>
#include "lexer.h"

namespace regex {

string lexer::error::what() const {
  ostringstream stream;
  stream << "LEXER: \n";
  stream << self_->get_str() << '\n';

  int p = self_->p_;
  while (p != 0) {
    --p;
    stream << ' ';
  }
  stream << "^\t" << base_error::what();

  return stream.str();
}

lexer::lexer(string const &str)
    : lexer(str.c_str())
{ }

lexer::lexer(char const *str)
    : stream_(str)
    , p_(0)
    , stat_(TAG_Stat::M)
{ stream_ >> c_; }

string lexer::get_str() const {
  return stream_.str();
}

bool lexer::eof() const {
  return stream_.eof();
}

token lexer::next_token() {
  if (eof())
    return token(mTAG::END);

  switch (stat_) {
    case TAG_Stat::M:
      return match_m();
    case TAG_Stat::O:
      return match_o();
    default:
      throw ERROR("未知的词法单元", this);
  }
}

// 匹配转义字符
token lexer::match_e() {
  auto tag = eTAG(c_);
  consume();
  switch (tag) {
    case eTAG::WORD:
    case eTAG::NWORD:
    case eTAG::NUMBER:
    case eTAG::NNUMBER:
    case eTAG::SPACE:
    case eTAG::NSPACE:
      return token(tag);
    default:
      if (stat_ == TAG_Stat::M)
        return token(mTAG::CHAT, toUType(tag));
      else
        return token(oTAG::CHAT, toUType(tag));
  }
}

// 匹配[]内元字符
token lexer::match_o() {
  auto tag = oTAG(c_);
  consume();
  switch (tag) {
    case oTAG::RANGE:
      return token(tag);
    case oTAG::ESCAPE:
      return match_e();
    case oTAG::ORBRACKET:
      stat_ = TAG_Stat::M;
      return token(mTAG::ORBRACKET);
    default:
      return token(oTAG::CHAT, toUType(tag));
  }
}

// 匹配非特殊状态的元字符
token lexer::match_m() {
  auto tag = mTAG(c_);
  consume();
  switch (tag) {
    case mTAG::END:
    case mTAG::ALL:
    case mTAG::IsMay:
    case mTAG::LBRACKET:
    case mTAG::RBRACKET:
    case mTAG::ORBRACKET:
    case mTAG::ZRepeat:
    case mTAG::ORepeat:
    case mTAG::Or:
      return token(tag);

    case mTAG::OLBRACKET:
      stat_ = TAG_Stat::O;
      if (c_ == '^') {
        consume();
        tag = mTAG::NOLBRACKET;
      }

      return token(tag);

    case mTAG::ESCAPE:
      return match_e();

    default:
      return token(mTAG::CHAT, toUType(tag));
  }
}

void lexer::consume() {
  ++p_;
  if (stream_.eof()) {
    stat_ = tag_to_stat(mTAG::END);
    c_ = toUType(mTAG::END);
  }
  stream_ >> c_;
}

}