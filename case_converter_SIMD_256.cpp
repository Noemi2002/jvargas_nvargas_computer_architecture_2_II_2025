// Para ejecutar
// g++ -O2 -mavx2 case_converter_SIMD_256.cpp -o main
// ./main


#include <immintrin.h>   // Para intrinsics AVX2
#include <cstddef> 
#include <vector>
#include <cstring>
#include <iostream>


// Declaración de la función serial con algunos cambios para acoplarse con la funicón SIMD
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

            

void case_converter_SIMD_256(char* text, int m, int case_type) {

    // Constantes
    const int VECTOR_SIZE = 32;

    // Procesar bloques completos de 32 caracteres
    int i = 0;
    while ((i + VECTOR_SIZE) <= m) {

        // 1. Cargar 32 caracteres con la función intrinsics
        __m256i vec = _mm256_loadu_si256((__m256i*)&text[i]);

        // 2. Crear máscara de detección
        __m256i lower, upper;
        if (case_type == 0) {    
            // Convertir a minúsculas                      
            lower = _mm256_set1_epi8('A' - 1); // 64
            upper = _mm256_set1_epi8('Z' + 1); // 91
        } else {
            // Convertir a mayúsculas                                        
            lower = _mm256_set1_epi8('a' - 1); // 96
            upper = _mm256_set1_epi8('z' + 1); // 123
        }

        // Máscaras de comparación
        __m256i mask1 = _mm256_cmpgt_epi8(vec, lower); // vec > lower
        __m256i mask2 = _mm256_cmpgt_epi8(upper, vec); // upper > vec

        // Combinar máscaras, es como un AND lógico para verificar que las letras estén dentro del rango
        __m256i mask = _mm256_and_si256(mask1, mask2);

        // 3. Aplicar conversión condicional
        __m256i offset, converted;
        if (case_type == 0) {
            offset = _mm256_set1_epi8(32);   // Sumar 32 para convertir a minúsculas
            converted = _mm256_add_epi8(vec, offset);
        } else {
            offset = _mm256_set1_epi8(-32);  // Restar 32 para convertir a mayúsculas
            converted = _mm256_add_epi8(vec, offset);
        }

        // Aplicar mezcla condicional usando la máscara
        __m256i result = _mm256_blendv_epi8(vec, converted, mask);

        // 4. Almacenar resultado
        _mm256_storeu_si256((__m256i*)&text[i], result);

        i = i + VECTOR_SIZE;
    }

    // De forma serial, se llama a la función
    if (i < m) {
        int remaining = m - i;
        case_converter_serial(&text[i], m, case_type);
    }


    // De forma serial, se llama a la función (versión vector - original)
    //if (i < m) {
      //  int remaining = m - i;

        // Crear un vector temporal con los caracteres restantes
        //std::vector<char> tail(text + i, text + i + remaining);

        // Procesar con la versión que devuelve un nuevo vector
        //std::vector<char> converted_tail = case_converter_serial(tail, case_type);

        // Copiar el resultado de vuelta al texto original
        //std::memcpy(&text[i], converted_tail.data(), remaining);
//}

}


int main() {
    // Crear un texto largo con letras, espacios y números (más de 128 caracteres)
    std::string long_text =
        "AVX2 SIMD CONVERSION TEST. "
        "This text includes UPPERCASE, lowercase, and NUMBERS 1234567890. "
        "It repeats several times to make sure we exceed 32, 64, 96, 128 bytes. "
        "abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ.";

    // Copiar el texto al vector
    std::vector<char> my_chars(long_text.begin(), long_text.end());

    std::cout << "Tamaño total del texto: " << my_chars.size() << " caracteres\n\n";

    // Mostrar parte del texto original
    std::cout << "Texto original (inicio): " << std::string(my_chars.begin(), my_chars.begin() + 60) << "...\n\n";

    // ===== Convertir a minúsculas =====
    std::vector<char> lower = my_chars;
    case_converter_SIMD_256(lower.data(), lower.size(), 0);

    std::cout << "Texto en minúsculas (inicio): " << std::string(lower.begin(), lower.begin() + 60) << "...\n\n";

    // ===== Convertir a mayúsculas =====
    std::vector<char> upper = my_chars;
    case_converter_SIMD_256(upper.data(), upper.size(), 1);

    std::cout << "Texto en mayúsculas (inicio): " << std::string(upper.begin(), upper.begin() + 60) << "...\n\n";

    // Mostrar parte final del texto también
    std::cout << "Texto original (final):   " << std::string(my_chars.end() - 60, my_chars.end()) << "\n";
    std::cout << "Texto en minúsculas (final): " << std::string(lower.end() - 60, lower.end()) << "\n";
    std::cout << "Texto en mayúsculas (final): " << std::string(upper.end() - 60, upper.end()) << "\n";

    return 0;
}
