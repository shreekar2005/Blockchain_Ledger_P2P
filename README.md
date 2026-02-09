# Blockchain_Ledger_P2P
**Assignment 1 - CSL7490**
**Objective:** Design a blockchain-based petroleum supply chain ledger on top of a P2P network.

This is our submission for the Blockchain assignment. We have implemented a P2P network simulation using **OMNeT++ 6.3.0** and **OpenSSL** on Ubuntu. The project simulates mining, block propagation, and consensus in a distributed network.

## Project Structure

We have split the code into different modules to make it clean and easy to understand:

```bash
Blockchain_Ledger_P2P/
├── BlockchainSim/
│   ├── Blockchain.ned          # Defines the network (Seeds and Peers)
│   ├── Blockchain.msg          # Defines message types (Block, Transaction, Gossip)
│   ├── omnetpp.ini             # Configuration file (Timers, Scenarios)
│   │
│   ├── Node .cc/.h             # Main simulation logic (Timers, Network handling)
│   ├── BlockchainLedger .cc/.h # Handles Block storage, Validation, and Double Spending check
│   ├── CryptoUtils .cc/.h      # Helper functions for OpenSSL (SHA256, Keys)
│   │
│   └── Makefile                # Build file
└── simulation_logs.log         # Sample output

```

## What We Have Implemented

As per the assignment requirements, the simulation covers:

1. **P2P Network:**
* There are **Seed Nodes** (Servers) and **Peer Nodes** (Miners).
* Peers connect to Seeds to register and then connect to each other.


2. **Blockchain Logic:**
* **Mining:** Nodes use an exponential timer to simulate Proof-of-Work. The difficulty depends on hash power.
* **Consensus:** Nodes follow the **Longest Chain Rule**. If a longer chain is received, they switch to it.
* **Merkle Tree:** Every block calculates a Merkle Root for its transactions (Task 2).


3. **Security & Liveness:**
* **Double Spending:** We implemented a local ledger that remembers spent assets. If a node tries to spend the same asset twice, the block is rejected (Task 3).
* **Gossip Protocol:** Messages are flooded to neighbors. Loops are prevented using a unique ID check.
* **Dead Node Detection:** Nodes send a "Ping" every 13s. If a neighbor is silent for 39s, it is reported as **DEAD**.



## How to Compile and Run

### 1. Prerequisites

1. You need **OMNeT++** installed

2. You need **OpenSSL** installed
```bash
sudo apt-get install libssl-dev
```

### 2. Build Steps

1. Open OMNeT++ IDE and import the folder `Blockchain_Ledger_P2P`.
2. Right-click `BlockchainSim` -> **Properties** -> **OMNeT++** -> **Makemake**.
3. Select the folder, go to **Options** -> **Link**.
4. In "More options to link...", make sure to add:
```text
-lssl -lcrypto

```


5. Apply and Close.
6. Right-click Project -> **Clean**, then **Build Project**.

### 3. Running the Simulation

1. Open `omnetpp.ini`.
2. Click the Green **Run** button.
3. Select the `General` config.

## Simulation Scenarios

In `omnetpp.ini`, We have configured specific events to test the dynamic behavior:

* **T=0s:** Network starts. Seeds and Peers 0, 1, 2, 4 are online.
* **T=20s (Node Arrival):** **Peer 3** wakes up and joins the network late. It starts syncing.
* **T=40s (Attack):** **Peer 2** tries to do a **Double Spend Attack**. The network rejects this invalid block.
* **T=50s (Node Death):** **Peer 4** crashes (simulated failure).
* **T=89s:** Neighbors detect Peer 4 is dead (after 39s timeout) and broadcast a report.

## Group Members

* (B23CS1069) <b23cs1069@iitj.ac.in> Shreekar
* (B23CS1101) <B23CS1101@iitj.ac.in> Tavishi Srivastava
* (B23CS1076) <b23cs1076@iitj.ac.in> Vadlamudi Jyothsna
* (B23CS1031) <b23cs1031@iitj.ac.in> Kurra Hema
