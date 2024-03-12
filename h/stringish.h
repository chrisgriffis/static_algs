#include <iostream>
#include <string_view>
#include <string>
#include <format>
#include <functional>
#include <sstream>
#include <ranges>
#include <type_traits>

namespace my
{
    using namespace std;
    template<typename T> struct get_char_array_base_type;
    template<typename T, size_t n> struct get_char_array_base_type<T(&)[n]> { using type = std::remove_reference_t<T>; };
    template<typename T> using get_char_array_base_type_t = typename get_char_array_base_type<T>::type;

    template<typename T> concept string_like = requires(T t) { typename T::value_type; { t.c_str() }; };
    template<typename T> concept char_array_like = requires(T) { std::is_same_v< get_char_array_base_type_t<T>(&)[], T>; };
    template<typename T> concept decay_char_array_like = requires(T)
    {
        (std::is_same_v<const std::remove_const_t<std::remove_pointer_t<T>>*, T> ||
            std::is_same_v<std::remove_const_t<std::remove_pointer_t<T>>*, T> ||
            std::is_same_v<const std::remove_const_t<std::remove_pointer_t<T>>*&, T> ||
            std::is_same_v<std::remove_const_t<std::remove_pointer_t<T>>*&, T>);
    };

    template<typename T> struct is_string_like : false_type {};
    template<string_like T> struct is_string_like<T> : true_type {};

    template<typename T> struct is_char_array_like : false_type {};
    template<char_array_like T> struct is_char_array_like<T> : true_type {};

    template<typename T> struct is_decay_char_array_like : false_type {};
    template<decay_char_array_like T> struct is_decay_char_array_like<T> : true_type {};

    template<typename T> struct get_value_type { using type = std::remove_const_t<std::remove_pointer_t<T>>; };
    template<string_like V> struct get_value_type<V> { using type = typename V::value_type; };

    template<char_array_like V> struct get_value_type<V> { using type = typename get_char_array_base_type<V>::type; };
    template<typename T> using get_value_type_t = typename get_value_type<T>::type;


    template<typename T> concept stringish = requires(T) { (is_string_like<T>::value || is_char_array_like<T>::value || is_decay_char_array_like<T>::value); };

    template<typename...> struct get_first_type;
    template<typename X, typename... Ts> struct get_first_type<X, Ts...> { using type = X; };

    template<stringish T, typename>
    struct match
    {
        using TOut = std::conditional_t<is_string_like<T>::value, T, std::decay_t<T>>;
        template<size_t... is>
        static constexpr std::wstring_view wplaceholder(std::index_sequence<is...>)
        {
            std::function<std::wstring(std::wstring, size_t)> a = [a](std::wstring v, size_t i) { return (i == sizeof...(is)) ? v : v + a(v, i + 1); };
            return a(std::wstring(L"{}"), size_t{ 0 });
        }
        template<size_t... is>
        static constexpr std::string_view placeholder(std::index_sequence<is...>)
        {
            std::function<std::string(std::string, size_t)> a = [a](std::string v, size_t i) { return (i == sizeof...(is)) ? v : v + a(v, i + 1); };
            return a(std::string("{}"), size_t{ 0 });
        }

        template<typename... StringTs>
        static constexpr auto join(StringTs... strings)
        {
            using Vt = get_value_type_t<typename get_first_type<StringTs...>::type>;
            if constexpr (std::is_same_v<Vt, wchar_t>)
            {
                auto ret = std::vformat(wplaceholder(std::make_index_sequence<sizeof...(strings)>{}), std::make_wformat_args(strings...));
                return ret;
            }
            else
            {
                auto ret = std::vformat(placeholder(std::make_index_sequence<sizeof...(strings)>{}), std::make_format_args(strings...));
                return ret;
            }
        }

