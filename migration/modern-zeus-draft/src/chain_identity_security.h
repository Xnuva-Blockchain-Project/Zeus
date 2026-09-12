#ifndef ZEUS_CHAIN_IDENTITY_SECURITY_H
#define ZEUS_CHAIN_IDENTITY_SECURITY_H

#include <string_view>

namespace zeus::security {

inline constexpr std::string_view MAINNET_GENESIS =
    "000007a830306520fa2efc02258706ec6bac5514f6396b9313df676ac498e8ee";

// These values must be obtained from and independently checked against the
// authoritative live Zeus chain before this experimental branch can be released.
inline constexpr std::string_view MAINNET_BLOCK1 = "";
inline constexpr int MAINNET_BURIED_ANCHOR_HEIGHT = -1;
inline constexpr std::string_view MAINNET_BURIED_ANCHOR = "";
inline constexpr std::string_view MAINNET_MINIMUM_CHAINWORK = "";

inline constexpr bool REQUIRE_NOT_IBD = true;
inline constexpr bool REQUIRE_PEER = true;
inline constexpr bool PROTECT_POW = true;
inline constexpr bool PROTECT_POS = true;
inline constexpr bool CHAIN_IDENTITY_QUALIFIED = false;

constexpr bool HasQualifiedChainIdentity()
{
    return CHAIN_IDENTITY_QUALIFIED &&
           MAINNET_BLOCK1.size() == 64 &&
           MAINNET_BURIED_ANCHOR_HEIGHT >= 1 &&
           MAINNET_BURIED_ANCHOR.size() == 64 &&
           MAINNET_MINIMUM_CHAINWORK.size() == 64;
}

} // namespace zeus::security

#endif // ZEUS_CHAIN_IDENTITY_SECURITY_H
