//https://wandbox.org/permlink/uHjIO3kAUCEsrcVq
#include <iostream>
#include <map>
#include <sstream>
#include <array>
#include <memory>
#include <algorithm>
#include <type_traits>
#include "meta.h"

using namespace std;

template<template <template <typename> typename, typename, typename...> typename, template <typename> typename, typename, typename...>
class basic_row_schema;

template<template <typename> typename Convert, typename String, typename... field_types>
class single_record : public basic_row_schema<single_record, Convert, String, field_types...>
{
    template<template <template <typename> typename, typename, typename...> typename a, template <typename> typename b, typename c, typename... d>
    friend class basic_row_schema;

    single_record() = delete;
    single_record& operator=(single_record& rec) = delete;
    std::tuple<field_types...> m_fields;

    //from args and rec
    template<std::size_t... Is>
    single_record(std::index_sequence<Is...>, const single_record& rec, field_types... fields) :
        basic_row_schema<single_record, Convert, String, field_types...>{ rec.m_column_names[Is]... },
        m_fields{ std::forward_as_tuple(fields...) }
    {}

    template<std::size_t... Is>
    single_record(std::index_sequence<Is...>,
        const basic_row_schema<single_record, Convert, String, field_types...>& spec, field_types... fields) :
        basic_row_schema<single_record, Convert, String, field_types...>{ spec.m_column_names[Is]... },
        m_fields{ std::forward_as_tuple(fields...) }
    {}
    template<std::size_t... Is>
    single_record(std::index_sequence<Is...>,
        const basic_row_schema<single_record, Convert, String, field_types...>& spec) :
        basic_row_schema<single_record, Convert, String, field_types...>{ spec.m_column_names[Is]... },
        m_fields{ }
    {}
    //from another rec
    template<std::size_t... Is>
    single_record(std::index_sequence<Is...>, const single_record& rec) :
        basic_row_schema<single_record, Convert, String, field_types...>{ rec.m_column_names[Is]... },
        m_fields(rec.m_fields)
    {}
    template<std::size_t... Is>
    constexpr std::string values_as_list(std::index_sequence<Is...>, std::string separator)
    {
        std::stringstream s;
        ([&] {s << std::get<Is>(m_fields); if (sizeof...(Is) != Is + 1) { s << separator; }}(), ...);
        return s.str();
    }
    template<std::size_t... Is>
    constexpr std::tuple<field_types...> converted_row_values(std::index_sequence<Is...>)
    {
        return std::make_tuple(
            Convert<std::remove_reference_t<decltype(std::get<Is>(m_fields))>>{}
            .convert(std::get<Is>(m_fields))...);
    }
public:
    using row_value_data_t = std::tuple<field_types...>;
    using basic_row_schema_t = basic_row_schema<single_record, Convert, String, field_types...>;
    using string_t = String;
    single_record& operator=(tuple<field_types...>& tup)
    {
        m_fields = tup; return *this;
    }
    single_record& operator=(tuple<field_types...>&& tup)
    {
        m_fields = tup; return *this;
    }

    constexpr std::tuple<field_types...>& row_values()
    {
        return m_fields;
    }
    constexpr std::tuple<field_types...> converted_row_values()
    {
        return converted_row_values(std::index_sequence_for<field_types...>{});
    }
    constexpr std::map<String, std::string> to_dictionary()
    {
        return to_dictionary(std::index_sequence_for<field_types...>{});
    }

    constexpr std::string values_as_list(std::string separator = ",")
    {
        return values_as_list(std::index_sequence_for<field_types...>{}, separator);
    }
    //from args
    single_record to_single_record(field_types... fields)
    {
        return single_record(std::index_sequence_for<field_types...>{}, * this, fields...);
    }
    single_record to_single_record()
    {
        return single_record(std::index_sequence_for<field_types...>{}, * this);
    }

private:
    template<std::size_t... Is>
    constexpr std::map<String, std::string> to_dictionary(std::index_sequence<Is...>)
    {
        return std::map<String, std::string>{
            {
                basic_row_schema<single_record, Convert, String, field_types...>::m_column_names[Is],
                    [this] { stringstream ss; ss << std::get<Is>(m_fields); return ss.str(); }()
            }... };
    }
};


