//https://wandbox.org/permlink/uHjIO3kAUCEsrcVq
#include <iostream>
#include "db_types.h"

/*
create a database table schema by specifying designated native types for field values, 
such that a given native type is restricted to those with an extant 
type-value-conversion-to-database-value-type. any non conformities create a compile 
error. database row records can only be created as splinters of an existing schema 
definition and type integrity is enforced. built in utilities allow serialization 
to output streams.
*/

template<typename... f_types>
using db_row_schema = row_schema<meta::is_type::supported, f_types...>;

int demo(std::ostream& sout) {

    auto DB_SCHEMA_PHOTOS = db_row_schema<
        std::string,    // path;
        winrt::uint64_t,// folderKey;
        winrt::uint64_t,// thumbnailCacheId;
        winrt::uint32_t,// fileAttributes;
        winrt::uint32_t,// ProviderKey;
        winrt::uint64_t,// itemDate;
        winrt::uint64_t,// dateModified;
        winrt::uint32_t,// width;
        winrt::uint32_t// height;
    >::create_using_headings(
        "Path",
        "FolderKey",
        "ThumbnailCacheId",
        "FileAttributes",
        "ProviderKey",
        "ItemDate",
        "DateModified",
        "Width",
        "Height");

    sout << "DB_SCHEMA_PHOTOS col headings: " << endl <<  DB_SCHEMA_PHOTOS.headings_as_list() << endl << endl ;
    sout << "DB_SCHEMA_PHOTOS to_single_record, empty: " << endl <<  DB_SCHEMA_PHOTOS.to_single_record() << endl << endl ;
    auto rec = DB_SCHEMA_PHOTOS.to_single_record("a/cool/path",7,3,67,23,4,6,45,7);
    sout << "DB_SCHEMA_PHOTOS to_single_record, with values: " << endl << rec  << endl << endl ;
    sout << "DB_SCHEMA_PHOTOS to_single_record, converted values: " << endl <<  rec.converted_row_values() << endl << endl ;

    return 0;
}
int main() { return demo(cout); }
