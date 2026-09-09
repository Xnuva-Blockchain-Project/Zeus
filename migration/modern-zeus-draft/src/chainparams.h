// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-present The Bitcoin Core developers
// Zeus migration draft: UNTESTED / DO NOT RELEASE
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CHAINPARAMS_H
#define BITCOIN_CHAINPARAMS_H

#include <kernel/chainparams.h> // IWYU pragma: export

#include <memory>

class ArgsManager;

std::unique_ptr<const CChainParams> CreateChainParams(const ArgsManager& args, ChainType chain);
const CChainParams& Params();
void SelectParams(ChainType chain);

#endif // BITCOIN_CHAINPARAMS_H
