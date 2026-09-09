# Zeus modernisation port status

**Branch:** `modernisation/bitcoin-core-31.1-experimental-do-not-release`  
**Release status:** **NOT FOR RELEASE**  
**Test status:** comprehensive chain testing intentionally deferred until the mechanical migration is substantially complete.

## Completed

- Created isolated modernisation branch; Zeus `main` remains untouched.
- Pinned signed Bitcoin Core v31.1 upstream commit `9be056a8a72b624dae9623b2f7bded92c2a21c91`.
- Seeded modern Bitcoin Core build/chain/consensus/validation reference files under `upstream-bitcoin-core-31.1/`.
- Recorded a Zeus consensus-preservation manifest from the current legacy source.
- Added a deterministic bootstrap script for materialising the exact full Bitcoin Core v31.1 upstream tree outside the live source tree.
- Identified PoS-specific structures that vanilla Bitcoin Core does not provide and therefore must be transplanted deliberately.
- Marked the existing 40m-related legacy subsidy cutoff as **unverified supply-cap logic**, not a migration assumption.

## Mechanical port sequence

1. Modern Bitcoin Core 31.1 infrastructure/build system.
2. Zeus network identity and genesis.
3. Zeus transaction/block serialization differences.
4. Zeus hybrid PoW/PoS primitives.
5. PoS kernel/modifier and retarget code.
6. Zeus block-validation integration.
7. Zeus wallet/RPC/mining/staking interfaces.
8. Branding/package names.
9. Only after the above: build and historical-chain validation.
10. Separately: derive and implement the correct prospective 40,000,000 ZUS maximum-supply rule.

## Hard gate

Nothing from this branch is to be merged to `main`, released, or connected as an authoritative live node until historical compatibility is proven.
