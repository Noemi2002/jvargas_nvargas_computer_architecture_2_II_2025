// Para compilar en ARM:
// g++ -O2 -march=armv8-a+simd LLM_case_converter_simd_arm.cpp -o main_neon
// ./main_neon

#include <arm_neon.h>    // Para intrinsics ARM NEON
#include <cstddef> 
#include <vector>
#include <cstring>
#include <iostream>


// Declaración de la función serial (sin cambios)
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

            

void case_converter_SIMD_NEON(char* text, int m, int case_type) {

    // Constantes - NEON procesa 16 bytes (128 bits) en lugar de 32
    const int VECTOR_SIZE = 16;

    // Procesar bloques completos de 16 caracteres
    int i = 0;
    while ((i + VECTOR_SIZE) <= m) {

        // 1. Cargar 16 caracteres con la función intrinsics NEON
        int8x16_t vec = vld1q_s8((const int8_t*)&text[i]);

        // 2. Crear máscara de detección
        int8x16_t lower, upper;
        if (case_type == 0) {    
            // Convertir a minúsculas                      
            lower = vdupq_n_s8('A' - 1); // 64
            upper = vdupq_n_s8('Z' + 1); // 91
        } else {
            // Convertir a mayúsculas                                        
            lower = vdupq_n_s8('a' - 1); // 96
            upper = vdupq_n_s8('z' + 1); // 123
        }

        // Máscaras de comparación
        uint8x16_t mask1 = vcgtq_s8(vec, lower); // vec > lower
        uint8x16_t mask2 = vcgtq_s8(upper, vec); // upper > vec

        // Combinar máscaras, es como un AND lógico para verificar que las letras estén dentro del rango
        uint8x16_t mask = vandq_u8(mask1, mask2);

        // 3. Aplicar conversión condicional
        int8x16_t offset, converted;
        if (case_type == 0) {
            offset = vdupq_n_s8(32);   // Sumar 32 para convertir a minúsculas
            converted = vaddq_s8(vec, offset);
        } else {
            offset = vdupq_n_s8(-32);  // Restar 32 para convertir a mayúsculas
            converted = vaddq_s8(vec, offset);
        }

        // Aplicar mezcla condicional usando la máscara
        // vbslq: bit select - si mask[i] entonces converted[i], sino vec[i]
        int8x16_t result = vbslq_s8(mask, converted, vec);

        // 4. Almacenar resultado
        vst1q_s8((int8_t*)&text[i], result);

        i = i + VECTOR_SIZE;
    }

    // De forma serial, se llama a la función
    if (i < m) {
        int remaining = m - i;
        case_converter_serial(&text[i], remaining, case_type);
    }
}


int main() {
    // Crear un texto largo con letras, espacios y números (más de 128 caracteres)
    std::string long_text =
        "ARM NEON CONVERSION TEST. "
        "This text includes UPPERCASE, lowercase, and NUMBERS 1234567890. "
        "It repeats several times to make sure we exceed 16, 32, 48, 64 bytes. "
        "abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ.";

    // Copiar el texto al vector
    std::vector<char> my_chars(long_text.begin(), long_text.end());

    std::cout << "Tamaño total del texto: " << my_chars.size() << " caracteres\n\n";

    // Mostrar parte del texto original
    std::cout << "Texto original (inicio): " << std::string(my_chars.begin(), my_chars.begin() + 60) << "...\n\n";

    // ===== Convertir a minúsculas =====
    std::vector<char> lower = my_chars;
    case_converter_SIMD_NEON(lower.data(), lower.size(), 0);

    std::cout << "Texto en minúsculas (inicio): " << std::string(lower.begin(), lower.begin() + 60) << "...\n\n";

    // ===== Convertir a mayúsculas =====
    std::vector<char> upper = my_chars;
    case_converter_SIMD_NEON(upper.data(), upper.size(), 1);

    std::cout << "Texto en mayúsculas (inicio): " << std::string(upper.begin(), upper.begin() + 60) << "...\n\n";

    // Mostrar parte final del texto también
    std::cout << "Texto original (final):   " << std::string(my_chars.end() - 60, my_chars.end()) << "\n";
    std::cout << "Texto en minúsculas (final): " << std::string(lower.end() - 60, lower.end()) << "\n";
    std::cout << "Texto en mayúsculas (final): " << std::string(upper.end() - 60, upper.end()) << "\n";

    return 0;
}
