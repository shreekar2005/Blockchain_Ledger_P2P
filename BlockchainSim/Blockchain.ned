// Blockchain.ned

simple Node
{
    parameters:
        bool isSeed = default(false);
        double hashPower = default(10.0);
        string myIp = default("192.168.0.x");

        double startUpTime @unit(s) = default(0s);
        double shutdownTime @unit(s) = default(-1s);

        @display("i=device/pc");
    gates:
        inout port[];
}

network SupplyChainNetwork
{
    // NEW: make the background canvas large and spacious (800x600 pixels)
    @display("bgb=800,800"); 
    
    types:
        channel Cable extends ned.DelayChannel {
            delay = 100ms;
        }
    submodules:
        seed[2]: Node {
            isSeed = true;
            // NEW: 'm' means matrix layout. 
            // places seed[0] at (300,300) and seed[1] at (500,300).
            // they will sit perfectly in the center of the screen.
            @display("i=device/server;is=n");
        }
        peer[5]: Node {
            isSeed = false;
            // NEW: 'ring' layout!
            // arranges all 5 peers in a perfect circle centered at (400,300) 
            // with a radius of 220 pixels.
            @display("i=device/pc;p=100,100,ring,320,320");
        }
    connections allowunconnected:
        // peer server
        peer[0].port++ <--> Cable <--> seed[0].port++;
        peer[1].port++ <--> Cable <--> seed[0].port++;
        peer[2].port++ <--> Cable <--> seed[1].port++;
        peer[3].port++ <--> Cable <--> seed[1].port++;
        peer[4].port++ <--> Cable <--> seed[0].port++;

        // peer peer
        peer[0].port++ <--> Cable <--> peer[1].port++;
        peer[1].port++ <--> Cable <--> peer[2].port++;
        peer[2].port++ <--> Cable <--> peer[3].port++;
        peer[3].port++ <--> Cable <--> peer[4].port++;
        peer[4].port++ <--> Cable <--> peer[0].port++;

        // server server
        seed[0].port++ <--> Cable <--> seed[1].port++;
}