/*
 * Copyright 2017-present Yeolar
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "crystal/foundation/json.h"

#include <algorithm>
#include <cctype>
#include <iterator>
#include <string_view>

#include "crystal/foundation/Bits.h"
#include "crystal/foundation/Conv.h"
#include "crystal/foundation/Logging.h"
#include "crystal/foundation/String.h"
#include "crystal/foundation/Unicode.h"

namespace crystal {

//////////////////////////////////////////////////////////////////////

namespace json {
namespace {

static inline bool is_confkey(char c) {
  return std::isalnum(c)
    || c == '_' || c == '-' || c == '+' || c == '/' || c == '.';
}

struct Printer {
  explicit Printer(
      std::string& out,
      unsigned* indentLevel,
      serialization_opts const* opts)
      : out_(out), indentLevel_(indentLevel), opts_(*opts) {}

  void operator()(dynamic const& v) const {
    switch (v.type()) {
    case dynamic::DOUBLE:
      if (!opts_.allow_nan_inf &&
          (std::isnan(v.asDouble()) || std::isinf(v.asDouble()))) {
        throw std::runtime_error("crystal::toJson: JSON object value was a "
          "NaN or INF");
      }
      toAppend(&out_, v.asDouble(), opts_.double_mode, opts_.double_num_digits);
      break;
    case dynamic::INT64: {
      auto intval = v.asInt();
      if (opts_.javascript_safe) {
        // Use crystal::to to check that this integer can be represented
        // as a double without loss of precision.
        intval = int64_t(to<double>(intval));
      }
      toAppend(&out_, intval);
      break;
    }
    case dynamic::BOOL:
      out_ += v.asBool() ? "true" : "false";
      break;
    case dynamic::NULLT:
      out_ += "null";
      break;
    case dynamic::STRING:
      escapeString(v.asString(), out_, opts_);
      break;
    case dynamic::OBJECT:
      printObject(v);
      break;
    case dynamic::ARRAY:
      printArray(v);
      break;
    default:
      CRYSTAL_CHECK(0) << "Bad type " << v.type();
    }
  }

 private:
  void printKV(const std::pair<const dynamic, dynamic>& p) const {
    if (!opts_.allow_non_string_keys && !p.first.isString()) {
      throw std::runtime_error(
          "crystal::toJson: JSON object key was not a string");
    }
    if (opts_.conf_style) {
      if (!p.first.isString()) {
        throw std::runtime_error(
            "crystal::toJson: JSON object key was not a string");
      }
      auto key = p.first.asString();
      if (!std::all_of(key.begin(), key.end(), is_confkey)) {
        throw std::runtime_error(
            "crystal::toJson: JSON object key was not a conf key");
      }
      out_ += key;
      mapEqual();
    } else {
      (*this)(p.first);
      mapColon();
    }
    (*this)(p.second);
  }

  template <typename Iterator>
  void printKVPairs(Iterator begin, Iterator end) const {
    printKV(*begin);
    for (++begin; begin != end; ++begin) {
      out_ += ',';
      newline();
      printKV(*begin);
    }
  }

  void printObject(dynamic const& o) const {
    if (o.empty()) {
      out_ += "{}";
      return;
    }

    out_ += '{';
    indent();
    newline();
    if (opts_.sort_keys) {
      std::vector<std::pair<dynamic, dynamic>> items(
        o.items().begin(), o.items().end());
      std::sort(items.begin(), items.end());
      printKVPairs(items.begin(), items.end());
    } else {
      printKVPairs(o.items().begin(), o.items().end());
    }
    outdent();
    newline();
    out_ += '}';
  }

  void printArray(dynamic const& a) const {
    if (a.empty()) {
      out_ += "[]";
      return;
    }

    out_ += '[';
    indent();
    newline();
    (*this)(a[0]);
    std::for_each(std::next(a.begin()), a.end(), [&](auto& val) {
      out_ += ',';
      newline();
      (*this)(val);
    });
    outdent();
    newline();
    out_ += ']';
  }

 private:
  void outdent() const {
    if (indentLevel_) {
      --*indentLevel_;
    }
  }

  void indent() const {
    if (indentLevel_) {
      ++*indentLevel_;
    }
  }

  void newline() const {
    if (indentLevel_) {
      out_ += toString('\n', std::string(*indentLevel_ * 2, ' '));
    }
  }

  void mapColon() const {
    out_ += indentLevel_ ? ": " : ":";
  }

  void mapEqual() const {
    out_ += indentLevel_ ? " = " : "=";
  }

 private:
  std::string& out_;
  unsigned* const indentLevel_;
  serialization_opts const& opts_;
};

//////////////////////////////////////////////////////////////////////

struct ParseError : std::runtime_error {
  explicit ParseError(
      unsigned int line,
      std::string const& context,
      std::string const& expected)
      : std::runtime_error(toString(
            "json parse error on line ",
            line,
            !context.empty() ? toString(" near `", context, '\'') : "",
            ": ",
            expected)) {}
};

// Wraps our input buffer with some helper functions.
struct Input {
  explicit Input(std::string_view range, json::serialization_opts const* opts)
      : range_(range)
      , opts_(*opts)
      , lineNum_(0)
  {
    storeCurrent();
  }

  Input(Input const&) = delete;
  Input& operator=(Input const&) = delete;

  char const* begin() const { return range_.begin(); }

  // Parse ahead for as long as the supplied predicate is satisfied,
  // returning a range of what was skipped.
  template <class Predicate>
  std::string_view skipWhile(const Predicate& p) {
    std::size_t skipped = 0;
    for (; skipped < range_.size(); ++skipped) {
      if (!p(range_[skipped])) {
        break;
      }
      if (range_[skipped] == '\n') {
        ++lineNum_;
      }
    }
    auto ret = range_.substr(0, skipped);
    range_.remove_prefix(skipped);
    storeCurrent();
    return ret;
  }

  std::string_view skipDigits() {
    return skipWhile([] (char c) { return c >= '0' && c <= '9'; });
  }

  std::string_view skipMinusAndDigits() {
    bool firstChar = true;
    return skipWhile([&firstChar] (char c) {
        bool result = (c >= '0' && c <= '9') || (firstChar && c == '-');
        firstChar = false;
        return result;
      });
  }

  void skipWhitespace() {
    range_ = crystal::ltrimWhitespace(range_);
    storeCurrent();
  }

  void expect(char c) {
    if (**this != c) {
      throw ParseError(lineNum_, context(), toString("expected '", c, '\''));
    }
    ++*this;
  }

  std::size_t size() const {
    return range_.size();
  }

  int operator*() const {
    return current_;
  }

  void operator++() {
    range_.remove_prefix(1);
    storeCurrent();
  }

  template <class T>
  T extract() {
    try {
      return to<T>(&range_);
    } catch (std::exception const& e) {
      error(e.what());
    }
  }

  bool consume(std::string_view str) {
    if (startsWith(range_, str)) {
      range_.remove_prefix(str.size());
      storeCurrent();
      return true;
    }
    return false;
  }

  std::string context() const {
    return std::string(range_.substr(0, 16 /* arbitrary */));
  }

  dynamic error(char const* what) const {
    throw ParseError(lineNum_, context(), what);
  }

  json::serialization_opts const& getOpts() {
    return opts_;
  }

  void incrementRecursionLevel() {
    if (currentRecursionLevel_ > opts_.recursion_limit) {
      error("recursion limit exceeded");
    }
    currentRecursionLevel_++;
  }

  void decrementRecursionLevel() {
    currentRecursionLevel_--;
  }

 private:
  void storeCurrent() {
    current_ = range_.empty() ? EOF : range_.front();
  }

 private:
  std::string_view range_;
  json::serialization_opts const& opts_;
  unsigned lineNum_;
  int current_;
  unsigned int currentRecursionLevel_{0};
};

