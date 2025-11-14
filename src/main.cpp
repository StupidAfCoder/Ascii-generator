#include "ascii-converter.hpp"
#include "cxxopts.hpp"
#include <algorithm>
#include <exception>

int main(int argc, char** argv) {
    try {
        cxxopts::Options options("ASCII Art", "Generate Ascii Art from any type of static image file!");

        options.add_options()
            ("i,input","Give the path to the file you want ASCII art of :)",cxxopts::value<std::string>())
            ("w,width","Give the width the output will cover ",cxxopts::value<int>()->default_value("150"))
            ("ch,charset", "Give the value of level of detail for charset from 1 to 3",cxxopts::value<int>()->default_value("3"))
            ("ca,char_aspect","Give the value for char aspect",cxxopts::value<double>()->default_value("0.65"))
            ("cu,custom_charset","Set a custom charset",cxxopts::value<std::string>())
            ("ht,height","Give the height of the output",cxxopts::value<int>()->default_value("0"))
            ("b,brightness","Give the brightness for the art ",cxxopts::value<double>()->default_value("0.5"))
            ("c,contrast","Give the contrast for the art ",cxxopts::value<double>()->default_value("1.5"))
            ("g,gamma","Give the gamma value for the art ",cxxopts::value<double>()->default_value("1.0"))
            ("v,version","version")
            ("h,help","See this again!");
        
        auto result = options.parse(argc,argv);

        if (result.count("help")){
            std::cout << options.help({""}) << std::endl;
            return 0;
        }

        if (result.count("version")){
            std::cout << "Version 0.1.0\n";
            return 0;
        }

        if (!result.count("input")) {
            std::cerr << "Give me an image path....\n\n" << options.help({""}) << std::endl;
            return 2;
        }

        std::string input = result["input"].as<std::string>();

        Ascii generator(input);
        
        if (result.count("custom_charset") && !result.count("charset")){
            std::string custom = result["custom_charset"].as<std::string>();
            if (custom == ""){
                std::cout << "Couldn't handle the custom charset...\n Switching to default " << std::endl;
                generator.SetCharLevel(4);
            } else {
                generator.SetCustomCharset(custom);
            }
        }

        if (result.count("charset") && !result.count("custom_charset")){
            int charset = result["charset"].as<int>();
            generator.SetCharLevel(charset);
        }

        if (result.count("charset") && result.count("custom_charset")){
            std::cerr << "Use either the custom charset or from the value set from 1 to 3....\n\n" << options.help({""}) << std::endl;
            return 3;
        }

        int width = result["width"].as<int>();
        int height = result["height"].as<int>();
        double brightness = result["brightness"].as<double>();
        double contrast = result["contrast"].as<double>();
        double gamma = result["gamma"].as<double>();
        double char_aspect = result["char_aspect"].as<double>();
        
        generator.setWidth(width);
        generator.setHeight(height);
        generator.setBrightness(brightness);
        generator.setContrast(contrast);
        generator.SetGamma(gamma);
        generator.SetCharAspect(char_aspect);



        generator.generate();
    } catch (const std::exception& e) {
        std::cerr << "Something happened! -> " << e.what() << std::endl;
        return 5;
    }
    return 0;
}