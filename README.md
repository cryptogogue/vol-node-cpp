# VOLITION NODE CPP - The Volition Reference Node in C++

This is an implementation of a Volition blockchain mining node in C++.

### Project Status

As of this writing (February 2021), this project is still in the very early stages, even after several years of development. It is working (minimally), but still has a long way to go to even be close to production-ready.

This codebase is also the idiosyncratic work of a single developer. Clearly, we need contributors. If you're interested in crypto and handy with C++, please get in touch via our Discord.


### Proof of Identity

While the Volition protocol was designed to be used in permissioned scenarios, participation in the Volition CCG mining network will eventually be "public" as well through "Proof of Idenity." What we mean by this is that anyone will eventually be able to run a mining node and participate in consensus if they a) can prove their identity and b) operate under a jurisdiction where there is rule of law (including legal recognition for digital signatures).

In other words, unlike *permissionless* protocols with *anonymous* miners, Volition is *permissioned*, but participation is open to *any qualifying member of the public* and, importantly, that qualification will be available in an open decentralized fashion with no single gatekeeper.

For this, we rely on third party KYC/AML vendors to issuing digital identities within a pluggable identity framework: you will be able to go to an independent KYC/AML vendor, receive a digital identity (notarized by that vendor), use that identity to agree to Volition's mining Terms of Service and then freely participate in the network. You do not need to get permission from us or anyone else.

We like this approach because it strikes a sensible balance between trustlessness (Volition is trustful) and decentralization (no single entity controls access).

Our vision is not a single, private ledger for one project or a universal public ledger for all projects, but many independent "public permissioned" ledgers, one for each publisher or project. And of course, once a miner has a valid digital identity, that can participate in as many different ledgers as they desire.

### Warning

Right now, this codebase is suitable for test deployment only. It has not been hardened against attacks (or even simple mistakes). If configured correctly, it should reach consensus with decent performance, but it can be trivially blown up with bad input.

In theory, the Volition consensus algorithm will deliver fault tolerance, but, as implemented now, there are no additional checks in place for certain attacks. Hardening the codebase against attacks will be an ongoing effort.

The current codebase has also not been optimized. For example, the current implementation uses sqlite3 as its persistent storage and HTTP as its protocol. In the near future we will need to separate storage from the host machine and split the server up to run behind a load balancer.

It's still too early in the project for a proper roadmap; that will come into focus as we move into open beta and start seeing the network operate, even at limited scale.

Also, please note that although Volition is a technically deep, complex and hopefully *interesting* project, it *not* the product of professional mathemeticians or cryptographers. Our approach has been intuitive; whether it can weather the attacks that will come with any attention we're luck enough to receive remains to be seen.

### Why Help?

- Maybe you think it is interesting.
- Maybe you'll get some VOL.
- Maybe you'll use the codebase for your owm project, once we have a public license.
- Maybe you're in it for the lulz.

Hopefully you'll help for a good reason (like you find it interesting) and not a dubious reason (sweet, sweet VOL).

### Getting Started

Deploying a Single Node
Deploying a Local Test Network
Mining Node Setup Guide

### How it Works

Volition Consensus
Games of Chance
Node Architecture
