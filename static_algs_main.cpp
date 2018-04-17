#include <iostream>
#include <functional>
#include <vector>
using namespace std;

namespace static_alg
{
    //natural number sequence
    template<size_t... Is>
    class Seq {};

    //print sequence metadata
    template<size_t... Is>
    void print(Seq<Is...>)
    {
        vector<size_t> v{ Is... };
        for (auto e : v) cout << e << " ";
        cout << endl;
    }

    //concatenate two sequences
    template<typename T1, typename T2> struct concat;
    template<size_t... s1n, size_t... s2n>
    struct concat<Seq<s1n...>, Seq<s2n...>>
    {
        using result = Seq<s1n..., s2n...>;
    };

    //get the front N subsequence
    template<size_t N, typename T> struct front;
    template<size_t N, size_t s1, size_t... s1n>
    struct front<N, Seq<s1, s1n...>>
    {
        using result = typename concat<
            Seq<s1>, 
            typename front<N - 1, Seq<s1n...>>::result
        >::result;
        using tail = typename front<N - 1, Seq<s1n...>>::tail;
    };
    template< size_t s1, size_t... s1n>
    struct front<0, Seq<s1, s1n...>>
    {
        using result = Seq<s1>;
        using tail = Seq<s1n...>;
    };

    //split a sequence type at an index
    template<typename T> struct split;
    template<size_t... s1n>
    class split<Seq<s1n...>>
    {
        constexpr static size_t mid = (sizeof...(s1n) - 1) / 2;
    public:
        using Left = typename front<mid, Seq<s1n...>>::result;
        using Right = typename front<mid, Seq<s1n...>>::tail;
    };

    //merge two sequences
    template<typename T1, typename T2> struct merge;
    template<size_t s1, size_t... s1n, size_t s2, size_t... s2n>
    struct merge<Seq<s1, s1n...>, Seq<s2, s2n...>>
    {
        using result = typename concat<
            typename conditional<
                (s1 < s2), 
                Seq<s1>, 
                Seq<s2>
            >::type, 
            typename conditional<
                (s1 < s2),
                typename merge<Seq<s1n...>, Seq<s2, s2n...>>::result,
                typename merge<Seq<s1, s1n...>, Seq<s2n...>>::result
            >::type
        >::result;
    };
    //merge specializations
    template<size_t... s1n> struct merge<Seq<s1n...>, Seq<>> { using result = Seq<s1n...>; };
    template<size_t... s2n> struct merge<Seq<>, Seq<s2n...>> { using result = Seq<s2n...>; };
    template<> struct merge<Seq<>, Seq<>> { using result = Seq<>; };

    //sort
    template<typename T> class sort;
    template<size_t... s1n>
    class sort<Seq<s1n...>>
    {
        using S = Seq<s1n...>;
        using L = typename split<S>::Left;
        using R = typename split<S>::Right;
    public:
        using result = typename merge<
            typename sort<L>::result,
            typename sort<R>::result
        >::result;
    };
    template<size_t I> class sort<Seq<I>> { public: using result = Seq<I>; };
    template<typename T>
    using sorted_t = typename sort<T>::result;
}

int main()
{
    using namespace static_alg;
    using Unsorted = Seq<6, 5, 8, 7, 9, 2, 3, 1, 11>;
    using Sorted = sorted_t<Unsorted>;
    print(Unsorted{});
    print(Sorted{});
    return 0;
}
