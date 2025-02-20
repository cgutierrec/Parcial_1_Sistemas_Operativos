/*
 * Huffman Compressor v1.0
 * 
 * Tipo de archivo a procesar:
 * - El programa determina si el archivo es de texto o binario antes de procesarlo.
 * - Si el archivo es de texto, se comprime utilizando el algoritmo de Huffman.
 
 
 * Algoritmo utilizado y justificación:
 * - Se utiliza la codificación de Huffman, un algoritmo de compresión sin pérdida basado en la frecuencia de caracteres.
 * - Justificación: Huffman es adecuado para texto porque reduce la cantidad de bits usados para representar caracteres frecuentes.
 * - Se construye un árbol de Huffman a partir de las frecuencias de los caracteres y se generan códigos binarios óptimos.
 * - Los datos comprimidos incluyen la información necesaria para reconstruir el árbol y descomprimir el archivo.
 * 
 * Requisitos cumplidos:
 * - Uso de llamadas al sistema: open(), read(), write(), close().
 * - No se utilizan librerías externas de compresión.
 * - Se aceptan argumentos de línea de comandos para operar.
 * - Se implementan las banderas: -h , -v , -c , -x .
 */


#include <iostream> //salida y entrada del usuario
#include <fcntl.h> //nos da las funciones para manipular archivos en linux
#include <unistd.h> //da acceso a las funciones del sistema operativo
#include <sys/types.h> //define tipos de datos y nos ayuda en los valores de retorno como write() open()
#include <sys/stat.h> //proporciona estructuras en el sistema linux
#include <map> // nos ayuda con los arboles balanceados
#include <queue> // implementa las colas
#include <vector> // nos da los vectores(arreglos que pueden cambiar de tamaño automaticamente)
#include <bitset> // secuencia de bits de manera mas eficiente
#include <functional> // para definir las funciones lambda recursiva de los arboles
#include <cstring> // manejar cadenas
#include <sstream> // para convertir datos numericos a cadenas tipo string y vice versa
#include <stdexcept> // para poder identificar mejor el error

using namespace std;

struct HuffmanNode {
    char data;
    int freq;
    HuffmanNode *left, *right;
    HuffmanNode(char d, int f) : data(d), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

void buildHuffmanTree(map<char, int>& freqMap, map<char, string>& huffmanCodes, HuffmanNode*& root) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> pq;
    for (auto& pair : freqMap) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }
    while (pq.size() > 1) {
        HuffmanNode *left = pq.top(); pq.pop();
        HuffmanNode *right = pq.top(); pq.pop();
        HuffmanNode *newNode = new HuffmanNode('\0', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;
        pq.push(newNode);
    }
    root = pq.top();
    function<void(HuffmanNode*, string)> generateCodes = [&](HuffmanNode* node, string code) {
        if (!node) return;
        if (node->data != '\0') huffmanCodes[node->data] = code;
        generateCodes(node->left, code + "0");
        generateCodes(node->right, code + "1");
    };
    generateCodes(root, "");
}

bool isBinaryFile(const char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error al abrir el archivo");
        return false;
    }
    char buffer[512];
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
    close(fd);
    if (bytesRead <= 0) return false;
    
    for (ssize_t i = 0; i < bytesRead; i++) {
        if ((unsigned char)buffer[i] < 9 || ((unsigned char)buffer[i] > 13 && (unsigned char)buffer[i] < 32)) {
            return true;
        }
    }
    return false;
}

void compressFile(const char* inputFile, const char* outputFile) {
    int fd = open(inputFile, O_RDONLY);
    if (fd == -1) {
        perror("Error al abrir el archivo de entrada");
        return;
    }
    map<char, int> freqMap;
    char c;
    while (read(fd, &c, 1) > 0) {
        freqMap[c]++;
    }
    close(fd);
    map<char, string> huffmanCodes;
    HuffmanNode* root = nullptr;
    buildHuffmanTree(freqMap, huffmanCodes, root);
    int out = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out == -1) {
        perror("Error al abrir el archivo de salida");
        return;
    }
    int freqSize = freqMap.size();
    write(out, &freqSize, sizeof(int));
    for (auto& pair : freqMap) {
        write(out, &pair.first, sizeof(char));
        write(out, &pair.second, sizeof(int));
    }
    fd = open(inputFile, O_RDONLY);
    if (fd == -1) {
        perror("Error al abrir el archivo de entrada");
        close(out);
        return;
    }
    string bitString;
    while (read(fd, &c, 1) > 0) {
        bitString += huffmanCodes[c];
    }
    close(fd);
    unsigned char padding = (8 - (bitString.size() % 8)) % 8;
    write(out, &padding, 1);
    bitString.append(padding, '0');
    for (size_t i = 0; i < bitString.size(); i += 8) {
        bitset<8> byte(bitString.substr(i, 8));
        unsigned char b = (unsigned char) byte.to_ulong();
        write(out, &b, 1);
    }
    close(out);
    cout << "Archivo comprimido con éxito!" << endl;
}

void decompressFile(const char* inputFile, const char* outputFile) {
    int fd = open(inputFile, O_RDONLY);
    if (fd == -1) {
        perror("Error al abrir el archivo comprimido");
        return;
    }
    map<char, int> freqMap;
    char c;
    int freqSize, freq;
    read(fd, &freqSize, sizeof(int));
    for (int i = 0; i < freqSize; i++) {
        read(fd, &c, sizeof(char));
        read(fd, &freq, sizeof(int));
        freqMap[c] = freq;
    }
    map<char, string> huffmanCodes;
    HuffmanNode* root = nullptr;
    buildHuffmanTree(freqMap, huffmanCodes, root);
    unsigned char padding;
    read(fd, &padding, 1);
    string bitString;
    unsigned char byte;
    while (read(fd, &byte, 1) > 0) {
        bitString += bitset<8>(byte).to_string();
    }
    close(fd);
    bitString = bitString.substr(0, bitString.size() - padding);
    int out = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out == -1) {
        perror("Error al abrir el archivo de salida");
        return;
    }
    HuffmanNode* currentNode = root;
    for (char bit : bitString) {
        currentNode = (bit == '0') ? currentNode->left : currentNode->right;
        if (!currentNode->left && !currentNode->right) {
            write(out, &currentNode->data, 1);
            currentNode = root;
        }
    }
    close(out);
    cout << "Archivo descomprimido con éxito!" << endl;
}

void printHelp() {
    cout << "Uso: ./huffman_compre [opción] <archivo>" << endl;
    cout << "Opciones:" << endl;
    cout << "  -c <archivo>: Comprime el archivo especificado" << endl;
    cout << "  -x <archivo>: Descomprime el archivo especificado" << endl;
    cout << "  -h, --help : Muestra este mensaje de ayuda" << endl;
    cout << "  -v, --version : Muestra la versión del programa" << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        printHelp();
        return 0;
    }
    if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
        cout << "Huffman Compressor v1.0" << endl;
        return 0;
    }
    cout << "El archivo es " << (isBinaryFile(argv[2]) ? "binario" : "de texto") << endl;
    if (strcmp(argv[1], "-c") == 0) {
        compressFile(argv[2], "compressed.huf");
    } else if (strcmp(argv[1], "-x") == 0) {
        decompressFile(argv[2], "decompressed.txt");
    } else {
        cerr << "Opción no válida." << endl;
        return 1;
    }
    return 0;
}
