#!/usr/bin/env bash
set -euo pipefail

# Zeus modernisation bootstrap
# DEVELOPMENT ONLY — DO NOT DEPLOY TO THE LIVE ZEUS NETWORK.
#
# This script materialises the exact modern upstream baseline used by the
# migration workspace. It does not alter the Zeus repository or live chain.

UPSTREAM_REPO="https://github.com/bitcoin/bitcoin.git"
UPSTREAM_TAG="v31.1"
UPSTREAM_COMMIT="9be056a8a72b624dae9623b2f7bded92c2a21c91"
DEST="${1:-modern-zeus-worktree}"

if [[ -e "$DEST" ]]; then
    echo "STOP: destination already exists: $DEST" >&2
    exit 1
fi

git clone --filter=blob:none --no-checkout "$UPSTREAM_REPO" "$DEST"
git -C "$DEST" checkout --detach "$UPSTREAM_TAG"

actual="$(git -C "$DEST" rev-parse HEAD)"
if [[ "$actual" != "$UPSTREAM_COMMIT" ]]; then
    echo "STOP: upstream commit mismatch" >&2
    echo "expected: $UPSTREAM_COMMIT" >&2
    echo "actual:   $actual" >&2
    exit 1
fi

cat <<'EOF'
Bitcoin Core v31.1 baseline materialised successfully.

NEXT MIGRATION PHASE (not performed automatically):
  * port Zeus chain identity and network parameters;
  * port transaction timestamps and block signatures;
  * port hybrid PoW/PoS primitives;
  * port PoS kernel/modifier and validation logic;
  * port Zeus reward/donation behaviour exactly as observed;
  * preserve the existing genesis and historical rules;
  * keep the 40m cap correction separate until consensus accounting is proven.

DO NOT run this tree against the live Zeus network.
DO NOT publish binaries from this tree.
EOF
