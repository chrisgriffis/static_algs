#include <iostream>
#include <map>
#include <sstream>
#include <array>
#include <memory>
#include <algorithm>
#include <type_traits>

using namespace std;

template<template <typename, typename...> typename, typename, typename...>
class basic_row_schema;

template<typename String, typename... field_types>
class single_record : public basic_row_schema<single_record, String, field_types...>
{
    template<template <typename, typename...> typename, typename, typename...>
    friend class basic_row_schema;

    single_record() = delete;
    single_record& operator=(single_record& rec) = delete;
    std::tuple<field_types...> m_fields;

    //from args and rec
    template<std::size_t... Is>
    single_record(std::index_sequence<Is...>, const single_record& rec, field_types... fields) :
        basic_row_schema<single_record, String, field_types...>{ rec.m_column_names[Is]... },
        m_fields{ std::forward_as_tuple(fields...) }
    {}

    template<std::size_t... Is>
    single_record(std::index_sequence<Is...>,
        const basic_row_schema<single_record, String, field_types...>& spec, field_types... fields) :
        basic_row_schema<single_record, String, field_types...>{ spec.m_column_names[Is]... },
        m_fields{ std::forward_as_tuple(fields...) }
    {}
    template<std::size_t... Is>
    single_record(std::index_sequence<Is...>,
        const basic_row_schema<single_record, String, field_types...>& spec) :
        basic_row_schema<single_record, String, field_types...>{ spec.m_column_names[Is]... },
        m_fields{ }
    {}
    //from another rec
    template<std::size_t... Is>
    single_record(std::index_sequence<Is...>, const single_record& rec) :
        basic_row_schema<single_record, String, field_types...>{ rec.m_column_names[Is]... },
        m_fields(rec.m_fields)
    {}
    template<std::size_t... Is>
    constexpr std::string values_as_list(std::index_sequence<Is...>, std::string separator = ",")
    {
        std::stringstream s;
        ([&] {s << std::get<Is>(m_fields); if (sizeof...(Is) != Is + 1) { s << separator; }}(), ...);
        return s.str();
    }
public:
    using row_value_data_t = std::tuple<field_types...>;
    using basic_row_schema_t = basic_row_schema<single_record, String, field_types...>;

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
    constexpr std::map<String, std::string> to_dictionary()
    {
        return to_dictionary(std::index_sequence_for<field_types...>{});
    }

    constexpr std::string values_as_list()
    {
        return values_as_list(std::index_sequence_for<field_types...>{});
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
                basic_row_schema<single_record, String, field_types...>::m_column_names[Is],
                    [this] { stringstream ss; ss << std::get<Is>(m_fields); return ss.str(); }()
            }... };
    }
};


template<template <typename, typename...> typename derived, typename String, typename... field_types>
class basic_row_schema
{
    template<template <typename, typename...> typename, typename, typename...>
    friend class basic_row_schema;
    friend class single_record<String, field_types...>;

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
    basic_row_schema(std::index_sequence<Is...>, const derived<String, field_types...>& r)
        : m_column_names{ r.basic_row_schema<derived, String, field_types...>::m_column_names[Is]... }
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

    template<typename... Strings>
    static auto create_using_headings(Strings... strs)
    {
        return basic_row_schema
            <derived, std::common_type_t<std::decay_t<Strings>...>, field_types...>
            (std::decay_t<Strings>(strs)...);
    }

    constexpr std::string headings_as_list()
    {
        return to_list(std::index_sequence_for<field_types...>{}, m_column_names);
    }
    constexpr std::string placeholders_as_list(char placeholder = '?')
    {
        return to_list(std::vector<char>(sizeof...(field_types), placeholder));
    }

    //from args

    derived<String, field_types...>
        to_single_record(field_types... fields)
    {
        return derived<String, field_types...>(
            std::index_sequence_for<field_types...>{}, * this, fields...);
    }
    derived<String, field_types...>
        to_single_record()
    {
        return derived<String, field_types...>(
            std::index_sequence_for<field_types...>{}, * this);
    }
};

template< typename... f_types>
using row_schema_cstr = basic_row_schema<single_record, const char*, f_types...>;
template< typename... f_types>
using row_schema = basic_row_schema<single_record, std::string, f_types...>;

