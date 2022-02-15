#pragma once
#include "object-traits.h"
#include "storage.h"
#include "type-descriptor.h"

template <typename F>
struct function;

template <typename R, typename... Args>
struct function<R (Args...)> {
  function() noexcept {
    storage.descriptor = function_impl::get_empty_type_descriptor<R, Args...>();
  }

  function(function const& other) : function() {
    other.storage.descriptor->copy(&storage, &other.storage);
  }

  function(function&& other) noexcept : function() {
    other.storage.descriptor->move(&storage, &other.storage);
  }

  template <typename T>
  function(T val) {
    using traits = function_impl::object_traits<T>;
    traits::init(storage, std::move(val));
    storage.descriptor = traits::template get_obj_descriptor<R, Args...>();
  }

  function& operator=(function const& rhs) {
    if (this != &rhs) {
      function(rhs).swap(*this);
    }
    return *this;
  }

  function& operator=(function&& rhs) noexcept {
    if (this != &rhs) {
      function(std::move(rhs)).swap(*this);
    }
    return *this;
  }

  ~function() {
    storage.descriptor->destroy(&storage);
  }

  explicit operator bool() const noexcept {
    return storage.contains_target();
  }

  R operator()(Args&&... args) const {
    return storage.descriptor->apply(&storage, std::forward<Args>(args)...);
  }

  template <typename T>
  T* target() noexcept {
    return storage.template get_target<T>();
  }

  template <typename T>
  T const* target() const noexcept {
    return storage.template get_target<T>();
  }

private:
  void swap(function& other) noexcept {
    std::swap(storage, other.storage);
  }

  mutable function_impl::storage<R, Args...> storage;
};
