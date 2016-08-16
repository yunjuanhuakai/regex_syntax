//
// Created by makai on 16-6-19.
//

#ifndef REGEX_SYNTAX_AST_H
#define REGEX_SYNTAX_AST_H

#include <base.h>
#include <vector>
#include <iterator>
#include <memory>
#include <iosfwd>
#include <token.h>

namespace regex {

struct ast;

using std::vector;
using std::ostream;
using std::make_unique;
using std::iterator;
using std::forward_iterator_tag;
using AstPtr = unique_ptr<ast>;
using AstPtrs = vector<AstPtr>;

struct ast_iterator
    : iterator<forward_iterator_tag, ast> {
  using _Self = ast_iterator;

  explicit ast_iterator(AstPtrs::iterator iter);

  reference operator*() const noexcept;
  pointer operator->() const noexcept;

  _Self& operator++() noexcept;
  _Self operator++(int) noexcept;

  bool operator==(_Self const &) const noexcept;
  bool operator!=(_Self const &) const noexcept;

 private:
  AstPtrs::iterator iter_;
};

struct ast_const_iterator
    : iterator<forward_iterator_tag, ast const> {
  using _Self = ast_const_iterator;

  explicit ast_const_iterator(AstPtrs::const_iterator iter);

  reference operator*() const noexcept;
  pointer operator->() const noexcept;

  _Self &operator++() noexcept;
  _Self operator++(int) noexcept;

  bool operator==(_Self const &) const noexcept;
  bool operator!=(_Self const &) const noexcept;

 private:
  AstPtrs::const_iterator iter_;
};

struct ast {
  ast() noexcept;
  explicit ast(token const &t) noexcept;

  ast(ast const&) = delete;
  ast &operator=(ast const&) = delete;

  ast(ast&&) noexcept;
  // ast &operator=(std::nullptr_t) noexcept;

  ast const &operator[](size_t) const noexcept;
  ast &operator[](size_t) noexcept;

  void clear() noexcept;
  size_t size() const noexcept;

  // void emplace_child();
  void emplace_child(token const&);
  void push_child(ast&&);

  ast& last() noexcept;
  ast const& last() const noexcept;

  bool empty() const noexcept;
  bool null() const noexcept;
  token const& get_token() const noexcept;
  token &get_token() noexcept;

  ast_iterator begin() noexcept;
  ast_const_iterator begin() const noexcept;

  ast_iterator end() noexcept;
  ast_const_iterator end() const noexcept;

  string to_string() const noexcept;
  string to_string_tree() const;

  void swap(ast& a);

 private:
  string _to_string_tree(int i) const;

  bool is_null_ = false;
  token t_;
  AstPtrs children_;
};


}

#endif //REGEX_SYNTAX_AST_H
