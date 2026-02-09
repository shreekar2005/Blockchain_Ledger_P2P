// Blockchain.ned
simple Node
{
    parameters:
        bool isSeed = default(false);
        double hashPower = default(10.0);
        string myIp = default("192.168.0.x");

        // NEW PARAMETERS for Dynamic Lifecycle
        double startUpTime @unit(s) = default(0s);    // When does this node come online?
        double shutdownTime @unit(s) = default(-1s);  // When does it crash? (-1 = never)

        @display("i=device/pc");
    gates:
        inout port[];
}

network SupplyChainNetwork
{
    types:
        channel Cable extends ned.DelayChannel {
            delay = 100ms;
        }
    submodules:
        seed[2]: Node {
            isSeed = true;
            @display("i=device/server;is=l");
        }
        peer[5]: Node {
            isSeed = false;
            @display("i=device/pc");
        }
    connections allowunconnected:
        // P2P and Seed connections (Same as before)
        peer[0].port++ <--> Cable <--> seed[0].port++;
        peer[1].port++ <--> Cable <--> seed[0].port++;
        peer[2].port++ <--> Cable <--> seed[1].port++;
        peer[3].port++ <--> Cable <--> seed[1].port++;
        peer[4].port++ <--> Cable <--> seed[0].port++;

        peer[0].port++ <--> Cable <--> peer[1].port++;
        peer[1].port++ <--> Cable <--> peer[2].port++;
        peer[2].port++ <--> Cable <--> peer[3].port++;
        peer[3].port++ <--> Cable <--> peer[4].port++;
        peer[4].port++ <--> Cable <--> peer[0].port++;
}
