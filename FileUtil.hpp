#ifndef FILE_UTIL
#define FILE_UTIL 1

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string> // Add this line for the std::string usage

class FileUtil {
private:
    int fileDescriptor; // File descriptor for the open file

    // Private helper function to convert mode string to integer
    int convertModeString(const char*);

public:
    // Default constructor
    FileUtil();

    // Constructor with filename and mode arguments
    FileUtil(const char*, const char*);

    // Destructor
    ~FileUtil();

    // Open the file with the specified filename and mode
    bool open(const char*, const char*);

    // Check if the file is open
    bool isOpen();

    // Read data from the file into the provided buffer
    ssize_t read(void*, size_t);
    
    // Read an int8_t value from the file
    int8_t read_int8();

    // Read a uint8_t value from the file
    uint8_t read_uint8();

    // Read an int32_t value from the file
    int32_t read_int32();

    // Read a uint32_t value from the file
    uint32_t read_uint32();

    // Read a bool value from the file
    bool read_bool();

    // Read an int64_t value from the file
    int64_t read_int64();

    // Read a uint64_t value from the file
    uint64_t read_uint64();
    
    // Read a string from the file
    std::string read_string();
    
    // Write data to the file
    ssize_t write(const void*, size_t);

    // Get the current file offset
    off_t tell();
    
    bool skip(size_t bytes);
    
    off_t seek(off_t, int);

    // Check if the file with the specified filename exists
    bool exist(const char*);

    // Get the file permissions of the specified file
    mode_t perm(const char*);

    // Get the size of the specified file
    off_t getSize(const char*);

    // Check if the file is readable
    bool isReadable(const char*);

    // Check if the file is writable
    bool isWritable(const char*);

    // Check if the file is executable
    bool isExecutable(const char*);

    // Close the file
    void close();
};

// Default constructor
FileUtil::FileUtil() : fileDescriptor(-1) {}

// Constructor with filename and mode arguments
FileUtil::FileUtil(const char* filename, const char* mode) : fileDescriptor(-1) {
    open(filename, mode);
}

// Destructor
FileUtil::~FileUtil() {
    close();
}

// Open the file with the specified filename and mode
bool FileUtil::open(const char* filename, const char* mode) {
    fileDescriptor = ::open(filename, convertModeString(mode), S_IRUSR | S_IWUSR);
    return isOpen();
}

// Check if the file is open
bool FileUtil::isOpen() {
    return fileDescriptor != -1;
}

// Read data from the file into the provided buffer
ssize_t FileUtil::read(void* buffer, size_t size) {
    return ::read(fileDescriptor, buffer, size);
}

int8_t FileUtil::read_int8() {
    int8_t value;
    if (read(&value, sizeof(int8_t)) == sizeof(int8_t)) {
        return value;
    }
    return 0; // You can choose an appropriate default value
}

uint8_t FileUtil::read_uint8() {
    uint8_t value;
    if (read(&value, sizeof(uint8_t)) == sizeof(uint8_t)) {
        return value;
    }
    return 0; // You can choose an appropriate default value
}

int32_t FileUtil::read_int32() {
    int32_t value;
    if (read(&value, sizeof(int32_t)) == sizeof(int32_t)) {
        return value;
    }
    return 0; // You can choose an appropriate default value
}

uint32_t FileUtil::read_uint32() {
    uint32_t value;
    if (read(&value, sizeof(uint32_t)) == sizeof(uint32_t)) {
        return value;
    }
    return 0; // You can choose an appropriate default value
}

bool FileUtil::read_bool() {
    bool value;
    if (read(&value, sizeof(bool)) == sizeof(bool)) {
        return value;
    }
    return false; // You can choose an appropriate default value
}

int64_t FileUtil::read_int64() {
    int64_t value;
    if (read(&value, sizeof(int64_t)) == sizeof(int64_t)) {
        return value;
    }
    return 0; // You can choose an appropriate default value
}

