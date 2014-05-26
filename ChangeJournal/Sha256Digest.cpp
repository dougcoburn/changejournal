// Copied java source from bouncycastle.org

#include <sstream>
#include <iomanip>

#include "Sha256Digest.hpp"

const int Sha256Digest::DIGEST_LENGTH = 32;
const int Sha256Digest::SIZEOF_X = 64;

/* SHA-256 Constants
* (represent the first 32 bits of the fractional parts of the
* cube roots of the first sixty-four prime numbers)
*/
const int Sha256Digest::K[] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

Sha256Digest::Sha256Digest() 
        : X(SIZEOF_X) {
    reset();
}

int bigEndianToInt(const std::vector<byte>& in, size_t inOff) {
    int retval = in[inOff] << 24;
    retval |= (in[inOff+1] & 0xff) << 16;
    retval |= (in[inOff+2] & 0xff) << 8;
    retval |= (in[inOff+3] & 0xff);
    return retval;
}

void intToBigEndian(int in, std::vector<byte>& out, size_t outOff) {
    out[outOff] = (in >> 24) & 0xff;
    out[outOff + 1] = (in >> 16) & 0xff;
    out[outOff + 2] = (in >> 8) & 0xff;
    out[outOff + 3] = in & 0xff;
}

void Sha256Digest::processWord(const std::vector<byte>& in, size_t inOff) {
    X[xOff] = bigEndianToInt(in, inOff);
    xOff += 1;

    if (xOff == 16) {
        processBlock();
    }    
}

void Sha256Digest::processLength(UINT64 bitLength) {
    if (xOff > 14) {
        processBlock();
    }
    X[14] = (int)(bitLength >> 32);
    X[15] = (int)(bitLength & 0xffffffff);
}

void Sha256Digest::processBlock() {
    //
    // expand 16 word block into 64 word blocks.
    //
    for (int t = 16; t <= 63; t++) {
        X[t] = Theta1(X[t - 2]) + X[t - 7] + Theta0(X[t - 15]) + X[t - 16];
    }

    //
    // set up working variables.
    //
    int     a = H1;
    int     b = H2;
    int     c = H3;
    int     d = H4;
    int     e = H5;
    int     f = H6;
    int     g = H7;
    int     h = H8;

    int t = 0;
    for (int i = 0; i < 8; i++) {
        // t = 8 * i
        h += (Sum1(e) + Ch(e, f, g) + K[t] + X[t]);
        d += h;
        h += Sum0(a) + Maj(a, b, c);
        t+=1;

        // t = 8 * i + 1
        g += Sum1(d) + Ch(d, e, f) + K[t] + X[t];
        c += g;
        g += Sum0(h) + Maj(h, a, b);
        t+=1;

        // t = 8 * i + 2
        f += Sum1(c) + Ch(c, d, e) + K[t] + X[t];
        b += f;
        f += Sum0(g) + Maj(g, h, a);
        t+=1;

        // t = 8 * i + 3
        e += Sum1(b) + Ch(b, c, d) + K[t] + X[t];
        a += e;
        e += Sum0(f) + Maj(f, g, h);
        t+=1;

        // t = 8 * i + 4
        d += Sum1(a) + Ch(a, b, c) + K[t] + X[t];
        h += d;
        d += Sum0(e) + Maj(e, f, g);
        t+=1;

        // t = 8 * i + 5
        c += Sum1(h) + Ch(h, a, b) + K[t] + X[t];
        g += c;
        c += Sum0(d) + Maj(d, e, f);
        t+=1;

        // t = 8 * i + 6
        b += Sum1(g) + Ch(g, h, a) + K[t] + X[t];
        f += b;
        b += Sum0(c) + Maj(c, d, e);
        t+=1;

        // t = 8 * i + 7
        a += Sum1(f) + Ch(f, g, h) + K[t] + X[t];
        e += a;
        a += Sum0(b) + Maj(b, c, d);
        t+=1;
    }

    H1 += a;
    H2 += b;
    H3 += c;
    H4 += d;
    H5 += e;
    H6 += f;
    H7 += g;
    H8 += h;

    //
    // reset the offset and clean out the word buffer.
    //
    xOff = 0;
    for (int i = 0; i < 16; i++) {
        X[i] = 0;
    }
}

std::string Sha256Digest::getAlgorithmName() {
    return "SHA-256";
}

int Sha256Digest::getDigestSize() {
    return DIGEST_LENGTH;
}

std::string Sha256Digest::doFinal() {
    finish();

    std::vector<byte> out(DIGEST_LENGTH);
    intToBigEndian(H1, out, 0);
    intToBigEndian(H2, out, 4);
    intToBigEndian(H3, out, 8);
    intToBigEndian(H4, out, 12);
    intToBigEndian(H5, out, 16);
    intToBigEndian(H6, out, 20);
    intToBigEndian(H7, out, 24);
    intToBigEndian(H8, out, 28);

    reset();

    std::ostringstream oss;
    for (auto i = begin(out); i < end(out); i++) {
        oss << std::setw(2) << std::setfill('0') << std::hex << int(*i);
    }
    return oss.str();
}

void Sha256Digest::reset() {
    GeneralDigest::reset();

    /* SHA-256 initial hash value
    * The first 32 bits of the fractional parts of the square roots
    * of the first eight prime numbers
    */
    H1 = 0x6a09e667;
    H2 = 0xbb67ae85;
    H3 = 0x3c6ef372;
    H4 = 0xa54ff53a;
    H5 = 0x510e527f;
    H6 = 0x9b05688c;
    H7 = 0x1f83d9ab;
    H8 = 0x5be0cd19;

    xOff = size_t(0);
    for (size_t i = 0; i != X.size(); i+=1) {
        X[i] = 0;
    }
}

int Sha256Digest::Ch(int x, int y, int z) {
    return (x & y) ^ ((~x) & z);
}

int Sha256Digest::Maj(int x, int y, int z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

int Sha256Digest::Sum0(int sx) {
    unsigned int x = sx;
    return ((x >> 2) | (x << 30)) ^ ((x >> 13) | (x << 19)) ^ ((x >> 22) | (x << 10));
}

int Sha256Digest::Sum1(int sx) {
    unsigned int x = sx;
    return ((x >> 6) | (x << 26)) ^ ((x >> 11) | (x << 21)) ^ ((x >> 25) | (x << 7));
}

int Sha256Digest::Theta0(int sx) {
    unsigned int x = sx;
    return ((x >> 7) | (x << 25)) ^ ((x >> 18) | (x << 14)) ^ (x >> 3);
}

int Sha256Digest::Theta1(int sx) {
    unsigned int x = sx;
    return ((x >> 17) | (x << 15)) ^ ((x >> 19) | (x << 13)) ^ (x >> 10);
}
