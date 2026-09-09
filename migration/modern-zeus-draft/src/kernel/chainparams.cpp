// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-present The Bitcoin Core developers
// Zeus migration draft: UNTESTED / DO NOT RELEASE
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kernel/chainparams.h>

#include <consensus/amount.h>
#include <consensus/merkle.h>
#include <consensus/params.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <script/script.h>
#include <uint256.h>
#include <util/chaintype.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <limits>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <vector>

namespace {

static CBlock CreateZeusGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion)
{
    // Preserve the historical Zeus genesis transaction byte-for-byte.
    const char* pszTimestamp =
        "The Telegraph 06 02 2025 Britain is inexorably heading for bankruptcy";

    CMutableTransaction txNew;
    txNew.version = 1;
    txNew.nTime = nTime;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig =
        CScript() << 0 << CScriptNum(42)
                  << std::vector<unsigned char>(
                         reinterpret_cast<const unsigned char*>(pszTimestamp),
                         reinterpret_cast<const unsigned char*>(pszTimestamp) + std::strlen(pszTimestamp));

    // Historical Zeus CreateGenesisBlock accepted a pubkey script argument
    // but did not assign it to vout[0]. Preserve that quirk.
    txNew.vout[0].nValue = 0;
    txNew.vout[0].scriptPubKey.clear();

    CBlock genesis;
    genesis.nTime = nTime;
    genesis.nBits = nBits;
    genesis.nNonce = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.vchBlockSig.clear();
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

static void DisableBitcoinOnlyDeployments(Consensus::Params& consensus)
{
    // Zeus historically did not activate SegWit or Taproot. Do not inherit
    // Bitcoin Core's activation history during the mechanical port.
    consensus.SegwitHeight = std::numeric_limits<int>::max();
    consensus.CSVHeight = std::numeric_limits<int>::max();
    consensus.BIP65Height = std::numeric_limits<int>::max();

    // DERSIG was always enforced by legacy Zeus.
    consensus.BIP66Height = 0;

    // Legacy Zeus set BIP34Height=-1, effectively retaining BIP30 checks.
    // Modern code uses height-based buried deployments, so keep BIP34 disabled
    // here and port the historical BIP30/BIP34 behaviour explicitly in validation.
    consensus.BIP34Height = std::numeric_limits<int>::max();
    consensus.BIP34Hash = uint256{};
    consensus.MinBIP9WarningHeight = std::numeric_limits<int>::max();

    consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
    consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime =
        Consensus::BIP9Deployment::NEVER_ACTIVE;
    consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout =
        Consensus::BIP9Deployment::NO_TIMEOUT;
    consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0;
}

static void SetLegacyTestDummy(Consensus::Params& consensus, uint32_t threshold, uint32_t period)
{
    consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
    consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601;
    consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999;
    consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0;
    consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].threshold = threshold;
    consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].period = period;
}

class CMainParams final : public CChainParams
{
public:
    CMainParams()
    {
        m_chain_type = ChainType::MAIN;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();

        // This modern Bitcoin field is not Zeus's monetary rule. Reward issuance
        // is ported separately from legacy Zeus and must not use Bitcoin halving.
        consensus.nSubsidyHalvingInterval = std::numeric_limits<int>::max();

        DisableBitcoinOnlyDeployments(consensus);
        SetLegacyTestDummy(consensus, 11, 12);

        consensus.powLimit = uint256{"00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        consensus.posLimit = uint256{"00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        consensus.posLimitV2 = uint256{"00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};

        consensus.nTargetTimespan = 60 * 60;
        consensus.nTargetSpacingV1 = 5 * 60;
        consensus.nTargetSpacing = 5 * 60;
        consensus.nPowTargetTimespan = consensus.nTargetTimespan;
        consensus.nPowTargetSpacing = consensus.nTargetSpacing;

        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.enforce_BIP94 = false;
        consensus.fPowNoRetargeting = false;
        consensus.fPoSNoRetargeting = false;

        consensus.nProtocolV1RetargetingFixedTime = 1738936389;
        consensus.nProtocolV2Time = 1738936390;
        consensus.nProtocolV3Time = 1738936391;
        consensus.nProtocolV3_1Time = 4102437600;
        consensus.nLastPOWBlock = 1'000'000;

        // Published Zeus maximum supply. The enforcement code is complete,
        // but mainnet activation is intentionally unset until the authoritative
        // live tip and historical issued supply are proven.
        consensus.nMaxMoneySupply = 40'000'000 * COIN;
        consensus.nSupplyCapActivationHeight = -1; // RELEASE BLOCKER

        consensus.nStakeTimestampMask = 0x0f;
        consensus.nCoinbaseMaturity = 100;
        consensus.nMaxReorganizationDepth = 500;

        consensus.nMinimumChainWork =
            uint256{"0000000000000000000000000000000000000000000000000000000000100001"};
        consensus.defaultAssumeValid = uint256{};

        pchMessageStart = {0x75, 0x80, 0x3c, 0xc0};
        nDefaultPort = 22396;
        nPruneAfterHeight = 100000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        genesis = CreateZeusGenesisBlock(1738936389, 955918, 0x1e0fffff, 1);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock ==
               uint256{"000007a830306520fa2efc02258706ec6bac5514f6396b9313df676ac498e8ee"});
        assert(genesis.hashMerkleRoot ==
               uint256{"f4dec70165441cbaef0a157c82321b594277f0c080021f5d5edf954dec3012b7"});

        vSeeds.clear();
        vFixedSeeds.clear(); // legacy fixed seeds need format conversion before reintroduction

        base58Prefixes[PUBKEY_ADDRESS] = {80};
        base58Prefixes[SCRIPT_ADDRESS] = {81};
        base58Prefixes[SECRET_KEY] = {208};
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};

        // Do not silently introduce a new SegWit/Bech32 address format.
        bech32_hrp.clear();
        cashaddr_prefix = "zeus";
        m_dev_fund_address = "ZFEKPrfhN7MdiznZo4xgajQGU7FAFErQVv";

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;
        m_assumeutxo_data.clear();
        chainTxData = {1738936389, 0, 0.0};

        // Non-consensus modern header-sync tuning; to be calibrated during testing.
        m_headers_sync_params = {.commitment_period = 641, .redownload_buffer_size = 15218};
    }
};

