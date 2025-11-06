#include <iostream>
#include <random>
#include <cstring>
#include <cstdlib>

// Generador de cadenas aleatorias UTF-8 configurable
char* generar_cadena_aleatoria(int longitud, int porcentaje_alfabeticos, bool alineado) {

    // Validar que el porcentaje esté en el rango indicado de 0-100
    if (porcentaje_alfabeticos < 0) porcentaje_alfabeticos = 0;
    if (porcentaje_alfabeticos > 100) porcentaje_alfabeticos = 100;

    // Para generar datos random
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist_tipo(0, 99);
    std::uniform_int_distribution<int> dist_letra(0, 51); 
    std::uniform_int_distribution<int> dist_otro(0, 31);

    char* cadena = nullptr;

    if (alineado) {
        // Crear buffer alineado a 32 bytes
        void* ptr = nullptr;
        if (posix_memalign(&ptr, 32, longitud + 1) != 0) {
            std::cerr << "Error: No se pudo alinear la memoria\n";
            return nullptr;
        }
        cadena = static_cast<char*>(ptr);
    } else {
        // Sin alineación - usar malloc normal
        cadena = new char[longitud + 1];
    }

    // Generar los caracteres
    for (int i = 0; i < longitud; ++i) {
        int tipo = dist_tipo(gen);
        if (tipo < porcentaje_alfabeticos) {
            // Generar letra aleatoria, ya sea mayúscula o minúscula
            int letra = dist_letra(gen);
            if (letra < 26)
                cadena[i] = 'A' + letra;
            else
                cadena[i] = 'a' + (letra - 26);
        } else {
            // Generar un símbolo o número
            int otro = dist_otro(gen);
            if (otro < 10)
                cadena[i] = '0' + otro;
            else
                cadena[i] = '!' + (otro - 10);
        }
    }

    // Agregar el caracter nulo
    cadena[longitud] = '\0';

    return cadena;
}


int main() {
    int longitud, porcentaje;
    bool alineado;

    std::cout << "Ingrese longitud de la cadena: ";
    std::cin >> longitud;

    std::cout << "Porcentaje de caracteres alfabéticos (0-100): ";
    std::cin >> porcentaje;

    std::cout << "¿Desea que la cadena esté alineada a 32 bytes? (1 = Sí, 0 = No): ";
    std::cin >> alineado;

    char* texto = generar_cadena_aleatoria(longitud, porcentaje, alineado);

    if (texto != nullptr) {
        std::cout << "\nCadena generada (" << longitud << " chars):\n";
        std::cout << texto << "\n";

        // Mostrar dirección de memoria para verificar alineación
        std::cout << "\nDirección inicial: " << static_cast<const void*>(texto) << "\n";
        bool is_aligned = (reinterpret_cast<uintptr_t>(texto) % 32 == 0);
        std::cout << "Alineada a 32 bytes: " << (is_aligned ? "Sí" : "No") << "\n";

        // Liberar memoria correctamente según cómo fue asignada
        if (alineado) {
            free(texto);
        } else {
            delete[] texto;
        }
    }

    return 0;
}
