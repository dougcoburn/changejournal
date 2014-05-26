#define NOMINMAX

#include <Windows.h>
#include <WinIoCtl.h>
#include <vector>
#include <iostream>
#include <string>
#include "zlib/deflate.h"

#include "Sha256Digest.hpp"

std::string compute_hash(std::string str) {
    Sha256Digest digest;
    std::vector<byte> message(str.length());
    memcpy(&message[0], &str[0], str.length());
    digest.update(message, 0, message.size());
    return digest.doFinal();
}

void test_sha() {
    std::string actual = compute_hash("hello");
    std::string expected = "2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824";
    if (actual != expected) {
        std::cout << "FAIL SHA-256('hello')" << std::endl;
        std::cout << "Expected : [" << expected << "]" << std::endl;
        std::cout << "Was      : [" << actual << "]" << std::endl;
    }
    actual = compute_hash("");
    expected = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
    if (actual != expected) {
        std::cout << "FAIL SHA-256('hello')" << std::endl;
        std::cout << "Expected : [" << expected << "]" << std::endl;
        std::cout << "Was      : [" << actual << "]" << std::endl;
    }
}

std::vector<char> deflate_message(std::string original_content) {
    z_stream z;
    memset(&z, 0, sizeof(z_stream));
    char input_buffer[1024];
    char output_buffer[1024];
    int status;
    /*
    * Setup code for your specific problem
    */
    strncpy_s(input_buffer, original_content.c_str(), original_content.length());
    z.next_in = (Bytef*)input_buffer;
    z.avail_in = original_content.length();
    z.next_out = (Bytef*)output_buffer;
    z.avail_out = 1024;
    deflateInit(&z, 9);
    for (;;) {
        if (z.avail_in == 0) {
            break;
        }
        status = deflate(&z, Z_SYNC_FLUSH);
    }
    deflate(&z, Z_FINISH);
    return std::vector<char>(output_buffer, output_buffer + (1024 - z.avail_out));
}

std::string inflate_message(std::vector<char> compressed_content) {
    z_stream z;
    memset(&z, 0, sizeof(z_stream));
    char input_buffer[1024];
    char output_buffer[1024];
    int status;
    /*
    * Setup code for your specific problem
    */
    strncpy_s(input_buffer, &compressed_content[0], compressed_content.size());
    z.next_in = (Bytef*)input_buffer;
    z.avail_in = compressed_content.size();
    z.next_out = (Bytef*)output_buffer;
    z.avail_out = 1024;
    inflateInit(&z);
    for (;;) {
        if (z.avail_in == 0) {
            break;
        }
        status = inflate(&z, Z_SYNC_FLUSH);
        if (status != Z_OK) {
            break;
        }
    }
    return std::string(output_buffer, output_buffer + (1024 - z.avail_out));
}

void test_zlib() {
    std::string originalMessage = "zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog."
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.")
        + std::string("zip zip zip zip zip zip zip zip zip. The quick brown fox jumps over the lazy dog.  The quick brown fox jumps over the lazy dog.");
    std::vector<char> compressed = deflate_message(originalMessage);
    std::string inflated = inflate_message(compressed);
    if (inflated != originalMessage) {
        std::cout << "Expected: [" << originalMessage << "]" << std::endl;
        std::cout << "Was:      [" << inflated << "]" << std::endl;
    }
}

int main() {
    test_zlib();
    test_sha();
    system("PAUSE");
    return 0;
}