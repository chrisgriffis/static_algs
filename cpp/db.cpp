#include <iostream>
#include <map>
#include <sstream>
#include <array>
#include <memory>
#include <algorithm>
using namespace std;

template<typename... field_types>
struct table_spec;

template<typename table_t, typename... field_types> struct Record;
template<template <typename...> typename table_spec, typename... field_types>
struct Record< table_spec<field_types...>, field_types...>
    : public table_spec<field_types...>
{
    using table_spec_t = table_spec<field_types...>;

    table_spec_t::fields_t m_fields;

    Record() = delete;

protected:
    //with args
    template<typename... field_types_ctor,
        typename = std::enable_if_t<sizeof...(field_types) == sizeof...(field_types_ctor)>>
        Record(table_spec_t schema, field_types_ctor... fields) :
        table_spec_t{ schema },
        m_fields{ std::make_tuple(fields...) }
    {}
    //with tup
    template<typename... field_types_ctor,
        typename = std::enable_if_t<sizeof...(field_types) == sizeof...(field_types_ctor)>>
        Record(table_spec_t schema, std::tuple<field_types_ctor...> tup) :
        table_spec<field_types_ctor...>{ schema },
        m_fields{ tup }
    {}
    //no args
    template<typename... field_types_ctor,
        typename = std::enable_if_t<sizeof...(field_types) == sizeof...(field_types_ctor)>>
        Record(table_spec_t schema) :
        table_spec<field_types_ctor...>{ schema },
        m_fields{  }
    {}
    //another rec
    template<typename... field_types_ctor,
        typename = std::enable_if_t<sizeof...(field_types) == sizeof...(field_types_ctor)>>
        Record(const Record< table_spec<field_types...>, field_types_ctor...>& rec) :
        table_spec<field_types...>{ rec.m_column_headings },
        m_fields{ rec.m_fields }
    {}
public:
    //with args
    template<typename... field_types_ctor,
        typename = std::enable_if_t<sizeof...(field_types) == sizeof...(field_types_ctor)>>
    static table_spec<field_types_ctor...>
    create(table_spec_t schema, field_types_ctor... fields)
    { return table_spec<table_spec_t>(std::forward<field_types_ctor>(fields)...);}
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
        create(const Record< table_spec<field_types...>, field_types_ctor...>& rec) :
        table_spec<field_types...>{ rec.m_column_headings },
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
    Record< table_spec_t, field_types...>
        to_record(field_types... fields) override
    {
        return Record< table_spec_t, field_types...>(*this, fields...);
    }

    //no args
    Record< table_spec_t, field_types...>
        to_record(table_spec_t schema) override
    {
        return Record< table_spec_t, field_types...>(*this);
    }

    //from tup
    Record< table_spec_t, field_types...>
        to_record(std::tuple<field_types...> tup) override
    {
        return Record< table_spec_t, field_types...>(*this, tup);
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


template<typename String, typename... field_types>
struct table_spec
{
    using record_t = Record<table_spec, field_types...>;
    using fields_t = std::tuple<field_types...>;
    using column_names_t = const std::array<String, sizeof...(field_types)>;

    column_names_t m_column_names;

    template<typename String... str>
        constexpr table_spec<String, std::remove_reference_t<field_types>>(String... str )
        : m_column_names{ std::forward<String>(str)... }
    {}

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
    virtual record_t to_record(table_spec schema)
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


template<template <typename...> typename table_spec, typename... field_types>
std::ostream& operator<<(std::ostream& s, Record<table_spec<field_types...>, field_types...>  rec)
{
    auto l = rec.to_lookup();
    for_each(l.begin(), l.end(), [&](auto e) {s << e.second; });
    return s;
}

int main() {
    auto t1 = make_tuple(9, "qwer", "dtyj");
    const char* id = "id";
    const char* name = "name";
    const char* type = "type";
    auto s = table_spec<int, const char*, const char*>(id, name, type);
    auto x = s.to_record(5, (const char*)"foo", (const char*)"g");
    cout << s.column_headings_list() << endl;
    cout << x.column_headings_list() << endl <<
        x.to_lookup()[id] << "," << x.to_lookup()[name] << "," << x.to_lookup()[type] << endl << endl;
    auto t2 = make_tuple(-5367, "68o.l", "ayio,u9yilo");
    auto t3 = make_tuple(67, "487k", "356g");
    x = t1;
    cout << x.column_headings_list() << endl <<
        x.to_lookup()[id] << "," << x.to_lookup()[name] << "," << x.to_lookup()[type] << endl << endl;
    auto z = s.to_record(x);
    cout << z.column_headings_list() << endl <<
        z.to_lookup()[id] << "," << z.to_lookup()[name] << "," << z.to_lookup()[type] << endl << endl;
    z = t2;
    cout << z.column_headings_list() << endl <<
        z.to_lookup()["id"] << "," << z.to_lookup()["name"] << "," << z.to_lookup()["type"] << endl << endl;
    auto z2 = s.to_record(t3);
    cout << z2.column_headings_list() << endl << z2 << endl;
    return 0;
}