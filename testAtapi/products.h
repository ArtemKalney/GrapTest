#pragma once

#include "stdafx.h"

enum  subcategories {id_programming, id_cooking, id_esoteric, id_cd, id_dvd};
enum  sub_subcategories {id_music, id_video, id_software};

// making classes
class product
{
protected:
    std::string _name_subcategory;
    std::string _name_sub_subcategory;

    std::string _name;
    double _price;
    std::string _barcode;
public:
    product(const std::string& a, const std::string& b, const std::string& c, double d, const std::string& e) :
            _name_subcategory(a),
            _name_sub_subcategory(b),
            _name(c),
            _price(d),
            _barcode(e)
    {}
    int get_id_subcategory();
    int get_id_sub_subcategory();
    void show();
};

class books : public product
{
protected:
    int _number_of_pages;
public:
    books(const std::string& a, const std::string& b, const std::string& c, double d, const std::string& e, int f) :
            product(a, b, c, d, e),
            _number_of_pages(f)
    {}
};

class programming : public books
{
    std::string _language_of_programming;
public:
    programming(const std::string& a, const std::string& b, const std::string& c, const double d, const std::string& e,
                int f, const std::string& h) :
            books(a, b, c, d, e, f),
            _language_of_programming(h)
    {}
    void show();
};

class cooking : public books
{
    std::string _main_ingradient;
public:
    cooking(const std::string& a, const std::string& b, const std::string& c, double d, const std::string& e, int f,
            const std::string& h) :
            books(a, b, c, d, e, f),
            _main_ingradient(h)
    {}
    void show();
};

class esoteric : public books
{
    int _min_age;
public:
    esoteric(const std::string& a, const std::string& b, const std::string& c, double d, const std::string& e, int f,
             int h) :
            books(a, b, c, d, e, f),
            _min_age(h)
    {}
    void show();
};