class RecursionGuard {
 public:
  explicit RecursionGuard(Input& in) : in_(in) {
    in_.incrementRecursionLevel();
  }

  ~RecursionGuard() {
    in_.decrementRecursionLevel();
  }

 private:
  Input& in_;
};

dynamic parseValue(Input& in);
std::string parseString(Input& in);
dynamic parseNumber(Input& in);

dynamic parseObject(Input& in) {
  DCHECK_EQ(*in, '{');
  ++in;

  dynamic ret = dynamic::object;

  in.skipWhitespace();
  if (*in == '}') {
    ++in;
    return ret;
  }

  for (;;) {
    if (in.getOpts().allow_trailing_comma && *in == '}') {
      break;
    }
    if (*in == '\"') { // string
      auto key = parseString(in);
      in.skipWhitespace();
      in.expect(':');
      in.skipWhitespace();
      ret.insert(std::move(key), parseValue(in));
    } else if (in.getOpts().conf_style) {
      std::string key(in.skipWhile(is_confkey));
      in.skipWhitespace();
      in.expect('=');
      in.skipWhitespace();
      ret.insert(std::move(key), parseValue(in));
    } else if (!in.getOpts().allow_non_string_keys) {
      in.error("expected string for object key name");
    } else {
      auto key = parseValue(in);
      in.skipWhitespace();
      in.expect(':');
      in.skipWhitespace();
      ret.insert(std::move(key), parseValue(in));
    }

    in.skipWhitespace();
    if (*in != ',') {
      break;
    }
    ++in;
    in.skipWhitespace();
  }
  in.expect('}');

  return ret;
}