template<template <template <typename> typename, typename, typename...> typename derived, template <typename> typename Convert, typename String, typename... field_types>
class basic_row_schema
{
    template<template <template <typename> typename, typename, typename...> typename a, template <typename> typename b, typename c, typename... d>
    friend class basic_row_schema;
    friend class single_record<Convert, String, field_types...>;
    
    const std::array<String, sizeof...(field_types)> m_column_names;

    template<typename... Strings>
    basic_row_schema(Strings... strs)
        : m_column_names{ std::decay_t<Strings>(strs)... }
    {}

    template<std::size_t... Is>
    basic_row_schema(std::index_sequence<Is...>, const basic_row_schema& t)
        : m_column_names{ t.m_column_names[Is]... }
    {}
    template<std::size_t... Is>
    basic_row_schema(std::index_sequence<Is...>, const derived<Convert, String, field_types...>& r)
        : m_column_names{ r.basic_row_schema<derived, Convert, String, field_types...>::m_column_names[Is]... }
    {}
    template<typename C, std::size_t... Is>
    constexpr std::string to_list(std::index_sequence<Is...>, C c, std::string separator = ",")
    {
        std::stringstream s;
        ([&](auto n) {s << c[n]; if (sizeof...(Is) != n + 1) { s << separator; }}(Is), ...);
        return s.str();
    }

public:
    using row_value_data_t = std::tuple<field_types...>;//typename record_t::row_value_data_t;
    using column_names_t = const std::array<String, sizeof...(field_types)>;
    using string_t = String;
    template<typename... Strings>
    static auto create_using_headings(Strings... strs)
    {
        return basic_row_schema
            <derived, Convert, std::common_type_t<std::decay_t<Strings>...>, field_types...>
            (std::decay_t<Strings>(strs)...);
    }

    constexpr std::string headings_as_list()
    {
        return to_list(std::index_sequence_for<field_types...>{}, m_column_names);
    }


    //from args

    derived<Convert, String, field_types...>
        to_single_record(field_types... fields)
    {
        return derived<Convert, String, field_types...>(
            std::index_sequence_for<field_types...>{}, *this, fields...);
    }
    derived<Convert, String, field_types...>
        to_single_record()
    {
        return derived<Convert, String, field_types...>(
            std::index_sequence_for<field_types...>{}, *this);
    }
};


template<template <typename> typename Convert2, typename... f_types>
using row_schema_cstr = basic_row_schema<single_record, Convert2, const char*, f_types...>;
template<template <typename> typename Convert2,  typename... f_types>
using row_schema = basic_row_schema<single_record, Convert2, std::string, f_types...>;

template< template <typename> typename Convert2, typename String2, typename... f_types>
std::ostream& operator<<(std::ostream& s, single_record< Convert2, String2, f_types...> rec)
{
    s << rec.headings_as_list() << std::endl;
    s << rec.values_as_list() << std::endl;
    return s;
}
template< template <typename> typename Convert2, typename String2, typename... f_types>
std::ostream& operator<<(std::ostream& s, basic_row_schema< single_record, Convert2, String2, f_types...> spec)
{
    s << spec.headings_as_list() << std::endl;
    return s;
}
template< typename... f_types>
std::ostream& operator<<(std::ostream& s, std::tuple< f_types...> tup)
{
        std::apply([&s](auto... args) { ((s << args << " "), ...); },tup);
        return s;
}
struct unknown_t {};
std::ostream& operator<<(std::ostream& s, unknown_t) { s << "<unknown-type>"; return s; }
