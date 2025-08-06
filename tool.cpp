#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <vector>
#include <bitset>

using namespace std;

struct Node {
    char ch;
    int freq;
    Node *left, *right;

    Node(char c, int f) {
        ch = c;
        freq = f;
        left = right = nullptr;
    }
};

// Custom comparator for priority queue
struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

unordered_map<char, string> huffmanCode;
unordered_map<string, char> reverseCode;

// Generate codes using DFS
void generateCodes(Node* root, string str) {
    if (!root)
        return;

    if (!root->left && !root->right) {
        huffmanCode[root->ch] = str;
        reverseCode[str] = root->ch;
    }

    generateCodes(root->left, str + "0");
    generateCodes(root->right, str + "1");
}

// Build Huffman Tree
Node* buildHuffmanTree(unordered_map<char, int> freqMap) {
    priority_queue<Node*, vector<Node*>, Compare> pq;

    for (auto pair : freqMap)
        pq.push(new Node(pair.first, pair.second));

    while (pq.size() > 1) {
        Node *left = pq.top(); pq.pop();
        Node *right = pq.top(); pq.pop();

        int totalFreq = left->freq + right->freq;
        Node *parent = new Node('\0', totalFreq);
        parent->left = left;
        parent->right = right;

        pq.push(parent);
    }

    return pq.top();
}

// Convert binary string to bytes and write to file
void writeBinaryFile(const string& binary, const string& outputPath) {
    ofstream out(outputPath, ios::binary);
    int padding = 8 - (binary.size() % 8);
    out.put(padding + '0'); // store padding in first byte

    string paddedBinary = binary + string(padding, '0');
    for (size_t i = 0; i < paddedBinary.size(); i += 8) {
        bitset<8> byte(paddedBinary.substr(i, 8));
        out.put((char)byte.to_ulong());
    }

    out.close();
}

// Read binary file and convert to binary string
string readBinaryFile(const string& inputPath) {
    ifstream in(inputPath, ios::binary);
    int padding = in.get() - '0';

    string binary = "";
    char byte;
    while (in.get(byte)) {
        bitset<8> b((unsigned char)byte);
        binary += b.to_string();
    }

    if (padding)
        binary = binary.substr(0, binary.size() - padding);

    in.close();
    return binary;
}

// Compression
void compressFile(const string& inputPath, const string& outputPath) {
    ifstream in(inputPath);
    string text((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    in.close();

    unordered_map<char, int> freqMap;
    for (char ch : text)
        freqMap[ch]++;

    Node* root = buildHuffmanTree(freqMap);
    generateCodes(root, "");

    string binary = "";
    for (char ch : text)
        binary += huffmanCode[ch];

    writeBinaryFile(binary, outputPath);
    cout << "Compression complete! Compressed file saved as " << outputPath << endl;
}

// Decompression
void decompressFile(const string& inputPath, const string& outputPath) {
    string binary = readBinaryFile(inputPath);

    string temp = "", text = "";
    for (char bit : binary) {
        temp += bit;
        if (reverseCode.count(temp)) {
            text += reverseCode[temp];
            temp = "";
        }
    }

    ofstream out(outputPath);
    out << text;
    out.close();
    cout << "Decompression complete! Output file saved as " << outputPath << endl;
}

// Main Menu
int main() {
    int choice;
    string input, output;
    cout << "1. Compress File\n2. Decompress File\nEnter choice: ";
    cin >> choice;

    if (choice == 1) {
        cout << "Enter input file path: ";
        cin >> input;
        cout << "Enter output file path: ";
        cin >> output;
        compressFile(input, output);
    } else if (choice == 2) {
        cout << "Enter compressed (.bin) file path: ";
        cin >> input;
        cout << "Enter output (decompressed) file path: ";
        cin >> output;
        decompressFile(input, output);
    } else {
        cout << "Invalid choice." << endl;
    }

    return 0;
}
