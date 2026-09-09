// Zeus legacy Scrypt PoW compatibility for Bitcoin Core 31.1.
// Algorithm preserved from the historical Zeus implementation.
// SHA-256 plumbing uses Bitcoin Core's internal CSHA256.
// UNTESTED / DO NOT RELEASE.

#include <zeus/crypto/scrypt.h>

#include <crypto/sha256.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace zeus {
namespace {

uint32_t ReadLE32(const unsigned char* p)
{
    return uint32_t{p[0]} |
           (uint32_t{p[1]} << 8) |
           (uint32_t{p[2]} << 16) |
           (uint32_t{p[3]} << 24);
}

void WriteLE32(unsigned char* p, uint32_t x)
{
    p[0] = static_cast<unsigned char>(x);
    p[1] = static_cast<unsigned char>(x >> 8);
    p[2] = static_cast<unsigned char>(x >> 16);
    p[3] = static_cast<unsigned char>(x >> 24);
}

void WriteBE32(unsigned char* p, uint32_t x)
{
    p[0] = static_cast<unsigned char>(x >> 24);
    p[1] = static_cast<unsigned char>(x >> 16);
    p[2] = static_cast<unsigned char>(x >> 8);
    p[3] = static_cast<unsigned char>(x);
}

class HMACSHA256
{
private:
    CSHA256 m_inner;
    CSHA256 m_outer;

public:
    HMACSHA256(const unsigned char* key, std::size_t keylen)
    {
        std::array<unsigned char, 64> normalized{};
        if (keylen > normalized.size()) {
            CSHA256{}.Write(key, keylen).Finalize(normalized.data());
            keylen = CSHA256::OUTPUT_SIZE;
        } else {
            std::memcpy(normalized.data(), key, keylen);
        }

        std::array<unsigned char, 64> ipad{};
        std::array<unsigned char, 64> opad{};
        ipad.fill(0x36);
        opad.fill(0x5c);
        for (std::size_t i = 0; i < normalized.size(); ++i) {
            ipad[i] ^= normalized[i];
            opad[i] ^= normalized[i];
        }
        m_inner.Write(ipad.data(), ipad.size());
        m_outer.Write(opad.data(), opad.size());
    }

    HMACSHA256& Write(const unsigned char* data, std::size_t len)
    {
        m_inner.Write(data, len);
        return *this;
    }

    void Finalize(unsigned char out[32])
    {
        std::array<unsigned char, 32> inner_hash{};
        m_inner.Finalize(inner_hash.data());
        m_outer.Write(inner_hash.data(), inner_hash.size()).Finalize(out);
    }
};

inline uint32_t RotL(uint32_t value, int shift)
{
    return (value << shift) | (value >> (32 - shift));
}

void XorSalsa8(uint32_t b[16], const uint32_t bx[16])
{
    uint32_t x[16];
    for (int i = 0; i < 16; ++i) x[i] = (b[i] ^= bx[i]);

    for (int i = 0; i < 8; i += 2) {
        x[ 4] ^= RotL(x[ 0] + x[12],  7);
        x[ 9] ^= RotL(x[ 5] + x[ 1],  7);
        x[14] ^= RotL(x[10] + x[ 6],  7);
        x[ 3] ^= RotL(x[15] + x[11],  7);

        x[ 8] ^= RotL(x[ 4] + x[ 0],  9);
        x[13] ^= RotL(x[ 9] + x[ 5],  9);
        x[ 2] ^= RotL(x[14] + x[10],  9);
        x[ 7] ^= RotL(x[ 3] + x[15],  9);

        x[12] ^= RotL(x[ 8] + x[ 4], 13);
        x[ 1] ^= RotL(x[13] + x[ 9], 13);
        x[ 6] ^= RotL(x[ 2] + x[14], 13);
        x[11] ^= RotL(x[ 7] + x[ 3], 13);

        x[ 0] ^= RotL(x[12] + x[ 8], 18);
        x[ 5] ^= RotL(x[ 1] + x[13], 18);
        x[10] ^= RotL(x[ 6] + x[ 2], 18);
        x[15] ^= RotL(x[11] + x[ 7], 18);

        x[ 1] ^= RotL(x[ 0] + x[ 3],  7);
        x[ 6] ^= RotL(x[ 5] + x[ 4],  7);
        x[11] ^= RotL(x[10] + x[ 9],  7);
        x[12] ^= RotL(x[15] + x[14],  7);

        x[ 2] ^= RotL(x[ 1] + x[ 0],  9);
        x[ 7] ^= RotL(x[ 6] + x[ 5],  9);
        x[ 8] ^= RotL(x[11] + x[10],  9);
        x[13] ^= RotL(x[12] + x[15],  9);

        x[ 3] ^= RotL(x[ 2] + x[ 1], 13);
        x[ 4] ^= RotL(x[ 7] + x[ 6], 13);
        x[ 9] ^= RotL(x[ 8] + x[11], 13);
        x[14] ^= RotL(x[13] + x[12], 13);

        x[ 0] ^= RotL(x[ 3] + x[ 2], 18);
        x[ 5] ^= RotL(x[ 4] + x[ 7], 18);
        x[10] ^= RotL(x[ 9] + x[ 8], 18);
        x[15] ^= RotL(x[14] + x[13], 18);
    }

    for (int i = 0; i < 16; ++i) b[i] += x[i];
}

} // namespace

