#include "Cclass.h"

void sub_subcategory :: add_new_product(std::shared_ptr<product> item)
{
    products.push_back(item);
}

void category :: show()
{
    for(int i = id_programming; i <= id_dvd; i++)
        subcategories[i]->show();
}

void subcategory :: show()
{
    for(int i = id_music; i <= id_software; i++)
        sub_subcategories[i]->show();

    for(int i = 0; i < programmings.size(), i++;)
        programmings[i]->show();
    for(int i = 0; i < cookings.size(), i++;)
        cookings[i]->show();
    for(int i = 0; i < esoterics.size(), i++;)
        esoterics[i]->show();
}

void sub_subcategory :: show()
{
    for(int i = 0; i < products.size(); i++)
        products[i]->show();
}





