#ifndef FILE_UTIL
#define FILE_UTIL 1

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <openssl/evp.h>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

class FileUtil {
	private: 
		int fileDescriptor;
		int convertModeString(const char*);
		char* filename = nullptr;
		
	public:
		FileUtil();
		FileUtil(const char*);
		FileUtil(const char*, const char*);
		~FileUtil();
		bool open(const char*, const char*);
		bool createDirectory(const std::string&);
		bool isOpen();
		ssize_t read(void*, size_t);
		ssize_t data(void* buffer);
		explicit operator bool() const;
		int8_t read_int8();
		uint8_t read_uint8();
		int32_t read_int32();
		uint32_t read_uint32();
		bool read_bool();
		int64_t read_int64();
		uint64_t read_uint64();
		std::string read_string();
		ssize_t write(const void*, size_t);
		off_t tell();
		bool sha1(uint8_t hash[SHA_DIGEST_LENGTH]);
        bool sha2(uint8_t hash[SHA256_DIGEST_LENGTH]);
        bool md5(uint8_t hash[MD5_DIGEST_LENGTH]);
        bool aes_128_ecb_process(const char*, uint8_t*, bool);
		bool skip(size_t bytes);
		const char *getFilename();
		off_t seek(off_t, int);
		bool exist(const char*);
		bool exist();
		mode_t perm(const char*);
		off_t getSize(const char*);
		off_t size();
		bool isReadable(const char*);
		bool isWritable(const char*);
		bool isExecutable(const char*);
		void close();
};

FileUtil::FileUtil() : fileDescriptor(-1) {
	OpenSSL_add_all_algorithms();
}

FileUtil::FileUtil(const char *filename) {
	OpenSSL_add_all_algorithms();
	fileDescriptor = -1;
	
	if (!filename || *filename == '\0') {
		std::cerr << "Error: Invalid filename.\n";
		return;
	}
	
	this->filename = (char*)malloc(strlen(filename) + 1);
	
	if (this->filename) {
		strcpy(this->filename, filename);
	} else {
		std::cerr << "Error: Memory allocation failed.\n";
	}
}

FileUtil::FileUtil(const char* filename, const char* mode) : fileDescriptor(-1) {
	OpenSSL_add_all_algorithms();
	if (!filename || *filename == '\0') {
		std::cerr << "Error: Invalid filename.\n";
		return;
	}
	
	this->filename = (char*)malloc(strlen(filename) + 1);
	
	if (this->filename) {
		strcpy(this->filename, filename);
	} else {
		std::cerr << "Error: Memory allocation failed.\n";
	}
    open(filename, mode);
}



FileUtil::~FileUtil() {
    if (filename && *filename != '\0') {
        free(filename);
        filename = nullptr;
    }
    close();
}

bool FileUtil::open(const char* filename, const char* mode) {
	if (!filename || *filename == '\0') {
		std::cerr << "Error: Invalid filename.\n";
		return 0;
	}
	
	this->filename = (char*)malloc(strlen(filename) + 1);
	
	if (this->filename) {
		strcpy(this->filename, filename);
	} else {
		std::cerr << "Error: Memory allocation failed.\n";
	}
    // Open the file
    fileDescriptor = ::open(filename, convertModeString(mode), S_IRUSR | S_IWUSR);
    return isOpen();
}


bool FileUtil::isOpen() {
    return fileDescriptor != -1;
}

FileUtil::operator bool() const {
    return fileDescriptor != -1;
}

ssize_t FileUtil::read(void* buffer, size_t size) {
    return ::read(fileDescriptor, buffer, size);
}

ssize_t FileUtil::data(void* buffer) {
    if (!isOpen()) {
        // addErrorMessage("File not open for reading data.");
        return -1;
    }
    printf("file: %lu\n", size());
    return read(buffer, size());
}

int8_t FileUtil::read_int8() {
    int8_t value;
    if (read(&value, sizeof(int8_t)) == sizeof(int8_t)) {
        return value;
    }
    return 0;
}

uint8_t FileUtil::read_uint8() {
    uint8_t value;
    if (read(&value, sizeof(uint8_t)) == sizeof(uint8_t)) {
        return value;
    }
    return 0;
}

int32_t FileUtil::read_int32() {
    int32_t value;
    if (read(&value, sizeof(int32_t)) == sizeof(int32_t)) {
        return value;
    }
    return 0;
}

uint32_t FileUtil::read_uint32() {
    uint32_t value;
    if (read(&value, sizeof(uint32_t)) == sizeof(uint32_t)) {
        return value;
    }
    return 0;
}

bool FileUtil::read_bool() {
    bool value;
    if (read(&value, sizeof(bool)) == sizeof(bool)) {
        return value;
    }
    return false;
}

int64_t FileUtil::read_int64() {
    int64_t value;
    if (read(&value, sizeof(int64_t)) == sizeof(int64_t)) {
        return value;
    }
    return 0;
}