dynamic parseArray(Input& in) {
  DCHECK_EQ(*in, '[');
  ++in;

  dynamic ret = dynamic::array;

  in.skipWhitespace();
  if (*in == ']') {
    ++in;
    return ret;
  }

  for (;;) {
    if (in.getOpts().allow_trailing_comma && *in == ']') {
      break;
    }
    ret.push_back(parseValue(in));
    in.skipWhitespace();
    if (*in != ',') {
      break;
    }
    ++in;
    in.skipWhitespace();
  }
  in.expect(']');

  return ret;
}

dynamic parseNumber(Input& in) {
  bool const negative = (*in == '-');
  if (negative && in.consume("-Infinity")) {
    if (in.getOpts().parse_numbers_as_strings) {
      return "-Infinity";
    } else {
      return -std::numeric_limits<double>::infinity();
    }
  }

  auto integral = in.skipMinusAndDigits();
  if (negative && integral.size() < 2) {
    in.error("expected digits after `-'");
  }

  auto const wasE = *in == 'e' || *in == 'E';

  constexpr const char* maxInt = "9223372036854775807";
  constexpr const char* minInt = "-9223372036854775808";
  constexpr auto maxIntLen = constexpr_strlen(maxInt);
  constexpr auto minIntLen = constexpr_strlen(minInt);

  if (*in != '.' && !wasE && in.getOpts().parse_numbers_as_strings) {
    return integral;
  }

  if (*in != '.' && !wasE) {
    if (CRYSTAL_LIKELY(!in.getOpts().double_fallback || integral.size() < maxIntLen) ||
        (!negative && integral.size() == maxIntLen && integral <= maxInt) ||
        (negative && integral.size() == minIntLen && integral <= minInt)) {
      auto val = to<int64_t>(integral);
      in.skipWhitespace();
      return val;
    } else {
      auto val = to<double>(integral);
      in.skipWhitespace();
      return val;
    }
  }

  auto end = !wasE ? (++in, in.skipDigits().end()) : in.begin();
  if (*in == 'e' || *in == 'E') {
    ++in;
    if (*in == '+' || *in == '-') {
      ++in;
    }
    auto expPart = in.skipDigits();
    end = expPart.end();
  }
  auto fullNum = make_string_view(integral.begin(), end);
  if (in.getOpts().parse_numbers_as_strings) {
    return fullNum;
  }
  auto val = to<double>(fullNum);
  return val;
}

