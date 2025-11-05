#include <iostream>
#include <vector>
#include <cstring>
#include <immintrin.h>

// Para ejecutar
// g++ -O2 -mavx2 validacion_correctitud.cpp -o validacion
// ./validacion

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
    const int VECTOR_SIZE = 32;
    int i = 0;
    
    while ((i + VECTOR_SIZE) <= m) {
        __m256i vec = _mm256_loadu_si256((__m256i*)&text[i]);
        
        __m256i lower, upper;
        if (case_type == 0) {
            lower = _mm256_set1_epi8('A' - 1);
            upper = _mm256_set1_epi8('Z' + 1);
        } else {
            lower = _mm256_set1_epi8('a' - 1);
            upper = _mm256_set1_epi8('z' + 1);
        }
        
        __m256i mask1 = _mm256_cmpgt_epi8(vec, lower);
        __m256i mask2 = _mm256_cmpgt_epi8(upper, vec);
        __m256i mask = _mm256_and_si256(mask1, mask2);
        
        __m256i offset, converted;
        if (case_type == 0) {
            offset = _mm256_set1_epi8(32);
            converted = _mm256_add_epi8(vec, offset);
        } else {
            offset = _mm256_set1_epi8(-32);
            converted = _mm256_add_epi8(vec, offset);
        }
        
        __m256i result = _mm256_blendv_epi8(vec, converted, mask);
        _mm256_storeu_si256((__m256i*)&text[i], result);
        
        i = i + VECTOR_SIZE;
    }
    
    if (i < m) {
        int remaining = m - i;
        case_converter_serial(&text[i], remaining, case_type);
    }
}

bool validar_conversion(const std::string& texto_prueba, int case_type) {
    // Crear dos copias de la misma cadena
    std::vector<char> copia_serial(texto_prueba.begin(), texto_prueba.end());
    std::vector<char> copia_simd(texto_prueba.begin(), texto_prueba.end());
    
    // Aplicar conversión serial
    case_converter_serial(copia_serial.data(), copia_serial.size(), case_type);
    
    // Aplicar conversión SIMD
    case_converter_SIMD_256(copia_simd.data(), copia_simd.size(), case_type);
    
    // Comparar byte por byte
    bool son_iguales = true;
    for (size_t i = 0; i < copia_serial.size(); i++) {
        if (copia_serial[i] != copia_simd[i]) {
            son_iguales = false;
            std::cout << "Diferencia encontrada en posicion " << i << ":\n";
            std::cout << "  Serial: '" << copia_serial[i] << "' (ASCII " << (int)copia_serial[i] << ")\n";
            std::cout << "  SIMD:   '" << copia_simd[i] << "' (ASCII " << (int)copia_simd[i] << ")\n";
            break;
        }
    }
    
    return son_iguales;
}

int main() {
    std::cout << "=== VALIDACION DE CORRECTITUD: SERIAL vs SIMD ===\n\n";
    
    // Caso de prueba 1: Texto mixto
    std::string test1 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%";
    std::cout << "Prueba 1: Texto mixto (" << test1.size() << " caracteres)\n";
    std::cout << "Texto original: " << test1 << "\n\n";
    
    std::cout << "Validando conversion a minusculas... ";
    bool resultado1_lower = validar_conversion(test1, 0);
    std::cout << (resultado1_lower ? "✓ CORRECTA" : "✗ ERROR") << "\n\n";
    
    std::cout << "Validando conversion a mayusculas... ";
    bool resultado1_upper = validar_conversion(test1, 1);
    std::cout << (resultado1_upper ? "✓ CORRECTA" : "✗ ERROR") << "\n\n";
    
    // Caso de prueba 2: Texto largo (múltiplo de 32)
    std::string test2 = "The Quick BROWN Fox Jumps Over THE Lazy DOG 1234567890 !@#$% abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::cout << "Prueba 2: Texto largo (" << test2.size() << " caracteres)\n";
    std::cout << "Texto original (primeros 60): " << test2.substr(0, 60) << "...\n\n";
    
    std::cout << "Validando conversion a minusculas... ";
    bool resultado2_lower = validar_conversion(test2, 0);
    std::cout << (resultado2_lower ? "✓ CORRECTA" : "✗ ERROR") << "\n\n";
    
    std::cout << "Validando conversion a mayusculas... ";
    bool resultado2_upper = validar_conversion(test2, 1);
    std::cout << (resultado2_upper ? "✓ CORRECTA" : "✗ ERROR") << "\n\n";
    
    // Caso de prueba 3: Texto corto (menor a 32)
    std::string test3 = "Hello World 123";
    std::cout << "Prueba 3: Texto corto (" << test3.size() << " caracteres)\n";
    std::cout << "Texto original: " << test3 << "\n\n";
    
    std::cout << "Validando conversion a minusculas... ";
    bool resultado3_lower = validar_conversion(test3, 0);
    std::cout << (resultado3_lower ? "✓ CORRECTA" : "✗ ERROR") << "\n\n";
    
    std::cout << "Validando conversion a mayusculas... ";
    bool resultado3_upper = validar_conversion(test3, 1);
    std::cout << (resultado3_upper ? "✓ CORRECTA" : "✗ ERROR") << "\n\n";
    
    // Resumen final
    int total_pruebas = 6;
    int pruebas_correctas = resultado1_lower + resultado1_upper + 
                            resultado2_lower + resultado2_upper + 
                            resultado3_lower + resultado3_upper;
    
    std::cout << "=== RESUMEN ===\n";
    std::cout << "Pruebas exitosas: " << pruebas_correctas << "/" << total_pruebas << "\n";
    
    if (pruebas_correctas == total_pruebas) {
        std::cout << "✓ TODAS LAS VALIDACIONES PASARON CORRECTAMENTE\n";
    } else {
        std::cout << "✗ ALGUNAS VALIDACIONES FALLARON\n";
    }
    
    return 0;
}
