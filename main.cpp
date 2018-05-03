#include "Huffman.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;


// A helper function to check if the file exists
bool file_exist(const char* filename) {
    ifstream myfile(filename);
    if (myfile.is_open()) {
        return true;
    }
    return false;
}

// A helper function to check if the filename ends in .txt
bool has_suffix(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

typedef enum {
    encode = 'e',
    decode = 'd',
    quit = 'q'
} action;

// Driver program to test above functions
int main()
{
    char action;
    string filename;

    while (true) {
        cout << "Encode(e) or Decode(d) or quit(q)? " << endl;
        cin >> action;
        if (action == quit) {
            break;
        }

        cout << "Enter the text filename (press 1 will default to alice.txt)" << endl;
        cin >> filename;

        filename = ("1" ? "alice" : filename);

        while (!file_exist((filename + ".txt").c_str())) {
            cout << filename << " does not exist. Please re-enter: ";
            cin >> filename;
        }

        Huffman huffman(filename);


        switch (action) {
            case encode:
                huffman.encode();
                break;
            case decode:
                cout << "Decode start..." << endl;
                huffman.decode();
                break;
            default:
                cout << "Exiting the program... " << action << endl;
        }
    }
    return 0;
}