/*
Compile-time merge sort example in modern C++

i think this impl is broken. see static_mergesort.h for a working example.

https://github.com/chrisgriffis/static_algs/h/static_sort.h
Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2012-2021 Chris Griffis <Chris.Griffis@outlook.com>
Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <iostream> 
#include <functional> //for std::conditional
#include <vector> //for debug output

namespace my
{
	namespace static_alg
	{
		//natural number sequence
		template<std::size_t... Is>
		class Seq {};

		//print sequence metadata
		template<std::size_t... Is>
		void print(Seq<Is...>)
		{
			constexpr std::size_t v[] = { Is... };
			std::cout << std::dec;
			for (auto e : v) std::cout << e << " ";
			std::cout << std::endl;
		}

		//
		//concatenate two sequences
		//
		template<typename T1, typename T2> struct concat;
		template<typename T1, typename T2> using concat_t = typename concat<T1, T2>::result;
		template<std::size_t... s1n, std::size_t... s2n>
		struct concat<Seq<s1n...>, Seq<s2n...>>
		{
			using result = Seq<s1n..., s2n...>;
		};

		//
		//split a sequence after zero-based index N into two subsequences
		//
		template<std::size_t N, typename T> struct split_after;
		template<std::size_t N, typename T> using front_t = typename split_after<N, T>::front;
		template<std::size_t N, typename T> using tail_t = typename split_after<N, T>::tail;
		template<std::size_t N, std::size_t s1, std::size_t... s1n>
		struct split_after<N, Seq<s1, s1n...>>
		{
			using front = concat_t<Seq<s1>, front_t<N - 1, Seq<s1n...>>>;
			using tail = tail_t<N - 1, Seq<s1n...>>;
		};
		template< std::size_t s1, std::size_t... s1n>
		struct split_after<0, Seq<s1, s1n...>>
		{
			using front = Seq<s1>;
			using tail = Seq<s1n...>;
		};

		//
		//merge two sequences
		//
		template<typename T1, typename T2> struct merge;
		template<typename T1, typename T2> using merge_t = typename merge<T1, T2>::result;
		template<std::size_t s1, std::size_t... s1n, std::size_t s2, std::size_t... s2n>
		struct merge<Seq<s1, s1n...>, Seq<s2, s2n...>>
		{
		private:
			constexpr static bool use_left = (s1 < s2);
			static bool f[use_left ? 1 : -1];
		public:
			//create a new Seq that starts with smallest head,
			//and merges the remaining sub-Seq with the other 
			//Seq, which is left unchanged in this pass.
			using result = concat_t<
				std::conditional_t<use_left, Seq<s1>, Seq<s2>>,

				typename std::conditional_t<
				use_left,
				merge_t<Seq<s1n...>, Seq<s2, s2n...>>,
				merge_t<Seq<s1, s1n...>, Seq<s2n...>>>>;
		};
		//merge specializations
		template<std::size_t... s1n> struct merge<Seq<s1n...>, Seq<>> { using result = Seq<s1n...>; };
		template<std::size_t... s2n> struct merge<Seq<>, Seq<s2n...>> { using result = Seq<s2n...>; };
		template<> struct merge<Seq<>, Seq<>> { using result = Seq<>; };

		//
		//sort
		//
		template<typename T> class sort;
		template<typename T> using sort_t = typename sort<T>::result;
		template<std::size_t... s1n>
		class sort<Seq<s1n...>>
		{
			constexpr static std::size_t mid = (sizeof...(s1n) - 1) / 2;
			using Left = front_t<mid, Seq<s1n...>>;
			using Right = tail_t<mid, Seq<s1n...>>;
		public:
			using result = merge_t<sort_t<Left>, sort_t<Right>>;
		};
		//sort specializations
	template<std::size_t I> class sort<Seq<I>> { public: using result = Seq<I>; };
	}
}
// int main()
// {
//     using namespace static_alg;
//     using Unsorted = Seq<6, 5, 8, 7, 9, 2, 3, 1, 11>;
//     using Sorted = sort_t<Unsorted>;
//     print(Unsorted{});
//     print(Sorted{});
//     return 0; 
// }
