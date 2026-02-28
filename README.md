# Blockchain_Ledger_P2P
**Assignment 1 - CSL7490**
**Objective:** Design a blockchain-based petroleum supply chain ledger on top of a P2P network.

This is our submission for the Blockchain assignment. We have implemented a P2P network simulation using **OMNeT++ 6.3.0** and **OpenSSL** on Ubuntu. The project simulates mining, block propagation, and consensus in a distributed network.

## Project Structure

We have split the code into different modules to make it clean and easy to understand:

```bash
Blockchain_Ledger_P2P/
├── Analytical/
│   └── task7_analysis.py       # Python script for Stochastic Analysis of Mining (Task 7)
│
├── Simulation/                 # Main OMNeT++ workspace
│   ├── Blockchain.ned          # Defines the network (Seeds and Peers)
│   ├── Blockchain.msg          # Defines message types (Block, Transaction, Gossip)
│   ├── omnetpp.ini             # Configuration file (Timers, Scenarios)
│   │
│   ├── Node .cc/.h             # Main simulation logic (Timers, Network handling)
│   ├── BlockchainLedger .cc/.h # Handles Block storage, Validation, and Double Spending check
│   ├── CryptoUtils .cc/.h      # Helper functions for OpenSSL (SHA256, Keys, ECDSA)
│   │
│   ├── Makefile                # Build file
│   ├── out/                    # Generated compiled objects and executables
│   └── results/                # Output simulation scalar/vector data
│
├── blockchain_assignment_1.pdf # Actual assignment given by Instructor
├── LICENSE                     # Project License
├── README.md                   # This documentation file
├── seed_ips.txt                # Bootstrapping IPs for the nodes
└── simulation_logs.log         # Complete execution trace output
```

## What We Have Implemented

As per the assignment requirements, the simulation covers:

1. **P2P Network:**

* There are **Seed Nodes** (Servers) and **Peer Nodes** (Miners).
* Peers connect to Seeds to register, obtain Peer Lists, and establish connections to at least 4 distinct peers to form a connected graph.

2. **Blockchain Logic:**

* **Mining:** Nodes use an exponential timer to simulate Proof-of-Work. The difficulty depends on hash power.
* **Consensus:** Nodes follow the **Longest Chain Rule**. If a longer chain is received, they switch to it.
* **Merkle Tree:** Every block calculates a Merkle Root for its transactions (Task 2).
* **Cryptography:** Implemented unique wallet address generation and true ECDSA digital signatures using the `secp256k1` curve via OpenSSL.

3. **Security & Liveness:**

* **Double Spending:** We implemented a local UTXO ledger that tracks unspent assets. If a node tries to spend the same asset twice, the block is rejected (Task 3).
* **Gossip Protocol:** Messages are flooded to neighbors. Loops are prevented using a unique ID check.
* **Dead Node Detection:** Nodes send a "Ping" every 13s. If a neighbor is silent for 39s, it is reported as **DEAD**.

4. **Analytical Modeling (Task 7):**

* Included a Python script (`task7_analysis.py`) that models the exponential random variables to plot the waiting time distribution and the relationship between lambda and Hash Power.

## How to Compile and Run

### 1. Prerequisites

1. You need **OMNeT++** installed.
2. You need **OpenSSL** installed for the cryptographic functions:

```bash
sudo apt-get install libssl-dev
```

3. You need **Python 3** and **Matplotlib** (only for Task 7 graphs):

```bash
sudo apt-get install python3-matplotlib python3-numpy
```

### 2. Build Steps (OMNeT++ IDE)

1. Open OMNeT++ IDE and import the `Simulation` folder.
2. Click on `Simulation` in the Project Explorer view.
3. Click on **Project** in top bar -> **Properties** -> **OMNeT++** -> **Makemake**.
4. Select the folder, go to **Options** -> **Link**.
5. In "Additional libraries to link with: (-l option)", make sure to add:
   1. `ssl`
   2. `crypto`
6. Click OK, Click Apply and Close.
7. Right-click the project in Project Explorer view -> **Clean**, then **Build Project**.
8. Open `omnetpp.ini` and click the **Run** button.

### 3. Build Steps (Terminal / VS Code)

If you prefer using **VS Code** or just the terminal, you can manage the build process using `opp_makemake`. This is what I use most of the time.

1. **Navigate to the Simulation directory:**
```bash
cd Simulation
```

2. **Generate the Makefile:**
This tells OMNeT++ to create a Makefile that links the necessary OpenSSL libraries:
```bash
opp_makemake -f --deep -O out -lssl -lcrypto
```

3. **Compile the Project:**
```bash
make
```

4. **Run the Simulation:**
```bash
./Simulation
```

*(Note: If you are on a Mac or have OpenSSL in a custom path, you might need to add `-L/path/to/ssl/lib -I/path/to/ssl/include` to the `opp_makemake` command).*

## Automatic File Generation (Optional Context)

When you build the project, OMNeT++ automatically generates several critical files to bridge the simulation logic with C++:

* **`Blockchain_m.h` & `Blockchain_m.cc`:** These are C++ classes automatically generated from `Blockchain.msg`. They handle the packing and unpacking of message data (like Blocks and Transactions) for the simulation.

## Simulation Scenarios (Verified by logs)
### Running the Demo

To observe specific blockchain security features, run the simulation and check the `simulation_logs.log` or the runtime Qtenv logs at these times:

1. **t=50s (Invalid Signature):** Peer-2 attempts to inject a malicious transaction (ID: `3d1fe103...`) with a forged signature. The network detects the mismatch and rejects it.
2. **t=100s (Liveness/Node Failure):** `peer-5` is forcefully killed. By **t=136s**, its neighbors detect the silence and report it to the Seeds, who then broadcast a "Confirmed DEAD" alert to update all Peer Lists.
3. **t=150s (Double Spend):** Peer-3 tries to spend the same "Initial_Supply" asset twice. The second transaction is caught in the mempool and rejected.
4. **t=200s (Malicious Block):** Peer-4 mines a block with a **Fake Previous Hash**. Even if the Proof-of-Work is valid, the block is rejected during the chain validation phase.

## Group Members

* (B23CS1069) b23cs1069@iitj.ac.in - Shreekar
* (B23CS1101) B23CS1101@iitj.ac.in - Tavishi Srivastava
* (B23CS1076) b23cs1076@iitj.ac.in - Vadlamudi Jyothsna
* (B23CS1031) b23cs1031@iitj.ac.in - Kurra Hema