class CTestNetParams final : public CChainParams
{
public:
    CTestNetParams()
    {
        m_chain_type = ChainType::TESTNET;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = std::numeric_limits<int>::max();

        DisableBitcoinOnlyDeployments(consensus);
        SetLegacyTestDummy(consensus, 11, 12);

        consensus.powLimit = uint256{"0000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        consensus.posLimit = uint256{"00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        consensus.posLimitV2 = uint256{"00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};

        consensus.nTargetTimespan = 60 * 60;
        consensus.nTargetSpacingV1 = 5 * 60;
        consensus.nTargetSpacing = 5 * 60;
        consensus.nPowTargetTimespan = consensus.nTargetTimespan;
        consensus.nPowTargetSpacing = consensus.nTargetSpacing;

        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.enforce_BIP94 = false;
        consensus.fPowNoRetargeting = false;
        consensus.fPoSNoRetargeting = false;

        consensus.nProtocolV1RetargetingFixedTime = 1738936389;
        consensus.nProtocolV2Time = 1738936390;
        consensus.nProtocolV3Time = 1738936391;
        consensus.nProtocolV3_1Time = 1738936392;
        consensus.nLastPOWBlock = std::numeric_limits<int>::max();
        consensus.nMaxMoneySupply = 40'000'000 * COIN;
        consensus.nSupplyCapActivationHeight = -1;
        consensus.nStakeTimestampMask = 0x0f;
        consensus.nCoinbaseMaturity = 10;
        consensus.nMaxReorganizationDepth = 500;

        consensus.nMinimumChainWork =
            uint256{"0000000000000000000000000000000000000000000000302f630a91c2ec0230"};
        consensus.defaultAssumeValid = uint256{};

        pchMessageStart = {0xf1, 0xf4, 0x5a, 0x63};
        nDefaultPort = 32396;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        genesis = CreateZeusGenesisBlock(1738936389, 53947, 0x1f00ffff, 1);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock ==
               uint256{"00000197072768a59596da0ad6579421bc820ad3a2a8056647705812f3bb4fb0"});
        assert(genesis.hashMerkleRoot ==
               uint256{"f4dec70165441cbaef0a157c82321b594277f0c080021f5d5edf954dec3012b7"});

        vSeeds.clear();
        vFixedSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = {80};
        base58Prefixes[SCRIPT_ADDRESS] = {81};
        base58Prefixes[SECRET_KEY] = {239};
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp.clear();
        cashaddr_prefix = "zustest";
        m_dev_fund_address = "ZHZsNLabknbVoSsNyrv597E48VANHrB75C";

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;
        m_assumeutxo_data.clear();
        chainTxData = {1738936389, 0, 0.0};
        m_headers_sync_params = {.commitment_period = 673, .redownload_buffer_size = 14460};
    }
};

class CRegTestParams final : public CChainParams
{
public:
    explicit CRegTestParams(const RegTestOptions& opts)
    {
        m_chain_type = ChainType::REGTEST;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = std::numeric_limits<int>::max();

        DisableBitcoinOnlyDeployments(consensus);
        SetLegacyTestDummy(consensus, 108, 144);

        consensus.powLimit = uint256{"0000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        consensus.posLimit = uint256{"00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        consensus.posLimitV2 = uint256{"00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};

        consensus.nTargetTimespan = 60 * 60;
        consensus.nTargetSpacingV1 = 64;
        consensus.nTargetSpacing = 5 * 60;
        consensus.nPowTargetTimespan = consensus.nTargetTimespan;
        consensus.nPowTargetSpacing = consensus.nTargetSpacing;

        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.enforce_BIP94 = false;
        consensus.fPowNoRetargeting = true;
        consensus.fPoSNoRetargeting = true;

        consensus.nProtocolV1RetargetingFixedTime = 1738936389;
        consensus.nProtocolV2Time = 1738936390;
        consensus.nProtocolV3Time = 1738936391;
        consensus.nProtocolV3_1Time = 4102437600;
        consensus.nLastPOWBlock = 1000;
        consensus.nMaxMoneySupply = 40'000'000 * COIN;
        consensus.nSupplyCapActivationHeight = -1;
        consensus.nStakeTimestampMask = 0x0f;
        consensus.nCoinbaseMaturity = 10;
        consensus.nMaxReorganizationDepth = 50;

        consensus.nMinimumChainWork = uint256{};
        consensus.defaultAssumeValid = uint256{};

        // Preserve only legacy Zeus regtest overrides that map cleanly.
        for (const auto& [deployment_pos, version_bits_params] : opts.version_bits_parameters) {
            consensus.vDeployments[deployment_pos].nStartTime = version_bits_params.start_time;
            consensus.vDeployments[deployment_pos].nTimeout = version_bits_params.timeout;
            consensus.vDeployments[deployment_pos].min_activation_height =
                version_bits_params.min_activation_height;
        }

        pchMessageStart = {0x75, 0x80, 0x3c, 0x06};
        nDefaultPort = 35714;
        nPruneAfterHeight = opts.fastprune ? 100 : 100000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        genesis = CreateZeusGenesisBlock(1738936389, 53947, 0x1f00ffff, 1);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock ==
               uint256{"00000197072768a59596da0ad6579421bc820ad3a2a8056647705812f3bb4fb0"});
        assert(genesis.hashMerkleRoot ==
               uint256{"f4dec70165441cbaef0a157c82321b594277f0c080021f5d5edf954dec3012b7"});

        vSeeds.clear();
        vFixedSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = {80};
        base58Prefixes[SCRIPT_ADDRESS] = {81};
        base58Prefixes[SECRET_KEY] = {208};
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};

        bech32_hrp.clear();
        cashaddr_prefix = "zusreg";

        fDefaultConsistencyChecks = true;
        m_is_mockable_chain = true;
        m_assumeutxo_data.clear();
        chainTxData = {1738936389, 0, 0.0};
        m_headers_sync_params = {.commitment_period = 275, .redownload_buffer_size = 7017};
    }
};

} // namespace

