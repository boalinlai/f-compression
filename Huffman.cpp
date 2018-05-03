#include "Huffman.h"

#include <iostream>
#include <fstream>
#include <bitset>
#include <thread>

using namespace std;

Huffman::Huffman(string filename)
{
	
	//read code
    ifstream codestream(filename);

    if (!codestream.is_open()) {
        cout << "Error: cannot open " << filename << endl;
        exit(1);
    }
    codestream >> noskipws; // read space?
    char ch; // a character
    for (;;) {  /* Create frequency map by reading character by character */
        codestream >> ch;
        if (codestream.eof()) return;
        freq[ch] += 1;
    }
	
	//construct table
	struct MinHeapNode *left, *right, *top;

    for (auto v = freq.begin(); v!=freq.end(); v++)
        minHeap.push(new MinHeapNode(v->first, v->second));

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
	
	//store code
	storeCodes(minHeap.top(), "");
	
}

void Huffman::storeCodes(struct MinHeapNode* root, string str)
{
    if (root==NULL)
        return;
	
    if (root->data != '$')
        codes[root->data]=str;
    storeCodes(root->left, str + "0");
    storeCodes(root->right, str + "1");
}


void Huffman::writeBinThread(string source_file, string encoded_file, int thread_id, int thread_no){
	
	ifstream codestream(source_file);
			
    codestream >> noskipws; // read space?
		
	codestream.seekg(0, codestream.end);
    int f_size = codestream.tellg();
	codestream.seekg(0, codestream.beg);
	
	cout << thread_id << ": f_size: " << f_size << "\n";
	
	codestream.seekg(thread_id*f_size/thread_no, codestream.beg);
	
    char ch; 
	string code;
	string bin_encoded_text;
	int counter = 0;
			
    for (;;) {
		
        codestream >> ch;
		
		counter++;
		
        code = codes[ch];
		
		//cout << ch << ": " << code << "\n";

		bin_encoded_text += code;
	
        if (counter>=f_size/thread_no) break;
      
    }

	ofstream decodestream;
	
	decodestream = ofstream(encoded_file+to_string(thread_id), ios::out | ios::binary); 
	
	int e_size = bin_encoded_text.size();

	const int l_size = sizeof(unsigned long)*8; //8 bits
	
	string sub_str;
	
	for (int i=0;i<e_size;i+=l_size) {
		
		sub_str = bin_encoded_text.substr(i, l_size);
		
		//cout << i << "i: " << sub_str << "size" << l_size << "\n";
		
		std::bitset<l_size> b_sets(sub_str);
			
		unsigned long n = b_sets.to_ulong();
		
		//cout << i << "i: " << b_sets << "\n";
	
		decodestream.write(reinterpret_cast<const char*>(&n), sizeof(n)) ;
		//decodestream.write((char*)&n, sizeof(unsigned long));
		
    }
	
	//decodestream.write((char*)&bin_encoded_text, sizeof(char)*bin_encoded_text.size());
	
	decodestream.close();

}

void Huffman::encode(string source_file, string encoded_file)
{
	
	int n = getCPUNo();
	
	thread myThreads[n];
	
	for (int i=0; i<n; i++){
        myThreads[i] = thread(&Huffman::writeBinThread, this, source_file, encoded_file, i, n);
    }
	
	for (int i=0; i<n; i++){
        myThreads[i].join();
    }
	
	cout << "Encode Finishes.\n";

}


void Huffman::readBinThread(string encoded_file, string decoded_file, int thread_id, int thread_no){

	cout << thread_id << "\n";

	ifstream codestream(encoded_file+to_string(thread_id), ios::binary);
	ostringstream ostrm;
			
    codestream >> noskipws; // read space?
	ostrm << codestream.rdbuf();
		
	string encoded_text = ostrm.str();	
	
	decodedStr = decodeBin(minHeap.top(), encoded_text);
	
	ofstream decodestream;
	
	decodestream = ofstream(decoded_file+to_string(thread_id), ios::out); 
	decodestream.write(decodedStr);
	decodestream.close()
	
}

void Huffman::decode(string encoded_file, string decoded_file){
	
	int n = getCPUNo();
	
	thread myThreads[n];
	
	for (int i=0; i<n; i++){
        myThreads[i] = thread(&Huffman::writeBinThread, this, encoded_file, string decoded_file, i, n);
    }
	
	for (int i=0; i<n; i++){
        myThreads[i].join();
    }
	
	cout << "Decode Finishes.\n";	
}

void Huffman::concatFiles(string encoded_file_part){
	//use linux command here?
}

int Huffman::getCPUNo()
{
	return thread::hardware_concurrency();	
}