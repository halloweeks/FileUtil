# FileUtil

FileUtil is a C++ class that provides utility functions for working with files at a low level. It simplifies file handling and provides an easy-to-use interface for performing low-level file operations.

## Features

- Open files with different modes (read, write, append, read/write).
- Read data from files into buffers.
- Write data to files from buffers.
- Retrieve the current file offset.
- Check if a file exists.
- Get file permissions.
- Get file size.
- Check file readability, writability, and executability.
- Close files.

## Usage

1. Include the "FileUtil.hpp" header in your C++ code.
2. Create a `FileUtil` object by providing the desired filename and mode.
3. Use the available member functions to perform file operations.
4. Close the file using the `close` function when finished.

## Functions

### `FileUtil(const char* filename, const char* mode)`

Creates a `FileUtil` object and opens the specified file with the given mode.

### `bool isOpen()`

Checks if the file is open and returns a boolean value accordingly.

### `ssize_t read(void* buffer, size_t size)`

Reads data from the file into the provided buffer of the specified size. Returns the number of bytes read.

### `ssize_t write(const void* data, size_t size)`

Writes data from the provided buffer of the specified size to the file. Returns the number of bytes written.

### `off_t tell()`

Returns the current file offset, representing the position in the file.

### `bool exist(const char* filename)`

Checks if the file with the specified filename exists. Returns true if the file exists, false otherwise.

### `mode_t perm(const char* filename)`

Gets the file permissions of the specified file. Returns the file permissions as a mode_t value.

### `off_t getSize(const char* filename)`

Gets the size of the specified file in bytes. Returns the size of the file or -1 if an error occurs.

### `bool isReadable(const char* filename)`

Checks if the specified file is readable. Returns true if the file is readable, false otherwise.

### `bool isWritable(const char* filename)`

Checks if the specified file is writable. Returns true if the file is writable, false otherwise.

### `bool isExecutable(const char* filename)`

Checks if the specified file is executable. Returns true if the file is executable, false otherwise.

### `void close()`

Closes the file.

Refer to the class declaration in "FileUtil.hpp" for more detailed information on each function.

## Example

```cpp
#include "FileUtil.hpp"

int main() {
    FileUtil file("test.txt", "w");  // Open the file

    if (file.is_open()) {
        uint8_t data[] = "halloweeks";
        size_t dataSize = sizeof(data);

        ssize_t bytesWritten = file.write(data, dataSize);
        if (bytesWritten > 0) {
            std::cout << "Data written successfully." << std::endl;
        } else {
            std::cout << "Error writing data to file." << std::endl;
        }
    } else {
        std::cout << "Failed to open the file." << std::endl;
    }

    file.close();  // Close the file

    return 0;
}
