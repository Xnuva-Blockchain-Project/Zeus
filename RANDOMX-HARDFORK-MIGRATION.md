# Zeus (ZUS) — Proposed RandomX hard-fork migration

**Status: engineering decision / experimental design, NOT activated, NOT release-ready.**
**Created:** 2026-11-21
**Branch:** `modernisation/randomx-hardfork-experimental-20261121` (forked from `modernisation/bitcoin-core-31.1-experimental-do-not-release`)
**Principle:** Reliability first, release second.

## Project decision and rationale

During each coin's independent modernisation, investigate transitioning **future Proof-of-Work blocks only** to CPU-optimised RandomX while preserving the existing independent Proof-of-Stake pathway through a deliberately specified, miner/node-coordinated **hard fork**. XNUVA is the existing RandomX reference, **not** a drop-in, independently qualified consensus implementation. Rationale: more accessible CPU mining for ordinary users, a more consistent miner maintenance framework, and potentially reduced long-term support burden. CPU preference is not CPU-only enforcement; RandomX neither guarantees decentralisation nor protects a small chain without sufficient independent hashrate.

## Immutable historical continuity

- Keep the existing genesis, all historical block headers/IDs, transactions, UTXO/balance accounting, wallets, addresses, private keys and coin economics unchanged.
- All pre-activation PoW and PoS blocks MUST validate under their original consensus and difficulty rules. Never rewrite history or silently relax consensus.
- RandomX is considered only for new **PoW** blocks at and after an explicit future activation height (or another carefully specified deterministic activation rule). Proof-of-Stake blocks must continue using their own stake validation, independently of RandomX. **No mainnet activation height is selected.**
- Define and test exactly what is hashed, byte order, canonical serialization, RandomX seed/key schedule, version identification, PoW comparison, compact target, difficulty bootstrap/retarget and chain-selection work across the transition. Avoid copying another coin's seed schedule or difficulty values blindly.
- Network operators must upgrade; legacy nodes will reject incompatible post-fork blocks. Pre-fork backups do not make a completed on-chain split automatically reversible.

## Hybrid PoW + PoS preservation (confirmed from source)

- Source review of `src/pos.h` and `src/pos.cpp` on the preserved legacy source found `ComputeStakeModifier`, `CheckStakeKernelHash`, `CheckProofOfStake`, coinstake timestamp checks and stake signature validation. This confirms a real PoS implementation exists in source; current live staking participation/functionality has **not** yet been independently verified.
- **Do not remove, replace or route PoS through RandomX.** Preserve historic and future PoS kernel hash and modifier rules, stake maturity/eligibility, coinstake formats/signatures, reward amounts, timestamps and staking wallet interfaces unless a **separate** explicitly reviewed change is unavoidable.
- Audit the hybrid block-type discriminator, PoW-vs-PoS difficulty and target rules, PoS chain selection/trust, stake modifiers spanning a PoW hard-fork boundary and all validation, minting and reorganisation paths. A simple `height < H` switch for every block is **incorrect** for hybrid chains.
- On isolated test nodes, verify both post-activation **RandomX PoW blocks** and **unchanged PoS blocks**, including reorgs across mixed block types and the fork boundary. Check staking rewards and spendability independently of PoW mining rewards.

## Coin-specific baseline and risks

Audit active PoW/difficulty/reward parameters and currently running miners; patched miner acceptance and treasury accounting require independent validation. Do not conflate any prospective RandomX code with outstanding repairs.

## Required stage gates

- [x] Separate experimental branch created. This is a planning milestone only.
- [ ] Freeze authoritative chain parameters, currently accepted mainnet tip and documented source revision; capture hashed cold backups.
- [ ] Document historical PoW and PoS validation/retarget/header/mining/staking paths, XNUVA RandomX implementation and upstream dependency/version; complete risk and licensing review.
- [ ] Develop a deterministic consensus specification including activation, key/seed epochs, transitions, difficulty/chainwork rules, miner handshake and independent test vectors.
- [ ] Implement dual historical/new PoW verification *only on isolated test code*; compile and run reproducible unit/boundary tests; negative-test historical blocks with the wrong algorithm.
- [ ] Establish an isolated network with explicit chain identity, fresh test keys, unique ports/datadirs, no access to production wallets and fail-closed mainnet mining guards. Do not invent an authoritative testnet genesis.
- [ ] Test historic PoW + PoS and new RandomX-PoW + original-PoS block production/peer validation, mixed-chain reorgs, tip selection, both difficulties, mining and staking reward maturity, wallet spends, restart/reindex/restore, independent nodes and treasury when applicable.
- [ ] Measure CPU/RAM/startup cost and realistic mining participation; assess cross-network attacks, hashrate shocks and miner usability before claiming security benefit.
- [ ] Obtain network consensus and publish explicit upgrade schedule, activation, installer/rollback and operator guidance **only after** engineering qualification and operator approval.
- [ ] Prove sustained real-network operation after safe, coordinated deployment; never treat code builds or checkpoint PASS as launch approval.

**Production policy:** No live configuration, miner, VPS, node, database or public wallet changes authorised by creation of this document. The currently working networks continue normally.

## Sources

- Upstream RandomX: https://github.com/tevador/RandomX
- Golden Standard handbook: https://github.com/Xnuva-Blockchain-Project/Xnuva/blob/docs/golden-standard-modernisation-handbook-v1/doc/engineering-standards/GOLDEN_STANDARD_CRYPTO_MODERNISATION.md
