#include <unordered_map>
#include <queue>
#include <string>

using namespace std;

class Huffman
{
	private:
	
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
	
		unordered_map<char, string> codes;  // a map that stores character : Huffman code pair
		unordered_map<char, int> freq;  
		priority_queue<MinHeapNode*, vector<MinHeapNode*>, compare> minHeap;
		
		
	public:
	
		Huffman(string filename);	
		void storeCodes(struct MinHeapNode* root, string str);
		
		void writeBinThread(string source_file, string encoded_file, int thread_id, int thread_no);
		void encode(string source_file, string encoded_file);

		void readBinThread(string encoded_file, string decoded_file, int thread_id, int thread_no);
		void decode(string encoded_file, string decoded_file);
		string decodeBin(struct MinHeapNode* root, string s);
		void concatFiles(string encoded_file_part);		
		
		int getCPUNo();
		
	
};