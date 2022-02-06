//https://wandbox.org/permlink/9YkG1T16igIWrYOw
#include <iostream>
#include "db_types.h"

using namespace std;
struct exampleType1 : unknown_t {};
struct exampleType2 : unknown_t { exampleType1 e1; int x; };

int demo(std::ostream& sout) {

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

/*
Start
schema1 headings:
id,name,type
schema2 headings:
exampleType1,exampleType1:2,exampleType2,a c_string,another c_string,int64_t
schema1 single_record serialization:
id,name,type
5,d,7

schema1 single_record serialization of factory rval:
id,name,type
9,fds,;7$

schema1 single_record serialization of base schema factory rval:
id,name,type
2,four,three

schema1 single_record serialization of base schema factory empty rval:
id,name,type
0,

dictionary lookup on heading: name: d
headings_as_list:id,name,type

schema2 single_record serialization:
exampleType1,exampleType1:2,exampleType2,a c_string,another c_string,int64_t
<unknown-type>,<unknown-type>,<unknown-type>,asdfg ,adsfgasdfv,-23623456

schema2 single_record serialization of factory rval:
exampleType1,exampleType1:2,exampleType2,a c_string,another c_string,int64_t
<unknown-type>,<unknown-type>,<unknown-type>,fhjnk re ,468kb67j,-5

schema2 single_record serialization of base schema factory rval:
exampleType1,exampleType1:2,exampleType2,a c_string,another c_string,int64_t
<unknown-type>,<unknown-type>,<unknown-type>,375jyb ety , 3e57j,67

schema2 single_record serialization of base schema factory empty rval:
exampleType1,exampleType1:2,exampleType2,a c_string,another c_string,int64_t
<unknown-type>,<unknown-type>,<unknown-type>,

dictionary lookup on heading: another c_string: adsfgasdfv
headings_as_list:exampleType1,exampleType1:2,exampleType2,a c_string,another c_string,int64_t

schema1 single_record operations:

-9 hdk gdj
id,name,type
-9,hdk,gdj

-9 hdk gdj
id,name,type
-9,hdk,gdj


schema2 single_record operations:

-9 hdk gdj
exampleType1,exampleType1:2,exampleType2,a c_string,another c_string,int64_t
<unknown-type>,<unknown-type>,<unknown-type>,fsb ety , 246g2,5

-9 hdk gdj
exampleType1,exampleType1:2,exampleType2,a c_string,another c_string,int64_t
<unknown-type>,<unknown-type>,<unknown-type>,37adfvjyb ety , wyn,7


0
Finish
*/
