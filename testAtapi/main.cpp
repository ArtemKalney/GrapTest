//#include "products.h"
#include "Cclass.h"

int main()
{
    //cout<<id_programming<<", "<<id_cooking<<", "<<id_esoteric<<", "<<id_cd<<", "<<id_dvd<<endl;

    product book1("cd", "music", "c++", 100.85, "###$#$#");
    //book1.show();
    //cout<<book1.get_id_subcategory()<<endl;
    //cout<<book1.get_id_sub_subcategory()<<endl;

    programming book2("programming", "", "c++", 100.85, "###$#$#", 162, "c++");
    //book2.show();
    //cout<<book2.get_id_subcategory()<<endl;
    //cout<<book2.get_id_sub_subcategory()<<endl;

    category books("Books");
    books.add_new_product(&book2);

    return 0;
}