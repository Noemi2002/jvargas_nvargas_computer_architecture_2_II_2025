#!/usr/bin/env python3
"""
Script para generar gráficas de los resultados del benchmark
Requisitos: pip install pandas matplotlib numpy

Para ejecutar:
python3 generar_graficas.py
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

# Leer datos del CSV
print("Leyendo resultados del benchmark...")
df = pd.read_csv('resultados_benchmark.csv')

# Crear directorio para las gráficas
os.makedirs('graficas', exist_ok=True)

# Configurar estilo de gráficas
plt.style.use('seaborn-v0_8-darkgrid')
colores_serial = '#FF6B6B'
colores_simd_alineado = '#4ECDC4'
colores_simd_no_alineado = '#95E1D3'

print(f"Total de mediciones: {len(df)}")
print(f"Tamaños probados: {df['tamano'].nunique()}")
print(f"Porcentajes probados: {df['porcentaje_alfa'].nunique()}")

# ============================================================
# Gráfica 1: Tiempo de ejecución vs Tamaño (porcentaje fijo)
# ============================================================
print("\nGenerando Gráfica 1: Tiempo vs Tamaño...")

porcentaje_ref = 50
df_50 = df[df['porcentaje_alfa'] == porcentaje_ref]

fig, ax = plt.subplots(figsize=(12, 7))

# Datos alineados
df_alineado = df_50[df_50['alineamiento'] == 1]
ax.plot(df_alineado['tamano'], df_alineado['tiempo_serial_ns'], 
        'o-', label='Serial', color=colores_serial, linewidth=2, markersize=4)
ax.plot(df_alineado['tamano'], df_alineado['tiempo_simd_ns'], 
        's-', label='SIMD (Alineado)', color=colores_simd_alineado, 
        linewidth=2, markersize=4)

# Datos no alineados
df_no_alineado = df_50[df_50['alineamiento'] == 0]
ax.plot(df_no_alineado['tamano'], df_no_alineado['tiempo_simd_ns'], 
        '^-', label='SIMD (No Alineado)', color=colores_simd_no_alineado, 
        linewidth=2, markersize=4)

ax.set_xlabel('Tamaño de la cadena (bytes)', fontsize=12, fontweight='bold')
ax.set_ylabel('Tiempo de ejecución (ns)', fontsize=12, fontweight='bold')
ax.set_title(f'Comparación de Tiempo de Ejecución\n(Porcentaje alfabético: {porcentaje_ref}%)', 
             fontsize=14, fontweight='bold')
ax.legend(fontsize=10)
ax.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig('graficas/tiempo_vs_tamano.png', dpi=300, bbox_inches='tight')
plt.close()

# ============================================================
# Gráfica 2: Speedup vs Tamaño (porcentaje fijo)
# ============================================================
print("Generando Gráfica 2: Speedup vs Tamaño...")

fig, ax = plt.subplots(figsize=(12, 7))

ax.plot(df_alineado['tamano'], df_alineado['speedup'], 
        's-', label='SIMD Alineado', color=colores_simd_alineado, 
        linewidth=2, markersize=5)
ax.plot(df_no_alineado['tamano'], df_no_alineado['speedup'], 
        '^-', label='SIMD No Alineado', color=colores_simd_no_alineado, 
        linewidth=2, markersize=5)

# Línea de referencia en speedup=1
ax.axhline(y=1, color='gray', linestyle='--', linewidth=1, alpha=0.5)

ax.set_xlabel('Tamaño de la cadena (bytes)', fontsize=12, fontweight='bold')
ax.set_ylabel('Speedup (Serial / SIMD)', fontsize=12, fontweight='bold')
ax.set_title(f'Speedup de SIMD sobre Serial\n(Porcentaje alfabético: {porcentaje_ref}%)', 
             fontsize=14, fontweight='bold')
ax.legend(fontsize=10)
ax.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig('graficas/speedup_vs_tamano.png', dpi=300, bbox_inches='tight')
plt.close()

# ============================================================
# Gráfica 3: Efecto del porcentaje alfabético
# ============================================================
print("Generando Gráfica 3: Efecto del porcentaje alfabético...")

# Usar un tamaño fijo representativo
tamano_ref = 1024
df_1024 = df[(df['tamano'] == tamano_ref) & (df['alineamiento'] == 1)]

fig, ax = plt.subplots(figsize=(12, 7))

ax.plot(df_1024['porcentaje_alfa'], df_1024['tiempo_serial_ns'], 
        'o-', label='Serial', color=colores_serial, linewidth=2, markersize=6)
ax.plot(df_1024['porcentaje_alfa'], df_1024['tiempo_simd_ns'], 
        's-', label='SIMD', color=colores_simd_alineado, linewidth=2, markersize=6)

ax.set_xlabel('Porcentaje de caracteres alfabéticos (%)', fontsize=12, fontweight='bold')
ax.set_ylabel('Tiempo de ejecución (ns)', fontsize=12, fontweight='bold')
ax.set_title(f'Impacto del Porcentaje Alfabético en el Tiempo\n(Tamaño: {tamano_ref} bytes, Alineado)', 
             fontsize=14, fontweight='bold')
ax.legend(fontsize=10)
ax.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig('graficas/efecto_porcentaje_alfabetico.png', dpi=300, bbox_inches='tight')
plt.close()

# ============================================================
# Gráfica 4: Comparación de alineamiento
# ============================================================
print("Generando Gráfica 4: Comparación de alineamiento...")

df_comp = df[(df['tamano'] == tamano_ref) & (df['porcentaje_alfa'] == porcentaje_ref)]

fig, ax = plt.subplots(figsize=(10, 7))

categorias = ['SIMD\nAlineado', 'SIMD\nNo Alineado']
tiempos_simd = [
    df_comp[df_comp['alineamiento'] == 1]['tiempo_simd_ns'].values[0],
    df_comp[df_comp['alineamiento'] == 0]['tiempo_simd_ns'].values[0]
]
speedups = [
    df_comp[df_comp['alineamiento'] == 1]['speedup'].values[0],
    df_comp[df_comp['alineamiento'] == 0]['speedup'].values[0]
]

x_pos = np.arange(len(categorias))
bars = ax.bar(x_pos, tiempos_simd, color=[colores_simd_alineado, colores_simd_no_alineado],
              edgecolor='black', linewidth=1.5)

# Añadir valores en las barras
for i, (bar, speedup) in enumerate(zip(bars, speedups)):
    height = bar.get_height()
    ax.text(bar.get_x() + bar.get_width()/2., height,
            f'{int(height)} ns\nSpeedup: {speedup:.2f}x',
            ha='center', va='bottom', fontsize=11, fontweight='bold')

ax.set_xlabel('Tipo de Implementación', fontsize=12, fontweight='bold')
ax.set_ylabel('Tiempo de ejecución (ns)', fontsize=12, fontweight='bold')
ax.set_title(f'Impacto del Alineamiento en SIMD\n(Tamaño: {tamano_ref} bytes, Porcentaje alfabético: {porcentaje_ref}%)', 
             fontsize=14, fontweight='bold')
ax.set_xticks(x_pos)
ax.set_xticklabels(categorias)
ax.grid(True, alpha=0.3, axis='y')
plt.tight_layout()
plt.savefig('graficas/comparacion_alineamiento.png', dpi=300, bbox_inches='tight')
plt.close()

# ============================================================
# Gráfica 5: Heatmap de Speedup
# ============================================================
print("Generando Gráfica 5: Heatmap de Speedup...")

# Crear pivot table para heatmap (solo alineado)
df_alineado_completo = df[df['alineamiento'] == 1]
pivot_speedup = df_alineado_completo.pivot_table(
    values='speedup', 
    index='porcentaje_alfa', 
    columns='tamano', 
    aggfunc='mean'
)

fig, ax = plt.subplots(figsize=(16, 8))
im = ax.imshow(pivot_speedup.values, cmap='RdYlGn', aspect='auto', 
               vmin=0, vmax=pivot_speedup.values.max())

# Configurar ejes
ax.set_xticks(np.arange(len(pivot_speedup.columns))[::5])
ax.set_xticklabels(pivot_speedup.columns[::5], rotation=45)
ax.set_yticks(np.arange(len(pivot_speedup.index)))
ax.set_yticklabels(pivot_speedup.index)

ax.set_xlabel('Tamaño de la cadena (bytes)', fontsize=12, fontweight='bold')
ax.set_ylabel('Porcentaje alfabético (%)', fontsize=12, fontweight='bold')
ax.set_title('Heatmap de Speedup SIMD vs Serial (Datos Alineados)', 
             fontsize=14, fontweight='bold')

# Añadir colorbar
cbar = plt.colorbar(im, ax=ax)
cbar.set_label('Speedup', fontsize=12, fontweight='bold')

plt.tight_layout()
plt.savefig('graficas/heatmap_speedup.png', dpi=300, bbox_inches='tight')
plt.close()

# ============================================================
# Estadísticas finales
# ============================================================
print("\n" + "="*60)
print("ESTADÍSTICAS DEL BENCHMARK")
print("="*60)

df_alineado = df[df['alineamiento'] == 1]
df_no_alineado = df[df['alineamiento'] == 0]

print(f"\nSpeedup promedio (Alineado): {df_alineado['speedup'].mean():.2f}x")
print(f"Speedup máximo (Alineado): {df_alineado['speedup'].max():.2f}x")
print(f"Speedup mínimo (Alineado): {df_alineado['speedup'].min():.2f}x")

print(f"\nSpeedup promedio (No Alineado): {df_no_alineado['speedup'].mean():.2f}x")
print(f"Speedup máximo (No Alineado): {df_no_alineado['speedup'].max():.2f}x")
print(f"Speedup mínimo (No Alineado): {df_no_alineado['speedup'].min():.2f}x")

mejor_caso = df_alineado.loc[df_alineado['speedup'].idxmax()]
print(f"\nMejor caso de speedup:")
print(f"  Tamaño: {int(mejor_caso['tamano'])} bytes")
print(f"  Porcentaje alfabético: {int(mejor_caso['porcentaje_alfa'])}%")
print(f"  Speedup: {mejor_caso['speedup']:.2f}x")

print("\n" + "="*60)
print("¡Gráficas generadas exitosamente en el directorio 'graficas/'!")
print("="*60)
print("\nArchivos generados:")
print("  - tiempo_vs_tamano.png")
print("  - speedup_vs_tamano.png")
print("  - efecto_porcentaje_alfabetico.png")
print("  - comparacion_alineamiento.png")
print("  - heatmap_speedup.png")
