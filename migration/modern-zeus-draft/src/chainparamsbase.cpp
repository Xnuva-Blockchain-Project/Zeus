// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-present The Bitcoin Core developers
// Zeus migration draft: UNTESTED / DO NOT RELEASE
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparamsbase.h>

#include <common/args.h>
#include <util/chaintype.h>

#include <cassert>
#include <memory>
#include <stdexcept>

void SetupChainParamsBaseOptions(ArgsManager& argsman)
{
    argsman.AddArg("-chain=<chain>",
                   "Use the Zeus chain <chain> (default: main). Allowed values: " LIST_CHAIN_NAMES,
                   ArgsManager::ALLOW_ANY, OptionsCategory::CHAINPARAMS);
    argsman.AddArg("-regtest",
                   "Enter Zeus regression-test mode.",
                   ArgsManager::ALLOW_ANY | ArgsManager::DEBUG_ONLY,
                   OptionsCategory::CHAINPARAMS);
    argsman.AddArg("-testnet",
                   "Use the Zeus test network. Equivalent to -chain=test.",
                   ArgsManager::ALLOW_ANY, OptionsCategory::CHAINPARAMS);
    argsman.AddArg("-vbparams=deployment:start:end[:min_activation_height]",
                   "Set version-bits parameters on Zeus regtest only.",
                   ArgsManager::ALLOW_ANY | ArgsManager::DEBUG_ONLY,
                   OptionsCategory::CHAINPARAMS);
}

static std::unique_ptr<CBaseChainParams> globalChainBaseParams;

const CBaseChainParams& BaseParams()
{
    assert(globalChainBaseParams);
    return *globalChainBaseParams;
}

std::unique_ptr<CBaseChainParams> CreateBaseChainParams(const ChainType chain)
{
    switch (chain) {
    case ChainType::MAIN:
        return std::make_unique<CBaseChainParams>("", 22395);
    case ChainType::TESTNET:
        return std::make_unique<CBaseChainParams>("testnet", 32395);
    case ChainType::REGTEST:
        return std::make_unique<CBaseChainParams>("regtest", 35715);
    case ChainType::TESTNET4:
        throw std::runtime_error("Zeus migration draft: testnet4 is not a Zeus network");
    case ChainType::SIGNET:
        throw std::runtime_error("Zeus migration draft: signet is not a Zeus network");
    }
    throw std::runtime_error("Zeus migration draft: unsupported chain type");
}

void SelectBaseParams(const ChainType chain)
{
    globalChainBaseParams = CreateBaseChainParams(chain);
    gArgs.SelectConfigNetwork(ChainTypeToString(chain));
}