std::string decodeUnicodeEscape(Input& in) {
  auto hexVal = [&] (int c) -> uint16_t {
    return uint16_t(
           c >= '0' && c <= '9' ? c - '0' :
           c >= 'a' && c <= 'f' ? c - 'a' + 10 :
           c >= 'A' && c <= 'F' ? c - 'A' + 10 :
           (in.error("invalid hex digit"), 0));
  };

  auto readHex = [&]() -> uint16_t {
    if (in.size() < 4) {
      in.error("expected 4 hex digits");
    }

    uint16_t ret = uint16_t(hexVal(*in) * 4096);
    ++in;
    ret += hexVal(*in) * 256;
    ++in;
    ret += hexVal(*in) * 16;
    ++in;
    ret += hexVal(*in);
    ++in;
    return ret;
  };

  /*
   * If the value encoded is in the surrogate pair range, we need to
   * make sure there is another escape that we can use also.
   */
  uint32_t codePoint = readHex();
  if (codePoint >= 0xd800 && codePoint <= 0xdbff) {
    if (!in.consume("\\u")) {
      in.error("expected another unicode escape for second half of "
        "surrogate pair");
    }
    uint16_t second = readHex();
    if (second >= 0xdc00 && second <= 0xdfff) {
      codePoint = 0x10000 + ((codePoint & 0x3ff) << 10) +
                  (second & 0x3ff);
    } else {
      in.error("second character in surrogate pair is invalid");
    }
  } else if (codePoint >= 0xdc00 && codePoint <= 0xdfff) {
    in.error("invalid unicode code point (in range [0xdc00,0xdfff])");
  }

  return codePointToUtf8(codePoint);
}

std::string parseString(Input& in) {
  DCHECK_EQ(*in, '\"');
  ++in;

  std::string ret;
  for (;;) {
    auto range = in.skipWhile(
      [] (char c) { return c != '\"' && c != '\\'; }
    );
    ret.append(range.begin(), range.end());

    if (*in == '\"') {
      ++in;
      break;
    }
    if (*in == '\\') {
      ++in;
      switch (*in) {
      case '\"':    ret.push_back('\"'); ++in; break;
      case '\\':    ret.push_back('\\'); ++in; break;
      case '/':     ret.push_back('/');  ++in; break;
      case 'b':     ret.push_back('\b'); ++in; break;
      case 'f':     ret.push_back('\f'); ++in; break;
      case 'n':     ret.push_back('\n'); ++in; break;
      case 'r':     ret.push_back('\r'); ++in; break;
      case 't':     ret.push_back('\t'); ++in; break;
      case 'u':     ++in; ret += decodeUnicodeEscape(in); break;
      default:
        in.error(toString("unknown escape ", *in, " in string").c_str());
      }
      continue;
    }
    if (*in == EOF) {
      in.error("unterminated string");
    }
    if (!*in) {
      /*
       * Apparently we're actually supposed to ban all control
       * characters from strings.  This seems unnecessarily
       * restrictive, so we're only banning zero bytes.  (Since the
       * string is presumed to be UTF-8 encoded it's fine to just
       * check this way.)
       */
      in.error("null byte in string");
    }

    ret.push_back(char(*in));
    ++in;
  }

  return ret;
}

dynamic parseValue(Input& in) {
  RecursionGuard guard(in);

  in.skipWhitespace();
  return *in == '[' ? parseArray(in) :
         *in == '{' ? parseObject(in) :
         *in == '\"' ? parseString(in) :
         (*in == '-' || (*in >= '0' && *in <= '9')) ? parseNumber(in) :
         in.consume("true") ? true :
         in.consume("false") ? false :
         in.consume("null") ? nullptr :
         in.consume("Infinity") ?
          (in.getOpts().parse_numbers_as_strings ? (dynamic)"Infinity" :
            (dynamic)std::numeric_limits<double>::infinity()) :
         in.consume("NaN") ?
           (in.getOpts().parse_numbers_as_strings ? (dynamic)"NaN" :
             (dynamic)std::numeric_limits<double>::quiet_NaN()) :
         in.error("expected json value");
}

} // namespace

//////////////////////////////////////////////////////////////////////

std::string serialize(dynamic const& dyn, serialization_opts const& opts) {
  std::string ret;
  unsigned indentLevel = 0;
  Printer p(ret, opts.pretty_formatting ? &indentLevel : nullptr, &opts);
  p(dyn);
  return ret;
}

