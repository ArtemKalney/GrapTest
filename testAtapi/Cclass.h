#pragma once

#include "stdafx.h"
#include "products.h"

// making Hierarchy
class sub_subcategory
{
    int id_sub_subcategory;
    vector<product*> products;
public:
    sub_subcategory(int id)
    {
        id_sub_subcategory = id;
    }
    void add_new_product(product *item);
    void show();
};

class subcategory
{
    int id_subcategory;
    vector<programming*> programmings;
    vector<cooking*> cookings;
    vector<esoteric*> esoterics;

    vector<sub_subcategory*> sub_subcategories;
public:
    subcategory(int id) {
        id_subcategory = id;
        if (id_subcategory > id_esoteric)
            for (int i = id_music; i <= id_software; i++) {
                //cout << "created sub_subcategory with id " << i << endl;
                sub_subcategory new_sub_subcategory(i);
                sub_subcategories.push_back(&new_sub_subcategory);
            }
    }
    template <class T>
    void add_new_product(T *item, int type);
    void show();
};

class category
{
    string name;
    vector<subcategory*> subcategories;
public:
    category(string str)
    {
        name = str;
        for(int i = id_programming; i <= id_dvd; i++)
        {
            //cout<<"created subcategory with id "<<i<<endl;
            subcategory new_subcategory(i);
            subcategories.push_back(&new_subcategory);
        }
    }
    template <class T>
    void add_new_product(T *item);
    void show();
};

template <class T>
void category :: add_new_product(T *item)
{
    int i = item->get_id_subcategory();

    switch(i)
    {
        case(id_programming):
            subcategories[id_programming]->add_new_product(item, i);
            break;
        case(id_cooking):
            subcategories[id_cooking]->add_new_product(item, i);
            break;
        case(id_esoteric):
            subcategories[id_esoteric]->add_new_product(item, i);
            break;
        case(id_cd):
            subcategories[id_cd]->add_new_product(item, i);
            break;
        case(id_dvd):
            subcategories[id_dvd]->add_new_product(item, i);
            break;
        default: cout << "no such subcategory";
    }
}

template <class T>
void subcategory :: add_new_product(T *item, int type)
{
    if (type > id_esoteric) {
        int i = item->get_id_sub_subcategory();

        switch(i) {
            case (id_music):
                sub_subcategories[id_music]->add_new_product(item);
                break;
            case (id_video):
                sub_subcategories[id_video]->add_new_product(item);
                break;
            case (id_software):
                sub_subcategories[id_software]->add_new_product(item);
                break;
            default: cout << "no such sub_subcategory";
        }
    }else {
        switch(type) {
            case (id_programming):
                programmings.push_back(item);
                break;
            case (id_cooking):
                cookings.push_back(item);
                break;
            case (id_esoteric):
                esoterics.push_back(item);
                break;
        }
    }
}