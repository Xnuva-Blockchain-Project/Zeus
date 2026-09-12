# Zeus modernisation port status

**Branch:** `modernisation/bitcoin-core-31.1-experimental-do-not-release`  
**Release status:** **NOT FOR RELEASE**  
**Test status:** comprehensive chain testing intentionally deferred until the mechanical migration is substantially complete.

**Hybrid status confirmed (2026-09-09): Zeus uses both Proof of Work and Proof of Stake. A temporary PoW-only correction was reverted; this branch is the hybrid PoW/PoS migration workspace.**

## Completed

- Created isolated modernisation branch; Zeus `main` remains untouched.
- Pinned signed Bitcoin Core v31.1 upstream commit `9be056a8a72b624dae9623b2f7bded92c2a21c91`.
- Added the **entire Bitcoin Core v31.1 source tree** as the pinned `modern-core` Git submodule.
- Seeded modern Bitcoin Core build/chain/consensus/validation reference files under `upstream-bitcoin-core-31.1/` for easy side-by-side inspection.
- Recorded a Zeus consensus-preservation manifest from the current legacy source.
- Snapshotted the critical legacy Zeus consensus files under `migration/legacy-consensus-snapshot/`.
- Added a deterministic bootstrap script for materialising the exact Bitcoin Core v31.1 upstream tree independently.
- Identified PoS-specific structures that vanilla Bitcoin Core does not provide and therefore must be transplanted deliberately.
- Marked the existing 40m-related legacy subsidy cutoff as **unverified supply-cap logic**, not a migration assumption.
- Began the actual 31.1 source port under `migration/modern-zeus-draft/`: legacy transaction timestamp serialization, coinstake semantics, empty staking output handling, block signatures and PoW/PoS block classification have draft modern implementations.
- Added the Europa-class chain-identity security contract and a fail-closed hybrid PoW/PoS policy scaffold. Genesis is fixed; block 1, a buried anchor and minimum-chainwork/trust floor remain deliberately unset pending live-chain qualification.

## Checkout

Use:

```bash
git clone --recurse-submodules -b modernisation/bitcoin-core-31.1-experimental-do-not-release https://github.com/Xnuva-Blockchain-Project/Zeus.git
```

The `modern-core` submodule is pinned to the exact Bitcoin Core v31.1 release commit. Do not advance it casually.

## Mechanical port sequence

1. Modern Bitcoin Core 31.1 infrastructure/build system.
2. Zeus network identity and genesis.
3. Zeus transaction/block serialization differences.
4. Zeus hybrid PoW/PoS primitives.
5. PoS kernel/modifier and retarget code.
6. Zeus block-validation integration.
7. Zeus wallet/RPC/mining/staking interfaces.
8. Integrate the mandatory chain-identity guard into the central PoW/PoS block-building path.
9. Branding/package names.
10. Only after the above: build and historical-chain validation.
11. Qualify canonical block 1, a buried live-chain anchor and a minimum-work/trust floor; prove wrong-chain refusal for both PoW and PoS and positive candidate creation on the genuine chain.
12. Separately: derive and implement the correct prospective 40,000,000 ZUS maximum-supply rule.

## Hard gate

Nothing from this branch is to be merged to `main`, released, or connected as an authoritative live node until historical compatibility is proven **and** `migration/CHAIN-IDENTITY-SECURITY.md` has passed its negative and positive qualification tests. The policy scaffold alone is not protection; the central PoW/PoS production path must consume it.

## Supply-cap implementation

- Published maximum configured: **40,000,000 ZUS**.
- Cumulative net issued supply is tracked in the modern block index.
- PoW/PoS fee-adjusted issuance helpers are implemented.
- Final subsidy can be trimmed to the exact remaining mintable amount.
- Post-activation over-cap blocks are rejected.
- Mainnet activation height remains deliberately unset (`-1`) until live-chain issuance and tip are verified.
- See `migration/SUPPLY-CAP.md`.
