//
// Created by makai on 16-6-4.
//

#include "token.h"
#include <unordered_map>

namespace regex {

base_error::base_error(string const &filename, size_t line, string const &msg)
    : filename_(filename)
    , line_(line)
    , error_msg_(msg)
{ }

string base_error::what() const {
  ostringstream stream;
  stream << filename_ << ": " << line_ << ":\n" << error_msg_ << '\n';
  return stream.str();
}

#define M_TOSTRING(m) { mTAG::m, "m_"#m }
#define E_TOSTRING(e) { eTAG::e, "e_"#e }
#define O_TOSTRING(o) { oTAG::o, "o_"#o }

const std::unordered_map<mTAG, string, TAG_hash<mTAG>>
    mTAG_string{
    // M_TOSTRING(CHAT),
    M_TOSTRING(CONCAT),
    M_TOSTRING(END),
    M_TOSTRING(ALL),
    M_TOSTRING(IsMay),
    M_TOSTRING(Or),
    M_TOSTRING(ESCAPE),
    M_TOSTRING(ZRepeat),
    M_TOSTRING(ORepeat),
    M_TOSTRING(LBRACKET),
    M_TOSTRING(RBRACKET),
    M_TOSTRING(OLBRACKET),
    M_TOSTRING(ORBRACKET),
    M_TOSTRING(NOLBRACKET)
};

const std::unordered_map<eTAG, string, TAG_hash<eTAG>>
    eTAG_string{
    E_TOSTRING(WORD),
    E_TOSTRING(NWORD),
    E_TOSTRING(NUMBER),
    E_TOSTRING(NNUMBER),
    E_TOSTRING(SPACE),
    E_TOSTRING(NSPACE),
    E_TOSTRING(MOMATE)
};

const std::unordered_map<oTAG, string, TAG_hash<oTAG>>
    oTAG_string{
    // O_TOSTRING(CHAT),
    O_TOSTRING(ESCAPE),
    O_TOSTRING(ORBRACKET),
    O_TOSTRING(RANGE)
};

string TAG_to_string(TAG_Stat, char);

}


namespace regex {

template<>
bool match_tag<eTAG>(TAG_Stat stat) { return stat == TAG_Stat::E; }
template<>
bool match_tag<oTAG>(TAG_Stat stat) { return stat == TAG_Stat::O; }
template<>
bool match_tag<mTAG>(TAG_Stat stat) { return stat == TAG_Stat::M; }

TAG_Stat tag_to_stat(mTAG e) {
  if (mTAG_string.find(e) == mTAG_string.end()) {
    return TAG_Stat::M_C;
  }

  return TAG_Stat::M;
}

TAG_Stat tag_to_stat(oTAG e) {
  if (oTAG_string.find(e) == oTAG_string.end()) {
    return TAG_Stat::O_C;
  }

  return TAG_Stat::O;
}

TAG_Stat tag_to_stat(eTAG) {
  return TAG_Stat::E;
}

}


namespace regex {

TAG_Stat token::stat() const noexcept {
  return stat_;
}

char token::c() const noexcept {
  return c_;
}

bool operator==(token const &lhs, token const &rhs) {
  return lhs.stat() == rhs.stat()
      && lhs.c() == rhs.c();
}

string token_to_string(token const &t) {
  return TAG_to_string(t.stat(), t.c());
}

std::ostream &operator<<(std::ostream &os, token const &t) {
  os << token_to_string(t);
  return os;
}

template<typename MapT>
string get_the_string(MapT const &m, char c) {
  using k_type = typename MapT::key_type;
  auto iter = m.find(k_type(c));
  if (iter != m.end()) {
    return iter->second;
  } else {
    throw token::error();
  }
}

string TAG_to_string(TAG_Stat stat, char c) {
  string s;

  switch (stat) {
    case TAG_Stat::M  :
      return get_the_string(mTAG_string, c);
    case TAG_Stat::O  :
      return get_the_string(oTAG_string, c);
    case TAG_Stat::E:
      return get_the_string(eTAG_string, c);

    default:
      break;
    /*
    case TAG_Stat::M_C:
      s = "m_CHAT: ";
      break;
    case TAG_Stat::O_C:
      s = "o_CHAT: ";
      break;
    */
  }

  s.push_back(c);
  return s;
}

}