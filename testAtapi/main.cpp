//#include "products.h"
#include "Cclass.h"

int main()
{
    //cout<<id_programming<<", "<<id_cooking<<", "<<id_esoteric<<", "<<id_cd<<", "<<id_dvd<<endl;

    std::shared_ptr<product> book1 (new product("cd", "music", "c++", 100.85, "###$#$#"));
    //book1.show();
    //cout<<book1.get_id_subcategory()<<endl;
    //cout<<book1.get_id_sub_subcategory()<<endl;

    std::shared_ptr<programming> book2 (new programming("programming", "", "c++", 100.85, "###$#$#", 162, "c++"));
    //book2.show();
    //cout<<book2.get_id_subcategory()<<endl;
    //cout<<book2.get_id_sub_subcategory()<<endl;

    category books("Books");
    books.add_new_product(book2);
    //books.show();

    return 0;
}