uint64_t FileUtil::read_uint64() {
    uint64_t value;
    if (read(&value, sizeof(uint64_t)) == sizeof(uint64_t)) {
        return value;
    }
    return 0;
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

bool FileUtil::sha1(uint8_t hash[SHA_DIGEST_LENGTH]) {
    if (!filename || *filename == '\0') {
        // std::cerr << "Error: Invalid filename.\n";
        return false;
    }

    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return false;
    }

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        perror("Error creating EVP_MD_CTX");
        fclose(file);
        return false;
    }

    if (!EVP_DigestInit(mdctx, EVP_sha1())) {
        perror("Error initializing digest");
        fclose(file);
        EVP_MD_CTX_free(mdctx);
        return false;
    }

    uint8_t chunk[1024 * 16];
    size_t bytes_read;

    while ((bytes_read = fread(chunk, 1, sizeof(chunk), file)) != 0) {
        if (!EVP_DigestUpdate(mdctx, chunk, bytes_read)) {
            perror("Error updating digest");
            fclose(file);
            EVP_MD_CTX_free(mdctx);
            return false;
        }
    }

    if (!EVP_DigestFinal(mdctx, hash, NULL)) {
        perror("Error finalizing digest");
        fclose(file);
        EVP_MD_CTX_free(mdctx);
        return false;
    }

    fclose(file);
    EVP_MD_CTX_free(mdctx);

    return true;
}

bool FileUtil::sha2(uint8_t hash[SHA256_DIGEST_LENGTH]) {
    if (!filename || *filename == '\0') {
        return false;
    }

    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return false;
    }

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        perror("Error creating EVP_MD_CTX");
        fclose(file);
        return false;
    }

    if (!EVP_DigestInit(mdctx, EVP_sha256())) {
        perror("Error initializing digest");
        fclose(file);
        EVP_MD_CTX_free(mdctx);
        return false;
    }

    uint8_t chunk[1024 * 16];
    size_t bytes_read;

    while ((bytes_read = fread(chunk, 1, sizeof(chunk), file)) != 0) {
        if (!EVP_DigestUpdate(mdctx, chunk, bytes_read)) {
            perror("Error updating digest");
            fclose(file);
            EVP_MD_CTX_free(mdctx);
            return false;
        }
    }

    if (!EVP_DigestFinal(mdctx, hash, NULL)) {
        perror("Error finalizing digest");
        fclose(file);
        EVP_MD_CTX_free(mdctx);
        return false;
    }

    fclose(file);
    EVP_MD_CTX_free(mdctx);

    return true;
}

bool FileUtil::md5(uint8_t hash[MD5_DIGEST_LENGTH]) {
    if (!filename || *filename == '\0') {
        return false;
    }

    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return false;
    }

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        perror("Error creating EVP_MD_CTX");
        fclose(file);
        return false;
    }

    if (!EVP_DigestInit(mdctx, EVP_md5())) {
        perror("Error initializing digest");
        fclose(file);
        EVP_MD_CTX_free(mdctx);
        return false;
    }

    uint8_t chunk[1024 * 16];
    size_t bytes_read;

    while ((bytes_read = fread(chunk, 1, sizeof(chunk), file)) != 0) {
        if (!EVP_DigestUpdate(mdctx, chunk, bytes_read)) {
            perror("Error updating digest");
            fclose(file);
            EVP_MD_CTX_free(mdctx);
            return false;
        }
    }

    if (!EVP_DigestFinal(mdctx, hash, NULL)) {
        perror("Error finalizing digest");
        fclose(file);
        EVP_MD_CTX_free(mdctx);
        return false;
    }

    fclose(file);
    EVP_MD_CTX_free(mdctx);

    return true;
}

bool FileUtil::aes_128_ecb_process(const char *filename, uint8_t *key, bool encrypt) {
    OpenSSL_add_all_algorithms();

    if (!this->filename || *this->filename == '\0') {
        return false;
    }

    if (!filename || *filename == '\0') {
        return false;
    }

    int inFile = ::open(this->filename, O_RDONLY);

    if (inFile == -1) {
        return false;
    }

    int outFile = ::open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    if (outFile == -1) {
        std::cerr << "Failed to create output file." << std::endl;
        return false;
    }

    const EVP_CIPHER *cipher = EVP_aes_128_ecb();
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    if (encrypt) {
        EVP_EncryptInit_ex(ctx, cipher, nullptr, key, nullptr);
    } else {
        EVP_DecryptInit_ex(ctx, cipher, nullptr, key, nullptr);
    }

    uint8_t inBuf[1024];
    uint8_t outBuf[1024 + EVP_MAX_BLOCK_LENGTH];
    int bytesRead;
    int outLen;

    while ((bytesRead = ::read(inFile, inBuf, sizeof(inBuf))) > 0) {
        if (encrypt) {
            EVP_EncryptUpdate(ctx, outBuf, &outLen, inBuf, bytesRead);
        } else {
            EVP_DecryptUpdate(ctx, outBuf, &outLen, inBuf, bytesRead);
        }
        if (::write(outFile, outBuf, outLen) != outLen) {
            EVP_CIPHER_CTX_free(ctx);
            ::close(inFile);
            ::close(outFile);
            return false;
        }
    }

    if (encrypt) {
        EVP_EncryptFinal_ex(ctx, outBuf, &outLen);
    } else {
        EVP_DecryptFinal_ex(ctx, outBuf, &outLen);
    }
    
    if (::write(outFile, outBuf, outLen) != outLen) {
        EVP_CIPHER_CTX_free(ctx);
        ::close(inFile);
        ::close(outFile);
        return false;
    }

    EVP_CIPHER_CTX_free(ctx);
    ::close(inFile);
    ::close(outFile);

    return true;
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

bool FileUtil::exist() {
	if (!filename || *filename == '\0') {
		// std::cerr << "Error: Invalid filename.\n";
		return false;
	}
	
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

off_t FileUtil::size() {
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