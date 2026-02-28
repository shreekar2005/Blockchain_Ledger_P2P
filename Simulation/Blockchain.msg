// Blockchain.msg
message TransactionMsg {
    string txId;              
    string senderAddress;     
    string senderPublicKey;   
    string receiverAddress;   
    string data;              
    string sigR;              
    string sigS;              
    double timestamp;
}

message BlockMsg {
    int index;
    string previousHash;
    string merkleRoot;
    double timestamp;         
    string minerId;
    string currentHash;
    TransactionMsg transactions[]; 
}

message GossipMsg {
    int type;                 // 1=Block, 2=Tx, 3=Liveness, 4=DeadNode
    string originatorIp;
    int msgId;
    string gossipFormat;      
    string payload;           
    TransactionMsg txPayload; // Payload for standalone transaction gossip
    int hopCount;
}

message RegisterMsg {
    string peerIp;
    int peerModuleId;
}

message PeerListMsg {
    string peers[];
    int peerModuleIds[];
}

message SyncRequestMsg {
    string requesterIp;
    int targetHeight;
}