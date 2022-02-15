#pragma once
#include "function-impl.h"

namespace function_impl {
  template <typename R, typename... Args>
  struct storage {
    template <typename T>
    T* get() {
      if constexpr (fits_small_storage<T>) {
        return reinterpret_cast<T*>(&data);
      } else {
        return *reinterpret_cast<T**>(&data);
      }
    }

    template <typename T>
    T const* get() const {
      if constexpr (fits_small_storage<T>) {
        return reinterpret_cast<T const*>(&data);
      } else {
        return *reinterpret_cast<T* const*>(&data);
      }
    }

    void set(void* ptr) {
      new (&data) (void*)(ptr);
    }

    template <typename T>
    void destroy() {
      if constexpr (fits_small_storage<T>) {
        get<T>()->~T();
      } else {
        delete get<T>();
      }
    }

    bool contains_target() const noexcept {
      return descriptor != get_empty_type_descriptor<R, Args...>();
    }

    template <typename Q>
    Q* get_target() {
      if (!contains_target() || descriptor != object_traits<Q>::template get_obj_descriptor<R, Args...>()) {
        return nullptr;
      }
      return get<Q>();
    }

    type_descriptor<R, Args...> const* descriptor;
    std::aligned_storage_t<sizeof(void*), alignof(void*)> data;
  };
}
