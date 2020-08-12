//https://ideone.com/XRyMHc
#include <string>
#include <iostream>

using namespace std;


template <char... chars>
struct static_string
{
	static string toString() { return string{ chars... }; }

	template<typename...>struct concat;
	template<char... C2>
	struct concat<static_string<C2...>>
	{
		using val = static_string<chars..., C2...>;
	};
};


template < typename T >
struct Jinja
{
	using statstr = static_string<'f', 'o', 'o', ' ', '+', ' '>;
};

template <
	template <char...> class templ,
	char... chars>
	struct Jinja<templ<chars...>>
{
	using statstr =
		typename Jinja<void>::statstr::concat<
		static_string<chars...>>::val;
};

template <
	template <unsigned, unsigned >
class TwoParamTempl,
	unsigned X,
	unsigned Y>
	struct Jinja<TwoParamTempl<X, Y>>
{
	using statstr = typename Jinja<void>::statstr::concat<
		static_string<char('0' + X), char('0' + Y)>>::val;
};

template <unsigned X, unsigned Y>
struct ActualTwoParamTempl {};

struct Foo : Jinja<ActualTwoParamTempl<5, 2>> {};

struct Bar : Jinja<static_string<'h', 'e', 'l', 'l', 'o'>> {};

int main()
{
	cout << Foo::statstr::toString() << endl;
	cout << Bar::statstr::toString() << endl;
	return 0;
}

