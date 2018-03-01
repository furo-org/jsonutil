#pragma once

/****************************************************************************
 * jsonutil: Utilities to write serialization functions for nlohmann::json
 * Copyright (C) 2018 Tomoaki Yoshida
 * Copyright (C) 2017 Future Robotics Technology Center (fuRo),
 *                    Chiba Institute of Technology.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 ****************************************************************************/

//======================================================================
/*
  Helper macro and template functions to embed comma sepalated names and access
  functions in class definitions.

  Defining API
    YOS_EMBED_NAMES(...)  macro
      Embed arbitrary number of names and access functions.
      To be used in class definitions.

  Accessing API
    constexpr size_t members_size_()
      Returns number of names defined by YOS_EMBED_NAMES

    constexpr std::pair<const char*,size_t> membername_const_(size_t pos);
    std::string membername_(size_t pos);  // (not constexpr)
      Returns pos-th name in the name definition.

      membername_() constructs and returns std::string at runtime.

      membername_const_() returns char* and length of the name which is
      processed in compile time. A name char* points at is NOT null-terminated.

*/

#include <array>
#include <typeinfo>
#include <utility>

#ifdef NOCONSTEXPR
#define CONSTEXPR
#else
#define CONSTEXPR constexpr
#endif

namespace yos {

template <typename Char>
CONSTEXPR bool is_blank(Char c) {
  // clang-format off
  return c == ' ' ? true
      : c == '\t' ? true
      : c == '\0' ? true
      : c == '\n' ? true
      : false;
  // clang-format on
}

CONSTEXPR size_t cstrlen(const char* s) {
  return *s != '\0' ? cstrlen(s + 1) + 1 : 0;
}

//-------------------------------------------------- strip
/*
  returns: How much increments required to reach last from first.
 */
template <typename Itr>
CONSTEXPR size_t distance(Itr first, Itr last) {
  return
#if 0  // gcc refuses to compile this. clang is ok.
      first>last ?throw std::logic_error("first>last") :
#endif
      first == last ? 0 : distance(first + 1, last) + 1;
}

/*
  strip(const char[])
  returns: strip heading and trailing blank chars
  blank characters are evaluated by template<Itr>is_blank()
 */
template <typename Itr>
CONSTEXPR std::pair<Itr, size_t> strip_impl(Itr first, Itr last) {
  // clang-format off
  return first == last ? (
      is_blank(*first) ? std::make_pair(first, distance(first, last))
                       : std::make_pair(first, distance(first, last) + 1))
      : (is_blank(*first) ? strip_impl(first + 1, last)
          : (is_blank(*last)  ? strip_impl(first, last - 1)
             : std::make_pair(first, distance(first, last + 1))));
  // clang-format on
}

template <unsigned N>
CONSTEXPR auto     strip(const char (&s)[N])
    -> decltype(strip_impl(&s[0], &s[N - 1])) {
  return strip_impl(&s[0], &s[N - 1]);
}

//-------------------------------------------------- tokenize
template <size_t N, typename Itr, typename... Ts>
CONSTEXPR auto tokenize_impl(size_t pos, Itr first, Itr last, Ts... args) ->
    typename std::enable_if<sizeof...(Ts) >= N,
                            const std::array<std::pair<Itr, size_t>, N>>::type {
  return std::array<std::pair<Itr, size_t>, N>();
}

template <size_t N, typename Itr, typename... Ts>
CONSTEXPR auto tokenize_impl(size_t pos, Itr first, Itr last, Ts... args) ->
    typename std::enable_if<sizeof...(Ts) < N,
                            const std::array<std::pair<Itr, size_t>, N>>::type {
  return
      /* if pos is eos -> make token list */
      first + pos == last || *(first + pos) == '\0'
          ? (pos == 0
                 ? std::array<std::pair<Itr, size_t>, N>{{args...}}
                 : std::array<std::pair<Itr, size_t>,
                              N>{{args..., strip_impl(first, first + pos)}})

          /* if pos==PUNCT -> add new token(first,pos) and resume parsing from
             next char */
          : *(first + pos) == ','
                ? tokenize_impl<N>(0, first + pos + 1, last, args...,
                                   strip_impl(first, first + pos - 1))

                /* if pos!=PUNCT -> advance pos */
                : tokenize_impl<N>(pos + 1, first, last, args...);
}

template <size_t NTok>
CONSTEXPR const std::array<std::pair<const char*, size_t>, NTok> tokenize(
    const char* s) {
  return tokenize_impl<NTok>(0, &s[0], s + cstrlen(s));
}

// Returns number of items
CONSTEXPR size_t countargn_impl(const char* s, const char* pos,
                                const size_t c = 0) {
  return *pos == '\0' ? (s == pos ? 0 : c + 1)  // if length==0 return 0
                      : *pos == ',' ? countargn_impl(s, pos + 1, c + 1)
                                    : countargn_impl(s, pos + 1, c);
}
CONSTEXPR size_t countargn(const char* s) { return countargn_impl(s, s); }

// Returns length of longest item
CONSTEXPR unsigned countargl_impl(const char* s, const char* pos,
                                  unsigned c = 0, unsigned l = 0) {
  return *pos == '\0'
             ? (s == pos ? 0 : (c >= l ? c : l))
             : *pos == ','
                   ? countargl_impl(s, pos + 1, 0,
                                    (c >= l ? c : l))     /* next arg */
                   : countargl_impl(s, pos + 1, c + 1, l) /* next char */
      ;
}
CONSTEXPR unsigned countargl(const char* s) { return countargl_impl(s, s); }

}  // namespace yos

