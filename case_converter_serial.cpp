#include <iostream>
#include <vector>

std::vector<char> case_converter_serial(std::vector<char> text, int case_type){
    // Pasar a minúscula
    if (case_type == 0){
        for (char &letter : text) {
            int value = (int)letter;
            if (value >= 65 && value <= 90){
                int new_val = value + 32;
                char new_letter = (char)new_val; 
                letter = new_letter; 
            }
        }
    }
    // Pasar a mayúscula
    if (case_type == 1){
        for (char &letter : text) {
            int value = (int)letter;
            if (value >= 97 && value <= 122){
                int new_val = value -32;
                char new_letter = (char)new_val; 
                letter = new_letter; 
            }
        }
    }
    return text;
}
            




int main() {
    // Crear un vector con letras y números
    std::vector<char> my_chars = {'A', 'b', 'C', '1', 'x', '9', 'Z'};

    std::cout << "Vector original: ";
    for (char c : my_chars) {
        std::cout << c << " ";
    }
    std::cout << std::endl;

    // Convertir a minúsculas
    std::vector<char> lower = case_converter_serial(my_chars, 0);

    std::cout << "Vector en minúsculas: ";
    for (char c : lower) {
        std::cout << c << " ";
    }
    std::cout << std::endl;

    // Convertir a mayúsculas
    std::vector<char> upper = case_converter_serial(my_chars, 1);

    std::cout << "Vector en mayúsculas: ";
    for (char c : upper) {
        std::cout << c << " ";
    }
    std::cout << std::endl;

    return 0;
}
