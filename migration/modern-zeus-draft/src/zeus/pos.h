// Copyright (c) 2014-2018 The Zeus Developers
// Copyright (c) 2011-2013 The PPCoin developers
// Stake cache ancestry: Qtum
// Zeus Bitcoin Core 31.1 migration draft: UNTESTED / DO NOT RELEASE

#ifndef ZEUS_POS_H
#define ZEUS_POS_H

#include <consensus/amount.h>
#include <consensus/validation.h>
#include <primitives/transaction.h>
#include <uint256.h>

#include <cstdint>

class CBlockIndex;
class Coin;
namespace Consensus { struct Params; }

namespace zeus {

uint256 ComputeStakeModifier(const CBlockIndex* pindex_prev, const uint256& kernel);

bool CheckCoinStakeTimestamp(int64_t block_time, int64_t tx_time, const Consensus::Params& params);
bool CheckStakeBlockTimestamp(int64_t block_time, const Consensus::Params& params);

bool CheckStakeKernelHash(const CBlockIndex* pindex_prev,
                          uint32_t nbits,
                          uint32_t block_from_time,
                          CAmount prevout_value,
                          const COutPoint& prevout,
                          uint32_t tx_time);

/**
 * Validate the consensus-specific first input of a Zeus coinstake against a
 * modern UTXO Coin. block_from_time is the creating transaction's nTime
 * when nonzero, otherwise the containing block's timestamp.
 *
 * This deliberately avoids txindex/full-transaction lookup.
 */
bool CheckProofOfStake(const CBlockIndex* pindex_prev,
                       const CTransaction& tx,
                       uint32_t nbits,
                       uint32_t tx_time,
                       const Coin& kernel_coin,
                       uint32_t block_from_time,
                       const Consensus::Params& params,
                       TxValidationState& state);

} // namespace zeus

#endif // ZEUS_POS_H
