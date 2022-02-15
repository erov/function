#pragma once
#include "function-impl.h"

namespace function_impl {

  template <typename T>
  struct object_traits<T, std::enable_if_t<fits_small_storage<T>>> {

    template <typename R, typename... Args>
    static type_descriptor<R, Args...> const* get_obj_descriptor() {

      using storage = storage<R, Args...>;
      static constexpr type_descriptor<R, Args...> descriptor = {
        /* copy */ [](storage* dst, storage const* src) {
          dst->descriptor->destroy(dst);
          new (&dst->data) T(*src->template get<T>());
          dst->descriptor = src->descriptor;
        },
        /* move */ [](storage* dst, storage* src) {
          dst->descriptor->destroy(dst);
          new (&dst->data) T(std::move(*src->template get<T>()));
          dst->descriptor = src->descriptor;
          src->descriptor = get_empty_type_descriptor<R, Args...>();
        },
        /* apply */ [](storage* dst, Args&&... args) -> R {
          return (*dst->template get<T>())(std::forward<Args>(args)...);
        },
        /* destroy */ [](storage* dst) {
          dst->template destroy<T>();
          dst->descriptor = get_empty_type_descriptor<R, Args...>();
        }
      };
      return &descriptor;
    }

    template <typename R, typename... Args>
    static void init(storage<R, Args...>& storage, T&& func) {
      new (&storage.data) T(std::forward<T>(func));
    }
  };

  template <typename T>
  struct object_traits<T, std::enable_if_t<!fits_small_storage<T>>> {

    template <typename R, typename... Args>
    static type_descriptor<R, Args...> const* get_obj_descriptor() {

      using storage = storage<R, Args...>;

      static constexpr type_descriptor<R, Args...> descriptor = {
        /* copy */ [](storage* dst, storage const* src) {
          dst->descriptor->destroy(dst);
          dst->set(static_cast<void*>(new T(*src->template get<T>())));
          dst->descriptor = src->descriptor;
        },
        /* move */ [](storage* dst, storage* src) {
          dst->descriptor->destroy(dst);
          dst->set(static_cast<void*>(src->template get<T>()));
          dst->descriptor = src->descriptor;
          src->descriptor = get_empty_type_descriptor<R, Args...>();
        },
        /* apply */ [](storage* dst, Args&&... args) -> R {
          return (*dst->template get<T>())(std::forward<Args>(args)...);
        },
        /* destroy */ [](storage* dst) {
          dst->template destroy<T>();
          dst->descriptor = get_empty_type_descriptor<R, Args...>();
        }
      };
      return &descriptor;
    }

    template <typename R, typename... Args>
    static void init(storage<R, Args...>& storage, T&& func) {
      storage.set(static_cast<void*>(new T(std::forward<T>(func))));
    }
  };
}
