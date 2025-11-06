#include <iostream>
#include <vector>
#include <chrono>
#include <sys/resource.h>
#include <cstring>
// Para ejecutar
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

long get_memory_usage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss; // En KB en Linux
}
            
int main() {
    // Crear un vector con letras y números (1024 caracteres para mediciones más precisas)
    std::vector<char> my_chars;
    const char test_string[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    for (int i = 0; i < 1024; i++) {
        my_chars.push_back(test_string[i % strlen(test_string)]);
    }
    
    std::cout << "Tamaño del vector: " << my_chars.size() << " caracteres" << std::endl;
    std::cout << "Primeros 50 caracteres: ";
    for (int i = 0; i < 50 && i < my_chars.size(); i++) {
        std::cout << my_chars[i];
    }
    std::cout << "..." << std::endl;
    
    // Medición de memoria inicial
    long mem_before = get_memory_usage();
    
    // Convertir a minúsculas
    std::vector<char> lower = my_chars;
    auto start = std::chrono::high_resolution_clock::now();
    case_converter_serial(lower.data(), lower.size(), 0);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    
    long mem_after = get_memory_usage();
    
    std::cout << "\nVector en minúsculas (primeros 50): ";
    for (int i = 0; i < 50 && i < lower.size(); i++) {
        std::cout << lower[i];
    }
    std::cout << "..." << std::endl;
    std::cout << "Tiempo de ejecución (minúsculas): " << duration.count() << " ns" << std::endl;
    std::cout << "Memoria utilizada: " << (mem_after - mem_before) << " KB" << std::endl;
    std::cout << "Throughput: " << (lower.size() * 1000.0 / duration.count()) 
              << " chars/us" << std::endl;
    
    // Convertir a mayúsculas
    mem_before = get_memory_usage();
    std::vector<char> upper = my_chars;
    start = std::chrono::high_resolution_clock::now();
    case_converter_serial(upper.data(), upper.size(), 1);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    mem_after = get_memory_usage();
    
    std::cout << "\nVector en mayúsculas (primeros 50): ";
    for (int i = 0; i < 50 && i < upper.size(); i++) {
        std::cout << upper[i];
    }
    std::cout << "..." << std::endl;
    std::cout << "Tiempo de ejecución (mayúsculas): " << duration.count() << " ns" << std::endl;
    std::cout << "Memoria utilizada: " << (mem_after - mem_before) << " KB" << std::endl;
    std::cout << "Throughput: " << (upper.size() * 1000.0 / duration.count()) 
              << " chars/us" << std::endl;
    
    return 0;
}
