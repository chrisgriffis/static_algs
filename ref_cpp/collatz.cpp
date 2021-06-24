#include <iostream>
using namespace std;

template<int N>
struct isEven
{
	constexpr static bool val = N % 2 == 0;
};

template<unsigned N, bool Pred>
struct collatzStepsImpl
{
	enum { count = 1 + collatzStepsImpl<N / 2, isEven<N / 2>::val>::count };
};

template<unsigned N>
struct collatzStepsImpl<N, false>
{
	enum { count = 1 + collatzStepsImpl<3 * N + 1, isEven<3 * N + 1>::val>::count };
};

template<>
struct collatzStepsImpl<1, false>
{
	enum { count = 0 };
};

template<unsigned N>
struct collatzSteps
{
	enum { count = collatzStepsImpl<N, isEven<N>::val>::count };
	static inline void Exec()
	{
		cout << "collatz count for " << N << ": " << boolalpha << collatzSteps<N>::count << endl;
	}
};

template<template <unsigned> class T, unsigned N>
struct Repeat
{
	static inline void Exec()
	{
		T<N - 1>::Exec();
		Repeat<T, N - 1>::Exec();
	}
};

template<template <unsigned> class T>
struct Repeat<T, 1>
{
	static inline void Exec() { T<1>::Exec(); }
};

template<template <unsigned> class T>
struct Repeat<T, 0>
{
	static inline void Exec() { T<0>::Exec(); }
};

int main()
{
	Repeat<collatzSteps, 101>::Exec();
}