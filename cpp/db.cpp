#include <iostream>
#include <map>
#include <sstream>
#include <array>
#include <memory>
#include <algorithm>
#include <type_traits>
using namespace std;

template<typename... field_types>
struct table_spec;


template<typename String, template <typename...> typename table_spec, typename... field_types>
struct Record
    : public table_spec<String, field_types...>
{
    using table_spec_t = table_spec<String, field_types...>;

    table_spec_t::fields_t m_fields;

    Record() = delete;

//protected:
    //with args
    template<typename String2, typename... field_types_ctor>
        Record(table_spec<String2, field_types_ctor...> schema, field_types_ctor... fields) :
        table_spec_t{ schema },
        m_fields{ std::make_tuple(fields...) }
    {}
    //with tup
    template<typename String2, typename... field_types_ctor>
        Record(table_spec<String2, field_types_ctor...> schema, std::tuple<field_types_ctor...> tup) :
        table_spec<String2, field_types_ctor...>( schema ),
        m_fields{ tup }
    {}
    //no args
    template<typename String2, typename... field_types_ctor>
        Record(table_spec<String2, field_types_ctor...> schema) :
        table_spec<String2, field_types_ctor...>{ schema },
        m_fields{  }
    {}
    //another rec
        Record(const Record& rec) :
        table_spec<String, field_types...>{ rec.m_column_headings },
        table_spec<String, field_types...>::m_fields{ rec.m_fields }
    {}
public:
    //with args
    static Record create(field_types... fields)
    { return Record(std::forward<field_types>(fields)...);}
    //with tup
    /*template<typename... field_types_ctor,
        typename = std::enable_if_t<sizeof...(field_types) == sizeof...(field_types_ctor)>>
        create(table_spec_t schema, std::tuple<field_types_ctor...> tup) :
        table_spec<field_types_ctor...>{ schema },
        m_fields{ tup }
    {}
    //no args
    template<typename... field_types_ctor,
        typename = std::enable_if_t<sizeof...(field_types) == sizeof...(field_types_ctor)>>
        create(table_spec_t schema) :
        table_spec<field_types_ctor...>{ schema },
        m_fields{  }
    {}
    //another rec
    template<typename... field_types_ctor,
        typename = std::enable_if_t<sizeof...(field_types) == sizeof...(field_types_ctor)>>
        create(const Record< table_spec<String, field_types...>, field_types_ctor...>& rec) :
        table_spec<String, field_types...>{ rec.m_column_headings },
        m_fields{ rec.m_fields }
    {}*/

    Record& operator=(tuple<field_types...>& tup)
    {
        m_fields = tup; return *this;
    }
    Record& operator=(Record& rec) = delete;
    constexpr std::map<const char*, std::string> to_lookup()
    {
        return to_lookup(std::index_sequence_for<field_types...>{});
    }
    //from args
    Record to_record(field_types... fields) override
    {
        return Record(*this, fields...);
    }



    //from tup
    Record to_record(std::tuple<field_types...> tup) override
    {
        return Record(*this, tup);
    }

private:
    template<std::size_t... Is>
    constexpr std::map<const char*, std::string> to_lookup(std::index_sequence<Is...>)
    {
        return std::map<const char*, std::string>{
            {
                table_spec_t::m_column_names[Is],
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
struct L<T,Ts...>
{
    using type = T;//std::enable_if_t<(std::is_same_v<T,Ts> && ...),T>;
};
template< typename String, typename... field_types>
struct table_spec<String, field_types...>
{
    using record_t = Record<String, table_spec, field_types...>;
    using fields_t = std::tuple<field_types...>;
    using column_names_t = const std::array<String, sizeof...(field_types)>;

    std::array<String, sizeof...(field_types)> m_column_names;

    template<template <typename...> typename L, typename... Strings>
        table_spec( L<Strings...>, Strings... strs )
        : m_column_names{ strs... }
    {}
    table_spec( const table_spec& t )
        : m_column_names{ t.m_column_names }
    {}
    template<typename... Strings>
    static auto create(Strings... strs)
    { return table_spec<typename L<Strings...>::type, field_types...>(L<Strings...>{}, strs...); }

    template<typename C>
    constexpr std::string to_list(C c, std::string separator = ",")
    {
        static const std::size_t N = sizeof...(field_types);
        std::stringstream s;
        for (unsigned int n = 0; n < N; ++n) { s << c[n]; if (N != n + 1) { s << separator; } }
        return s.str();
    }
    //from args
    virtual record_t to_record(field_types... fields)
    {
        return record_t(*this, fields...);
    }

    //no args
    virtual record_t to_record()
    {
        return record_t(*this);
    }

    //from tup
    virtual record_t to_record(std::tuple<field_types...> tup)
    {
        return record_t(*this, tup);
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


template<typename... Ts>
std::ostream& operator<<(std::ostream& s, Record<Ts...>  rec)
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
    auto s = table_spec<const char*, int, const char*, const char*>::template create(id, name, type);
    cout << s.column_headings_list();
    return 0;
}