template< typename... f_types>
std::ostream& operator<<(std::ostream& s, single_record< f_types...> rec)
{
    s << rec.headings_as_list() << std::endl;
    s << rec.values_as_list() << std::endl;
    return s;
}
template< typename String2, typename... f_types>
std::ostream& operator<<(std::ostream& s, basic_row_schema< single_record, String2, f_types...> spec)
{
    s << spec.headings_as_list() << std::endl;
    return s;
}
struct unknown_t {};
std::ostream& operator<<(std::ostream& s, unknown_t) { s << "<unknown-type>"; return s; }

int demo(std::ostream& sout) {
    using namespace std;
    struct exampleType1 : unknown_t {};
    struct exampleType2 : unknown_t {
        exampleType1 e1;
        int x;
    };
    // auto s = row_schema<int, const char*, const char*>::create_using_headings(id, name, type);
    auto schema1 = row_schema<int, const char*, const char*>::
        create_using_headings("id", "name", "type");
    auto schema2 = row_schema<exampleType1, exampleType1, exampleType2, const char*, const char*, int64_t>::
        create_using_headings("exampleType1", "exampleType1:2", "exampleType2", "a c_string", "another c_string", "int64_t");

    sout << "schema1 headings:" << endl << schema1.headings_as_list() << endl;
    sout << "schema2 headings:" << endl << schema2.headings_as_list() << endl;

    auto r1s1 = schema1.to_single_record(5, "d", "7");
    //auto r1s2 = schema2.to_single_record(5, "d", "7"); //wont compile
    auto r1s2 = schema2.to_single_record(exampleType1{}, exampleType1{}, exampleType2{}, "asdfg ", "adsfgasdfv", -23623456);

    sout << "schema1 single_record serialization:" << endl << r1s1 << endl;
    sout << "schema1 single_record serialization of factory rval:" << endl
        << r1s1.to_single_record(9, "fds", ";7$") << endl
        << "schema1 single_record serialization of base schema factory rval:" << endl
        << schema1.to_single_record(2, "four", "three") << endl
        << "schema1 single_record serialization of base schema factory empty rval:" << endl
        << schema1.to_single_record() << endl
        << "dictionary lookup on heading: name: " << r1s1.to_dictionary()["name"] << endl
        << "headings_as_list:" << r1s1.headings_as_list() << endl << endl;

    sout << "schema2 single_record serialization:" << endl << r1s2 << endl;
    sout << "schema2 single_record serialization of factory rval:" << endl
        << r1s2.to_single_record(exampleType1{}, exampleType1{}, exampleType2{}, "fhjnk re ", "468kb67j", -5) << endl
        << "schema2 single_record serialization of base schema factory rval:" << endl
        << schema2.to_single_record(exampleType1{}, exampleType1{}, exampleType2{}, "375jyb ety ", " 3e57j", 67) << endl
        << "schema2 single_record serialization of base schema factory empty rval:" << endl
        << schema2.to_single_record() << endl
        << "dictionary lookup on heading: another c_string: " << r1s2.to_dictionary()["another c_string"] << endl
        << "headings_as_list:" << r1s2.headings_as_list() << endl << endl;


    sout << "schema1 single_record operations:" << endl;
    auto t1s1 = make_tuple(-9, "hdk", "gdj");
    std::tuple<int, const char*, const char*> t2s1(-9, "hdk", "gdj");
    r1s1 = t1s1;
    auto& [id, name, type] = r1s1.row_values();
    sout << endl << id << " " << name << " " << type << endl;
    sout << r1s1;
    r1s1 = t2s1;
    std::tie(id, name, type) = r1s1.row_values();
    sout << endl << id << " " << name << " " << type << endl;
    sout << r1s1 << endl << endl;

    sout << "schema2 single_record operations:" << endl;
    auto t1s2 = make_tuple(exampleType1{}, exampleType1{}, exampleType2{}, "fsb ety ", " 246g2", 5);
    std::tuple<exampleType1, exampleType1, exampleType2, const char*, const char*, int64_t> t2s2(exampleType1{}, exampleType1{}, exampleType2{}, "37adfvjyb ety ", " wyn", 7);
    r1s2 = t1s2;
    auto& [exampleType1v, exampleType1_2v, exampleType2v, ac_stringv, anotherc_stringv, int64_tv] = r1s2.row_values();
    sout << endl << id << " " << name << " " << type << endl;
    sout << r1s2;
    r1s2 = t2s2;
    std::tie(exampleType1v, exampleType1_2v, exampleType2v, ac_stringv, anotherc_stringv, int64_tv) = r1s2.row_values();
    sout << endl << id << " " << name << " " << type << endl;
    sout << r1s2 << endl << endl;
    return 0;
}
int main() { return demo(cout); }
