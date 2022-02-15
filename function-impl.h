#pragma once
#include <cstddef>
#include <cstdlib>
#include <type_traits>
#include <utility>

struct bad_function_call : std::runtime_error {
  explicit bad_function_call(const std::string& arg)
      : std::runtime_error(arg) {}

  explicit bad_function_call(const char* arg)
      : std::runtime_error(arg) {}
};

namespace function_impl {
  template <typename T>
  static constexpr bool fits_small_storage = (sizeof(T) <= sizeof(void*) &&
                                              alignof(void*) % alignof(T) == 0 &&
                                              std::is_nothrow_move_constructible_v<T>);

  template <typename T, typename = void>
  struct object_traits;

  template <typename R, typename... Args>
  struct type_descriptor;

  template <typename R, typename... Args>
  static type_descriptor<R, Args...> const* get_empty_type_descriptor();

  template <typename R, typename... Args>
  struct storage;
}
