# Volition's Consensus Algorithm

The Volition distributed ledger is a blockchain maintained by a network of participants. Each participant has a private key that is used to sign blocks. We want those blocks to be ordered in an unpredictable yet verifiable fashion. We also want the distribution of blocks across participants to approach equality over time.  

### Intuition

Digital signatures can have the quality of being unpredictable yet verifiable: the value of the signature is unfeasible to predict, but, once produced, can be verified.

This suggests an idea: digital signatures can play a role in ordering the chain.

Consider a naive implementation in which the Nth block of a chain is chosen from the result of a contest: each participant uses their private key to sign the integer N and the signatures are converted to hexadecimal. The lowest alphabetical signature wins.

Of course, each participant's public key must be known in advance, and their private must key tied to something they value, such as their right to participate or their ability to redeem rewards.

Already we can make some observations:

- The signature algorithm needs to be deterministic.
- The choice of input must also be deterministic lest participants influence the outcome.
- Without knowing the private keys, in it unfeasible to predict the outcome.
- For any block and any group of participants, participant scores may be sorted from most to least favored.
- Given any two branches, one will eventually have a better score.

The actual process Volition uses is a bit more complex. For example, in this naive example, it would be easy for participants to compute their scores in advance and predict when they might win a block. In our full implementation, previous outcomes are incorporated into the score to prevent this.

### Charm, Pose and Visage

For a block at height N, given a participant:

```
N = The height of the block.
KEY = The key of the winning participant at height N.
MOTTO = An optional string constant chosen at account creation; may be the empty string.
SIG = A deterministic digital signature algorithm.
VISAGE = SIG ( KEY, MOTTO )
POSE ( N ) = SIG ( KEY, SHA256 ( POSE ( N - 1 )))
CHARM ( N ) = POSE ( N - 1 ) ^ VISAGE.
```

Each participant has a VISAGE. VISAGE is simply the signature produced by signing MOTTO with KEY. Once computed, VISAGE, MOTTO and KEY may not change during the contest.

POSE is the hash of the previous block's POSE signed by the KEY.

CHARM is the previous block's POSE combined with the participant's VISAGE via a bitwise XOR.

Winning blocks are selected based on CHARM. For two branches that differ by only a single block, it is easy to choose a winner by comparing the CHARM of the two candidate blocks.

Longer branches are compared to each other and the more CHARMing branch is selected. If branches are of different lengths, the longer branch is truncated prior to comparison. If branches are tied, they must both be extended until the tie is broken.

Because branches are clipped to the shortest common length, it may happen that a short branch may defeat a long branch. For example if a participant mines the most charming block at height N but waits until N+1000 to reveal their block, then all 1000 subsequent blocks will be discarded. To prevent this, LOOKBACK is defined as the maximum amount of time in which an earlier block may be submitted. We can use this to calculate the minimum length a shorter chain would need to be to "defeat" a longer chain. For example, consider two chains with a common root N. The longer chain has a length of N + 100 and the shorter has a length of N + 5. The "defeat count" is then:

```
DEFEAT_COUNT = FLOOR (( TIME ( N + 100 ) - TIME ( N + 1 )) / LOOKBACK ) + 1
```

DEFEAT_COUNT has nothing to do with the length of the branch; it based only on the time difference between top and base of the branch.

In order to be considered against a longer branch, the shorter branch must be at least DEFEAT_COUNT long, the idea being that DEFEAT_COUNT is the shortest chain that could have been produced had the winning participants submitted their blocks at the latest time possible (i.e. the end of the LOOKBACK).

### Games of Chance

Games of chance can be based the unpredictable-yet-verifiable ordering of the chain. Because the ordering is derived from secret information held by each participant, no third party (such as an oracle) is required.

To determine the outcome of a game of chance, participants record their bets at block N along with an offset, M, which is the number of blocks to wait before determining the outcome. The outcome is then determined at block N+M.

This approach is suitable for multi-player games of chance, as well as single-player games such as opening a cryptographic booster pack.

### Preventing Sybil Attacks

Blockchain protocols try to avoid [Sybil attacks](https://en.wikipedia.org/wiki/Sybil_attack) by making them expensive, either in terms of hash power or in "stake."

Volition's "[Proof of Personhood](https://en.wikipedia.org/wiki/Proof_of_personhood)" model makes Sybil attacks expensive by requiring participants to procure strong digital identities from third-party KYC/AML vendors. Mounting an effective Sybil attack against Volition would entail identity theft at a large enough scale.

Volition's consensus model is still compatible with permissionless scenarios, even as we eschew this approach. Instead of a digital identity, miners may be required to submit a proof-of-work solution or a proof-of-stake accounting that meets a certain threshold for their blocks to be eligible.
