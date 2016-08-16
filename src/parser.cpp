//
// Created by makai on 16-6-6.
//

#include <iostream>
#include "parser.h"

namespace regex {

parser::parser(lexer &&lex)
    : lex_(std::move(lex)) {
  consume();
}

template<>
void parser::item<eTAG>(ast &a) { escape(a); }

void parser::regex(ast &a) {
  while (!LT(1).is(mTAG::END))
    element(a);
  match(mTAG::END);
}

// item<m> | unit
void parser::element(ast &a) {
  bool b = true;

  if (LT(1).is_unit()) {
    unit(a);
  } else if (LT(1).is_item()) {
    item<mTAG>(a);
  } else if (LT(1).is(mTAG::ALL)) {
    a.emplace_child(LT(1));
    match(mTAG::ALL);
  } else if (LT(1).is(mTAG::Or)) {
    parallel(a);
  } else {
    b = false;
  }

  if (b && LT(1).is_suffix()) {
    suffix(a.last());
  }

  if (!b)
    throw ERROR("未知的语法: " + token_to_string(LT(1)), this);
}

// match(eTAG::*)
void parser::escape(ast &a) {
  a.emplace_child(LT(1));
  match(to_tag<eTAG>(LT(1)));
}

// [ oelement ] | ( element )
void parser::unit(ast &a) {
  if (LT(1).is(mTAG::LBRACKET))
    bracket(a);
  else
    o_bracket(LT(1).is(mTAG::NOLBRACKET), a);
}

// ( element )
void parser::bracket(ast &a) {
  // ast chlid{LT(1)};
  match(mTAG::LBRACKET);
  is_b = true;
  ast brack;
  while (!LT(1).is(mTAG::RBRACKET)) {
    if (!checkout())
      throw ERROR("缺少匹配: ')'", this);

    element(brack);
    if (LT(1).is(mTAG::Or)) {
      parallel(brack);
    }
  }

  // chlid.push_child(std::move(brack));
  // chlid.emplace_child(LT(1));
  is_b = false;
  match(mTAG::RBRACKET);

  a.push_child(std::move(brack));
}

void parser::parallel(ast &a) {
   mTAG tag = is_b? mTAG::RBRACKET : mTAG::END;

  if (a.empty())
    throw ERROR("'|'操作符为一个二元操作符", this);
  // if (a.get_token().is(mTAG::Or)) {
  // }

  ast or_{LT(1)};
  match(mTAG::Or);
  or_.push_child(std::move(a));
  ast n_elem;
  while (!LT(1).is(tag)) {
    /*
    if (LT(1).is(mTAG::Or)) {
      or_.push_child(std::move(n_elem));
      match(mTAG::Or);
    }
     */
    element(n_elem);
  }

  is_b = ~is_b;
  or_.push_child(std::move(n_elem));
  or_.swap(a);
}

// [ oelement ]
void parser::o_bracket(bool b, ast &a) {
  ast child{LT(1)};

  if (b) match(mTAG::NOLBRACKET);
  else match(mTAG::OLBRACKET);

  while (!LT(1).is(mTAG::ORBRACKET)) {
    if (!checkout())
      throw ERROR("缺少匹配: ']'", this);

    if (LT(1).stat() == TAG_Stat::E) {
      escape(child);
    } else if (LT(1).is(oTAG::RANGE)) {
      if (child.empty())
        throw ERROR("'-'操作符缺乏前缀", this);

      ast range{LT(1)};
      match(oTAG::RANGE);
      char l = child.last().get_token().c();
      range.push_child(std::move(child.last()));
      item<oTAG>(range);

      if (range.last().get_token().stat() != TAG_Stat::O_C)
        throw ERROR("'-'操作符缺乏正确的后缀", this);
      if (range.last().get_token().c() < l)
        throw ERROR("'-'操作符左侧大于右侧", this);

      range.swap(child.last());
    } else
      item<oTAG>(child);
  }

  // child.emplace_child(LT(1));
  match(mTAG::ORBRACKET);

  a.push_child(std::move(child));
}

void parser::suffix(ast &a) {
  ast child{LT(1)};
  auto tag = to_tag<mTAG>(LT(1));
  switch (tag) {
    case mTAG::IsMay:
    case mTAG::ORepeat:
    case mTAG::ZRepeat:
      child.push_child(std::move(a));
      child.swap(a);
      match(tag);
      break;
    default:
      throw ERROR("未知的匹配: " + token_to_string(LT(1)), this);
  }
}

}

namespace regex {

token const &parser::LT(size_t) const {
  return lookahead_;
}

bool parser::checkout() const noexcept {
  return !LT(1).is(mTAG::END);
}

void parser::consume() {
  lookahead_ = lex_.next_token();
}

}

namespace regex {

string parser::error::what() const {
  ostringstream stream;
  stream << "PARSER: \n";
  stream << self_->lex_.get_str() << '\n';

  size_t p = self_->lex_.pos() - 1;
  while (p != 0) {
    --p;
    stream << ' ';
  }
  stream << "^\t" << base_error::what();

  return stream.str();
}

}


