# Zeus -> Bitcoin Core 31.1 mechanical port map

This is an engineering map for the experimental migration branch. It is **not a test report** and not a release specification.

## Important architectural finding

The current XNUVA modern tree follows modern Bitcoin transaction/block primitives and does **not** contain Zeus-style transaction timestamps, coinstake detection, block signatures, `nLastPOWBlock`, `posLimit` or stake timestamp parameters.

Therefore Zeus cannot be modernised merely by changing XNUVA/Bitcoin chain parameters. Its hybrid PoW/PoS consensus extensions must be deliberately reintroduced into the modern architecture.

## Legacy -> modern mapping

| Legacy Zeus area | Modern target area | Migration action |
|---|---|---|
| `src/chainparams.cpp` | `modern-core/src/kernel/chainparams.cpp` + chain parameter helpers | Port Zeus genesis/network/address identity without changing historical chain |
| `src/consensus/params.h` | `modern-core/src/consensus/params.h` | Add Zeus-only PoS/retarget/last-PoW parameters |
| `src/primitives/transaction.h` | `modern-core/src/primitives/transaction.h` + serialization helpers | Preserve historical transaction timestamp serialization and coinstake semantics |
| `src/primitives/block.h` | `modern-core/src/primitives/block.h` | Preserve block signature data and PoW/PoS classification semantics |
| `src/pow.cpp/.h` | `modern-core/src/pow.cpp/.h` | Port Zeus hybrid target calculation / retarget behaviour |
| `src/pos.cpp/.h` | new Zeus PoS module integrated with modern validation | Port stake modifier, kernel, timestamp and stake-proof verification |
| legacy `src/main.cpp` PoS checks | `modern-core/src/validation.cpp`, block validation helpers and chainstate | Re-home PoS block checks into modern validation flow |
| mining/staking RPC | modern RPC/mining/wallet modules | Port only after consensus primitives exist |
| wallet staking | modern wallet architecture | Port after chain validation layer is mechanically complete |

## Serialization is the first hard boundary

Legacy Zeus transactions can serialize `nTime` for transaction version < 2 and define `IsCoinStake()`. Modern Bitcoin/XNUVA transactions do not carry this field.

Legacy Zeus blocks also carry `vchBlockSig` and determine Proof-of-Stake using the second transaction.

These are historical wire/disk-format rules. They cannot be dropped or replaced without creating a different chain.

## Mechanical migration order

1. Pin modern Bitcoin Core 31.1 baseline — **DONE**.
2. Freeze/snapshot legacy consensus files — **DONE**.
3. Port historical transaction serialization.
4. Port historical block serialization/signatures and PoS classification.
5. Port chain parameters and original genesis.
6. Port hybrid difficulty logic.
7. Port stake kernel/modifier verification.
8. Integrate PoS validation into modern Chainstate.
9. Port reward accounting exactly as legacy behaviour first.
10. Port networking/RPC/wallet/mining/staking surfaces.
11. Build.
12. Historical-chain testing from genesis.
13. Only separately, settle the true prospective 40,000,000 ZUS cap.

## Rule

When modern architecture and historical Zeus behaviour conflict, historical Zeus consensus wins for all already-existing blocks. Modernisation is allowed to change implementation, not history.
