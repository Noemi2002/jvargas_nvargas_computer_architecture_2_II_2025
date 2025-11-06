# Taller 2: Extensiones SIMD e Intrinsics
## CE-4302 Arquitectura de Computadores II

**Estudiantes:**
- Noemí Vargas Soto - 2021082564
- José Andrés Vargas Torres - 2021453583

**Profesor:** Luis Alonso Barboza Artavia  
**Semestre:** II 2025

---

## Descripción

Este proyecto implementa algoritmos de conversión de mayúsculas/minúsculas utilizando técnicas de vectorización SIMD con intrinsics AVX2. Se compara el rendimiento entre una implementación serial tradicional y una versión optimizada con SIMD de 256 bits.

---

## Requisitos del Sistema

- **Sistema Operativo:** Linux (no virtualizado)
- **Compilador:** GCC con soporte para C++11 o superior
- **Extensiones SIMD:** Procesador con soporte AVX2
- **Python:** Versión 3.6 o superior (para generación de gráficas)
- **Librerías Python:** pandas, matplotlib, numpy

### Instalación de dependencias Python
```bash
pip install pandas matplotlib numpy
```

### Verificación de soporte AVX2
```bash
lscpu | grep avx2
# O también:
cat /proc/cpuinfo | grep avx2
```

---

## Estructura de Archivos
```
.
├── case_converter_serial.cpp          # Implementación serial con medición de desempeño
├── case_converter_SIMD_256.cpp        # Implementación SIMD con AVX2
├── validaciones_correctitud.cpp       # Programa de validación serial vs SIMD
├── generador_strings.cpp              # Generador de cadenas aleatorias configurables
├── benchmark_exhaustivo.cpp           # Benchmark completo con múltiples configuraciones
├── generar_graficas.py                # Script Python para generación de gráficas
├── LLM_case_converter_simd_arm.cpp    # Traducción ARM NEON generada por LLM
└── README.md                          # Este archivo
```

---

## Compilación

### 1. Generador de cadenas aleatorias
```bash
g++ -O2 generador_strings.cpp -o generador_strings
```

### 2. Implementación serial
```bash
g++ -O2 case_converter_serial.cpp -o case_serial
```

### 3. Implementación SIMD AVX2
```bash
g++ -O2 -mavx2 case_converter_SIMD_256.cpp -o case_simd
```

### 4. Validación de correctitud
```bash
g++ -O2 -mavx2 validaciones_correctitud.cpp -o validacion
```

### 5. Benchmark exhaustivo
```bash
g++ -O2 -mavx2 benchmark_exhaustivo.cpp -o benchmark
```

---

## Ejecución

### Pruebas individuales

#### Generador de cadenas aleatorias
```bash
./generador_strings
```
El programa solicitará:
- Longitud de la cadena
- Porcentaje de caracteres alfabéticos (0-100)
- Si desea alineamiento a 32 bytes (1=Sí, 0=No)

#### Algoritmo serial
```bash
./case_serial
```
Ejecuta la conversión serial sobre un texto de prueba de 1024 caracteres y muestra métricas de desempeño.

#### Algoritmo SIMD
```bash
./case_simd
```
Ejecuta la conversión SIMD sobre un texto de prueba y muestra los resultados de conversión.

#### Validación de correctitud
```bash
./validacion
```
Compara byte por byte los resultados de las implementaciones serial y SIMD sobre 3 casos de prueba diferentes.

---

## Benchmarking y Generación de Gráficas

### 1. Ejecutar benchmark exhaustivo
```bash
./benchmark
```

Este programa:
- Realiza **1000 mediciones** combinando:
  - 50 tamaños de cadena (32 a 8192 bytes)
  - 2 tipos de alineamiento (alineado/no alineado)
  - 10 porcentajes alfabéticos (0% a 100%)
- Genera el archivo `resultados_benchmark.csv`
- **Tiempo estimado:** 5-10 minutos

### 2. Generar gráficas
```bash
python3 generar_graficas.py
```

Este script:
- Lee `resultados_benchmark.csv`
- Genera 5 gráficas en el directorio `graficas/`:
  1. `tiempo_vs_tamano.png` - Comparación de tiempos de ejecución
  2. `speedup_vs_tamano.png` - Speedup SIMD vs Serial
  3. `efecto_porcentaje_alfabetico.png` - Impacto del porcentaje alfabético
  4. `comparacion_alineamiento.png` - Efecto del alineamiento
  5. `heatmap_speedup.png` - Heatmap de speedup
- Muestra estadísticas en consola

---

## Traducción ISA con LLM

### Código ARM NEON
El archivo `LLM_case_converter_simd_arm.cpp` contiene la traducción de AVX2 a ARM NEON generada por Claude (Anthropic).

### Validación en Compiler Explorer
**Enlace:** https://godbolt.org/z/vofKEa41r

**Configuración utilizada:**
- Compilador: ARM64 GCC 13.2.0
- Flags: `-O2 -march=armv8-a+simd`
- Resultado: Compilación exitosa sin errores

---

## Resultados Esperados

### Validación de Correctitud
Todas las pruebas deben mostrar:
```
Validando conversión a minúsculas... CORRECTA
Validando conversión a mayúsculas... CORRECTA
TODAS LAS VALIDACIONES PASARON CORRECTAMENTE
```

### Métricas de Desempeño (Aproximadas)
- **Speedup promedio:** 40-50x
- **Speedup máximo:** ~50x (para cadenas grandes)
- **Impacto de alineamiento:** Mínimo en hardware moderno (<5% diferencia)

---

## Justificación de Rangos de Valores

### Tamaños de cadena (32-8192 bytes)
- **Mínimo (32 bytes):** Tamaño de un vector AVX2
- **Máximo (8192 bytes):** ~2x tamaño típico de caché L1
- **Distribución:** 50 valores para observar comportamiento desde vectorización mínima hasta múltiples iteraciones

### Porcentajes alfabéticos (0%-100%)
- Evalúa si la proporción de caracteres convertibles afecta el rendimiento
- Pasos del 10% cubren desde textos sin letras hasta textos puramente alfabéticos

### Alineamientos (2 valores)
- **Alineado:** Caso óptimo (32 bytes)
- **No alineado:** Caso realista de datos externos

---

## Notas Importantes

1. **NO incluir ejecutables** en el archivo ZIP de entrega
2. Los archivos `.cpp` deben compilarse en el sistema del evaluador
3. El benchmark puede tardar varios minutos en completarse
4. Las gráficas se generan automáticamente en el directorio `graficas/`
5. Verificar soporte AVX2 antes de ejecutar implementaciones SIMD

---

## Troubleshooting

### Error: "illegal instruction"
- **Causa:** Procesador sin soporte AVX2
- **Solución:** Verificar con `lscpu | grep avx2`

### Error al compilar SIMD
- **Causa:** Falta flag `-mavx2`
- **Solución:** Incluir `-mavx2` en comando de compilación

### Script Python no encuentra el CSV
- **Causa:** No se ejecutó el benchmark
- **Solución:** Ejecutar `./benchmark` primero

### Dependencias Python faltantes
```bash
pip install --user pandas matplotlib numpy
```

---

## Referencias

- Intel Intrinsics Guide: https://www.intel.com/content/www/us/en/docs/intrinsics-guide/
- Compiler Explorer: https://godbolt.org/
- Documentación AVX2: https://gcc.gnu.org/onlinedocs/gcc/x86-Built-in-Functions.html

---

**Fecha de entrega:** 30/10/25 (grupo 02) | 31/10/25 (grupo 01)
