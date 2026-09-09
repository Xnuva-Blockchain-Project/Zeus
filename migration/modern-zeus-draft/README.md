# Modern Zeus draft source

**UNTESTED — DO NOT RELEASE — DO NOT CONNECT TO LIVE ZEUS**

This directory contains draft Bitcoin Core 31.1 source files being mechanically adapted to preserve Zeus historical consensus formats.

The files here are not yet a complete client and are intentionally separate from both:

- the live legacy Zeus source at repository root; and
- the pristine pinned Bitcoin Core 31.1 `modern-core` submodule.

## Drafted so far

- transaction timestamp serialization for legacy transaction version < 2;
- Zeus version-1 transaction default;
- coinstake identification;
- empty coinstake first-output helpers;
- block signature serialization (`vchBlockSig`);
- Proof-of-Stake / Proof-of-Work block classification.

## Not yet ported

- Zeus chain parameters / exact modern consensus translation;
- PoS kernel and stake modifier;
- hybrid retarget logic;
- block-validation integration;
- reward/donation accounting;
- wallet staking;
- mining/staking RPC;
- networking/package branding;
- historical-chain validation;
- corrected prospective 40,000,000 ZUS cap.

These draft files must not be treated as release-ready merely because they resemble modern Bitcoin Core.
