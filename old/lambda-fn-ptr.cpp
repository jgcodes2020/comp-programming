
namespace meta {
  namespace detail {
    template <class T>
    struct extract_fn_ptr;

    template <class R, class C, class... Args>
    struct extract_fn_ptr<R (C::*)(Args...)> {
      using type = R (*)(Args...);
    };
    template <class R, class C, class... Args>
    struct extract_fn_ptr<R (C::*)(Args...) const> {
      using type = R (*)(Args...);
    };

    template <class T>
    requires(requires { &T::operator(); }) struct extract_lambda_fn {
      using type = typename extract_fn_ptr<decltype(&T::operator())>::type;
    };
  }  // namespace detail
  template <class T>
  concept lambda = requires {
    &T::operator();
    &T::operator typename detail::extract_fn_ptr<
      decltype(&T::operator())>::type;
  };
  template <class T>
  using lambda_fnptr = typename detail::extract_lambda_fn<T>::type;

  using gcallback = void (*)();

  template <lambda T>
  gcallback as_gcallback(T x) {
    return reinterpret_cast<void (*)()>(
      static_cast<lambda_fnptr<T>>(x)
    );
  }
}  // namespace meta

int main() {
  auto l = [](int x) {
    return x + 1;
  };

  static_assert(meta::lambda<decltype(l)>, "message");
}