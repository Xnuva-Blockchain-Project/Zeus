// Copyright (c) 2014-2018 The Zeus Developers
// Copyright (c) 2011-2013 The PPCoin developers
// Zeus Bitcoin Core 31.1 migration draft: UNTESTED / DO NOT RELEASE

#include <zeus/pos.h>

#include <arith_uint256.h>
#include <chain.h>
#include <coins.h>
#include <consensus/params.h>
#include <hash.h>
#include <script/interpreter.h>

namespace zeus {

uint256 ComputeStakeModifier(const CBlockIndex* pindex_prev, const uint256& kernel)
{
    if (pindex_prev == nullptr) return uint256{};

    HashWriter writer{};
    writer << kernel << pindex_prev->nStakeModifier;
    return writer.GetHash();
}

bool CheckCoinStakeTimestamp(const int64_t block_time,
                             const int64_t tx_time,
                             const Consensus::Params& params)
{
    if (params.IsProtocolV2(block_time)) {
        return block_time == tx_time &&
               ((tx_time & params.nStakeTimestampMask) == 0);
    }
    return block_time == tx_time;
}

bool CheckStakeBlockTimestamp(const int64_t block_time, const Consensus::Params& params)
{
    return CheckCoinStakeTimestamp(block_time, block_time, params);
}

bool CheckStakeKernelHash(const CBlockIndex* pindex_prev,
                          const uint32_t nbits,
                          const uint32_t block_from_time,
                          const CAmount prevout_value,
                          const COutPoint& prevout,
                          const uint32_t tx_time)
{
    if (pindex_prev == nullptr) return false;
    if (tx_time < block_from_time) return false;
    if (prevout_value <= 0) return false;

    arith_uint256 target;
    target.SetCompact(nbits);
    target *= arith_uint256{static_cast<uint64_t>(prevout_value)};

    HashWriter writer{};
    writer << pindex_prev->nStakeModifier;
    writer << block_from_time;
    writer << prevout.hash;
    writer << prevout.n;
    writer << tx_time;

    return UintToArith256(writer.GetHash()) <= target;
}

bool CheckProofOfStake(const CBlockIndex* pindex_prev,
                       const CTransaction& tx,
                       const uint32_t nbits,
                       const uint32_t tx_time,
                       const Coin& kernel_coin,
                       const uint32_t block_from_time,
                       const Consensus::Params& params,
                       TxValidationState& state)
{
    if (pindex_prev == nullptr) {
        return state.Invalid(TxValidationResult::TX_CONSENSUS,
                             "bad-pos-no-prev");
    }
    if (!tx.IsCoinStake() || tx.vin.empty()) {
        return state.Invalid(TxValidationResult::TX_CONSENSUS,
                             "bad-pos-not-coinstake");
    }
    if (kernel_coin.IsSpent()) {
        return state.Invalid(TxValidationResult::TX_MISSING_INPUTS,
                             "bad-pos-kernel-spent");
    }

    const int next_height = pindex_prev->nHeight + 1;
    if (next_height - static_cast<int>(kernel_coin.nHeight) < params.nCoinbaseMaturity) {
        return state.Invalid(TxValidationResult::TX_PREMATURE_SPEND,
                             "bad-pos-kernel-immature");
    }

    ScriptError script_error{SCRIPT_ERR_UNKNOWN_ERROR};
    const auto& txin = tx.vin[0];
    TransactionSignatureChecker checker{
        &tx,
        0,
        kernel_coin.out.nValue,
        MissingDataBehavior::ASSERT_FAIL
    };
    if (!VerifyScript(txin.scriptSig,
                      kernel_coin.out.scriptPubKey,
                      nullptr,
                      SCRIPT_VERIFY_NONE,
                      checker,
                      &script_error)) {
        return state.Invalid(TxValidationResult::TX_CONSENSUS,
                             "bad-pos-kernel-signature");
    }

    if (!CheckStakeKernelHash(pindex_prev,
                              nbits,
                              block_from_time,
                              kernel_coin.out.nValue,
                              txin.prevout,
                              tx_time)) {
        return state.Invalid(TxValidationResult::TX_CONSENSUS,
                             "bad-pos-kernel-hash");
    }

    return true;
}

} // namespace zeus
