// Blockchain.msg

message TransactionMsg {
    string sender;
    string receiver;
    double amount;
    string signature;
    string txId;
    string data;
    int timestamp;
}

message BlockMsg {
    int index;
    string previousHash;
    string merkleRoot;
    int timestamp;
    string minerId;
    string currentHash;
    string transactions[]; 
    int blockHeight;
}

message GossipMsg {
    int type; // 1=NewBlock, 2=NewTx, 3=Liveness, 4=DeadNodeReport
    string originatorIp;
    int msgId;
    string payload; 
    int hopCount;
}