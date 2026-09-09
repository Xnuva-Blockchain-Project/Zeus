// Zeus legacy consensus rule adapter for Bitcoin Core 31.1.
// UNTESTED / DO NOT RELEASE.
#ifndef ZEUS_CONSENSUS_RULES_H
#define ZEUS_CONSENSUS_RULES_H

#include <consensus/amount.h>
#include <script/verify_flags.h>

#include <cstdint>

namespace Consensus { struct Params; }

namespace zeus {

inline constexpr unsigned int DONATION_PERCENTAGE{14};

CAmount ProofOfWorkSubsidy();
CAmount ProofOfStakeSubsidy(int height, const Consensus::Params& params);

// Prospective capped subsidy helpers. These preserve the legacy scheduled
// subsidy before activation and trim only the final minting block if needed.
CAmount CappedProofOfWorkSubsidy(int height, CAmount previous_issued, const Consensus::Params& params);
CAmount CappedProofOfStakeSubsidy(int height, CAmount previous_issued, const Consensus::Params& params);

// Historical Zeus script flags are time-based, not Bitcoin buried-deployment heights.
script_verify_flags ScriptVerifyFlags(int64_t block_time, const Consensus::Params& params);

// Historical Zeus enables BIP68 sequence locks only at protocol-v3.1.
unsigned int LockTimeFlags(int64_t block_time, const Consensus::Params& params);

} // namespace zeus

#endif // ZEUS_CONSENSUS_RULES_H
