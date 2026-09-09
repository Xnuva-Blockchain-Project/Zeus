// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-present The Bitcoin Core developers
// Zeus migration draft: UNTESTED / DO NOT RELEASE
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_KERNEL_CHAINPARAMS_H
#define BITCOIN_KERNEL_CHAINPARAMS_H

#include <consensus/params.h>
#include <kernel/messagestartchars.h>
#include <primitives/block.h>
#include <uint256.h>
#include <util/chaintype.h>
#include <util/hash_type.h>
#include <util/vector.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

struct AssumeutxoHash : public BaseHash<uint256> {
    explicit AssumeutxoHash(const uint256& hash) : BaseHash(hash) {}
};

struct AssumeutxoData {
    int height;
    AssumeutxoHash hash_serialized;
    uint64_t m_chain_tx_count;
    uint256 blockhash;
};

struct ChainTxData {
    int64_t nTime;
    uint64_t tx_count;
    double dTxRate;
};

struct HeadersSyncParams {
    size_t commitment_period{0};
    size_t redownload_buffer_size{0};
};

class CChainParams
{
public:
    enum Base58Type {
        PUBKEY_ADDRESS,
        SCRIPT_ADDRESS,
        SECRET_KEY,
        EXT_PUBLIC_KEY,
        EXT_SECRET_KEY,
        MAX_BASE58_TYPES
    };

    const Consensus::Params& GetConsensus() const { return consensus; }
    const MessageStartChars& MessageStart() const { return pchMessageStart; }
    uint16_t GetDefaultPort() const { return nDefaultPort; }
    const CBlock& GenesisBlock() const { return genesis; }
    bool DefaultConsistencyChecks() const { return fDefaultConsistencyChecks; }
    bool IsTestChain() const { return m_chain_type != ChainType::MAIN; }
    bool IsMockableChain() const { return m_is_mockable_chain; }
    uint64_t PruneAfterHeight() const { return nPruneAfterHeight; }
    uint64_t AssumedBlockchainSize() const { return m_assumed_blockchain_size; }
    uint64_t AssumedChainStateSize() const { return m_assumed_chain_state_size; }
    bool MineBlocksOnDemand() const { return consensus.fPowNoRetargeting; }
    std::string GetChainTypeString() const { return ChainTypeToString(m_chain_type); }
    ChainType GetChainType() const { return m_chain_type; }
    const std::vector<std::string>& DNSSeeds() const { return vSeeds; }
    const std::vector<unsigned char>& Base58Prefix(Base58Type type) const { return base58Prefixes[type]; }

    // Modern Bitcoin uses Bech32 here. Historical Zeus did not.
    // The migration keeps this blank until an address-format decision is made.
    const std::string& Bech32HRP() const { return bech32_hrp; }

    // ZEUS LEGACY IDENTITY: retained for the legacy CashAddr-compatible codec
    // and development-fund reward path when those modules are ported.
    const std::string& CashAddrPrefix() const { return cashaddr_prefix; }
    const std::string& DevFundAddress() const { return m_dev_fund_address; }

    const std::vector<uint8_t>& FixedSeeds() const { return vFixedSeeds; }
    const HeadersSyncParams& HeadersSync() const { return m_headers_sync_params; }

    std::vector<int> GetAvailableSnapshotHeights() const;
    std::optional<AssumeutxoData> AssumeutxoForHeight(int height) const
    {
        return FindFirst(m_assumeutxo_data, [&](const auto& d) { return d.height == height; });
    }
    std::optional<AssumeutxoData> AssumeutxoForBlockhash(const uint256& blockhash) const
    {
        return FindFirst(m_assumeutxo_data, [&](const auto& d) { return d.blockhash == blockhash; });
    }
    const ChainTxData& TxData() const { return chainTxData; }

    struct SigNetOptions {
        std::optional<std::vector<uint8_t>> challenge{};
        std::optional<std::vector<std::string>> seeds{};
    };

    struct VersionBitsParameters {
        int64_t start_time;
        int64_t timeout;
        int min_activation_height;
    };

    struct RegTestOptions {
        std::unordered_map<Consensus::DeploymentPos, VersionBitsParameters> version_bits_parameters{};
        std::unordered_map<Consensus::BuriedDeployment, int> activation_heights{};
        bool fastprune{false};
        bool enforce_bip94{false};
    };

    // Historical Zeus has main, test and regtest. Bitcoin testnet4/signet are
    // intentionally not treated as Zeus networks in this migration.
    static std::unique_ptr<const CChainParams> Main();
    static std::unique_ptr<const CChainParams> TestNet();
    static std::unique_ptr<const CChainParams> RegTest(const RegTestOptions& options);
    static std::unique_ptr<const CChainParams> TestNet4();
    static std::unique_ptr<const CChainParams> SigNet(const SigNetOptions& options);

protected:
    CChainParams() = default;

    Consensus::Params consensus;
    MessageStartChars pchMessageStart{};
    uint16_t nDefaultPort{0};
    uint64_t nPruneAfterHeight{0};
    uint64_t m_assumed_blockchain_size{0};
    uint64_t m_assumed_chain_state_size{0};
    std::vector<std::string> vSeeds;
    std::vector<unsigned char> base58Prefixes[MAX_BASE58_TYPES];
    std::string bech32_hrp;
    std::string cashaddr_prefix;
    std::string m_dev_fund_address;
    ChainType m_chain_type{ChainType::MAIN};
    CBlock genesis;
    std::vector<uint8_t> vFixedSeeds;
    bool fDefaultConsistencyChecks{false};
    bool m_is_mockable_chain{false};
    std::vector<AssumeutxoData> m_assumeutxo_data;
    ChainTxData chainTxData{0, 0, 0};
    HeadersSyncParams m_headers_sync_params{};
};

std::optional<ChainType> GetNetworkForMagic(const MessageStartChars& pchMessageStart);

#endif // BITCOIN_KERNEL_CHAINPARAMS_H