#define YOS_EMBED_NAMES(...)                                               \
  CONSTEXPR static const char*  members_() { return #__VA_ARGS__; }        \
  CONSTEXPR static const size_t members_size_() {                          \
    return yos::countargn(members_());                                     \
  }                                                                        \
  /* constexpr */                                                          \
  CONSTEXPR static const std::pair<const char*, size_t> membername_const_( \
      int i) {                                                             \
    return yos::tokenize<members_size_()>(members_())[i];                  \
  }                                                                        \
  /* membername_const_ with type conversion. */                            \
  template <typename RT = std::string>                                     \
  static RT membername_(int i) {                                           \
    CONSTEXPR static const auto tok =                                      \
        yos::tokenize<members_size_()>(members_());                        \
    return RT(tok[i].first, tok[i].second);                                \
  }

/*
  nlohmann::json serialize function implementing helper

  macro defining to_json/from_json member functions:
  JSON_MEMBER(...)
  JSON_MEMBER_OBJ(...)
  JSON_MEMBER_ARRAY(...)

  generic to_json/from_json
*/

#define JSON_MEMBER(...)       \
  YOS_EMBED_NAMES(__VA_ARGS__) \
  FROM_JSON_(__VA_ARGS__)      \
  TO_JSON_ARRAY(__VA_ARGS__)   \
  TO_JSON_OBJ(__VA_ARGS__)

#define JSON_MEMBER_OBJ(...)   \
  YOS_EMBED_NAMES(__VA_ARGS__) \
  FROM_JSON_(__VA_ARGS__)      \
  TO_JSON_OBJ(__VA_ARGS__)

#define JSON_MEMBER_ARRAY(...) \
  YOS_EMBED_NAMES(__VA_ARGS__) \
  FROM_JSON_(__VA_ARGS__)      \
  TO_JSON_ARRAY(__VA_ARGS__)

#define FROM_JSON_(...)                                                     \
  template <typename BasicJsonType>                                         \
  void from_json(BasicJsonType&& j) {                                       \
    if (j.is_array())                                                       \
      from_json_array(std::forward<BasicJsonType>(j), 0, __VA_ARGS__);      \
    else                                                                    \
      from_json(std::forward<BasicJsonType>(j), 0, __VA_ARGS__);            \
  }                                                                         \
  template <typename BasicJsonType, typename M1, typename... Ts>            \
  void from_json(BasicJsonType&& j, size_t pos, M1& m, Ts&... rest) {       \
    m = j.at(membername_(pos)).template get<M1>();                          \
    from_json(std::forward<BasicJsonType>(j), pos + 1, rest...);            \
  }                                                                         \
  template <typename BasicJsonType>                                         \
  void from_json(const BasicJsonType& j, size_t pos) {}                     \
  template <typename BasicJsonType, typename M1, typename... Ts>            \
  void from_json_array(BasicJsonType&& j, size_t pos, M1& m, Ts&... rest) { \
    m = j.at(pos).template get<M1>();                                       \
    from_json_array(std::forward<BasicJsonType>(j), pos + 1, rest...);      \
  }                                                                         \
  template <typename BasicJsonType>                                         \
  void from_json_array(const BasicJsonType& j, size_t pos) {}

#define TO_JSON_OBJ(...)                                                       \
  template <typename BasicJsonType>                                            \
  BasicJsonType to_json_obj() const {                                          \
    auto j = BasicJsonType::object();                                          \
    to_json_obj(j, 0, __VA_ARGS__);                                            \
    return j;                                                                  \
  }                                                                            \
  template <typename BasicJsonType>                                            \
  void to_json_obj(BasicJsonType& j) const& {                                  \
    j = BasicJsonType::object();                                               \
    return to_json_obj(j, 0, __VA_ARGS__);                                     \
  }                                                                            \
  template <typename BasicJsonType>                                            \
  void to_json_obj(BasicJsonType& j) && {                                      \
    j = BasicJsonType::object();                                               \
    return to_json_obj_move(j, 0, __VA_ARGS__);                                \
  }                                                                            \
  template <typename BasicJsonType, typename M1, typename... Ts>               \
  void to_json_obj(BasicJsonType& j, size_t pos, M1&& m, Ts&&... rest) const { \
    j.push_back({membername_(pos), m});                                        \
    to_json_obj(j, pos + 1, rest...);                                          \
  }                                                                            \
  template <typename BasicJsonType, typename M1, typename... Ts>               \
  void to_json_obj_move(BasicJsonType& j, size_t pos, M1&& m, Ts&&... rest) {  \
    j.push_back({membername_(pos), std::move(m)});                             \
    to_json_obj_move(j, pos + 1, rest...);                                     \
  }                                                                            \
  template <typename BasicJsonType>                                            \
  void to_json_obj(BasicJsonType& j, size_t pos) const {}                      \
  template <typename BasicJsonType>                                            \
  void to_json_obj_move(BasicJsonType& j, size_t pos) const {}

#define TO_JSON_ARRAY(...)                                               \
  template <typename BasicJsonType>                                      \
  BasicJsonType to_json_array() const {                                  \
    auto j = BasicJsonType::array();                                     \
    to_json_array(j, 0, __VA_ARGS__);                                    \
    return j;                                                            \
  }                                                                      \
  template <typename BasicJsonType>                                      \
  void to_json_array(BasicJsonType& j) const& {                          \
    j = BasicJsonType::array();                                          \
    to_json_array(j, 0, __VA_ARGS__);                                    \
    return;                                                              \
  }                                                                      \
  template <typename BasicJsonType>                                      \
  void to_json_array(BasicJsonType& j) && {                              \
    j = BasicJsonType::array();                                          \
    to_json_array_move(j, 0, __VA_ARGS__);                               \
    return;                                                              \
  }                                                                      \
  template <typename BasicJsonType, typename M1, typename... Ts>         \
  void to_json_array(BasicJsonType& j, size_t pos, M1&& m, Ts&&... rest) \
      const& {                                                           \
    j.push_back(m);                                                      \
    to_json_array(j, pos + 1, std::forward<Ts>(rest)...);                \
  }                                                                      \
  template <typename BasicJsonType, typename M1, typename... Ts>         \
  void to_json_array_move(BasicJsonType& j, size_t pos, M1&& m,          \
                          Ts&&... rest) {                                \
    j.push_back(std::move(m));                                           \
    to_json_array_move(j, pos + 1, std::forward<Ts>(rest)...);           \
  }                                                                      \
  template <typename BasicJsonType>                                      \
  void to_json_array(BasicJsonType& j, size_t pos) const {}              \
  template <typename BasicJsonType>                                      \
  void to_json_array_move(BasicJsonType& j, size_t pos) const {}

// meta
#define DEFINE_HAS_MEMBER(FUN)                                           \
  template <typename T>                                                  \
  struct has_##FUN {                                                     \
  private:                                                               \
    template <typename U>                                                \
    static auto check(U    x) -> decltype(x.FUN(), std::true_type{});    \
    static std::false_type check(...);                                   \
                                                                         \
  public:                                                                \
    static bool const value = decltype(check(std::declval<T>()))::value; \
  };

