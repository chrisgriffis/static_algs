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
template<template <typename,typename...> typename, typename, typename...> 
struct basic_table_spec;

template <typename...> 
class basic_record;
template<typename String, typename... field_types>
class basic_record<String,field_types...> : basic_table_spec<basic_record, String, field_types...>
{
    basic_record() = delete;
public:
    using fields_t = std::tuple<field_types...>;

    std::tuple<field_types...> m_fields;


    //from args and rec
template<std::size_t... Is>
    basic_record(std::index_sequence<Is...>,const basic_record& rec, field_types... fields) :
        basic_table_spec<basic_record, String, field_types...>{
rec.m_column_names[Is]...},
        m_fields{ std::forward_as_tuple(fields...) }
    {}
    
template<std::size_t... Is>
    basic_record(std::index_sequence<Is...>,
    const basic_table_spec<basic_record, String, field_types...>& spec, field_types... fields) :
        basic_table_spec<basic_record, String, field_types...>{
spec.m_column_names[Is]...},
        m_fields{ std::forward_as_tuple(fields...) }
    {}
    //from another rec
template<std::size_t... Is>
    basic_record(std::index_sequence<Is...>,const basic_record& rec) :
        basic_table_spec<basic_record, String, field_types...>{ 
        rec.m_column_names[Is]... },
        m_fields( rec.m_fields )
    {}    

    basic_record& operator=(tuple<field_types...>& tup)
    {
        m_fields = tup; return *this;
    }
    basic_record& operator=(basic_record& rec) = delete;
    constexpr std::map<String, std::string> to_lookup()
    {
        return to_lookup(std::index_sequence_for<field_types...>{});
    }
    //from args
    basic_record to_record(field_types... fields) 
    {
        return basic_record(std::index_sequence_for<field_types...>{},
*this, fields...);
    }

    

private:
    template<std::size_t... Is>
    constexpr std::map<String, std::string> to_lookup(std::index_sequence<Is...>)
    {
        return std::map<String, std::string>{
            {
                     basic_table_spec<basic_record, String, field_types...>::m_column_names[Is],
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
template<template <typename,typename...> typename derived, typename String, typename... field_types>
struct basic_table_spec
{
    using fields_t = std::tuple<field_types...>;//typename record_t::fields_t;
    using column_names_t = const std::array<String, sizeof...(field_types)>;

    std::array<String, sizeof...(field_types)> m_column_names;

    template<typename... Strings>
    basic_table_spec(Strings... strs)
        : m_column_names{ std::decay_t<Strings>(strs)... }
    {}
public:
    template<std::size_t... Is>
    basic_table_spec(std::index_sequence<Is...>, const basic_table_spec& t)
        : m_column_names{ t.m_column_names[Is]... }
    {}
    template<std::size_t... Is>
    basic_table_spec(std::index_sequence<Is...>, const derived<String, field_types...>& r)
        : m_column_names{ r.basic_table_spec<derived, String, field_types...>::m_column_names[Is]... }
    {}
    template<typename... Strings>
    static auto create(Strings... strs)
    {
        return basic_table_spec
            <derived, typename L<std::decay_t<Strings>...>::type, field_types...>
            (std::decay_t<Strings>(strs)...);
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
    
    //from args

derived<String,field_types...> 
    to_record(field_types... fields)
{
    return derived<String,field_types...>(
        std::index_sequence_for<field_types...>{},*this,fields...);
}

};
//no args


template< typename... f_types>
using table_spec_cstr = basic_table_spec<basic_record, const char*, f_types...>;
template< typename... f_types>
using table_spec = basic_table_spec<basic_record, std::string, f_types...>;


template< typename... f_types>
std::ostream& operator<<(std::ostream& s, basic_record< f_types...> rec)
{
    auto l = rec.to_lookup();
    for_each(l.begin(), l.end(), [&](auto e) {s << e.second << " "; });
    return s;
}

int main() {

    // auto s = table_spec<int, const char*, const char*>::create(id, name, type);
auto s = table_spec<int, const char*, const char*>::
    create(" id"," name", "type");
cout << s.column_headings_list() << endl;
    auto r = s.to_record(5,"d","7");

cout << r <<  endl
     << r.to_record(9,"fds","(3") << endl
    << s.to_record(2,"","") << endl 
    << "name: " << r.to_lookup()[" name"] << endl << endl << endl ;
}
