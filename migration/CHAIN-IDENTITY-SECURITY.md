# Zeus chain-identity security contract

This document is part of the Bitcoin Core 31.1 modernisation branch and is a **release gate**.

## Purpose

Prevent a repeat of the Europa failure class: a node loses, resets, replaces, or starts with the wrong blockchain state and then creates a valid-looking alternative history from the same genesis.

Zeus is hybrid Proof of Work + Proof of Stake. The modern Zeus block-production path must therefore refuse **both PoW block-template creation and PoS/coinstake production** unless the active chain is proven to be the canonical Zeus chain.

## Known immutable identity

- Genesis: `000007a830306520fa2efc02258706ec6bac5514f6396b9313df676ac498e8ee`

The canonical block-1 fingerprint, well-buried anchor and meaningful minimum-chainwork floor are deliberately not invented here. They must be derived from the live authoritative chain and independently verified before release.

## Mandatory mainnet production guard

Before the modern code can create a mainnet PoW or PoS candidate, the central block-production path must fail closed unless all of the following are true:

1. the security policy is explicitly configured for production;
2. the canonical block-1 fingerprint is configured and matches the active chain;
3. the active chain has reached the configured buried anchor height;
4. the active-chain block at that height exactly matches the configured canonical hash;
5. the active tip chainwork/trust is at least the configured canonical minimum;
6. the node is not in initial block download / synchronisation state;
7. at least one peer connection is present.

The guard must protect the **central block-building path shared by PoW and PoS**, not merely a miner launcher or one RPC method.

## Fail-closed configuration rule

`migration/modern-zeus-draft/src/chain_identity_security.h` deliberately leaves the block-1 fingerprint, buried anchor and minimum-chainwork values unset and `production_block_creation_enabled = false`.

Those values must not be guessed. Until they are qualified, the modernised branch remains **DO NOT RELEASE / DO NOT MINE / DO NOT STAKE MAINNET**.

## Qualification required before release

Negative test: present a disposable wrong/truncated/alternate Zeus chain and prove that both PoW and PoS production are refused and the tip does not change.

Positive test: present a disposable copy of the genuine Zeus chain, prove the configured identity, and create valid PoW and PoS candidates/templates without solving/submitting a production block.

Only after both paths pass may `production_block_creation_enabled` be set true and the modernised Zeus code be considered for release.
