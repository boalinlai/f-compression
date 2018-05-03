#include <unordered_map>
#include <queue>
#include <string>
#include <thread>

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
	

		priority_queue<MinHeapNode*, vector<MinHeapNode*>, compare> minHeap;
		string filename;
		int num_of_cpus;
		
	public:

		unordered_map<char, string> codes;  // a map that stores character : Huffman code pair
		std::unordered_map<char, int> freq;
	
		Huffman(string filename);	
		void storeCodes(struct MinHeapNode* root, string str);
		
		void writeBinThread(int thread_id, int thread_no);
		void encode();

		void readBinThread(int thread_id, int thread_no);
		void decode();
		string decodeBin(struct MinHeapNode* root, string s);
		void concatFiles();		
        void constructHeap();
		// int cpuNo = thread::hardware_concurrency();

		std::hash<std::string> str_hash;
		
		int getCPUNo();
};
