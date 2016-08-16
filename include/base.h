//
// Created by makai on 16-6-4.
//

#ifndef REGEX_SYNTAX_BASE_H
#define REGEX_SYNTAX_BASE_H
#include <string>
#include <sstream>
#include <type_traits>
#include <memory>

namespace regex {

#define ERROR(msg, self) error(__FILE__, __LINE__, msg, self)

using std::string;
using std::istringstream;
using std::ostringstream;
using std::istreambuf_iterator;
using std::unique_ptr;
using std::shared_ptr;
using std::weak_ptr;

using std::__and_;
using std::__or_;
using std::is_same;

struct base_error {
  base_error(string const &filename, size_t line, string const &msg);

  virtual string what() const;
 private:
  string filename_;
  size_t line_;
  string error_msg_;
};

// 将enum中存储的内容取出
template<typename E>
constexpr auto toUType(E enumerator) noexcept {
  return static_cast<std::underlying_type_t<E>>(enumerator);
}

}

#endif //REGEX_SYNTAX_BASE_H
