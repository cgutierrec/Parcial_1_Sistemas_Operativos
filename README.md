# Parcial_1_Sistemas_Operativos
# Huffman Compressor
## Cristóbal Gutiérrez Castro, Jose Manuel Barrientos Palacios, Pedro Santiago Mafla Jaramillo

## Descripción
Este programa implementa un compresor de archivos basado en la codificación de Huffman, una técnica de compresión sin pérdida que reduce el tamaño de los archivos de texto.

## Tipo de archivo a procesar
El programa determina si el archivo es de texto o binario antes de procesarlo:
- **Archivos de texto**: Se comprimen utilizando el algoritmo de Huffman.
- **Archivos binarios**: Aunque se pueden procesar, la compresión no será eficiente debido a la distribución de bits.

## Algoritmo utilizado y justificación
- **Codificación de Huffman**: Es un algoritmo de compresión sin pérdida que asigna códigos de longitud variable a los caracteres según su frecuencia de aparición.
- **Justificación**: Este método es eficiente para comprimir texto porque reduce la cantidad de bits utilizados para representar caracteres frecuentes, disminuyendo el tamaño del archivo sin pérdida de información.
- **Funcionamiento**:
  - Se calcula la frecuencia de cada carácter en el archivo.
  - Se construye un árbol de Huffman con estas frecuencias.
  - Se generan códigos binarios óptimos para cada carácter.
  - Se guarda el archivo comprimido junto con la información necesaria para su reconstrucción.

## Estrategia de almacenamiento y recuperación
- **Compresión**:
  - Se guarda la tabla de frecuencias en el archivo comprimido.
  - Se almacena la representación binaria del texto codificado usando la tabla de Huffman.
- **Descompresión**:
  - Se reconstruye el árbol de Huffman a partir de la tabla de frecuencias.
  - Se decodifica la secuencia binaria para obtener el archivo original.

## Uso del programa
El programa se ejecuta desde la línea de comandos y acepta las siguientes opciones:

### Opciones disponibles
- `-h` o `--help`: Muestra un mensaje con las opciones de uso del programa.
- `-v` o `--version`: Muestra la versión del programa.
- `-c <archivo>` o `--compress <archivo>`: Comprime el archivo indicado.
- `-x <archivo>` o `--decompress <archivo>`: Descomprime el archivo indicado.

### Ejemplo de uso
Para comprimir un archivo:
```sh
./huffman -c archivo.txt
```

Para descomprimir un archivo:
```sh
./huffman -x archivo_comprimido.huff
```

Para mostrar la ayuda:
```sh
./huffman -h
```

Para ver la versión del programa:
```sh
./huffman -v
```

## Requisitos
- Uso de llamadas al sistema: `open()`, `read()`, `write()`, `close()`.
- No se utilizan librerías externas de compresión.
- Funciona en sistemas basados en Linux.

## Notas adicionales
- Se recomienda utilizar este programa en archivos de texto para obtener una mayor eficiencia en la compresión.
- Si se intenta comprimir un archivo binario, el resultado puede no ser óptimo.