void PBKDF2_SHA256(const uint8_t* passwd, const std::size_t passwdlen,
                   const uint8_t* salt, const std::size_t saltlen,
                   const uint64_t iterations, uint8_t* out, const std::size_t outlen)
{
    for (std::size_t block = 0; block * 32 < outlen; ++block) {
        std::array<unsigned char, 4> counter{};
        WriteBE32(counter.data(), static_cast<uint32_t>(block + 1));

        HMACSHA256 first{passwd, passwdlen};
        first.Write(salt, saltlen).Write(counter.data(), counter.size());

        std::array<unsigned char, 32> u{};
        std::array<unsigned char, 32> t{};
        first.Finalize(u.data());
        t = u;

        for (uint64_t iter = 2; iter <= iterations; ++iter) {
            HMACSHA256 next{passwd, passwdlen};
            next.Write(u.data(), u.size()).Finalize(u.data());
            for (std::size_t i = 0; i < t.size(); ++i) t[i] ^= u[i];
        }

        const std::size_t offset = block * 32;
        const std::size_t take = std::min<std::size_t>(32, outlen - offset);
        std::memcpy(out + offset, t.data(), take);
    }
}

void Scrypt1024_1_1_256(const unsigned char* input80, unsigned char* output32)
{
    std::array<unsigned char, 128> b{};
    std::array<uint32_t, 32> x{};
    std::vector<uint32_t> v(1024 * 32);

    PBKDF2_SHA256(input80, 80, input80, 80, 1, b.data(), b.size());

    for (std::size_t k = 0; k < x.size(); ++k) {
        x[k] = ReadLE32(b.data() + 4 * k);
    }

    for (uint32_t i = 0; i < 1024; ++i) {
        std::memcpy(v.data() + i * 32, x.data(), 128);
        XorSalsa8(x.data(), x.data() + 16);
        XorSalsa8(x.data() + 16, x.data());
    }

    for (uint32_t i = 0; i < 1024; ++i) {
        const uint32_t j = 32 * (x[16] & 1023);
        for (uint32_t k = 0; k < 32; ++k) x[k] ^= v[j + k];
        XorSalsa8(x.data(), x.data() + 16);
        XorSalsa8(x.data() + 16, x.data());
    }

    for (std::size_t k = 0; k < x.size(); ++k) {
        WriteLE32(b.data() + 4 * k, x[k]);
    }

    PBKDF2_SHA256(input80, 80, b.data(), b.size(), 1, output32, 32);
}

} // namespace zeus
