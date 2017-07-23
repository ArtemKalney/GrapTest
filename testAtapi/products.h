#pragma once

#include "stdafx.h"

enum subcategories {id_programming, id_cooking, id_esoteric, id_cd, id_dvd};
enum sub_subcategories {id_music, id_video, id_software};

// making classes
class product
{
protected:
    string name_subcategory;
    string name_sub_subcategory;

    string name;
    double price;
    string barcode;
public:
    product(string a, string b, string c, double d, string e)
    {
        name_subcategory = a;
        name_sub_subcategory = b;
        name = c;
        price = d;
        barcode = e;
    }
    int get_id_subcategory();
    int get_id_sub_subcategory();
    void show();
};

class books : public product
{
protected:
    int number_of_pages;
public:
    books(string a, string b, string c, double d, string e, int f) : product(a, b, c, d, e)
    {
        number_of_pages = f;
    }
};

class programming : public books
{
    string language_of_programming;
public:
    programming(string a, string b, string c, double d, string e, int f, string h) : books(a, b, c, d, e, f)
    {
        language_of_programming = h;
    }
    void show();
};

class cooking : public books
{
    string main_ingradient;
public:
    cooking(string a, string b, string c, double d, string e, int f, string h) : books(a, b, c, d, e, f)
    {
        main_ingradient = h;
    }
    void show();
};

class esoteric : public books
{
    int min_age;
public:
    esoteric(string a, string b, string c, double d, string e, int f, int h) : books(a, b, c, d, e, f)
    {
        min_age = h;
    }
    void show();
};
