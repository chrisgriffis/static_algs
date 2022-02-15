//https://wandbox.org/permlink/uHjIO3kAUCEsrcVq

//fake stuff to make the demo seem more authentic
namespace winrt {
    struct hstring { const char* c_str() { return "hstring"; } operator const char* () { return "hstring"; } };
    struct uint32_t { uint32_t() {} uint32_t(unsigned v) { val = v; } unsigned val; operator unsigned() { return val; } };
    struct uint64_t { uint64_t() {} uint64_t(long long unsigned v) { val = v; } long long unsigned val; operator long long unsigned() { return val; } };
    struct int32_t { int32_t() {} int32_t(int v) { val = v; } int val; operator int() { return val; } };
    struct int64_t { int64_t() {} int64_t(long long int v) { val = v; } long long int val; operator long long int() { return val; } };
}

/*
create a database table schema by specifying designated native types for field values, 
such that a given native type is restricted to those with an extant 
type-value-conversion-to-database-value-type. any non conformities create a compile 
error. database row records can only be created as splinters of an existing schema 
definition and type integrity is enforced. built in utilities allow serialization 
to output streams.
*/

//native-to-db type conversion rules
namespace meta
{
    namespace is_type {

        template<typename U>
        struct supported : std::false_type
        {
            using failed = U;
        };
        template<>
        struct supported<winrt::hstring> : std::true_type
        {
            const char* convert(winrt::hstring hs)
            {
                return hs.c_str();
            }
        };

        template<>
        struct supported < std::string> : std::true_type
        {
            const char* convert(std::string s)
            {
                return s.c_str();
            }
        };
        template<>
        struct supported < winrt::uint32_t> : std::true_type
        {
            unsigned int convert(winrt::uint32_t v)
            {
                return v.val;
            }
        };
        template<>
        struct supported < winrt::int32_t> : std::true_type
        {
            int convert(winrt::int32_t v)
            {
                return v.val;
            }
        };
        template<>
        struct supported < winrt::uint64_t > : std::true_type
        {
            unsigned long long convert(winrt::uint64_t v)
            {
                return v.val;
            }
        };
        template<>
        struct supported < winrt::int64_t> : std::true_type
        {
            long long int convert(winrt::int64_t v)
            {
                return v.val;
            }
        };


        template<typename T>
        struct string_type : std::false_type {};
        template<>
        struct string_type< winrt::hstring> : std::true_type {};
        template<>
        struct string_type<std::string > : std::true_type {};
        template<typename T>
        struct integral32_type : std::false_type {};
        template<>
        struct integral32_type< winrt::int32_t> : std::true_type {};
        template<>
        struct integral32_type<winrt::uint32_t> : std::true_type {};
        template<typename T>
        struct integral64_type : std::false_type {};
        template<>
        struct integral64_type<winrt::uint64_t> : std::true_type {};
        template<>
        struct integral64_type<winrt::int64_t> : std::true_type {};
    }
}
