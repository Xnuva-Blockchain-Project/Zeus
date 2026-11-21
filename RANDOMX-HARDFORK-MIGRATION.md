# Zeus (ZUS) — Proposed RandomX hard-fork migration

**Status: engineering decision / experimental design, NOT activated, NOT release-ready.**
**Created:** 2026-11-21
**Branch:** `modernisation/randomx-hardfork-experimental-20261121` (forked from `modernisation/bitcoin-core-31.1-experimental-do-not-release`)
**Principle:** Reliability first, release second.

## Project decision and rationale

During each coin's independent modernisation, investigate transitioning **future blocks** to CPU-optimised RandomX Proof of Work through a deliberately specified, miner/node-coordinated **hard fork**. XNUVA is the existing RandomX reference, **not** a drop-in, independently qualified consensus implementation. Rationale: more accessible CPU mining for ordinary users, a more consistent miner maintenance framework, and potentially reduced long-term support burden. CPU preference is not CPU-only enforcement; RandomX neither guarantees decentralisation nor protects a small chain without sufficient independent hashrate.

## Immutable historical continuity

- Keep the existing genesis, all historical block headers/IDs, transactions, UTXO/balance accounting, wallets, addresses, private keys and coin economics unchanged.
- All pre-activation blocks MUST validate with the original Proof-of-Work algorithm and original difficulty rules. Never rewrite history or silently relax consensus.
- RandomX is considered only for new blocks at and after an explicit future activation height (or another carefully specified deterministic activation rule). **No mainnet activation height is selected.**
- Define and test exactly what is hashed, byte order, canonical serialization, RandomX seed/key schedule, version identification, PoW comparison, compact target, difficulty bootstrap/retarget and chain-selection work across the transition. Avoid copying another coin's seed schedule or difficulty values blindly.
- Network operators must upgrade; legacy nodes will reject incompatible post-fork blocks. Pre-fork backups do not make a completed on-chain split automatically reversible.

## Coin-specific baseline and risks

Audit active PoW/difficulty/reward parameters and currently running miners; patched miner acceptance and treasury accounting require independent validation. Do not conflate any prospective RandomX code with outstanding repairs.

## Required stage gates

- [x] Separate experimental branch created. This is a planning milestone only.
- [ ] Freeze authoritative chain parameters, currently accepted mainnet tip and documented source revision; capture hashed cold backups.
- [ ] Document historical PoW/retarget/header/mining paths, XNUVA RandomX implementation and upstream dependency/version; complete risk and licensing review.
- [ ] Develop a deterministic consensus specification including activation, key/seed epochs, transitions, difficulty/chainwork rules, miner handshake and independent test vectors.
- [ ] Implement dual historical/new PoW verification *only on isolated test code*; compile and run reproducible unit/boundary tests; negative-test historical blocks with the wrong algorithm.
- [ ] Establish an isolated network with explicit chain identity, fresh test keys, unique ports/datadirs, no access to production wallets and fail-closed mainnet mining guards. Do not invent an authoritative testnet genesis.
- [ ] Test old-before/new-after activation: block production/peer validation, reorgs, tip selection, difficulty, reward maturity, wallet spends, restart/reindex/restore, independent nodes and treasury when applicable.
- [ ] Measure CPU/RAM/startup cost and realistic mining participation; assess cross-network attacks, hashrate shocks and miner usability before claiming security benefit.
- [ ] Obtain network consensus and publish explicit upgrade schedule, activation, installer/rollback and operator guidance **only after** engineering qualification and operator approval.
- [ ] Prove sustained real-network operation after safe, coordinated deployment; never treat code builds or checkpoint PASS as launch approval.

**Production policy:** No live configuration, miner, VPS, node, database or public wallet changes authorised by creation of this document. The currently working networks continue normally.

## Sources

- Upstream RandomX: https://github.com/tevador/RandomX
- Golden Standard handbook: https://github.com/Xnuva-Blockchain-Project/Xnuva/blob/docs/golden-standard-modernisation-handbook-v1/doc/engineering-standards/GOLDEN_STANDARD_CRYPTO_MODERNISATION.md
