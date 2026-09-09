# ZUS prospective maximum-supply enforcement

**Status:** IMPLEMENTED IN MODERN DRAFT / MAINNET ACTIVATION NOT YET SET  
**Release status:** DO NOT RELEASE

## Maximum

Published maximum supply: **40,000,000 ZUS**.

The modern consensus draft now contains:

- a hard maximum-supply parameter;
- cumulative net-issued-supply tracking in every modern block-index entry;
- PoW and PoS net-issuance accounting;
- subsidy trimming for the final minting block;
- a consensus check that rejects any post-activation block whose actual new issuance would exceed the cap;
- deterministic cumulative-supply handling across competing branches/reorganisations.

## What counts as new issuance

For Proof of Work:

`net new issuance = max(0, coinbase outputs - transaction fees)`

For Proof of Stake:

`net new issuance = max(0, actual coinstake reward - transaction fees)`

Fees are excluded because they are existing coins transferred from users to the block producer. Development-fund/donation outputs are not counted twice: the actual total coinstake reward already captures all newly created outputs.

Burned fees, unclaimed subsidy, or deliberate underpayment do **not** create new future minting allowance. The accumulator tracks coins actually issued, not current circulating/UTXO supply.

## Historical preservation

The accumulator is calculated for historical blocks even before activation, but the cap check is not applied to those blocks.

If historical issuance is already at or above the published maximum when the prospective rule activates, existing history remains valid and all further new issuance must be zero. No historical block or balance is rewritten.

## Activation gate

Mainnet currently has:

`nSupplyCapActivationHeight = -1`

This is intentional. Before release we must:

1. build the complete modern client;
2. reindex the authoritative existing chain from genesis;
3. compute and independently verify cumulative historical issuance at the current live tip;
4. choose an activation height **strictly after the verified live tip**, allowing adequate deployment notice;
5. put that fixed activation height into consensus;
6. test old/new behavior on both sides of the activation boundary;
7. only then release.

The cap implementation must not be activated at an invented height.

## Integration hook

When a full block has been validated enough to know fees and actual PoW/PoS reward:

`ValidateAndRecordSupply(...)`

must be called before the block is accepted as consensus-valid.

Block-template/miner/staker creation must use the capped subsidy helpers so the final issuance block can mint only the exact remaining amount.

This cap is a **prospective consensus correction on the existing blockchain**, not a chain reset.
