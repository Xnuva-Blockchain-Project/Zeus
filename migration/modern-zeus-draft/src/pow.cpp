// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-present The Bitcoin Core developers
// Zeus migration draft: UNTESTED / DO NOT RELEASE
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pow.h>

#include <arith_uint256.h>
#include <chain.h>
#include <primitives/block.h>
#include <uint256.h>
#include <util/check.h>

static arith_uint256 GetTargetLimit(int64_t nTime, const Consensus::Params& params, bool fProofOfStake)
{
    const uint256& limit = fProofOfStake
        ? (params.IsProtocolV2(nTime) ? params.posLimitV2 : params.posLimit)
        : params.powLimit;
    return UintToArith256(limit);
}

unsigned int GetNextTargetRequired(const CBlockIndex* pindexLast, const CBlockHeader* /*pblock*/, const Consensus::Params& params, bool fProofOfStake)
{
    // Genesis / first indexed block.
    if (pindexLast == nullptr) {
        return UintToArith256(params.powLimit).GetCompact();
    }

    const unsigned int nTargetLimit = GetTargetLimit(pindexLast->GetBlockTime(), params, fProofOfStake).GetCompact();
    const CBlockIndex* pindexPrev = GetLastBlockIndex(pindexLast, fProofOfStake);

    if (pindexPrev == nullptr || pindexPrev->pprev == nullptr) {
        return nTargetLimit;
    }

    const CBlockIndex* pindexPrevPrev = GetLastBlockIndex(pindexPrev->pprev, fProofOfStake);
    if (pindexPrevPrev == nullptr || pindexPrevPrev->pprev == nullptr) {
        return nTargetLimit;
    }

    return CalculateNextTargetRequired(pindexPrev, pindexPrevPrev->GetBlockTime(), params, fProofOfStake);
}

unsigned int CalculateNextTargetRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params, bool fProofOfStake)
{
    if (fProofOfStake) {
        if (params.fPoSNoRetargeting) return pindexLast->nBits;
    } else {
        if (params.fPowNoRetargeting) return pindexLast->nBits;
    }

    const int64_t nTargetSpacing = params.IsProtocolV2(pindexLast->GetBlockTime())
        ? params.nTargetSpacing
        : params.nTargetSpacingV1;

    int64_t nActualSpacing = pindexLast->GetBlockTime() - nFirstBlockTime;

    if (params.IsProtocolV1RetargetingFixed(pindexLast->GetBlockTime()) && nActualSpacing < 0) {
        nActualSpacing = nTargetSpacing;
    }
    if (params.IsProtocolV3(pindexLast->GetBlockTime()) && nActualSpacing > nTargetSpacing * 10) {
        nActualSpacing = nTargetSpacing * 10;
    }

    const arith_uint256 bnTargetLimit = GetTargetLimit(pindexLast->GetBlockTime(), params, fProofOfStake);
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);

    const int64_t nInterval = params.nTargetTimespan / nTargetSpacing;
    bnNew *= ((nInterval - 1) * nTargetSpacing + nActualSpacing + nActualSpacing);
    bnNew /= ((nInterval + 1) * nTargetSpacing);

    if (bnNew <= 0 || bnNew > bnTargetLimit) {
        bnNew = bnTargetLimit;
    }
    return bnNew.GetCompact();
}

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader* pblock, const Consensus::Params& params)
{
    return GetNextTargetRequired(pindexLast, pblock, params, false);
}

unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    return CalculateNextTargetRequired(pindexLast, nFirstBlockTime, params, false);
}

// Check that on difficulty adjustments, the new difficulty does not increase
// or decrease beyond the permitted limits.
bool PermittedDifficultyTransition(const Consensus::Params& /*params*/, int64_t /*height*/, uint32_t /*old_nbits*/, uint32_t /*new_nbits*/)
{
    // ZEUS LEGACY CONSENSUS retargets per proof type rather than using Bitcoin's
    // interval transition rule. Exact nBits validation must use
    // GetNextTargetRequired() in the Zeus block-validation path.
    return true;
}

// Bypasses the actual proof of work check during fuzz testing with a simplified validation checking whether
// the most significant bit of the last byte of the hash is set.
bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    if (EnableFuzzDeterminism()) return (hash.data()[31] & 0x80) == 0;
    return CheckProofOfWorkImpl(hash, nBits, params);
}

std::optional<arith_uint256> DeriveTarget(unsigned int nBits, const uint256 pow_limit)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(pow_limit))
        return {};

    return bnTarget;
}

bool CheckProofOfWorkImpl(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    auto bnTarget{DeriveTarget(nBits, params.powLimit)};
    if (!bnTarget) return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}
