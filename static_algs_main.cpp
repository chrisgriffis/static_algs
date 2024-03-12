#include <iostream> 
#include "h/static_sort.h"
#include "h/my_tuple.h"

int static_sort_main();
int my_tuple_main();

constexpr struct foo { int (*_main)(); const char *_name; } labels[] = 
{
    {&static_sort_main,"static_sort_main"},
    {&my_tuple_main,"my_tuple_main"},
};

int main(int argc, char const *argv[])
{
    using namespace std;
    constexpr size_t k(sizeof(labels)/sizeof(foo));
    
    cout << "Choose a main function:" <<endl;
    for(int i = 0; i < k; ++i)
        cout << (i+1) << ": 0x" << hex << labels[i]._main << " | " << labels[i]._name <<endl;
    
    int c(0); 
    if (argc == 1) {
        cout << "Console input: ";
        cin >> c;
        c -= 1;
    }
    else {
        c = argv[1][0] - '1';
        //cout << "argv0: " << argv[0] << " | argv1: " << argv[1] << " | c: " << c << endl;
        cout << "chosen: " << (c+1) << endl;
    }
    
    return (c < k ? (labels[c]._main)() : 1);
}

int static_sort_main()
{
    using namespace my::static_alg;
//	using Unsorted = Seq<6, 5, 8, 7, 9, 2, 3, 1, 11, 5, 8, 7, 9, 2, 3, 1, 11, 5, 8, 7, 9, 2, 3, 1, 11, 5, 8, 7, 9, 2, 3, 1, 11, 5, 8, 7, 9, 2, 3, 1, 11, 5, 8, 7, 9, 2, 3, 1, 11, 5, 8, 7, 9, 2, 3, 1, 11, 5, 8, 7, 9, 2, 3, 1, 11, 5, 8, 7, 9, 2, 3, 1, 11, 5, 8, 7, 9, 2, 3, 1, 11, 5, 8, 7, 9, 2, 3>;
 	using Unsorted = Seq<1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20>;
	using Sorted = sort_t<Unsorted>;
    print(Unsorted{});
    print(Sorted{});
    return 0; 
}


int my_tuple_main()
{
    using namespace std;
    my::tuple<int, char, int>
        tup1(5, 'a', -11);
    my::tuple<int, int, int, int, int, int>
        tup2(-2, -1, 0, 1, 2, 3);
    my::tuple<bool, const char*, void*, size_t>
        tup3(true, "i am tup3", nullptr, 1);
    my::tuple<bool, const char*, void*, size_t>
        tup4; //default construct
    auto 
        tup5 = my::make_tuple("hello","tup5",'a','b');

    tup3.set<2>(&tup3); //explicit set
    tup4 = tup3; //copy assign
    tup3.template get<3>() += 1; //object function call to lvalue access and re-assign
    my::get<2>(tup4) = &tup4; //lvalue access and re-assign
    tup4.set<1>("i am tup4");
    const char* some_text = "some_text";

    cout <<
        boolalpha <<
        "rval tuple " << my::tuple<int,bool>(1234, true) <<
        "make_tuple " << my::make_tuple(543, 'x', "foo") <<
        "tup1 " << tup1 <<
        "tup2 " << tup2 <<
        hex <<
        "tup3 " << tup3 <<
        "tup4 " << tup4 <<
        "tup5 " << tup5 <<
    endl;

    return 0;
}