// Fast path to determine the longest prefix that can be left
// unescaped in a string of sizeof(T) bytes packed in an integer of
// type T.
template <class T>
size_t firstEscapableInWord(T s) {
  static_assert(std::is_unsigned<T>::value, "Unsigned integer required");
  static constexpr T kOnes = ~T() / 255; // 0x...0101
  static constexpr T kMsbs = kOnes * 0x80; // 0x...8080

  // Sets the MSB of bytes < b. Precondition: b < 128.
  auto isLess = [](T w, uint8_t b) {
    // A byte is < b iff subtracting b underflows, so we check that
    // the MSB wasn't set before and it's set after the subtraction.
    return (w - kOnes * b) & ~w & kMsbs;
  };

  auto isChar = [&](uint8_t c) {
    // A byte is == c iff it is 0 if xored with c.
    return isLess(s ^ (kOnes * c), 1);
  };

  // The following masks have the MSB set for each byte of the word
  // that satisfies the corresponding condition.
  auto isHigh = s & kMsbs; // >= 128
  auto isLow = isLess(s, 0x20); // <= 0x1f
  auto needsEscape = isHigh | isLow | isChar('\\') | isChar('"');

  if (!needsEscape) {
    return sizeof(T);
  }

  if (crystal::kIsLittleEndian) {
    return crystal::findFirstSet(needsEscape) / 8 - 1;
  } else {
    return sizeof(T) - crystal::findLastSet(needsEscape) / 8;
  }
}

// Escape a string so that it is legal to print it in JSON text.
void escapeString(
    std::string_view input,
    std::string& out,
    const serialization_opts& opts) {
  auto hexDigit = [] (uint8_t c) -> char {
    return c < 10 ? c + '0' : c - 10 + 'a';
  };

  out.push_back('\"');

  auto* p = reinterpret_cast<const unsigned char*>(input.begin());
  auto* q = reinterpret_cast<const unsigned char*>(input.begin());
  auto* e = reinterpret_cast<const unsigned char*>(input.end());

  while (p < e) {
    // Find the longest prefix that does not need escaping, and copy
    // it literally into the output string.
    auto firstEsc = p;
    while (firstEsc < e) {
      auto avail = e - firstEsc;
      uint64_t word = 0;
      if (avail >= 8) {
        word = crystal::loadUnaligned<uint64_t>(firstEsc);
      } else {
        word = crystal::partialLoadUnaligned<uint64_t>(firstEsc, avail);
      }
      auto prefix = firstEscapableInWord(word);
      DCHECK_LE(prefix, size_t(avail));
      firstEsc += prefix;
      if (prefix < 8) {
        break;
      }
    }
    if (firstEsc > p) {
      out.append(reinterpret_cast<const char*>(p), firstEsc - p);
      p = firstEsc;
      // We can't be in the middle of a multibyte sequence, so we can reset q.
      q = p;
      if (p == e) {
        break;
      }
    }

    // Handle the next byte that may need escaping.

    // Since non-ascii encoding inherently does utf8 validation
    // we explicitly validate utf8 only if non-ascii encoding is disabled.
    if ((opts.validate_utf8 || opts.skip_invalid_utf8)
        && !opts.encode_non_ascii) {
      // To achieve better spatial and temporal coherence
      // we do utf8 validation progressively along with the
      // string-escaping instead of two separate passes.

      // As the encoding progresses, q will stay at or ahead of p.
      CRYSTAL_CHECK_GE(q, p);

      // As p catches up with q, move q forward.
      if (q == p) {
        // calling utf8_decode has the side effect of
        // checking that utf8 encodings are valid
        char32_t v = utf8ToCodePoint(q, e, opts.skip_invalid_utf8);
        if (opts.skip_invalid_utf8 && v == U'\ufffd') {
          out.append(u8"\ufffd");
          p = q;
          continue;
        }
      }
    }
    if (opts.encode_non_ascii && (*p & 0x80)) {
      // note that this if condition captures utf8 chars
      // with value > 127, so size > 1 byte
      char32_t v = utf8ToCodePoint(p, e, opts.skip_invalid_utf8);
      char buf[] = "\\u\0\0\0\0";
      buf[2] = hexDigit(uint8_t(v >> 12));
      buf[3] = hexDigit((v >> 8) & 0x0f);
      buf[4] = hexDigit((v >> 4) & 0x0f);
      buf[5] = hexDigit(v & 0x0f);
      out.append(buf, 6);
    } else if (*p == '\\' || *p == '\"') {
      char buf[] = "\\\0";
      buf[1] = char(*p++);
      out.append(buf, 2);
    } else if (*p <= 0x1f) {
      switch (*p) {
        case '\b': out.append("\\b"); p++; break;
        case '\f': out.append("\\f"); p++; break;
        case '\n': out.append("\\n"); p++; break;
        case '\r': out.append("\\r"); p++; break;
        case '\t': out.append("\\t"); p++; break;
        default:
          // Note that this if condition captures non readable chars
          // with value < 32, so size = 1 byte (e.g control chars).
          char buf[] = "\\u00\0\0";
          buf[4] = hexDigit(uint8_t((*p & 0xf0) >> 4));
          buf[5] = hexDigit(uint8_t(*p & 0xf));
          out.append(buf, 6);
          p++;
      }
    } else {
      out.push_back(char(*p++));
    }
  }

  out.push_back('\"');
}

