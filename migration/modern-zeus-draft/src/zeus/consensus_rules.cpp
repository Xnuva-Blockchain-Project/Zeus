// Zeus legacy consensus rule adapter for Bitcoin Core 31.1.
// UNTESTED / DO NOT RELEASE.

#include <zeus/consensus_rules.h>

#include <consensus/consensus.h>
#include <consensus/params.h>
#include <script/interpreter.h>
#include <zeus/supply_cap.h>

namespace zeus {

CAmount ProofOfWorkSubsidy()
{
    // Preserve current legacy Zeus behaviour during mechanical migration.
    return 40 * COIN;
}

CAmount ProofOfStakeSubsidy(const int height, const Consensus::Params& params)
{
    // Preserve the CURRENT legacy source behaviour before supply-cap activation.
    if (height > params.nLastPOWBlock) return 0;
    return 14 * COIN;
}

CAmount CappedProofOfWorkSubsidy(const int height,
                                 const CAmount previous_issued,
                                 const Consensus::Params& params)
{
    return LimitSubsidyBySupplyCap(height, previous_issued, ProofOfWorkSubsidy(), params);
}

CAmount CappedProofOfStakeSubsidy(const int height,
                                  const CAmount previous_issued,
                                  const Consensus::Params& params)
{
    return LimitSubsidyBySupplyCap(height, previous_issued, ProofOfStakeSubsidy(height, params), params);
}

script_verify_flags ScriptVerifyFlags(const int64_t block_time, const Consensus::Params& params)
{
    // These four rules are always active in current legacy Zeus ConnectBlock().
    script_verify_flags flags{
        SCRIPT_VERIFY_P2SH |
        SCRIPT_VERIFY_DERSIG |
        SCRIPT_VERIFY_LOW_S |
        SCRIPT_VERIFY_ZEUS_DERKEY
    };

    // Legacy Zeus activates CLTV and NULLDUMMY by protocol-v3 timestamp.
    if (params.IsProtocolV3(block_time)) {
        flags |= SCRIPT_VERIFY_CHECKLOCKTIMEVERIFY;
        flags |= SCRIPT_VERIFY_NULLDUMMY;
    }

    // Legacy Zeus activates CSV script semantics only by protocol-v3.1 timestamp.
    if (params.IsProtocolV3_1(block_time)) {
        flags |= SCRIPT_VERIFY_CHECKSEQUENCEVERIFY;
    }

    // Deliberately NO WITNESS and NO TAPROOT flags here.
    return flags;
}

unsigned int LockTimeFlags(const int64_t block_time, const Consensus::Params& params)
{
    return params.IsProtocolV3_1(block_time) ? LOCKTIME_VERIFY_SEQUENCE : 0;
}

} // namespace zeus