#define DEFINE_HAS_TEMPLATE_MEMBER(FUN)                                        \
  template <typename T, typename V>                                            \
  struct has_##FUN {                                                           \
  private:                                                                     \
    template <typename U>                                                      \
    static auto check(U x) -> decltype(x.template FUN<V>(), std::true_type{}); \
    static std::false_type check(...);                                         \
                                                                               \
  public:                                                                      \
    static bool const value = decltype(check(std::declval<T>()))::value;       \
  };

namespace yos {
// helper class for detecting some serialize methids
DEFINE_HAS_TEMPLATE_MEMBER(to_json_array);
DEFINE_HAS_TEMPLATE_MEMBER(to_json_obj);

template <typename, typename SFINAE = void>
struct array_adl_serializer;

// ------------------------------
// array_json : json type to use to_json_array() only
/*
  If T does not have T::to_json_array(), forward to_json call to
  nlohmann::to_json() even if T has T::to_json_obj().
*/
using array_json =
    nlohmann::basic_json<std::map, std::vector, std::string, bool, std::int64_t,
                         std::uint64_t, double, std::allocator,
                         array_adl_serializer>;

template <typename T>
struct array_adl_serializer<T, typename std::enable_if<!yos::has_to_json_array<
                                   T, array_json>::value>::type> {
  template <typename BasicJsonType, typename ValueType>
  static void from_json(BasicJsonType&& j, ValueType& t) {
    ::nlohmann::from_json(std::forward<BasicJsonType>(j), t);
  }
  template <typename BasicJsonType, typename ValueType>
  static void to_json(BasicJsonType& j, ValueType&& t) {
    ::nlohmann::to_json(j, std::forward<ValueType>(t));
  }
};
template <typename T>
struct array_adl_serializer<T, typename std::enable_if<yos::has_to_json_array<
                                   T, array_json>::value>::type> {
  template <typename BasicJsonType, typename ValueType>
  static void from_json(BasicJsonType&& j, ValueType& t) {
    t.from_json(std::forward<BasicJsonType>(j));
  }
  template <typename BasicJsonType, typename ValueType>
  static void to_json(BasicJsonType& j, ValueType&& t) {
    // forward rvalue
    std::forward<ValueType>(t).to_json_array(j);
  }
};

// ------------------------------
// map_json : json type to use to_json_obj() only
/*
  If T does not have T::to_json_obj(), forward to_json call to
  nlohmann::to_json() even if T has T::to_json_array().
*/
template <typename, typename SFINAE = void>
struct map_adl_serializer;
using map_json = nlohmann::basic_json<std::map, std::vector, std::string, bool,
                                      std::int64_t, std::uint64_t, double,
                                      std::allocator, map_adl_serializer>;

template <typename T>
struct map_adl_serializer<T, typename std::enable_if<!yos::has_to_json_array<
                                 T, map_json>::value>::type> {
  template <typename BasicJsonType, typename ValueType>
  static void from_json(BasicJsonType&& j, ValueType& t) {
    nlohmann::from_json(std::forward<BasicJsonType>(j), t);
  }

  template <typename BasicJsonType, typename ValueType>
  static void to_json(BasicJsonType& j, ValueType&& t) {
    nlohmann::to_json(j, std::forward<ValueType>(t));
  }
};
template <typename T>
struct map_adl_serializer<
    T,
    typename std::enable_if<yos::has_to_json_obj<T, map_json>::value>::type> {
  template <typename BasicJsonType, typename ValueType>
  static void from_json(BasicJsonType&& j, ValueType& t) {
    t.from_json(std::forward<BasicJsonType>(j));
  }
  template <typename BasicJsonType, typename ValueType>
  static void to_json(BasicJsonType& j, ValueType&& t) {
    std::forward<ValueType>(t).to_json_obj(j);
  }
};
}

