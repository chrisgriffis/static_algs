#include <iostream>
#include <map>
#include <sstream>
#include <array>
#include <memory>
#include <algorithm>
#include <type_traits>
using namespace std;

//template<typename String, template <typename,template <typename...> typename,typename...> typename tspec, typename... field_types>
//template<typename String, template <typename...> typename tspec, typename... field_types>
template<typename, typename...> 
struct basic_table_spec;

template<typename String, template <typename...> typename basic_table_spec, typename... field_types>
class basic_record : basic_table_spec<String, field_types...>
{
    basic_record() = delete;
public:
    using fields_t = std::tuple<field_types...>;

    std::tuple<field_types...> m_fields;

    //with args
    template<typename String2, typename... field_types_ctor>
    basic_record(
        basic_table_spec<String2, basic_record, field_types_ctor...> schema, 
        field_types_ctor... fields) 
        :   basic_table_spec<String, field_types_ctor...>(schema),
            m_fields{ std::make_tuple(fields...) }
    {}
    //with tup
    template<typename String2, typename... field_types_ctor>
    basic_record(
        basic_table_spec<String2, basic_record,field_types_ctor...> schema, 
        std::tuple<field_types_ctor...> tup)
        :   basic_table_spec<String2, field_types_ctor...>(schema),
            m_fields{ tup }
    {}
    //no args
    template<typename String2, typename... field_types_ctor>
    basic_record(
        basic_table_spec<String2, basic_record, field_types_ctor...> schema)
        :   basic_table_spec<String2, field_types_ctor...>(schema),
            m_fields{  }
    {}
    //from another rec
    basic_record(const basic_record& rec) :
        basic_table_spec<String, basic_record, field_types...>{ rec.m_column_headings },
        basic_table_spec<String, basic_record, field_types...>::m_fields{ rec.m_fields }
    {}

    basic_record& operator=(tuple<field_types...>& tup)
    {
        m_fields = tup; return *this;
    }
    basic_record& operator=(basic_record& rec) = delete;
    constexpr std::map<String, String> to_lookup()
    {
        return to_lookup(std::index_sequence_for<field_types...>{});
    }
    //from args
    basic_record to_record(field_types... fields) 
    {
        return basic_record(*this, fields...);
    }

    //from tup
    basic_record to_record(std::tuple<field_types...> tup) 
    {
        return basic_record(*this, tup);
    }

private:
    template<std::size_t... Is>
    constexpr std::map<const char*, std::string> to_lookup(std::index_sequence<Is...>)
    {
        return std::map<const char*, std::string>{
            {
                basic_table_spec<String, field_types...>::m_column_names[Is],
                    [this] { stringstream ss; ss << std::get<Is>(m_fields); return ss.str(); }()
            }... };
    }
};


template <typename...> struct L;
template <typename T>
struct L<T>
{
    using type = T;//std::enable_if_t<(std::is_same_v<T,Ts> && ...),T>;
};
template <typename T, typename... Ts>
struct L<T, Ts...>
{
    using type = T;//std::enable_if_t<(std::is_same_v<T,Ts> && ...),T>;
};
//template<typename String, template <typename...> typename record_t, typename... field_types>
//template<typename String, template <typename,template <typename...> typename,typename...> typename record_t, typename... field_types>
template<typename String, typename... field_types>
struct basic_table_spec
{
    using fields_t = std::tuple<field_types...>;//typename record_t::fields_t;
    using column_names_t = const std::array<String, sizeof...(field_types)>;

    std::array<String, sizeof...(field_types)> m_column_names;

    template<template <typename...> typename L, typename... Strings>
    basic_table_spec(L<Strings...>, Strings... strs)
        : m_column_names{ strs... }
    {}
public:
    basic_table_spec(const basic_table_spec& t)
        : m_column_names{ t.m_column_names }
    {}
    template<typename... Strings>
    static auto create(Strings... strs)
    {
        return basic_table_spec(L<Strings...>{}, strs...);
    }

    template<typename C>
    constexpr std::string to_list(C c, std::string separator = ",")
    {
        static const std::size_t N = sizeof...(field_types);
        std::stringstream s;
        for (unsigned int n = 0; n < N; ++n) { s << c[n]; if (N != n + 1) { s << separator; } }
        return s.str();
    }

    constexpr std::string column_headings_list()
    {
        return to_list(m_column_names);
    }
    constexpr std::string placeholder_list(char placeholder = '?')
    {
        return to_list(std::vector<char>(sizeof...(field_types), placeholder));
    }
};
//from args
template<typename String, template <typename...> typename basic_table_spec, typename... field_types>
basic_record<String, basic_table_spec, field_types...> to_record(
    basic_table_spec<String, field_types...> ts,
    field_types... fields)
{
    return basic_record<String, basic_table_spec, field_types...>(ts, fields...);
}

//no args
template<typename String, template <typename...> typename basic_table_spec, typename... field_types>
decltype(auto) to_record(
    basic_table_spec<String, field_types...> ts)
{
    return basic_record<String, basic_table_spec, field_types...>(ts);
}

//from tup
template<typename String, template <typename...> typename basic_table_spec, typename... field_types>
decltype(auto) to_record(
    basic_table_spec<String, field_types...> ts, 
    std::tuple<field_types...> tup)
{
    return basic_record<String, basic_table_spec, field_types...>(ts, tup);
}

template<typename... field_types>
using record_cstr = basic_record<const char*, basic_table_spec, field_types...>;
template<typename... field_types>
using record = basic_record<std::string, basic_table_spec, field_types...>;
template<typename... field_types>
using table_spec_cstr = basic_table_spec<const char*, field_types...>;
template<typename... field_types>
using table_spec = basic_table_spec<std::string, field_types...>;


template<typename String, template <typename...> typename tspec, typename... field_types>
std::ostream& operator<<(std::ostream& s, basic_record<String, tspec, field_types...>  rec)
{
    auto l = rec.to_lookup();
    for_each(l.begin(), l.end(), [&](auto e) {s << e.second; });
    return s;
}

int main() {
    //auto t1 = make_tuple(9, "qwer", "dtyj");
    const char* id = "id";
    const char* name = "name";
    const char* type = "type";
    //auto s = table_spec<int, const char*, const char*>::create(id, name, type);
    auto s = table_spec<int, const char*, const char*>::create("id", "name", "type");
    cout << s.column_headings_list() << endl;
    auto r = to_record(s, 9, "qwer", "dtyj");
    cout << r << endl;
    return 0;
}