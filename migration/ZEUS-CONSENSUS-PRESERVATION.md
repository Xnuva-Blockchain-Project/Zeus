# Zeus consensus preservation manifest

**Purpose:** migration reference only. This file records consensus/network properties observed in the current Zeus `main` source before the Bitcoin Core 31.1 port is completed.

**Authoritative legacy source commit:** `d17d23d7ce57b2723a9ef24545f601873ebabfdf`

## Non-negotiable chain identity

The modern client must remain on the existing Zeus blockchain. It must not create a replacement genesis, reset balances, or define a new history.

### Mainnet identity observed in legacy source

- Genesis hash: `000007a830306520fa2efc02258706ec6bac5514f6396b9313df676ac498e8ee`
- Genesis merkle root: `f4dec70165441cbaef0a157c82321b594277f0c080021f5d5edf954dec3012b7`
- Genesis time: `1738936389`
- Genesis nonce: `955918`
- Genesis bits: `0x1e0fffff`
- Genesis version: `1`
- Message start: `75 80 3c c0`
- Default P2P port: `22396`
- CashAddr prefix: `zeus`
- P2PKH prefix: `80`
- P2SH prefix: `81`
- WIF/secret-key prefix: `208`
- Extended public key bytes: `04 88 b2 1e`
- Extended secret key bytes: `04 88 ad e4`

## Mainnet timing / hybrid consensus observed in legacy source

- Proof of Work + Proof of Stake hybrid chain
- Target timespan: `3600` seconds
- Target spacing V1: `300` seconds
- Target spacing: `300` seconds
- Last PoW block parameter: `1,000,000`
- Stake timestamp mask: `0x0f`
- Coinbase maturity: `100`
- PoW subsidy function currently returns: `40 * COIN`
- PoS subsidy function currently returns: `14 * COIN` subject to the current source's height cutoff
- Development-fund address observed in legacy mainnet parameters: `ZFEKPrfhN7MdiznZo4xgajQGU7FAFErQVv`

## Historical data structures that must be ported, not discarded

The legacy block/transaction model contains Proof-of-Stake-specific data absent from vanilla modern Bitcoin Core, including:

- transaction timestamp handling;
- block signatures (`vchBlockSig`);
- `CBlock::IsProofOfStake()` / `IsProofOfWork()` semantics;
- coinstake transaction semantics;
- stake-kernel and stake-modifier logic;
- PoS difficulty/retarget logic;
- PoS timestamp rules;
- PoS validation in block connection;
- stake-cache behaviour;
- hybrid PoW/PoS chain-selection and block-index flags where applicable.

These are consensus-sensitive. They must be deliberately transplanted into the modern validation architecture.

## 40,000,000 ZUS cap — NOT YET VALIDATED

The current legacy source contains a PoS subsidy cutoff tied to `nLastPOWBlock` and comments describing the published 40,000,000 ZUS maximum.

**Do not treat that comment/cutoff as proof that aggregate historical + future issuance is exactly capped at 40,000,000 ZUS.** PoW and PoS issuance can overlap before the cutoff, so total-supply enforcement must be independently derived from actual chain history and consensus accounting.

For the mechanical migration:

1. preserve the currently observed behaviour;
2. do not silently redesign the cap;
3. later calculate historical issued supply from the real chain;
4. introduce any corrected cap only as an explicit forward-activation consensus rule;
5. prove the new client follows the existing chain before and after activation.

## Release invariant

A release candidate is unacceptable unless it can validate the existing Zeus history from the original genesis to the live tip and agrees with the authoritative chain on all historical consensus decisions.
