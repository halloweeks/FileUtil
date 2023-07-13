#ifndef FILE_UTIL
#define FILE_UTIL 1

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdint>
#include <cstring>
#include <iostream>

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

    // Write data to the file
    ssize_t write(const void*, size_t);

    // Get the current file offset
    off_t tell();

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

// Write data to the file
ssize_t FileUtil::write(const void* data, size_t size) {
    return ::write(fileDescriptor, data, size);
}

// Get the current file offset
off_t FileUtil::tell() {
    return ::lseek(fileDescriptor, 0, SEEK_CUR);
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

#endif