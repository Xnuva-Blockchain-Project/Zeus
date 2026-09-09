// Zeus prospective supply-cap consensus helpers.
// UNTESTED / DO NOT RELEASE.
#ifndef ZEUS_SUPPLY_CAP_H
#define ZEUS_SUPPLY_CAP_H

#include <consensus/amount.h>

class CBlockIndex;
namespace Consensus { struct Params; }

namespace zeus {

/** Net new issuance for a PoW block: coinbase outputs minus transaction fees. */
CAmount NetNewPoWIssuance(CAmount coinbase_value, CAmount fees);

/** Net new issuance for a PoS block: coinstake reward minus recycled transaction fees. */
CAmount NetNewPoSIssuance(CAmount actual_stake_reward, CAmount fees);

/** Remaining mintable supply once the prospective cap is active. */
CAmount RemainingMintableSupply(CAmount previous_issued, const Consensus::Params& params);

/**
 * Limit a scheduled subsidy so the next block cannot push cumulative issued
 * supply beyond the configured maximum.
 *
 * Before activation, the scheduled subsidy is returned unchanged.
 */
CAmount LimitSubsidyBySupplyCap(int height,
                               CAmount previous_issued,
                               CAmount scheduled_subsidy,
                               const Consensus::Params& params);

/** Consensus check for the block's actual net new issuance. */
bool CheckSupplyCap(int height,
                    CAmount previous_issued,
                    CAmount block_net_new_issuance,
                    const Consensus::Params& params);

/** Deterministic cumulative supply value to write into CBlockIndex. */
CAmount NextIssuedSupply(CAmount previous_issued, CAmount block_net_new_issuance);

/**
 * Validate the block's actual net new issuance and record cumulative issued
 * supply in its modern CBlockIndex entry.
 *
 * For PoW pass coinbase_value and actual_stake_reward=0.
 * For PoS pass actual_stake_reward and coinbase_value=0.
 */
bool ValidateAndRecordSupply(CBlockIndex& index,
                             bool proof_of_stake,
                             CAmount coinbase_value,
                             CAmount actual_stake_reward,
                             CAmount fees,
                             const Consensus::Params& params);

} // namespace zeus

#endif // ZEUS_SUPPLY_CAP_H
