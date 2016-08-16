//
// Created by makai on 16-6-19.
//

#include <iostream>
#include <algorithm>
#include <iomanip>
#include "ast.h"

namespace regex {

using std::cout;
using std::copy;
using std::make_move_iterator;
using std::ostream_iterator;

ast::ast() noexcept
    : is_null_(true), t_(mTAG::CONCAT) {}

ast::ast(token const &t) noexcept
    : is_null_(false), t_(t) {}

ast::ast(ast &&a) noexcept
    : is_null_(a.is_null_), t_(a.t_), children_(std::move(a.children_)) { a.clear(); }

void ast::clear() noexcept {
  is_null_ = true;
  children_.clear();
}

ast &ast::operator[](size_t i) noexcept {
  return *children_[i];
}

ast const &ast::operator[](size_t i) const noexcept {
  return *children_[i];
}


token const &ast::get_token()
const noexcept {
  return t_;
}

token &ast::get_token() noexcept {
  return t_;
}

bool ast::null()
const noexcept {
  return is_null_;
}

bool ast::empty()
const noexcept {
  return children_.empty();
}

size_t ast::size() const noexcept {
  return children_.size();
}

void ast::emplace_child(token const &t) {
  this->is_null_ = false;
  children_.emplace_back(
      make_unique<ast>(t));
}

void ast::push_child(ast &&a) {
  this->is_null_ = false;

  AstPtr item;

  if (/*(*/a.t_.is(mTAG::CONCAT) /*|| a.t_.is(mTAG::Or))*/ && a.children_.size() == 1) {
    item = std::move(a.children_[0]);
    a.clear();
  } else {
    item = std::make_unique<ast>(std::move(a));
  }

  children_.push_back(std::move(item));

  // children_.push_back(make_unique<ast>(std::move(a)));
}

ast &ast::last() noexcept {
  return *children_.back();
}

ast const &ast::last()
const noexcept {
  return *children_.back();
}

void ast::swap(ast &a) {
  std::swap(a.t_, t_);
  std::swap(a.is_null_, is_null_);
  children_.swap(a.children_);
}

ast_iterator ast::begin() noexcept {
  return ast_iterator(children_.begin());
}

ast_const_iterator ast::begin() const noexcept {
  return ast_const_iterator(children_.cbegin());
}

ast_iterator ast::end() noexcept {
  return ast_iterator(children_.end());
}

ast_const_iterator ast::end() const noexcept {
  return ast_const_iterator(children_.cend());
}

string ast::to_string()
const noexcept {
  return is_null_ ? "null" : token_to_string(t_);
}

string ast::to_string_tree() const {
  return _to_string_tree(0);
}

struct Set_E { int N_; };

Set_E set_e(int i) { return {i}; }

ostream &operator<<(ostream &os, Set_E const &e) {
  std::fill_n(ostream_iterator<char>(os), e.N_, ' ');
  return os;
}

string ast::_to_string_tree(int i) const {
  ostringstream buf;

  if (children_.empty()) {
    buf << set_e(i) << to_string();
    return buf.str();
  }

  if (!null()) {
    buf << set_e(i) << '(' << t_ << '\n';
  }

  auto beg = children_.begin();
  while (beg != children_.end() - 1) {
    buf << (*beg++)->_to_string_tree(i + 1) << '\n';
  }
  buf << (*beg)->_to_string_tree(i + 1);

  if (!null()) buf << ')';

  return buf.str();
}

}

namespace regex {

ast_iterator::ast_iterator(AstPtrs::iterator iter)
    : iter_(iter) {}

ast &ast_iterator::operator*() const noexcept {
  return *(*iter_);
}

ast *ast_iterator::operator->() const noexcept {
  return iter_->get();
}

ast_iterator& ast_iterator::operator++() noexcept {
  ++iter_;
  return *this;
}

ast_iterator ast_iterator::operator++(int) noexcept {
  return _Self(iter_++);
}

bool ast_iterator::operator==(_Self const &rhs) const noexcept {
  return rhs.iter_ == iter_;
}

bool ast_iterator::operator!=(_Self const &rhs) const noexcept {
  return !(this->operator==(rhs));
}

ast_const_iterator::ast_const_iterator(AstPtrs::const_iterator iter)
    : iter_(iter) {}

ast const& ast_const_iterator::operator*() const noexcept {
  return *(*iter_);
}

ast const *ast_const_iterator::operator->() const noexcept {
  return iter_->get();
}

ast_const_iterator &ast_const_iterator::operator++() noexcept {
  ++iter_;
  return *this;
}

ast_const_iterator ast_const_iterator::operator++(int) noexcept {
  return _Self(iter_++);
}

bool ast_const_iterator::operator==(_Self const &rhs) const noexcept {
  return rhs.iter_ == iter_;
}

bool ast_const_iterator::operator!=(_Self const &rhs) const noexcept {
  return !(this->operator==(rhs));
}

}