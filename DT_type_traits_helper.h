#ifndef DT_TYPE_TRAITS_HELPER_H
#define DT_TYPE_TRAITS_HELPER_H

namespace dtlib {
    // integral_constant

    template<class T, T v>
    struct integral_constant
    {
        static constexpr T value = v;
        using value_type = T;
        using type = integral_constant;
        constexpr operator value_type() const noexcept { return value; }
        constexpr value_type operator()() const noexcept { return value; }
    };

    using true_type = integral_constant<bool, true>;
    using false_type = integral_constant<bool, false>;

    template <class _Ty, class = void>
    struct _Add_reference { // add reference (non-referenceable type)
        using _Lvalue = _Ty;
        using _Rvalue = _Ty;
    };

    template <class... _Types>
    using void_t = void;

    template <class _Ty>
    struct _Add_reference<_Ty, void_t<_Ty&>> { // (referenceable type)
        using _Lvalue = _Ty&;
        using _Rvalue = _Ty&&;
    };

    template <class _Ty>
    struct add_lvalue_reference {
        using type = typename _Add_reference<_Ty>::_Lvalue;
    };

    template <class _Ty>
    using add_lvalue_reference_t = typename _Add_reference<_Ty>::_Lvalue;

    template <class _Ty>
    struct add_rvalue_reference {
        using type = typename _Add_reference<_Ty>::_Rvalue;
    };

    template <class _Ty>
    using add_rvalue_reference_t = typename _Add_reference<_Ty>::_Rvalue;

    template <class _Ty>
    add_rvalue_reference_t<_Ty> declval() noexcept {
    }

    //Enable if
    template <bool, typename T = void>
    struct enable_if;
    template <typename T>
    struct enable_if <true, T> { using type = T; };

    template<bool, class T, class F>
    struct conditional { using type = T; };
    template<class T, class F>
    struct conditional<false, T, F> { using type = F; };

    template <typename T, typename U>
    struct is_same : false_type {};
    template <typename T>
    struct is_same <T, T> : true_type {};


    template <typename T>
    struct is_integral : false_type {};
    template <> struct is_integral <bool> : true_type {};
    template <> struct is_integral <char> : true_type {};
    template <> struct is_integral <signed char> : true_type {};
    template <> struct is_integral <unsigned char> : true_type {};
    template <> struct is_integral <char16_t> : true_type {};
    template <> struct is_integral <char32_t> : true_type {};
    template <> struct is_integral <wchar_t> : true_type {};
    template <> struct is_integral <short> : true_type {};
    template <> struct is_integral <unsigned short> : true_type {};
    template <> struct is_integral <int> : true_type {};
    template <> struct is_integral <unsigned> : true_type {};
    template <> struct is_integral <long> : true_type {};
    template <> struct is_integral <unsigned long> : true_type {};
    template <> struct is_integral <long long> : true_type {};
    template <> struct is_integral <unsigned long long> : true_type {};


    template <typename T>
    struct is_floating_point : false_type {};
    template <> struct is_floating_point<float> : true_type {};
    template <> struct is_floating_point<double> : true_type {};
    template <> struct is_floating_point<long double> : true_type {};


    template <typename T>
    struct is_arithmetic
        : conditional<
        is_integral<T>::value || is_floating_point<T>::value,
        true_type,
        false_type
        >::type
    {};


    namespace detail
    {
        template <typename T, bool = is_arithmetic<T>::value>
        struct is_signed : integral_constant<bool, T(-1) < T(0) > {};
        template <typename T>
        struct is_signed<T, false> : false_type {};
    }
    template <typename T>
    struct is_signed : detail::is_signed<T>::type {};


    namespace detail
    {
        template<typename T, bool = is_arithmetic<T>::value>
        struct is_unsigned : integral_constant<bool, T(0) < T(-1) > {};
        template<typename T>
        struct is_unsigned<T, false> : false_type {};
    }
    template<typename T>
    struct is_unsigned : detail::is_unsigned<T>::type {};

    template<class T>
    struct is_array : false_type {};
    template<class T>
    struct is_array<T[]> : true_type {};
    template<class T, size_t N>
    struct is_array<T[N]> : true_type {};


    namespace details
    {
        template <class... Ts>
        struct Tester { using type = void; };
        template <class... Ts>
        using void_t = typename Tester<Ts...>::type;
        template<template<class...>class Z, class, class...Ts>
        struct can_apply : false_type {};
        template<template<class...>class Z, class...Ts>
        struct can_apply<Z, void_t<Z<Ts...>>, Ts...> : true_type {};

        template<class From, class To>
        using try_convert = decltype(To{ declval<From>() });
    }
    template<template<class...>class Z, class...Ts>
    using can_apply = details::can_apply<Z, void, Ts...>;

    template<class From, class To>
    struct is_convertible
        : conditional <
        can_apply <details::try_convert, From, To>::value
        , true_type
        , typename conditional <
        is_arithmetic<From>::value&& is_arithmetic<To>::value,
        true_type,
        false_type
        >::type
        >::type
    {};

    template<>
    struct is_convertible<void, void> : true_type {};
}

#endif // !DT_TYPE_TRAITS_HELPER_H