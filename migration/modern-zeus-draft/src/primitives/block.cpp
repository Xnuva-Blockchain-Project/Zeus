// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-present The Bitcoin Core developers
// Zeus migration draft: UNTESTED / DO NOT RELEASE
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <primitives/block.h>

#include <hash.h>
#include <streams.h>
#include <tinyformat.h>
#include <zeus/crypto/scrypt.h>

#include <cassert>
#include <memory>
#include <span>
#include <sstream>

uint256 CBlockHeader::GetPoWHash() const
{
    // Historical Zeus Scrypt hashes the canonical 80-byte serialized header.
    DataStream stream;
    stream << *this;
    assert(stream.size() == 80);

    uint256 out;
    zeus::Scrypt1024_1_1_256(stream.data(), out.data());
    return out;
}

uint256 CBlockHeader::GetHash() const
{
    // Historical Zeus changed the block-id hash at block version 7.
    // The genesis and all <=6 blocks therefore retain their Scrypt IDs.
    if (nVersion <= 6) return GetPoWHash();
    return (HashWriter{} << *this).GetHash();
}

std::string CBlock::ToString() const
{
    std::stringstream s;
    s << strprintf("CBlock(hash=%s, ver=0x%08x, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%u, vtx=%u)\n",
        GetHash().ToString(),
        nVersion,
        hashPrevBlock.ToString(),
        hashMerkleRoot.ToString(),
        nTime, nBits, nNonce,
        vtx.size());
    s << strprintf("  vchBlockSig.size=%u, proof=%s\n",
        vchBlockSig.size(), IsProofOfStake() ? "stake" : "work");
    for (const auto& tx : vtx) {
        s << "  " << tx->ToString() << "\n";
    }
    return s.str();
}
