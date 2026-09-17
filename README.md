Zeus
=====================================

https://europazeus.org

What is Zeus?
----------------

Zeus is a decentralised digital currency with near-instant transaction speeds and negligible transaction fees built upon Proof of Stake 3.0 (PoSV3, BPoS) as
introduced by the Zeus development team.

For more information about Zeus itself, see https://europazeus.org.

What is Zeus?
----------------

Zeus is the name of open source software which enables the use of this currency. It takes Zeus to the next level by building upon
Bitcoin Core 0.13.2 with some patches from newer Bitcoin Core versions to offer performance enhancements, wider compatibility with third party services and a more advanced base.

The maintained Zeus source is published at https://github.com/Xnuva-Blockchain-Project/Zeus.
Official binary releases will be published only after qualification and tagging.

Official project locations
--------------------------

The canonical maintained Zeus source repository is:

https://github.com/Xnuva-Blockchain-Project/Zeus

The current project website is:

https://europazeus.org

Older Zeus repositories, GitHub Pages sites or project accounts should be treated as legacy references only and are not authoritative for current development or releases.

Important Scam Warning
----------------------

As interest in cryptocurrency projects grows, unofficial websites, social-media accounts, marketplaces or individuals may sometimes claim to sell, distribute or represent Zeus (ZUS).

**Please exercise appropriate caution and always verify information through official project channels.**

The preferred way to obtain ZUS is to **mine it directly through the Zeus blockchain network**, rather than purchasing coins from unknown individuals or unofficial third-party sellers. Mining allows coins to be obtained through the network's own consensus process without relying on an unknown intermediary.

In particular:

- Be cautious of unsolicited offers to buy or sell ZUS directly.
- Do not assume that a website, exchange, seller or individual is affiliated with the project simply because they use the Zeus name, logo or branding.
- Never send cryptocurrency or money based solely on a private message, social-media post or unofficial website.
- Never disclose your wallet seed phrase, private keys or passwords to anyone.
- Download wallet and mining software only from this official GitHub repository or other download locations specifically identified by the project.
- Verify announcements, releases and important information against the official project repository and website.
- The project team will never ask for your wallet seed phrase or private keys.

Cryptocurrency transactions are generally irreversible, so taking a few moments to independently verify an offer or communication can provide important protection.

**If you are uncertain whether something is genuine, verify it through the project's official channels before sending funds or providing sensitive information.**

Network bootstrap
-----------------

Until fixed-seed discovery is included in maintained clients, a fresh Zeus node
can bootstrap from the project public node:

```ini
addnode=81.130.208.151:22396
```

The address above is the Xnuva Blockchain Project T620 node on a static public
IPv4 address. Only the Zeus P2P port is used; RPC should not be exposed to the
public Internet.

License
-------

Zeus is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.

Development Process
-------------------

The `main` branch contains the maintained Zeus source. Changes intended for release
are qualified against the published source before an official tag is created.
[Tags](https://github.com/Xnuva-Blockchain-Project/Zeus/tags) identify specific
maintained releases.

Change log can be found in [CHANGELOG.md](CHANGELOG.md).

The contribution workflow is described in [CONTRIBUTING.md](CONTRIBUTING.md).


Testing
-------

Testing and code review might be the bottleneck for development. Please help out by testing
other people's pull requests, and remember this is a security-critical project where any mistake might cost people
lots of money.

### Automated Testing

Developers are strongly encouraged to write [unit tests](/doc/unit-tests.md) for new code, and to
submit new unit tests for old code. Unit tests can be compiled and run
(assuming they weren't disabled in configure) with: `make check`

There are also [regression and integration tests](/qa) of the RPC interface, written
in Python. These tests can be run (if the [test dependencies](/qa) are installed) with: `qa/pull-tester/rpc-tests.py`

Maintained releases are qualified against the exact public source commit before tagging.

### Manual Quality Assurance (QA) Testing

Changes should be tested by somebody other than the developer who wrote the
code. This is especially important for large or high-risk changes. It is useful
to add a test plan to the pull request description if testing the changes is
not straightforward.
