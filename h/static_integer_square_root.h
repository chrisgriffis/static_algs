#include <iostream>
#include <utility>
using namespace std;

namespace my
{
    namespace natural_num
    {
        template<bool, unsigned REMAIN, unsigned ROOT, unsigned ODD>
        class root_impl
        {
            constexpr static bool keepgoing = REMAIN > ODD;
            using NEXT = root_impl<keepgoing, REMAIN - ODD, ROOT + 1, ODD + 2>;
        public:
            constexpr static unsigned val = NEXT::val;
            constexpr static bool is_exact = NEXT::is_exact;
        };

        template<unsigned REMAIN, unsigned ROOT, unsigned ODD>
        class root_impl<false, REMAIN, ROOT, ODD>
        {
        public:
            constexpr static unsigned val = ROOT;
            constexpr static bool is_exact = (REMAIN == 0);
        };

        template<unsigned V>
        class square_root
        {
            using CEILING = root_impl<true, V, 0, 1>;
        public:
            constexpr static unsigned ceiling = CEILING::val;
            constexpr static unsigned floor = ceiling - (CEILING::is_exact ? 0 : 1);
            constexpr static bool is_exact = CEILING::is_exact;
        };

        template<>
        class square_root<0>
        {
        public:
            constexpr static unsigned floor = 0;
            constexpr static unsigned ceiling = 0;
            constexpr static bool is_exact = true;
        };

        template<size_t I>
        struct sq_root_info
        {
            int operator()() {
                using RESULT = natural_num::square_root<I>;
                cout << "I: " << I << endl;
                cout << "floor: " << RESULT::floor << " (" << RESULT::floor*RESULT::floor << ")" << endl;
                cout << "ceiling: " << RESULT::ceiling << " (" << RESULT::ceiling*RESULT::ceiling << ")" << endl;
                cout << boolalpha << "is_exact: " << RESULT::is_exact << endl << endl;
                return 0;
            }
        };
    } //namespace natural_num


    template<template<size_t> class Indexed_Ftr, size_t... Is>
    constexpr void index_apply_impl(index_sequence<Is...>)
    {
        //lambda just gives me a place to expand...
        [](auto...) {}(Indexed_Ftr<Is>{}()...);
    }

    template<template<size_t> class Indexed_Ftr, size_t N>
    constexpr void index_apply()
    {
        //converts a sequence type into a template with an inferable,
        //packable index sequence
        return index_apply_impl<Indexed_Ftr>(make_index_sequence<N>{});
    }

    template<size_t N>
    void display_sq_root_info_from_0_to_()
    {
        index_apply<natural_num::sq_root_info, N>();
    }
}

int square_root_main()
{
    my::display_sq_root_info_from_0_to_<30>();
    return 0;
}
