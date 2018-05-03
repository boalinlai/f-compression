#include "Huffman.h"

#include <iostream>
#include <fstream>
#include <bitset>
#include <thread>
#include <functional>
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;


Huffman::Huffman(string fn):filename(fn), num_of_cpus(getCPUNo())
{

}

void Huffman::storeCodes(struct MinHeapNode* root, string str)
{
    if (root==NULL)
        return;
	
	cout << root->data << str << endl;
	
    if (root->data != '$')
        codes[root->data]=string(str);
    storeCodes(root->left, str + "0");
    storeCodes(root->right, str + "1");

}

void Huffman::writeBinThread(int thread_id, int thread_no){
	
	string source_file = filename;
	string encoded_file = filename+"en";
	
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
	
	//checksum after encode
    unsigned long long encodeChecksum[num_of_cpus];
	encodeChecksum[thread_id] = str_hash(bin_encoded_text);
	//cout << thread_id << ": checksum: " << check[thread_id] << "\n";
	
	ofstream encodestream;
	
	encodestream = ofstream(encoded_file+to_string(thread_id), ios::out | ios::binary); 
	
	int e_size = bin_encoded_text.size();

	const int l_size = sizeof(unsigned long)*8; //8 bits
	
	string sub_str;
	
	for (int i=0;i<e_size;i+=l_size) {
		
		sub_str = bin_encoded_text.substr(i, l_size);
		
		//cout << i << "i: " << sub_str << "size" << l_size << "\n";
		
		std::bitset<l_size> b_sets(sub_str);
			
		unsigned long n = b_sets.to_ulong();
		
		//cout << i << "i: " << b_sets << "\n";
	
		encodestream.write(reinterpret_cast<const char*>(&n), sizeof(n)) ;
		
    }
	
	encodestream.close();
}

void Huffman::encode()
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
        if (codestream.eof()) break;
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

	
	int n = getCPUNo();
	
	thread myThreads[n];
	
	for (int i=0; i<n; i++){
        myThreads[i] = thread(&Huffman::writeBinThread, this, i, n);
    }
	
	for (int i=0; i<n; i++){
        myThreads[i].join();
    }
	
	cout << "Encode Finishes.\n";

}


void Huffman::readBinThread(int thread_id, int thread_no){
    unsigned long long decodeChecksum[num_of_cpus];


	string encoded_file = filename+"en";
	string decoded_file = filename+"de"; 

	cout << thread_id << "\n";

	ifstream codestream(encoded_file+to_string(thread_id), ios::binary);
	ostringstream ostrm;
			
    codestream >> noskipws; // read space?
	ostrm << codestream.rdbuf();
		
	string encoded_text = ostrm.str();	
	
	//checksum before decode
	decodeChecksum[thread_id] = str_hash(encoded_text);
	
	string decoded_text = decodeBin(minHeap.top(), encoded_text);
	
	ofstream decodestream;
	
	decodestream = ofstream(decoded_file+to_string(thread_id), ios::out); 
	decodestream.write((char *)&decoded_text, sizeof(string));
	decodestream.close();
	
}

void Huffman::decode(){
	
	int n = getCPUNo();
	
	thread myThreads[n];
	
	for (int i=0; i<n; i++){
        myThreads[i] = thread(&Huffman::readBinThread, this, i, n);
    }
	
	for (int i=0; i<n; i++){
        myThreads[i].join();
    }
	
	cout << "Decode Finishes.\n";	
}

string Huffman::decodeBin(struct MinHeapNode* root, string s){
	   
	string ans = "";
    struct MinHeapNode* curr = root;
    for (int i=0;i<s.size();i++)
    {
        if (s[i] == '0')
            curr = curr->left;
        else
            curr = curr->right;

        // reached leaf node
        if (curr->left==NULL and curr->right==NULL)
        {
            ans += curr->data;
            curr = root;
        }
    }

    return ans;
	
}

void Huffman::concatFiles(){
	//use linux command here?
}

int Huffman::getCPUNo()
{
	return thread::hardware_concurrency();	
}

/*int main(){
	Huffman huff("alice.txt");
	huff.encode();
	
	json j_umap(huff.codes);
	cout << "Dumped: " << j_umap.dump(4) << "\n";
	

	unordered_map<char, string>::iterator it;
	for (it = huff.codes.begin(); it != huff.codes.end(); it++ )
	{
		cout << it->first  // string (key)
				  << ':'
				  << it->second   // string's value 
				  << endl ;
	}
	
}*/