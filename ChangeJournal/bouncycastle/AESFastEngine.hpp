/**
* an implementation of the AES (Rijndael), from FIPS-197.
* <p>
* For further details see: <a href="http://csrc.nist.gov/encryption/aes/">http://csrc.nist.gov/encryption/aes/</a>.
*
* This implementation is based on optimizations from Dr. Brian Gladman's paper and C code at
* <a href="http://fp.gladman.plus.com/cryptography_technology/rijndael/">http://fp.gladman.plus.com/cryptography_technology/rijndael/</a>
*
* There are three levels of tradeoff of speed vs memory
* Because java has no preprocessor, they are written as three separate classes from which to choose
*
* The fastest uses 8Kbytes of static tables to precompute round calculations, 4 256 word tables for encryption
* and 4 for decryption.
*
* The middle performance version uses only one 256 word table for each, for a total of 2Kbytes,
* adding 12 rotate operations per round to compute the values contained in the other tables from
* the contents of the first
*
* The slowest version uses no static tables at all and computes the values in each round
* <p>
* This file contains the fast version with 8Kbytes of static tables for round precomputation
*
*/

#include <string>

#include "BlockCipher.hpp"
#include "../jump/matrix.hpp"

namespace bouncycastle {
    class AESKey {
    public:
        AESKey();
        AESKey(std::vector<byte> key);
        AESKey(std::vector<byte>&& key);
        std::vector<byte> getKey() const;
    private:
        bool validKeyLength(const std::vector<byte>& key);
        std::vector<byte> key;
    };

    class AESFastEngine : public BlockCipher {
    private:
        // The S box
        static const std::vector<byte> S;
        // The inverse S-box
        static const std::vector<byte> Si;
        // vector used in calculating key schedule (powers of x in GF(256))
        static const std::vector<uint32_t> rcon;
        // precomputation tables of calculations for rounds
        static const std::vector<uint32_t> T0;
        static const std::vector<uint32_t> T1;
        static const std::vector<uint32_t> T2;
        static const std::vector<uint32_t> T3;
        static const std::vector<uint32_t> Tinv0;
        static const std::vector<uint32_t> Tinv1;
        static const std::vector<uint32_t> Tinv2;
        static const std::vector<uint32_t> Tinv3;

        static int shift(uint32_t r, uint32_t shift);

        /* multiply four bytes in GF(2^8) by 'x' {02} in parallel */
        static const uint32_t m1;
        static const uint32_t m2;
        static const uint32_t m3;

        static uint32_t FFmulX(uint32_t x);

        /*
        The following defines provide alternative definitions of FFmulX that might
        give improved performance if a fast 32-bit multiply is not available.

        int FFmulX(int x) { int u = x & m1; u |= (u >> 1); return ((x & m2) << 1) ^ ((u >>> 3) | (u >>> 6)); }
        static int  m4 = 0x1b1b1b1b;
        int FFmulX(int x) { int u = x & m1; return ((x & m2) << 1) ^ ((u - (u >>> 7)) & m4); }
        */
        static uint32_t inv_mcol(uint32_t x);

        static uint32_t subWord(uint32_t x);

        /**
        * Calculate the necessary round keys
        * The number of calculations depends on key size and block size
        * AES specified a fixed block size of 128 bits and key sizes 128/192/256 bits
        * This code is written assuming those are the only possible values
        */
        jump::matrix<uint32_t> generateWorkingKey(
            std::vector<byte> key,
            bool forEncryption);

        size_t ROUNDS;
        jump::matrix<uint32_t> WorkingKey;
        uint32_t C0, C1, C2, C3;
        bool forEncryption;

        static const int BLOCK_SIZE = 16;

        /**
        * default constructor - 128 bit block size.
        */
    public:
        AESFastEngine();

        /**
        * initialise an AES cipher.
        *
        * @param forEncryption whether or not we are for encryption.
        * @param params the parameters required to set up the cipher.
        * @exception IllegalArgumentException if the params argument is
        * inappropriate.
        */
    public:
        void init(bool forEncryption, const AESKey& key);

        std::string getAlgorithmName();

        int getBlockSize();

        int processBlock(
            const std::vector<byte>& in,
            size_t inOff,
            std::vector<byte>& out,
            size_t outOff);

        void reset();

    private:
        void unpackBlock(
            const std::vector<byte>& bytes,
            int off);

        void packBlock(
            std::vector<byte>& bytes,
            int off);

        void encryptBlock(const jump::matrix<uint32_t>& KW);

        void decryptBlock(const jump::matrix<uint32_t>& KW);
    };
}