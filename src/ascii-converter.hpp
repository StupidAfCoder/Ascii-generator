#ifndef ASCII_GENERATOR_HPP
#define ASCII_GENERATOR_HPP

#include <string>
#include<iostream>
using String = std::string;

class Ascii{
    private:
        String image_path;
    public:
        Ascii(const String& path) : image_path(path) {}
        void generate();
        const String& getPath() const {return image_path;}     
};

#endif