#include "products.h"

int product:: get_id_subcategory()
{
    if (_name_subcategory == "programming")
        return id_programming;
    if (_name_subcategory == "cooking")
        return id_cooking;
    if (_name_subcategory == "esoteric")
        return id_esoteric;
    if (_name_subcategory == "cd")
        return id_cd;
    if (_name_subcategory == "dvd")
        return id_dvd;

    return 404;
}

int product:: get_id_sub_subcategory()
{
    if (_name_sub_subcategory == "music")
        return id_music;
    if (_name_sub_subcategory == "video")
        return id_video;
    if (_name_sub_subcategory == "software")
        return id_software;

    return 404;
}

void product:: show()
{
    std::cout<< "name: " << _name << ", price: " << _price << ", barcode: " << _barcode << std::endl;
}

void programming:: show()
{
    std::cout<< "name: " << _name << ", price: " << _price << ", barcode: " << _barcode << ", language_of_programming: "
        << _language_of_programming << std::endl;
}

void cooking:: show()
{
    std::cout<< "name: " << _name << ", price: " << _price << ", barcode: " << _barcode
        << ", main_ingradient: " << _main_ingradient << std::endl;
}

void esoteric:: show()
{
    std::cout<< "name: " << _name << ", price: " << _price << ", barcode: " << _barcode << ", min_age: " << _min_age << std::endl;
}
