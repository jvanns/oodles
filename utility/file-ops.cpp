// oodles
#include "file-ops.hpp"

// STL
#include <fstream>
#include <iterator>
#include <algorithm>

// IO streams
using std::ios;
using std::ifstream;
using std::ios_base;
using std::istream_iterator;

// Algorithms
using std::copy;

// Containers
using std::string;

int read_file_data(const string &path, string &output)
{
    ifstream input(path.c_str());

    if (!input)
        return -1;

    /* Calculate file size and reserve that no. of bytes in RAM */
    input.seekg(0, ios::end);
    const size_t i = input.tellg(), j = output.size();
    output.resize(i + j);

    if (output.size() == 0)
        return 0; // Seems there was no data to read?

    /* Do not consume whitespace and seek to start of file */
    input.unsetf(ios::skipws);
    input.seekg(0, ios::beg);

    /* Copy the entire file into our storage buffer, output */
    copy(istream_iterator<char>(input),
         istream_iterator<char>(),
         output.begin() + j);

    return i;
}
