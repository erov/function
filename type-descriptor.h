#pragma once
#include "function-impl.h"

namespace function_impl {
  template <typename R, typename... Args>
  struct type_descriptor {
    using storage = function_impl::storage<R, Args...>;

    void (*copy)(storage*, storage const*);
    void (*move)(storage*, storage*);
    R (*apply)(storage*, Args&&...);
    void (*destroy)(storage*);
  };

  template <typename R, typename... Args>
  static type_descriptor<R, Args...> const* get_empty_type_descriptor() {
    using storage = function_impl::storage<R, Args...>;

    static constexpr type_descriptor<R, Args...> descriptor = {
        /* copy */ [](storage* dst, storage const*) {
          dst->descriptor = get_empty_type_descriptor<R, Args...>();
        },
        /* move */ [](storage* dst, storage*) {
          dst->descriptor = get_empty_type_descriptor<R, Args...>();
        },
        /* apply */ [](storage*, Args&&...) -> R {
          throw bad_function_call("calling an empty function");
        },
        /* destroy */ [](storage*) {}
    };
    return &descriptor;
  }
}
