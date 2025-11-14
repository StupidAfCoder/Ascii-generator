#ifndef ASCII_GENERATOR_HPP
#define ASCII_GENERATOR_HPP

#include <string>
#include<iostream>
using String = std::string;

enum class Chars : int {
    LOW = 1 , MED , HIGH , CUSTOM
};

class Ascii{
    private:
        String image_path;
        int width;
        int height;
        double brightness;
        double contrast;
        double gamma;
        double char_aspect;
        Chars level;
        String Low = "@\"%# ";
        String Med = "@%#*+=-:. ";
        String High = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/()1{}[]?-_+~<>i!lI;:,'. ";
        String Custom;

    public:
        Ascii(const String& path) : image_path(path) , width(150) , height(0){}
        void generate();
        const String& getPath() const {return image_path;}
        int getWidth() const {return width;}
        int getHeight() const {return height;}
        double getBrightness() const {return brightness;}
        double getContrast() const {return contrast;}
        double getGamma() const {return gamma;}
        String& getCharLevel() {
            if (level == Chars::LOW){
                return Low;
            } else if (level == Chars::MED) {
                return Med;
            } else if (level == Chars::HIGH){
                return High;
            } else {
                return Custom;
            }
        }
        double getCharAspect() const { return char_aspect;}

        //Setters
        void setWidth(int Width) { width = Width; }
        void setHeight(int high) { height = high; }
        void setBrightness(double bright) { brightness = bright; }
        void setContrast(double Contrast) { contrast = Contrast;}
        void SetGamma(double Gamma) { gamma = Gamma;}
        void SetCharLevel(int Level) {
            switch (Level) {
                case 1: level = Chars::LOW; break;
                case 2: level = Chars::MED; break;
                case 3: level = Chars::HIGH; break;
                case 4: level = Chars::CUSTOM; break;
                default:
                    level = Chars::CUSTOM; break;
            }
        }
        void SetCustomCharset(const String& charset){
            Custom = charset;
        }
        void SetCharAspect(double CHAR_ASPECT){
            char_aspect = CHAR_ASPECT;
        }
}; 

#endif