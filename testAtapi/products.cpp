#include "products.h"

int product:: get_id_subcategory()
{
    if (name_subcategory == "programming")
        return id_programming;
    if (name_subcategory == "cooking")
        return id_cooking;
    if (name_subcategory == "esoteric")
        return id_esoteric;
    if (name_subcategory == "cd")
        return id_cd;
    if (name_subcategory == "dvd")
        return id_dvd;

    return 404;
}

int product:: get_id_sub_subcategory()
{
    if (name_sub_subcategory == "music")
        return id_music;
    if (name_sub_subcategory == "video")
        return id_video;
    if (name_sub_subcategory == "software")
        return id_software;

    return 404;
}

void product:: show()
{
    cout<< "name: " << name << ", price: " << price << ", barcode: " << barcode << endl;
}

void programming:: show()
{
    cout<< "name: " << name << ", price: " << price << ", barcode: " << barcode << ", language_of_programming: "
        << language_of_programming << endl;
}

void cooking:: show()
{
    cout<< "name: " << name << ", price: " << price << ", barcode: " << barcode
        << ", main_ingradient: " << main_ingradient << endl;
}

void esoteric:: show()
{
    cout<< "name: " << name << ", price: " << price << ", barcode: " << barcode << ", min_age: " << min_age << endl;
}
