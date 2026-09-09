# ZEUS MODERNISATION WORKSPACE — DO NOT RELEASE

**Status:** EXPERIMENTAL / DEVELOPMENT ONLY  
**Branch:** `modernisation/bitcoin-core-31.1-experimental-do-not-release`  
**Created:** 2026-09-09

This branch exists solely to modernise the Zeus codebase against the current stable Bitcoin Core baseline.

## Critical safety rules

- **DO NOT merge this branch into `main` until the migration and consensus validation programme is complete.**
- **DO NOT build public releases or binaries from this branch.**
- **DO NOT deploy this branch to the live Zeus network.**
- The existing Zeus blockchain must be preserved. This work is a software migration, **not a new chain**.
- Genesis block, historical block acceptance, existing balances, keys/addresses, network identity, reward history, PoW/PoS behaviour and all other consensus-relevant behaviour must be preserved unless a separately specified forward-activation change is deliberately introduced and tested.
- The planned 40,000,000 ZUS maximum-supply rule is **not** to be silently introduced during the mechanical source migration. It must be handled as an explicit, separately reviewed consensus change with a future activation point.
- Before any release, the modern client must successfully validate the existing Zeus chain from genesis to the live tip and must not create or follow a replacement chain.

## Upstream baseline

Target upstream baseline: **Bitcoin Core v31.1** (stable release).

The first phase of this branch is an engineering import/port workspace. Upstream source will be introduced in controlled steps so that Zeus-specific historical and consensus behaviour can be audited and transplanted deliberately rather than overwritten blindly.

## Release gate

This branch is **NOT FOR RELEASE** until all migration, historical-chain, wallet, networking, mining/staking, RPC and consensus tests have passed.