std::string stripComments(std::string_view jsonC) {
  std::string result;
  enum class State {
    None,
    InString,
    InlineComment,
    LineComment
  } state = State::None;

  for (size_t i = 0; i < jsonC.size(); ++i) {
    auto s = jsonC.substr(i);
    switch (state) {
      case State::None:
        if (startsWith(s, "/*")) {
          state = State::InlineComment;
          ++i;
          continue;
        } else if (startsWith(s, "//")) {
          state = State::LineComment;
          ++i;
          continue;
        } else if (s[0] == '\"') {
          state = State::InString;
        }
        result.push_back(s[0]);
        break;
      case State::InString:
        if (s[0] == '\\') {
          if (CRYSTAL_UNLIKELY(s.size() == 1)) {
            throw std::logic_error("Invalid JSONC: string is not terminated");
          }
          result.push_back(s[0]);
          result.push_back(s[1]);
          ++i;
          continue;
        } else if (s[0] == '\"') {
          state = State::None;
        }
        result.push_back(s[0]);
        break;
      case State::InlineComment:
        if (startsWith(s, "*/")) {
          state = State::None;
          ++i;
        }
        break;
      case State::LineComment:
        if (s[0] == '\n') {
          // skip the line break. It doesn't matter.
          state = State::None;
        }
        break;
      default:
        throw std::logic_error("Unknown comment state");
    }
  }
  return result;
}

} // namespace json

//////////////////////////////////////////////////////////////////////

dynamic parseJson(std::string_view range) {
  return parseJson(range, json::serialization_opts());
}

dynamic parseJson(
    std::string_view range,
    json::serialization_opts const& opts) {

  json::Input in(range, &opts);

  auto ret = parseValue(in);
  in.skipWhitespace();
  if (in.size() && *in != '\0') {
    in.error("parsing didn't consume all input");
  }
  return ret;
}

std::string toJson(dynamic const& dyn) {
  return json::serialize(dyn, json::serialization_opts());
}

std::string toPrettyJson(dynamic const& dyn) {
  json::serialization_opts opts;
  opts.pretty_formatting = true;
  return json::serialize(dyn, opts);
}

dynamic parseCson(std::string_view range) {
  json::serialization_opts opts;
  opts.conf_style = true;
  return parseJson(range, opts);
}

std::string toCson(dynamic const& dyn) {
  json::serialization_opts opts;
  opts.conf_style = true;
  opts.sort_keys = true;
  return json::serialize(dyn, opts);
}

std::string toPrettyCson(dynamic const& dyn) {
  json::serialization_opts opts;
  opts.conf_style = true;
  opts.pretty_formatting = true;
  opts.sort_keys = true;
  return json::serialize(dyn, opts);
}

//////////////////////////////////////////////////////////////////////
// dynamic::print_as_pseudo_json() is implemented here for header
// ordering reasons (most of the dynamic implementation is in
// dynamic-inl.h, which we don't want to include json.h).

void dynamic::print_as_pseudo_json(std::ostream& out) const {
  json::serialization_opts opts;
  opts.allow_non_string_keys = true;
  opts.allow_nan_inf = true;
  out << json::serialize(*this, opts);
}

void PrintTo(const dynamic& dyn, std::ostream* os) {
  json::serialization_opts opts;
  opts.allow_nan_inf = true;
  opts.allow_non_string_keys = true;
  opts.pretty_formatting = true;
  opts.sort_keys = true;
  *os << json::serialize(dyn, opts);
}

//////////////////////////////////////////////////////////////////////

} // namespace crystal
