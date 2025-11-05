#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>
#include <cstring>
#include <cstdlib>
#include <immintrin.h>

// Para compilar y ejecutar:
// g++ -O2 -mavx2 benchmark_exhaustivo.cpp -o benchmark
// ./benchmark

// Generador de cadenas aleatorias
char* generar_cadena_aleatoria(int longitud, int porcentaje_alfabeticos, bool alineado) {
    if (porcentaje_alfabeticos < 0) porcentaje_alfabeticos = 0;
    if (porcentaje_alfabeticos > 100) porcentaje_alfabeticos = 100;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist_tipo(0, 99);
    std::uniform_int_distribution<int> dist_letra(0, 51);
    std::uniform_int_distribution<int> dist_otro(0, 31);

    char* cadena = nullptr;

    if (alineado) {
        void* ptr = nullptr;
        if (posix_memalign(&ptr, 32, longitud + 1) != 0) {
            std::cerr << "Error: No se pudo alinear la memoria\n";
            return nullptr;
        }
        cadena = static_cast<char*>(ptr);
    } else {
        cadena = new char[longitud + 1];
    }

    for (int i = 0; i < longitud; ++i) {
        int tipo = dist_tipo(gen);
        if (tipo < porcentaje_alfabeticos) {
            int letra = dist_letra(gen);
            if (letra < 26)
                cadena[i] = 'A' + letra;
            else
                cadena[i] = 'a' + (letra - 26);
        } else {
            int otro = dist_otro(gen);
            if (otro < 10)
                cadena[i] = '0' + otro;
            else
                cadena[i] = '!' + (otro - 10);
        }
    }
    cadena[longitud] = '\0';
    return cadena;
}

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

int main() {
    std::cout << "Iniciando benchmark exhaustivo...\n";
    std::cout << "Esto puede tomar varios minutos...\n\n";

    // Abrir archivo CSV para guardar resultados
    std::ofstream csv_file("resultados_benchmark.csv");
    csv_file << "tamano,alineamiento,porcentaje_alfa,tiempo_serial_ns,tiempo_simd_ns,speedup\n";

    // Definir tamaños a probar (50 tamaños diferentes)
    std::vector<int> tamanos;
    // Tamaños pequeños (múltiplos de 32)
    for (int i = 32; i <= 512; i += 32) {
        tamanos.push_back(i);
    }
    // Tamaños medianos
    for (int i = 576; i <= 2048; i += 64) {
        tamanos.push_back(i);
    }
    // Tamaños grandes
    for (int i = 2176; i <= 8192; i += 256) {
        tamanos.push_back(i);
    }

    // Definir porcentajes alfabéticos (10 valores)
    std::vector<int> porcentajes = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

    // Definir alineamientos (2 valores)
    std::vector<bool> alineamientos = {false, true};

    int total_pruebas = tamanos.size() * porcentajes.size() * alineamientos.size();
    int prueba_actual = 0;

    // Realizar mediciones
    for (int tamano : tamanos) {
        for (bool alineado : alineamientos) {
            for (int porcentaje : porcentajes) {
                prueba_actual++;
                
                if (prueba_actual % 50 == 0) {
                    std::cout << "Progreso: " << prueba_actual << "/" 
                              << total_pruebas << " pruebas completadas\n";
                }

                // Generar cadena de prueba
                char* texto = generar_cadena_aleatoria(tamano, porcentaje, alineado);
                if (texto == nullptr) continue;

                // Crear copias para cada prueba
                char* texto_serial = new char[tamano + 1];
                char* texto_simd = new char[tamano + 1];
                std::memcpy(texto_serial, texto, tamano + 1);
                std::memcpy(texto_simd, texto, tamano + 1);

                // Medir versión SERIAL
                auto start_serial = std::chrono::high_resolution_clock::now();
                case_converter_serial(texto_serial, tamano, 0);
                auto end_serial = std::chrono::high_resolution_clock::now();
                auto tiempo_serial = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    end_serial - start_serial).count();

                // Medir versión SIMD
                auto start_simd = std::chrono::high_resolution_clock::now();
                case_converter_SIMD_256(texto_simd, tamano, 0);
                auto end_simd = std::chrono::high_resolution_clock::now();
                auto tiempo_simd = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    end_simd - start_simd).count();

                // Calcular speedup
                double speedup = (tiempo_simd > 0) ? 
                    (double)tiempo_serial / tiempo_simd : 0.0;

                // Guardar resultados en CSV
                csv_file << tamano << "," 
                        << (alineado ? 1 : 0) << "," 
                        << porcentaje << "," 
                        << tiempo_serial << "," 
                        << tiempo_simd << "," 
                        << speedup << "\n";

                // Liberar memoria
                delete[] texto_serial;
                delete[] texto_simd;
                if (alineado) {
                    free(texto);
                } else {
                    delete[] texto;
                }
            }
        }
    }

    csv_file.close();
    
    std::cout << "\n¡Benchmark completado!\n";
    std::cout << "Resultados guardados en: resultados_benchmark.csv\n";
    std::cout << "Total de pruebas realizadas: " << total_pruebas << "\n";
    std::cout << "\nAhora ejecuta el script Python para generar las graficas:\n";
    std::cout << "python3 generar_graficas.py\n";

    return 0;
}
