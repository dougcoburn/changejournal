// Copied java source from bouncycastle.org

#include "GeneralDigest.hpp"

const int GeneralDigest::BYTE_LENGTH = 64;

GeneralDigest::GeneralDigest()
    : xBuf(4) {
    xBufOff = 0;
}

GeneralDigest::~GeneralDigest() {
}

void GeneralDigest::update(byte in) {
    xBuf[xBufOff] = in;
    xBufOff += 1;
    if (xBufOff == xBuf.size()) {
        processWord(xBuf, 0);
        xBufOff = 0;
    }
    byteCount += 1;
}

void GeneralDigest::update(const std::vector<byte>& in, size_t inOff, int len) {
    //
    // fill the current word
    //
    while ((xBufOff != 0) && (len > 0)) {
        update(in[inOff]);

        inOff+=1;
        len-=1;
    }

    //
    // process whole words.
    //
    while (len > int(xBuf.size())) {
        processWord(in, inOff);

        inOff += xBuf.size();
        len -= xBuf.size();
        byteCount += xBuf.size();
    }

    //
    // load in the remainder.
    //
    while (len > 0) {
        update(in[inOff]);

        inOff += 1;
        len -= 1;
    }
}

void GeneralDigest::finish() {
    UINT64 bitLength = (byteCount << 3);

    //
    // add the pad bytes.
    //
    update(byte(128));

    while (xBufOff != 0) {
        update(byte(0));
    }

    processLength(bitLength);

    processBlock();
}

void GeneralDigest::reset() {
    byteCount = 0;

    xBufOff = 0;
    for (size_t i = 0; i < xBuf.size(); i++) {
        xBuf[i] = 0;
    }
}

int GeneralDigest::getByteLength() {
    return BYTE_LENGTH;
}
