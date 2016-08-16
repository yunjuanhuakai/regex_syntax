//
// Created by makai on 16-6-4.
//

#ifndef REGEX_SYNTAX_LEXER_H
#define REGEX_SYNTAX_LEXER_H

#include <token.h>

namespace regex {

struct lexer {
  using stream_type = std::istringstream;

  friend class error;

  class error : public base_error {
   public:
    error(const string &filename, size_t line, const string &msg, lexer *self)
        : base_error(filename, line, msg)
        , self_(self)
    { }

    virtual string what() const override;
   private:
    lexer *self_;
  };

  explicit lexer(char const *);
  explicit lexer(string const &);

  token next_token();

  bool eof() const;

  string get_str() const;

  size_t pos() const { return static_cast<size_t>(p_); }

 private:


  void consume();

  token match_m();
  token match_o();
  token match_e();

  stream_type stream_;
  TAG_Stat stat_;
  char c_;
  int p_;
};

}


#endif //REGEX_SYNTAX_LEXER_H
