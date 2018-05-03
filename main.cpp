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
        if (action == 'q') {
            break;
        }

        cout << "Enter the text filename (press space will default to alice.txt)" << endl;
        cin >> filename;
        string suffix = ".txt";
        if (!has_suffix(filename, suffix)) {
            filename.append(".txt");
        }

        while (!file_exist(filename.c_str())) {
            cout << filename << " does not exist. Please re-enter: ";
            cin >> filename;
            if (!has_suffix(filename, suffix)) {
                filename.append(".txt");
            }
        }
        switch(action) {
            case 'e':
                /* fill in function */

                cout << "You pressed " << action << endl;
                break;
            case 'd':
                /* fill in function */
                
                cout << "You pressed " << action << endl;
                break;
            default:
                cout << "You pressed " << action << endl;
        }

    }

    return 0;
}