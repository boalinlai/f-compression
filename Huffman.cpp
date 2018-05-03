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

    if (root->data != '$')
        codes[root->data]=string(str);
    storeCodes(root->left, str + "0");
    storeCodes(root->right, str + "1");

}

void Huffman::writeBinThread(int thread_id, int thread_no){
	
	string source_file = filename + ".txt";
	string encoded_file = filename + "_en_" +  to_string(thread_id);
	
	ifstream codestream(source_file);
			
    codestream >> noskipws; // read space and calculate offset
		
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

		bin_encoded_text += code;
	
        if (counter>=f_size/thread_no) break;
      
    }
	
    unsigned long long checksum = str_hash(bin_encoded_text);

	ofstream encodestream;
	
	encodestream = ofstream(encoded_file, ios::out | ios::binary);

    unsigned long long e_size = bin_encoded_text.size();

    json parity_json;

    parity_json["checksum"]  = checksum;
    parity_json["size"] = e_size;

    ofstream o(encoded_file + ".json");
    o << setw(4) << parity_json << endl;

    // store checksum, e size into json

	const int l_size = sizeof(unsigned long)*8; //8 bits
	
	string sub_str;
	
	for (int i=0;i<e_size;i+=l_size) {
		
		sub_str = bin_encoded_text.substr(i, l_size);
				
		std::bitset<l_size> b_sets(sub_str);
			
		unsigned long n = b_sets.to_ulong();
			
		encodestream.write(reinterpret_cast<const char*>(&n), sizeof(n)) ;
		
    }

	encodestream.close();
}


void Huffman::constructHeap()
{
    codes.clear();
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
    storeCodes(minHeap.top(), "");
}



void Huffman::encode()
{
    //read code
    string source_file = filename + ".txt";

    ifstream codestream(source_file);

    if (!codestream.is_open()) {
        cout << "Error: cannot open " << source_file << endl;
        exit(1);
    }
    codestream >> noskipws; // read space?
    char ch; // a character
    for (;;) {  /* Create frequency map by reading character by character */
        codestream >> ch;
        if (codestream.eof()) break;
        freq[ch] += 1;
    }
    constructHeap();

	int n = getCPUNo();
	
	thread myThreads[n];
	
	for (int i=0; i<n; i++){
        myThreads[i] = thread(&Huffman::writeBinThread, this, i, n);
    }
	
	for (int i=0; i<n; i++){
        myThreads[i].join();
    }
	
	cout << "Encode Finishes.\n";

    json freq_map_json;

    for(auto iter = freq.begin(); iter != freq.end(); ++iter) {
        freq_map_json[to_string(iter->first)] = iter->second;
    }

	ofstream o(filename + ".json");
    o << setw(4) << freq_map_json << endl;

}


void Huffman::readBinThread(int thread_id, int thread_no) {


    ifstream i(filename + "_en_" + to_string(thread_id) +  ".json");
    json parity_json;
    i >> parity_json;
    unsigned long long checksum = parity_json["checksum"].get<unsigned long long>();
    unsigned long long size = parity_json["size"].get<unsigned long long>();

    cout << "checksum: " << checksum << endl;
    cout << "size : " << size << endl;

	string encoded_file = filename + "_en_" + to_string(thread_id);
	string decoded_file = filename + "_de_" + to_string(thread_id);

	cout << thread_id << "\n";

	ifstream codestream(encoded_file, ios::binary | ios::in);
	ostringstream ostrm;
			
    codestream >> noskipws; // read space?
	
	ostrm << codestream.rdbuf();
	string encoded_text = ostrm.str();
			
	//cout << "Encoded String = " << encoded_text << endl ;
	
	string bin_encoded_text = "";
	for (size_t i = 0; i < encoded_text.size(); ++i)
	{
		string part = bitset<8>(encoded_text[i]).to_string();
		//cout << part << endl;
		bin_encoded_text += part;		
	}

	bin_encoded_text = bin_encoded_text.substr(0,size);
	//cout << "Binary Encoded String = " << bin_encoded_text << endl ;
	
	//assert(checksum == str_hash(bin_encoded_text));
	//checksum before decode
	cout << "Checksum = " << checksum << "New Checksum = " << str_hash(bin_encoded_text) << endl ;

	string decoded_text = decodeBin(minHeap.top(), bin_encoded_text);
	//cout << "decoded_text = " << decoded_text << endl ;

	ofstream decodestream;
	
	decodestream = ofstream(decoded_file, ios::out);
	decodestream << decoded_text;
    decodestream.close();
	
}

void Huffman::decode(){
	
	int n = getCPUNo();
    // read a JSON file
    std::ifstream i(filename + ".json");
    json j;
    i >> j;
    freq.clear();
//    freq = j.get< unordered_map <char, int> >();
    for (json::iterator it = j.begin(); it != j.end(); ++it) {
        freq[char(stoi(it.key()))] = it.value();
    }
    constructHeap();

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
