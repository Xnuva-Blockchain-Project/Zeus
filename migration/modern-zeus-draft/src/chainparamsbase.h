// Copyright (c) 2014-present The Bitcoin Core developers
// Zeus migration draft: UNTESTED / DO NOT RELEASE
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CHAINPARAMSBASE_H
#define BITCOIN_CHAINPARAMSBASE_H

#include <util/chaintype.h>

#include <cstdint>
#include <memory>
#include <string>

class ArgsManager;

class CBaseChainParams
{
public:
    const std::string& DataDir() const { return strDataDir; }
    uint16_t RPCPort() const { return m_rpc_port; }

    CBaseChainParams() = delete;
    CBaseChainParams(const std::string& data_dir, uint16_t rpc_port)
        : m_rpc_port{rpc_port}, strDataDir{data_dir} {}

private:
    const uint16_t m_rpc_port;
    std::string strDataDir;
};

std::unique_ptr<CBaseChainParams> CreateBaseChainParams(ChainType chain);
void SetupChainParamsBaseOptions(ArgsManager& argsman);
const CBaseChainParams& BaseParams();
void SelectBaseParams(ChainType chain);

// Historical Zeus networks only. Bitcoin testnet4/signet are intentionally
// excluded from the migration surface.
#define LIST_CHAIN_NAMES "main, test, regtest"

#endif // BITCOIN_CHAINPARAMSBASE_H
