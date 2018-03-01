#include <iostream>
#include <nlohmann/json.hpp>
#include <type_traits>

namespace ns {
struct foo {
  int x;
};

template <typename, typename SFINAE = void>
struct foo_serializer;

template <typename T>
struct foo_serializer<
    T, typename std::enable_if<std::is_same<foo, T>::value>::type> {
  template <typename BasicJsonType, typename VT>
  static void to_json(BasicJsonType& j, VT&& value) {
    j = BasicJsonType{{"x", value.x}};
  }
  template <typename BasicJsonType, typename VT>
  static void from_json(BasicJsonType&& j, VT& value) {
    nlohmann::from_json(j.at("x"), value.x);
  }
};

template <typename T>
struct foo_serializer<
    T, typename std::enable_if<!std::is_same<foo, T>::value>::type> {
  template <typename BasicJsonType, typename VT>
  static void to_json(BasicJsonType& j, VT&& value) {
    ::nlohmann::to_json(j, std::forward<VT>(value));
  }
  template <typename BasicJsonType, typename VT>
  static void from_json(BasicJsonType&& j, VT& value) {
    ::nlohmann::from_json(std::forward<BasicJsonType>(j), value);
  }
};
}

using foo_json = nlohmann::basic_json<std::map, std::vector, std::string, bool,
                                      std::int64_t, std::uint64_t, double,
                                      std::allocator, ns::foo_serializer>;

int main() {
  foo_json lj = ns::foo{3};
  ns::foo  ff = lj;
  std::cout << lj << std::endl;
  std::cout << ff.x << std::endl;
  nlohmann::json nj = lj;// may crash (https://github.com/nlohmann/json/issues/977)
  std::cout << nj.dump(3) << std::endl;
}
