Zeus
=====================================

https://europazeus.org

What is Zeus?
----------------

Zeus is a decentralised digital currency with near-instant transaction speeds and negligible transaction fees built upon Proof of Stake 3.0 (PoSV3, BPoS) as
introduced by the Zeus development team.

For more information about Zeus itself, see https://zeus.org.

What is Zeus?
----------------

Zeus is the name of open source software which enables the use of this currency. It takes Zeus to the next level by building upon
Bitcoin Core 0.13.2 with some patches from newer Bitcoin Core versions to offer performance enhancements, wider compatibility with third party services and a more advanced base.

The maintained Zeus source is published at https://github.com/Xnuva-Blockchain-Project/Zeus.
Official binary releases will be published only after qualification and tagging.

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
in Python. These tests can be run (if the [test dependencies](/qa) are installed)
with: `qa/pull-tester/rpc-tests.py`

Maintained releases are qualified against the exact public source commit before tagging.

### Manual Quality Assurance (QA) Testing

Changes should be tested by somebody other than the developer who wrote the
code. This is especially important for large or high-risk changes. It is useful
to add a test plan to the pull request description if testing the changes is
not straightforward.
