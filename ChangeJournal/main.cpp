#define NOMINMAX

#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include "zlib/zlib.h"

#include "bouncycastle/Sha256Digest.hpp"
#include "bouncycastle/AESFastEngine.hpp"
#include "jump/matrix.hpp"

std::string compute_hash(std::string str) {
    bouncycastle::Sha256Digest digest;
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

	for (size_t offset = 0; offset < original_content.length(); offset += input_buffer.size()) {
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
	int status = inflateInit(&z);
	int flush = Z_NO_FLUSH;
	std::string retval;

        for (size_t offset = 0; offset < compressed_content.size(); offset += input_buffer.size()) {
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

void test_aes(std::string secretMessage) {
    bouncycastle::AESFastEngine aesEngine;
    const int BLOCK_SIZE = aesEngine.getBlockSize();
    const int msgLength = secretMessage.length();
    std::vector<byte> plainText(BLOCK_SIZE * (msgLength / BLOCK_SIZE) + BLOCK_SIZE,
                                BLOCK_SIZE - msgLength % BLOCK_SIZE);
    memcpy(plainText.data(), secretMessage.c_str(), secretMessage.length());

    std::vector<byte> vec_key {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2 };
    bouncycastle::AESKey key(vec_key);
    aesEngine.init(true, key);
    std::vector<byte> encryptedText(plainText.size());
    size_t off = 0;
    do {
        off += aesEngine.processBlock(plainText, off, encryptedText, off);
    } while (off < plainText.size());

    aesEngine.init(false, key);
    std::vector<byte> decryptedText(plainText.size());
    off = 0;
    do {
        off += aesEngine.processBlock(encryptedText, off, decryptedText, off);
    } while (off < plainText.size());
    size_t padding_length = decryptedText[decryptedText.size() - 1];
    size_t message_length = decryptedText.size() - padding_length;
    std::string roundtrip(message_length, 0);
    memcpy(&roundtrip[0], decryptedText.data(), message_length);
    if (secretMessage != roundtrip) {
        std::cout << "Expected: [" << secretMessage << "]" << std::endl;
        std::cout << "Was:      [" << roundtrip << "]" << std::endl;
    }
}

int main() {
    test_zlib();
    test_sha();
    test_aes("This is a secret message");
    test_aes("This is a secret message123456");
    test_aes("This is a secret message1234567");
    test_aes("This is a secret message12345678");
    test_aes("This is a secret message123456789");
    test_aes("This is a secret message1234567890");

    jump::matrix<int> mat(2, 4);
    mat[1][1] = 2;
    for (int i = 0; i < mat.columns(); i += 1) {
        for (int j = 0; j < mat.rows(); j += 1) {
            std::cout << mat[i][j] << "  ";
        }
        std::cout << std::endl;
    }

    system("PAUSE");
    return 0;
}