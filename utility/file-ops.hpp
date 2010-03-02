#ifndef FILE_OPS_H
#define FILE_OPS_H

/*
 * Generic file operations such as reading, writing, renaming etc.
 */

// STL
#include <string>

namespace oodles {

/*
 * Read-in (appending if necessary) data from the file
 * located at path input the string referenced by output. 
 *
 * Returns no. of bytes read from path.
 */
size_t read_file_data(const std::string &path, std::string &output);

} // oodles

#endif
