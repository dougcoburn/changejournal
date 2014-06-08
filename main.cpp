#define NOMINMAX

#include <Windows.h>
#include <WinIoCtl.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include "zlib/deflate.h"

#include "Sha256Digest.hpp"

std::string compute_hash(std::string str) {
    Sha256Digest digest;
    std::vector<byte> message(str.length());
	if (str.length() > 0) {
		memcpy(&message[0], &str[0], str.length());
	}
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
    std::vector<char> input_buffer(1024);
	std::vector<char> output_buffer(1024);
	z.next_out = (Bytef*)output_buffer.data();
	z.avail_out = output_buffer.size();
	int status = deflateInit(&z, 9);
	int flush = Z_NO_FLUSH;
	std::vector<char> retval;

	for (int offset = 0; offset < original_content.length(); offset += input_buffer.size()) {
		z.avail_in = original_content.length() - offset;
		if (input_buffer.size() < z.avail_in) {
			z.avail_in = input_buffer.size();
		}
		z.next_in = (Bytef*)input_buffer.data();
		memcpy(input_buffer.data(), &original_content[offset], z.avail_in);
		while (z.avail_in) {
			deflate(&z, flush);
			if (z.avail_out == 0) {
				retval.insert(end(retval), begin(output_buffer), end(output_buffer));
				z.next_out = (Bytef*)output_buffer.data();
				z.avail_out = output_buffer.size();
			}
 		}
	}
	flush = Z_FINISH;
	do {
		deflate(&z, flush);
		if (z.avail_out == 0) {
			retval.insert(end(retval), begin(output_buffer), end(output_buffer));
			z.next_out = (Bytef*)output_buffer.data();
			z.avail_out = output_buffer.size();
			continue;
		}
	} while (z.avail_out == 0);

	if (z.avail_out != output_buffer.size()) {
		retval.insert(end(retval), begin(output_buffer), end(output_buffer) - z.avail_out);
		z.next_out = (Bytef*)output_buffer.data();
		z.avail_out = output_buffer.size();
	}
	return retval;
}

std::string inflate_message(std::vector<char> compressed_content) {
	z_stream z;
	memset(&z, 0, sizeof(z_stream));
	std::vector<char> input_buffer(1024);
	std::vector<char> output_buffer(1024);
	z.next_out = (Bytef*)output_buffer.data();
	z.avail_out = output_buffer.size();
	int status = inflateInit(&z, 9);
	int flush = Z_NO_FLUSH;
	std::string retval;

	for (int offset = 0; offset < compressed_content.size(); offset += input_buffer.size()) {
		z.avail_in = compressed_content.size() - offset;
		z.next_in = (Bytef*)input_buffer.data();
		if (input_buffer.size() < z.avail_in) {
			z.avail_in = input_buffer.size();
		}
		memcpy(input_buffer.data(), &compressed_content[offset], z.avail_in);
		while (z.avail_in) {
			inflate(&z, flush);
			if (z.avail_out == 0) {
				retval.insert(end(retval), begin(output_buffer), end(output_buffer));
				z.next_out = (Bytef*)output_buffer.data();
				z.avail_out = output_buffer.size();
			}
		}
	}
	flush = Z_FINISH;
	do {
		inflate(&z, flush);
		if (z.avail_out == 0) {
			retval.insert(end(retval), begin(output_buffer), end(output_buffer));
			z.next_out = (Bytef*)output_buffer.data();
			z.avail_out = output_buffer.size();
			continue;
		}
	} while (z.avail_out == 0);

	if (z.avail_out != output_buffer.size()) {
		retval.insert(end(retval), begin(output_buffer), end(output_buffer) - z.avail_out);
		z.next_out = (Bytef*)output_buffer.data();
		z.avail_out = output_buffer.size();
	}
	return retval;
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