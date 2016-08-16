//
// Created by makai on 16-6-6.
//

#ifndef REGEX_SYNTAX_PARSER_H
#define REGEX_SYNTAX_PARSER_H

#include "lexer.h"
#include "ast.h"
#include <array>
#include <functional>
#include <sstream>

namespace regex {

using namespace std::placeholders;
using std::function;
using std::bind;
using std::array;
using std::unordered_map;

struct parser {
  friend class error;

  class error: public base_error {
   public:
    error(const string &filename, size_t line, const string &msg, parser *self)
        : base_error(filename, line, msg)
        , self_(self)
    { }

    virtual string what() const override;
   private:
    parser *self_;
  };

  explicit parser(lexer &&);

  void regex(ast&);

  void bracket(ast&);       // ( m_regex )
  void o_bracket(bool, ast&); // [ o_regex ]

  void escape(ast&);        // match(eTAG::*)
  void element(ast&);       // item | unit
  void unit(ast&);          // [ oelement ] | ( element )
  void parallel(ast&);      // | element
  void suffix(ast&);        // element [?+*]

  // match(TAG::CHAT) | escape
  template<typename EnumT>
  void item(ast& a) {
    a.emplace_child(LT(1));
    match(EnumT::CHAT);
  }

 private:
  bool checkout() const noexcept;

  template<typename EnumT>
  void match(EnumT e) {
    auto stat = tag_to_stat(e);
    switch (stat) {
      case TAG_Stat::M:
      case TAG_Stat::O:
      case TAG_Stat::E:
        if (LT(1).is(e)) {
          consume();
        } else {
          ostringstream stream;
          stream << LT(1) << " 不能匹配到: " << token(e);
          throw ERROR(stream.str(), this);
        }
        break;
      default:
        consume();
    }
  }

  void consume();

  token const &LT(size_t k) const; // 返回第k个词法单元
  bool is_b = false;
  lexer lex_;
  token lookahead_;
};

template<>
void parser::item<eTAG>(ast &a);

}


#endif //REGEX_SYNTAX_PARSER_H
