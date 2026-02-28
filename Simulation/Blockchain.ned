// Blockchain.ned
simple Node {
    parameters:
        bool isSeed = default(false);
        double hashPower = default(10.0);
        double meanBlockTime = default(2.0);
        string myIp = default("");
        double startUpTime @unit(s) = default(0s);
        double killTime @unit(s) = default(-1s);
        double invalidTxTime @unit(s) = default(-1s);
        double doubleSpendTime @unit(s) = default(-1s);
        double invalidBlockTime @unit(s) = default(-1s);
    gates:
        inout port[]; // Empty brackets allow dynamic resizing at runtime
}

network SupplyChainNetwork {
    parameters:
        @display("bgb=800,800"); 
        int numSeeds = default(3);
        int numPeers = default(10);
        
    submodules:
        seed[numSeeds]: Node {
            parameters:
                isSeed = true;
                myIp = "seed-" + string(index);
                @display("i=device/server;is=n");
        }
        peer[numPeers]: Node {
            parameters:
                isSeed = false;
                myIp = "peer-" + string(index);
                @display("i=device/pc;p=100,100,ring,320,320");
        }
        
    connections allowunconnected:
        // Connections are built dynamically in C++
}