// ------------------------------
//  Interface to nlohmann::json.
/*
  Adding some specailzation for T which has T::to_json_obj or T::to_json_array.
  If both methods are available, use T::to_json_obj() over T::to_json_array().
 */
namespace nlohmann {
template <typename T>
struct adl_serializer<
    T, typename std::enable_if<yos::has_to_json_array<T, json>::value &&
                               !yos::has_to_json_obj<T, json>::value>::type> {
  template <typename BasicJsonType, typename ValueType>
  static void from_json(BasicJsonType&& j, ValueType& t) {
    t.from_json(std::forward<BasicJsonType>(j));
  }
  template <typename BasicJsonType, typename ValueType>
  static void to_json(BasicJsonType& j, ValueType&& t) {
    std::forward<ValueType>(t).to_json_array(j);
  }
};
template <typename T>
struct adl_serializer<
    T, typename std::enable_if<!yos::has_to_json_array<T, json>::value &&
                               yos::has_to_json_obj<T, json>::value>::type> {
  template <typename BasicJsonType, typename ValueType>
  static void from_json(BasicJsonType&& j, ValueType& t) {
    t.from_json(std::forward<BasicJsonType>(j));
  }
  template <typename BasicJsonType, typename ValueType>
  static void to_json(BasicJsonType& j, ValueType&& t) {
    std::forward<ValueType>(t).to_json_obj(j);
  }
};
template <typename T>
struct adl_serializer<
    T, typename std::enable_if<yos::has_to_json_array<T, json>::value &&
                               yos::has_to_json_obj<T, json>::value>::type> {
  template <typename BasicJsonType, typename ValueType>
  static void from_json(BasicJsonType&& j, ValueType& t) {
    t.from_json(std::forward<BasicJsonType>(j));
  }
  template <typename BasicJsonType, typename ValueType>
  static void to_json(BasicJsonType& j, ValueType&& t) {
    std::forward<ValueType>(t).to_json_obj(j);
  }
};
}
