//
// Created by Boa-Lin Lai on 4/14/18.
//
// C++ program for Huffman Coding


// refactored from

// https://cs.calvin.edu/activities/books/c++/ds/2e/WebItems/Chapter15/Huffman.pdf
// https://www.geeksforgeeks.org/greedy-algorithms-set-3-huffman-coding/
// http://www.umich.edu/~umfandsf/other/ebooks/alice30.txt

#include <string>
#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>

using namespace std;
unordered_map<char, string> codes;

// A Huffman tree node
struct MinHeapNode {

    // One of the input characters
    char data;
    // Frequency of the character
    unsigned freq;

    // Left and right child
    MinHeapNode *left, *right;

    MinHeapNode(char data, unsigned freq) {
        left = right = NULL;
        this->data = data;
        this->freq = freq;
    }
};

// For comparison of
// two heap nodes (needed in min heap)
struct compare {

    bool operator()(MinHeapNode* l, MinHeapNode* r)

    {
        return (l->freq > r->freq);
    }
};

// Prints huffman codes from
// the root of Huffman Tree.
void printCodes(struct MinHeapNode* root, string str)
{

    if (!root)
        return;

    if (root->data != '$')
        cout << root->data << ": " << str << "\n";

    printCodes(root->left, str + "0");
    printCodes(root->right, str + "1");
}

void storeCodes(struct MinHeapNode* root, string str)
{
    if (root==NULL)
        return;
	
    if (root->data != '$')
        codes[root->data]=str;
    storeCodes(root->left, str + "0");
    storeCodes(root->right, str + "1");
}

void HuffmanCodes(unordered_map<char, unsigned>& m)
{
    struct MinHeapNode *left, *right, *top;

    // Create a min heap & inserts all characters of data[]
    priority_queue<MinHeapNode*, vector<MinHeapNode*>, compare> minHeap;

    for (auto & e : m)
        minHeap.push(new MinHeapNode(e.first, e.second));

    // Iterate while size of heap doesn't become 1
    while (minHeap.size() != 1) {

        // Extract the two minimum
        // freq items from min heap
        left = minHeap.top();
        minHeap.pop();

        right = minHeap.top();
        minHeap.pop();

        // Create a new internal node with
        // frequency equal to the sum of the
        // two nodes frequencies. Make the
        // two extracted node as left and right children
        // of this new node. Add this node
        // to the min heap '$' is a special value
        // for internal nodes, not used
        top = new MinHeapNode('$', left->freq + right->freq);

        top->left = left;
        top->right = right;

        minHeap.push(top);
    }

    // Print Huffman codes using
    // the Huffman tree built above
    printCodes(minHeap.top(), "");
	storeCodes(minHeap.top(), "");
}


void convertStrToBin(string source_file, string decode_file)
{
	ifstream codestream(source_file);
	
	ofstream decodestream;
	
	decodestream = ofstream(decode_file, ios::out ); //| ios::binary
	
    codestream >> noskipws; // read space?
	
    char ch; 
	string code;
	string encoded_text;
	
    for (;;) {
		
        codestream >> ch;
		
        code = codes[ch];
		
		cout << ch << ": " << code << "\n";

		encoded_text += code;
	
        if (codestream.eof()) break;
      
    }

	decodestream.write((char*)&encoded_text, sizeof(char)*encoded_text.size());
	
	cout << "Decode Finishes\n" << encoded_text << "\nsize:" << encoded_text.size();

	decodestream.close();
	
}

unordered_map<char, unsigned> ReadSourceFile(string filename) {

    unordered_map<char, unsigned> m;

    ifstream codestream(filename);

    if (!codestream.is_open()) {
        cout << "Cannot open code file.\n";
        exit(1);
    }
    codestream >> noskipws; // read space?
    char ch; // a character
    for (;;) {
        codestream >> ch;
        if (codestream.eof()) return m;
        m[ch] += 1;
        //cout << ch << endl;
    }

}

// Driver program to test above functions
int main()
{
	string source_file = "alice.txt";
	string decode_file = "alice_decode.binary";
	string resource_file = "alice_reconstruct.binary";

    unordered_map<char, unsigned>  m = ReadSourceFile(source_file);

    HuffmanCodes(m);
	
	convertStrToBin(source_file, decode_file);
	
	//convertBinToStr(m, re_source_file, decode_file);

    return 0;
}

// This code is contributed by Aditya Goel, Refactored with STL + stream by Boa-Lin