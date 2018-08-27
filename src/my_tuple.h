#include <utility>
#include <iostream>

//tuple impl from scratch without looking... with a stream output overload
namespace my
{
    namespace detail //sequestered TMP utilities
    {
        //handmade equiv of std::index_sequence 
        //and std::make_index_sequence
        //used for static indexing of tuple elements
        template< size_t... >
        struct idx_seq {};

        template<typename, size_t>
        struct append_idx_seq;

        template<size_t... Is, size_t K>
        struct append_idx_seq<idx_seq<Is...>, K>
        {
            using type = idx_seq<Is..., K>;
        };

        template<>
        struct append_idx_seq<void, 0>
        {
            using type = idx_seq<0>;
        };

        template< size_t K>
        struct make_idx_seq
        {
            using type =
                typename append_idx_seq<
                    typename make_idx_seq<K - 1>::type,
                    K - 1
                >::type;
        };

        template<> struct make_idx_seq<0> { using type = void; };

        template< size_t K>
        using make_idx_seq_t = typename make_idx_seq<K>::type;
    }

    namespace impl //sequestered implementation details
    {
        //tuple element wrapper
        //type is statically indexed to differentiate
        //tuple elements with duplicate types, e.g. tuple<int,int>
        //and support index based lookup
        template< size_t K, typename T>
        struct tuple_elem
        {
            tuple_elem() = default;
            tuple_elem(const T& t) :_t(t) {}
            T& get() { return _t; }
            void set(T& t) { _t = t; }
        private:
            T _t;
        };

        //impl::tuple 
        //implementation detail aggregates tuple elem wrappers as 
        //base classes, which are unique for duplicate value types because
        //each tuple wrapper element is statically indexed
        template<typename... > struct tuple; //inside namespace impl

        //impl::tuple's only specialization, which requires an 
        //index sequence that coincides with the typelist
        template<size_t... Is, typename... Ts>
        struct tuple<detail::idx_seq<Is...>, Ts...> :
            //private multiple inheritance via param pack expansion
            tuple_elem<Is, Ts>...
        {
            //constructors
            tuple() = default;
            tuple(Ts... args) :
                //ctr initializer list via param pack expansion
                tuple_elem<Is, Ts>(std::forward<Ts>(args))...
                //forwarding mechanism preserves cv-unqualified 
                //value category
            {}
        };
    }

    //the actual tuple class
    template<typename... Ts>
    class tuple :
        //private base class imbues statically indexed elements as base classes
        //impl at this level uses overloads to select correct base classes
        impl::tuple<detail::make_idx_seq_t<sizeof...(Ts)>, Ts...>
    {
        //private local implementation details
        struct local_impl
        {
            //logic in here is not
            //needed for tuple itself, contains
            //tools needed for sending tuple object 
            //to std::ostream e.g. std::cout 

            //TMP function for static index-based lookup
            template< size_t K, typename, typename... Tn>
            struct at_index {
                using type = typename at_index< K - 1, Tn...>::type;
            };

            template< typename T0, typename... Tn>
            struct at_index<0, T0, Tn...> { using type = T0; };

            //TMP function for unpacking tuple object elements 
            //into a function call
            template<typename> struct unpack;
            template<size_t... Is>
            struct unpack<detail::idx_seq<Is...>>
            {
                template<typename F>
                static void call(F& f, tuple& t)
                {
                    f(t.template get<Is>()...);
                }
            };

        };

        //private convenience aliases
        template<size_t Idx>
        using tup_elem_type =
            typename local_impl::template
                at_index<Idx, Ts...>::type;

        template<size_t Idx>
        using tup_elem =
            impl::tuple_elem<Idx, tup_elem_type<Idx>>;

    public:
        //constructors
        tuple() = default;
        tuple(Ts... args) :
            //ctr initializer list via param pack expansion
            impl::tuple<
                detail::make_idx_seq_t<sizeof...(Ts)>,
                Ts...
            >(std::forward<Ts>(args)...)
        {}

        //element lvalue accessor
        template<size_t Idx>
        tup_elem_type<Idx>& get()
        {
            //forward call to statically indexed 
            //tuple elem base class
            return tup_elem<Idx>::get();
        }

        //element mutator
        template<size_t Idx>
        void set(tup_elem_type<Idx> t)
        {
            //forward call to statically indexed 
            //tuple elem base class
            return tup_elem<Idx>::set(t);;
        }

        //for output to streams
        friend std::ostream& operator<<(std::ostream& s, tuple& tup)
        {
            //lambda to encapsulate logic for putting the
            //respective tuple element values into the stream
            auto to_stream = [&s](Ts... args)
            {
                s << "elements: ";
                //call to empty lambda used to expand param pack
                [](...) {}(
                    //param pack expansion converts "mult args in single call"
                    //to "multiple calls each with single arg"
                    [](std::ostream& s, auto& v)
                    {
                        s << v << " "; return 0; //non-void return type
                    }(s, args)... //param pack expansion happens here
                );
                s << std::endl;
            };

            //mechanism to unpack the tuple element values
            //into the lambda defined above
            local_impl::template
                unpack<
                    detail::make_idx_seq_t<sizeof...(Ts)>
                >::call(to_stream, tup);
            return s;
        }
        //an overload to handle putting rvalues into stream
        friend std::ostream& operator<<(std::ostream& s, tuple&& tup)
        {
            return s << tup;
        }
    };

    template<size_t Idx, typename T>
    auto& get(T& t) { return t.template get<Idx>(); }

    template<typename... Ts>
    constexpr tuple<Ts...> make_tuple(Ts&&... args)
    { return tuple<Ts...>(std::forward<Ts>(args)...); }
}


// int main()
// {
//     using namespace std;
//     my::tuple<int, char, int>
//         tup1(5, 'a', -11);
//     my::tuple<int, int, int, int, int, int>
//         tup2(-2, -1, 0, 1, 2, 3);
//     my::tuple<bool, const char*, void*, size_t>
//         tup3(true, "i am tup3", nullptr, 1);
//     my::tuple<bool, const char*, void*, size_t>
//         tup4; //default construct
//     auto 
//         tup5 = my::make_tuple("hello","tup5",'a','b');

//     tup3.set<2>(&tup3); //explicit set
//     tup4 = tup3; //copy assign
//     tup3.template get<3>() += 1; //object function call to lvalue access and re-assign
//     my::get<2>(tup4) = &tup4; //lvalue access and re-assign
//     tup4.set<1>("i am tup4");
//     const char* some_text = "some_text";

//     cout <<
//         boolalpha <<
//         "rval tuple " << my::tuple<int,bool>(1234, true) <<
//         "make_tuple " << my::make_tuple(543, 'x', "foo") <<
//         "tup1 " << tup1 <<
//         "tup2 " << tup2 <<
//         hex <<
//         "tup3 " << tup3 <<
//         "tup4 " << tup4 <<
//         "tup5 " << tup5 <<
//     endl;

//     return 0;
// }
