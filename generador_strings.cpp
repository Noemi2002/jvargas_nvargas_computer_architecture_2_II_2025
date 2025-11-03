#include <iostream>
#include <vector>
#include <random>
#include <cstring>
#include <immintrin.h> 

// Generador de cadenas aleatorias UTF-8 configurable
std::vector<char> generar_cadena_aleatoria(int longitud, int porcentaje_alfabeticos, bool alineado) {

    // Validar que el porcentaje esté en el rango indicado de 0-100
    if (porcentaje_alfabeticos < 0) porcentaje_alfabeticos = 0;
    if (porcentaje_alfabeticos > 100) porcentaje_alfabeticos = 100;

    // Para generar datos random
    std::random_device rd; // Semilla del sistema
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist_tipo(0, 99); // Decide si es letra, singo o número
    std::uniform_int_distribution<int> dist_letra(0, 51); 
    std::uniform_int_distribution<int> dist_otro(0, 31);

    // Reserva de memoria (alineada o no alineada)
    std::vector<char> cadena(longitud + 1); // +1 para el terminador \0

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
                cadena[i] = '!' + (otro - 10);  // caracteres ASCII no alfabéticos
        }
    }

    // Agregar el caracter nulo
    cadena[longitud] = '\0';

    // Alineación
    if (alineado) {
        // Crear un nuevo buffer alineado a 32 bytes
        void* ptr = nullptr;
        if (posix_memalign(&ptr, 32, longitud + 1) == 0) {
            std::memcpy(ptr, cadena.data(), longitud + 1);
            std::vector<char> alineada((char*)ptr, (char*)ptr + longitud + 1);
            free(ptr); // liberar temporal
            return alineada;
        }
    }

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

    std::vector<char> texto = generar_cadena_aleatoria(longitud, porcentaje, alineado);

    std::cout << "\nCadena generada (" << texto.size() - 1 << " chars):\n";
    std::cout << std::string(texto.begin(), texto.end()) << "\n";

    // Mostrar dirección de memoria para verificar alineación
    std::cout << "\nDirección inicial: " << static_cast<const void*>(texto.data()) << "\n";
    std::cout << "Alineada a 32 bytes: " << (reinterpret_cast<uintptr_t>(texto.data()) % 32 == 0 ? "Sí" : "No") << "\n";

    return 0;
}