std::unique_ptr<const CChainParams> CChainParams::Main()
{
    return std::make_unique<const CMainParams>();
}

std::unique_ptr<const CChainParams> CChainParams::TestNet()
{
    return std::make_unique<const CTestNetParams>();
}

std::unique_ptr<const CChainParams> CChainParams::RegTest(const RegTestOptions& options)
{
    return std::make_unique<const CRegTestParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::TestNet4()
{
    throw std::runtime_error("Zeus migration draft: testnet4 is not a historical Zeus network");
}

std::unique_ptr<const CChainParams> CChainParams::SigNet(const SigNetOptions&)
{
    throw std::runtime_error("Zeus migration draft: signet is not a historical Zeus network");
}

std::vector<int> CChainParams::GetAvailableSnapshotHeights() const
{
    std::vector<int> heights;
    heights.reserve(m_assumeutxo_data.size());
    for (const auto& data : m_assumeutxo_data) heights.push_back(data.height);
    return heights;
}

std::optional<ChainType> GetNetworkForMagic(const MessageStartChars& message)
{
    const auto mainnet = CChainParams::Main()->MessageStart();
    const auto testnet = CChainParams::TestNet()->MessageStart();
    const auto regtest = CChainParams::RegTest({})->MessageStart();

    if (std::ranges::equal(message, mainnet)) return ChainType::MAIN;
    if (std::ranges::equal(message, testnet)) return ChainType::TESTNET;
    if (std::ranges::equal(message, regtest)) return ChainType::REGTEST;
    return std::nullopt;
}