        template<stringish S, typename... StringTs>
        static constexpr auto join(S s, StringTs... strings)
        {

            using Vt = get_value_type_t<S>;
            if constexpr (std::is_same_v<Vt, wchar_t>)
            {
                if constexpr (sizeof...(StringTs) > 0)
                {
                    auto ret = std::vformat(wplaceholder(std::make_index_sequence<2>{}), std::make_wformat_args(s, join(strings...)));
                    return ret;
                }
                else
                {
                    auto ret = std::vformat(wplaceholder(std::make_index_sequence<1>{}), std::make_wformat_args(s));
                    return ret;

                }
            }
            else
            {
                if constexpr (sizeof...(StringTs) > 0)
                {
                    auto ret = std::vformat(placeholder(std::make_index_sequence<2>{}), std::make_format_args(s, join(strings...)));
                    return ret;
                }
                else
                {
                    auto ret = std::vformat(placeholder(std::make_index_sequence<1>{}), std::make_format_args(s));
                    return ret;
                }
            }
        }

        template<stringish S, typename... StringTs>
        static constexpr auto join(StringTs... strings, S s)
        {

            using Vt = get_value_type_t<S>;
            if constexpr (std::is_same_v<Vt, wchar_t>)
            {
                if constexpr (sizeof...(StringTs) > 0)
                {
                    auto ret = std::vformat(wplaceholder(std::make_index_sequence<2>{}), std::make_wformat_args(join(strings...), s));
                    return ret;
                }
                else
                {
                    auto ret = std::vformat(wplaceholder(std::make_index_sequence<1>{}), std::make_wformat_args(s));
                    return ret;

                }
            }
            else
            {
                if constexpr (sizeof...(StringTs) > 0)
                {
                    auto ret = std::vformat(placeholder(std::make_index_sequence<2>{}), std::make_format_args(join(strings...), s));
                    return ret;
                }
                else
                {
                    auto ret = std::vformat(placeholder(std::make_index_sequence<1>{}), std::make_format_args(s));
                    return ret;
                }
            }
        }

        template<char_array_like C, stringish... Afters>
        static constexpr auto append(C str, Afters... after)
        {
            auto ret = join(str, after...);
            return ret.c_str();
        }
        template<string_like C, stringish... Afters>
        static constexpr auto append(C str, Afters... after)
        {
            auto ret = join(str, after...);
            return ret;
        }
        template<decay_char_array_like C, stringish... Afters>
        static constexpr auto append(C str, Afters... after)
        {
            auto ret = join(str, after...);
            return ret.c_str();;
        }
        template<char_array_like C, stringish... Befores>
        static constexpr auto prepend(Befores... bef, C str)
        {
            auto ret = join(bef..., str);
            return ret.c_str();;
        }
        template<string_like C, stringish... Befores>
        static constexpr auto prepend(Befores... bef, C str)
        {
            auto ret = join(bef..., str);
            return ret;
        }
        template<decay_char_array_like C, stringish... Befores>
        static constexpr auto prepend(Befores... bef, C str)
        {
            auto ret = join(bef..., str);
            return ret.c_str();;
        }

        template<stringish S, stringish... Befores, stringish... Afters>
        static constexpr TOut wrap(Befores... bef, S str, Afters... after)
        {
            auto ret = prepend(bef..., append(str, after...));
            return ret;
        }

        template<typename S, std::enable_if_t<is_string_like<S>::value, int> = 0>
        static constexpr TOut bracket(S str)
        {
            auto ret = wrap("[", str, "]");
            return ret;
        }

        template<typename S, size_t n, std::enable_if_t<is_char_array_like<S>::value, int> = 1>
        static constexpr TOut bracket(typename get_char_array_base_type<S>::type(&str)[n])
        {
            auto ret = wrap("[", std::decay_t<S>(str), "]");
            return ret;
        }
        template<typename S, std::enable_if_t<is_decay_char_array_like<S>::value, int> = 2>
        static constexpr TOut bracket(S str)
        {
            auto ret = wrap("[", str, "]");
            return ret;
        }

    };
    template<stringish X>
    struct match<X, std::enable_if_t<is_char_array_like<X>::value, int>> : my::match<std::decay_t<X>, int> {};
}


int stringish_main()
{
    using namespace std;
    using namespace my;

    cout << match<decltype("f"), int>::bracket(L"sdfg");

}
