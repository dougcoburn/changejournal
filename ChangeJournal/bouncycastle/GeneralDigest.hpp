// Copied java source from bouncycastle.org

#include <string>
#include <vector>

#include "types.hpp"

namespace bouncycastle {
    class GeneralDigest {
    private:
        static const int BYTE_LENGTH;
        std::vector<byte> xBuf;
        size_t xBufOff;
        size_t byteCount;
    protected:
        GeneralDigest();
        virtual ~GeneralDigest();
        virtual void processWord(const std::vector<byte>& in, size_t inOff) = 0;
        virtual void processLength(uint64_t bitLength) = 0;
        virtual void processBlock() = 0;
    public:
        void update(byte in);
        void update(const std::vector<byte>& in, size_t inOff, int len);
        void finish();
        int getByteLength();

        virtual void reset();

        virtual std::string getAlgorithmName() = 0;
        virtual int getDigestSize() = 0;
        virtual std::string doFinal() = 0;
    };
}