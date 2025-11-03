#include <iostream>
#include <vector>

//Para ejecutar
// g++ -O2 case_converter_serial.cpp -o case_serial
// ./case_serial 

void case_converter_serial(char* text, int m, int case_type) {
    if (case_type == 0) {
        for (int i = 0; i < m; i++) {
            int val = (int)text[i];
            if (val >= 65 && val <= 90)
                text[i] = (char)(val + 32);
        }
    } else if (case_type == 1) {
        for (int i = 0; i < m; i++) {
            int val = (int)text[i];
            if (val >= 97 && val <= 122)
                text[i] = (char)(val - 32);
        }
    }
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
    std::vector<char> lower = my_chars;  // Crear una copia
    case_converter_serial(lower.data(), lower.size(), 0);  // Pasar puntero y tamaño

    std::cout << "Vector en minúsculas: ";
    for (char c : lower) {
        std::cout << c << " ";
    }
    std::cout << std::endl;

    // Convertir a mayúsculas
    std::vector<char> upper = my_chars;  // Crear otra copia del original
    case_converter_serial(upper.data(), upper.size(), 1);  // Pasar puntero y tamaño

    std::cout << "Vector en mayúsculas: ";
    for (char c : upper) {
        std::cout << c << " ";
    }
    std::cout << std::endl;

    return 0;
}