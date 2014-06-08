#include <string>
#include <vector>

#include "types.hpp"


namespace bouncycastle {
    /**
    * Block cipher engines are expected to conform to this interface.
    */
    class BlockCipher {
    public:
        /**
        * Initialise the cipher.
        *
        * @param forEncryption if true the cipher is initialised for
        *  encryption, if false for decryption.
        * @param params the key and other data required by the cipher.
        * @exception IllegalArgumentException if the params argument is
        * inappropriate.
        */
        // virtual void init(bool forEncryption, std::vector<byte> key) = 0;

        /**
        * Return the name of the algorithm the cipher implements.
        *
        * @return the name of the algorithm the cipher implements.
        */
        virtual std::string getAlgorithmName() = 0;

        /**
        * Return the block size for this cipher (in bytes).
        *
        * @return the block size for this cipher in bytes.
        */
        virtual int getBlockSize() = 0;

        /**
        * Process one block of input from the array in and write it to
        * the out array.
        *
        * @param in the array containing the input data.
        * @param inOff offset into the in array the data starts at.
        * @param out the array the output data will be copied into.
        * @param outOff the offset into the out array the output will start at.
        * @exception DataLengthException if there isn't enough data in in, or
        * space in out.
        * @exception IllegalStateException if the cipher isn't initialised.
        * @return the number of bytes processed and produced.
        */
        virtual int processBlock(const std::vector<byte>& in, size_t inOff, std::vector<byte>& out, size_t outOff) = 0;

        /**
        * Reset the cipher. After resetting the cipher is in the same state
        * as it was after the last init (if there was one).
        */
        virtual void reset() = 0;
    };
}