uint64_t FileUtil::read_uint64() {
    uint64_t value;
    if (read(&value, sizeof(uint64_t)) == sizeof(uint64_t)) {
        return value;
    }
    return 0; // You can choose an appropriate default value
}

std::string FileUtil::read_string() {
    // Read the length of the string
    uint32_t length = read_uint32();

    // Create a buffer to hold the string data
    char buffer[length + 1]; // Adding +1 for the null terminator

    // Read the string data from the file
    ssize_t bytesRead = read(buffer, length);

    if (bytesRead != -1) {
        buffer[bytesRead] = '\0'; // Null-terminate the string

        // Convert the buffer to a C++ string
        return std::string(buffer);
    }

    // Return an empty string in case of error
    return "";
}

// Write data to the file
ssize_t FileUtil::write(const void* data, size_t size) {
    return ::write(fileDescriptor, data, size);
}

// Get the current file offset
off_t FileUtil::tell() {
    return ::lseek(fileDescriptor, 0, SEEK_CUR);
}

bool FileUtil::skip(size_t bytes) {
    off_t currentPosition = tell();
    if (currentPosition != -1 && seek(currentPosition + bytes, SEEK_SET) != -1) {
        return true;
    }
    return false;
}

off_t FileUtil::seek(off_t offset, int whence) {
	return lseek(fileDescriptor, offset, whence);
}

// Check if the file with the specified filename exists
bool FileUtil::exist(const char* filename) {
    return access(filename, F_OK) == 0;
}

// Get the file permissions of the specified file
mode_t FileUtil::perm(const char* filename) {
    struct stat fileStat;
    if (stat(filename, &fileStat) == 0) {
        return fileStat.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
    }
    return 0;
}

// Get the size of the specified file
off_t FileUtil::getSize(const char* filename) {
    struct stat fileStat;
    if (stat(filename, &fileStat) == 0) {
        return fileStat.st_size;
    }
    return -1;
}

// Check if the file is readable
bool FileUtil::isReadable(const char* filename) {
    return access(filename, R_OK) == 0;
}

// Check if the file is writable
bool FileUtil::isWritable(const char* filename) {
    return access(filename, W_OK) == 0;
}

// Check if the file is executable
bool FileUtil::isExecutable(const char* filename) {
    return access(filename, X_OK) == 0;
}

// Close the file
void FileUtil::close() {
    if (isOpen()) {
        ::close(fileDescriptor);
        fileDescriptor = -1;
    }
}

// Private helper function to convert mode string to integer
int FileUtil::convertModeString(const char* mode) {
    if (strcmp(mode, "r") == 0) {
        return O_RDONLY;
    } else if (strcmp(mode, "w") == 0) {
        return O_CREAT | O_WRONLY | O_TRUNC;
    } else if (strcmp(mode, "a") == 0) {
        return O_CREAT | O_WRONLY | O_APPEND;
    } else if (strcmp(mode, "r+") == 0) {
        return O_RDWR;
    } else if (strcmp(mode, "w+") == 0) {
        return O_CREAT | O_RDWR | O_TRUNC;
    } else if (strcmp(mode, "a+") == 0) {
        return O_CREAT | O_RDWR | O_APPEND;
    } else {
        // Unsupported mode, return default
        return O_CREAT | O_WRONLY | O_TRUNC;
    }
}

bool isExist(const char* filename) {
    return access(filename, F_OK) == 0;
}

// Function to check if a path corresponds to a regular file
bool isFile(const char *path) {
    struct stat fileStat;

    if (stat(path, &fileStat) == 0) {
        return S_ISREG(fileStat.st_mode);
    }

    return false; // Return false if there's an error or if it's not a regular file
}

// Function to check if a path corresponds to a directory
bool isDir(const char *path) {
    struct stat fileStat;

    if (stat(path, &fileStat) == 0) {
        return S_ISDIR(fileStat.st_mode);
    }

    return false; // Return false if there's an error or if it's not a directory
}

#endif