// oodles
#include "utility/file-ops.h"
#include "utility/DistinctItemVector.hpp"

// STL
#include <iostream>

#include <ctype.h> // For isspace()

// IO streams
using std::cout;
using std::cerr;
using std::endl;

// Containers
using std::string;

int main(int argc, char *argv[]) {
    if (!argv[1]) {
        cerr << "Provide the location (file path) to a raw, ASCII text file.\n";
        return 1;
    }

    string s;
    const int n = read_file_data(argv[1], s);

    if (n == -1) {
        cerr << "Failed to open " << argv[1] << " for reading.\n";
        return 1;
    }

    if (n != static_cast<int>(s.size())) {
        cout << "No. bytes read into buffer not equal to size of input.\n";
        return 1;
    }

    cout << "Read " << n << " bytes from " << argv[1] << ".\n";

    string word;
    oodles::DistinctItemVector<string> words;
    string::const_iterator i = s.begin(), j = s.end();

    while (i != j) {
        if (isspace(*i)) {
            if (!word.empty()) {
                cout << "Appended '" << word << "' at index "
                     << words.append_item(word) << endl;
                word.clear();
            }
        } else
            word += *i;

        i++;
    }

    for (uint32_t p = 0 ; p < words.sequence_count() ; ++p) {
        words.retrieve_item(p, word);
        cout << word << endl;
    }

    return 1;
}
