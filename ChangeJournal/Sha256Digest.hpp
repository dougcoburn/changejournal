// Copied java source from bouncycastle.org

#include "GeneralDigest.hpp"

class Sha256Digest : public GeneralDigest {
private:
    static const int DIGEST_LENGTH;
    static const int K[];
    static const int SIZEOF_X;
    int H1, H2, H3, H4, H5, H6, H7, H8;
    std::vector<int> X;
    size_t xOff;

    int Ch(int x, int y, int z);
    int Maj(int x, int y, int z);
    int Sum0(int x);
    int Sum1(int x);
    int Theta0(int x);
    int Theta1(int x);
protected:
    virtual void processWord(const std::vector<byte>& in, size_t inOff);
    virtual void processLength(UINT64 bitLength);
    virtual void processBlock();
public:
    Sha256Digest();
    virtual std::string getAlgorithmName();
    virtual int getDigestSize();
    virtual std::string doFinal();
    virtual void reset();
};