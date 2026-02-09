//
// Generated file, do not edit! Created by opp_msgtool 6.3 from Blockchain.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "Blockchain_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

Register_Class(TransactionMsg)

TransactionMsg::TransactionMsg(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

TransactionMsg::TransactionMsg(const TransactionMsg& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

TransactionMsg::~TransactionMsg()
{
}

TransactionMsg& TransactionMsg::operator=(const TransactionMsg& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void TransactionMsg::copy(const TransactionMsg& other)
{
    this->sender = other.sender;
    this->receiver = other.receiver;
    this->amount = other.amount;
    this->signature = other.signature;
    this->txId = other.txId;
    this->data = other.data;
    this->timestamp = other.timestamp;
}

void TransactionMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->sender);
    doParsimPacking(b,this->receiver);
    doParsimPacking(b,this->amount);
    doParsimPacking(b,this->signature);
    doParsimPacking(b,this->txId);
    doParsimPacking(b,this->data);
    doParsimPacking(b,this->timestamp);
}

void TransactionMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->sender);
    doParsimUnpacking(b,this->receiver);
    doParsimUnpacking(b,this->amount);
    doParsimUnpacking(b,this->signature);
    doParsimUnpacking(b,this->txId);
    doParsimUnpacking(b,this->data);
    doParsimUnpacking(b,this->timestamp);
}

const char * TransactionMsg::getSender() const
{
    return this->sender.c_str();
}

void TransactionMsg::setSender(const char * sender)
{
    this->sender = sender;
}

const char * TransactionMsg::getReceiver() const
{
    return this->receiver.c_str();
}

void TransactionMsg::setReceiver(const char * receiver)
{
    this->receiver = receiver;
}

double TransactionMsg::getAmount() const
{
    return this->amount;
}

void TransactionMsg::setAmount(double amount)
{
    this->amount = amount;
}

const char * TransactionMsg::getSignature() const
{
    return this->signature.c_str();
}

void TransactionMsg::setSignature(const char * signature)
{
    this->signature = signature;
}

const char * TransactionMsg::getTxId() const
{
    return this->txId.c_str();
}

void TransactionMsg::setTxId(const char * txId)
{
    this->txId = txId;
}

const char * TransactionMsg::getData() const
{
    return this->data.c_str();
}

void TransactionMsg::setData(const char * data)
{
    this->data = data;
}

int TransactionMsg::getTimestamp() const
{
    return this->timestamp;
}

void TransactionMsg::setTimestamp(int timestamp)
{
    this->timestamp = timestamp;
}

class TransactionMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_sender,
        FIELD_receiver,
        FIELD_amount,
        FIELD_signature,
        FIELD_txId,
        FIELD_data,
        FIELD_timestamp,
    };
  public:
    TransactionMsgDescriptor();
    virtual ~TransactionMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(TransactionMsgDescriptor)

TransactionMsgDescriptor::TransactionMsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(TransactionMsg)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

TransactionMsgDescriptor::~TransactionMsgDescriptor()
{
    delete[] propertyNames;
}

bool TransactionMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<TransactionMsg *>(obj)!=nullptr;
}

const char **TransactionMsgDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *TransactionMsgDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int TransactionMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 7+base->getFieldCount() : 7;
}

unsigned int TransactionMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_sender
        FD_ISEDITABLE,    // FIELD_receiver
        FD_ISEDITABLE,    // FIELD_amount
        FD_ISEDITABLE,    // FIELD_signature
        FD_ISEDITABLE,    // FIELD_txId
        FD_ISEDITABLE,    // FIELD_data
        FD_ISEDITABLE,    // FIELD_timestamp
    };
    return (field >= 0 && field < 7) ? fieldTypeFlags[field] : 0;
}

const char *TransactionMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "sender",
        "receiver",
        "amount",
        "signature",
        "txId",
        "data",
        "timestamp",
    };
    return (field >= 0 && field < 7) ? fieldNames[field] : nullptr;
}

int TransactionMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "sender") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "receiver") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "amount") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "signature") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "txId") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "data") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "timestamp") == 0) return baseIndex + 6;
    return base ? base->findField(fieldName) : -1;
}

const char *TransactionMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",    // FIELD_sender
        "string",    // FIELD_receiver
        "double",    // FIELD_amount
        "string",    // FIELD_signature
        "string",    // FIELD_txId
        "string",    // FIELD_data
        "int",    // FIELD_timestamp
    };
    return (field >= 0 && field < 7) ? fieldTypeStrings[field] : nullptr;
}

const char **TransactionMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *TransactionMsgDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int TransactionMsgDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    TransactionMsg *pp = omnetpp::fromAnyPtr<TransactionMsg>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void TransactionMsgDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    TransactionMsg *pp = omnetpp::fromAnyPtr<TransactionMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'TransactionMsg'", field);
    }
}

const char *TransactionMsgDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    TransactionMsg *pp = omnetpp::fromAnyPtr<TransactionMsg>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string TransactionMsgDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    TransactionMsg *pp = omnetpp::fromAnyPtr<TransactionMsg>(object); (void)pp;
    switch (field) {
        case FIELD_sender: return oppstring2string(pp->getSender());
        case FIELD_receiver: return oppstring2string(pp->getReceiver());
        case FIELD_amount: return double2string(pp->getAmount());
        case FIELD_signature: return oppstring2string(pp->getSignature());
        case FIELD_txId: return oppstring2string(pp->getTxId());
        case FIELD_data: return oppstring2string(pp->getData());
        case FIELD_timestamp: return long2string(pp->getTimestamp());
        default: return "";
    }
}

void TransactionMsgDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    TransactionMsg *pp = omnetpp::fromAnyPtr<TransactionMsg>(object); (void)pp;
    switch (field) {
        case FIELD_sender: pp->setSender((value)); break;
        case FIELD_receiver: pp->setReceiver((value)); break;
        case FIELD_amount: pp->setAmount(string2double(value)); break;
        case FIELD_signature: pp->setSignature((value)); break;
        case FIELD_txId: pp->setTxId((value)); break;
        case FIELD_data: pp->setData((value)); break;
        case FIELD_timestamp: pp->setTimestamp(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TransactionMsg'", field);
    }
}

omnetpp::cValue TransactionMsgDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    TransactionMsg *pp = omnetpp::fromAnyPtr<TransactionMsg>(object); (void)pp;
    switch (field) {
        case FIELD_sender: return pp->getSender();
        case FIELD_receiver: return pp->getReceiver();
        case FIELD_amount: return pp->getAmount();
        case FIELD_signature: return pp->getSignature();
        case FIELD_txId: return pp->getTxId();
        case FIELD_data: return pp->getData();
        case FIELD_timestamp: return pp->getTimestamp();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'TransactionMsg' as cValue -- field index out of range?", field);
    }
}

void TransactionMsgDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    TransactionMsg *pp = omnetpp::fromAnyPtr<TransactionMsg>(object); (void)pp;
    switch (field) {
        case FIELD_sender: pp->setSender(value.stringValue()); break;
        case FIELD_receiver: pp->setReceiver(value.stringValue()); break;
        case FIELD_amount: pp->setAmount(value.doubleValue()); break;
        case FIELD_signature: pp->setSignature(value.stringValue()); break;
        case FIELD_txId: pp->setTxId(value.stringValue()); break;
        case FIELD_data: pp->setData(value.stringValue()); break;
        case FIELD_timestamp: pp->setTimestamp(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TransactionMsg'", field);
    }
}

const char *TransactionMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr TransactionMsgDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    TransactionMsg *pp = omnetpp::fromAnyPtr<TransactionMsg>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void TransactionMsgDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    TransactionMsg *pp = omnetpp::fromAnyPtr<TransactionMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TransactionMsg'", field);
    }
}

Register_Class(BlockMsg)

BlockMsg::BlockMsg(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

BlockMsg::BlockMsg(const BlockMsg& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

BlockMsg::~BlockMsg()
{
    delete [] this->transactions;
}

BlockMsg& BlockMsg::operator=(const BlockMsg& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void BlockMsg::copy(const BlockMsg& other)
{
    this->index = other.index;
    this->previousHash = other.previousHash;
    this->merkleRoot = other.merkleRoot;
    this->timestamp = other.timestamp;
    this->minerId = other.minerId;
    this->currentHash = other.currentHash;
    delete [] this->transactions;
    this->transactions = (other.transactions_arraysize==0) ? nullptr : new omnetpp::opp_string[other.transactions_arraysize];
    transactions_arraysize = other.transactions_arraysize;
    for (size_t i = 0; i < transactions_arraysize; i++) {
        this->transactions[i] = other.transactions[i];
    }
    this->blockHeight = other.blockHeight;
}

void BlockMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->index);
    doParsimPacking(b,this->previousHash);
    doParsimPacking(b,this->merkleRoot);
    doParsimPacking(b,this->timestamp);
    doParsimPacking(b,this->minerId);
    doParsimPacking(b,this->currentHash);
    b->pack(transactions_arraysize);
    doParsimArrayPacking(b,this->transactions,transactions_arraysize);
    doParsimPacking(b,this->blockHeight);
}

void BlockMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->index);
    doParsimUnpacking(b,this->previousHash);
    doParsimUnpacking(b,this->merkleRoot);
    doParsimUnpacking(b,this->timestamp);
    doParsimUnpacking(b,this->minerId);
    doParsimUnpacking(b,this->currentHash);
    delete [] this->transactions;
    b->unpack(transactions_arraysize);
    if (transactions_arraysize == 0) {
        this->transactions = nullptr;
    } else {
        this->transactions = new omnetpp::opp_string[transactions_arraysize];
        doParsimArrayUnpacking(b,this->transactions,transactions_arraysize);
    }
    doParsimUnpacking(b,this->blockHeight);
}

int BlockMsg::getIndex() const
{
    return this->index;
}

void BlockMsg::setIndex(int index)
{
    this->index = index;
}

const char * BlockMsg::getPreviousHash() const
{
    return this->previousHash.c_str();
}

void BlockMsg::setPreviousHash(const char * previousHash)
{
    this->previousHash = previousHash;
}

const char * BlockMsg::getMerkleRoot() const
{
    return this->merkleRoot.c_str();
}

void BlockMsg::setMerkleRoot(const char * merkleRoot)
{
    this->merkleRoot = merkleRoot;
}

int BlockMsg::getTimestamp() const
{
    return this->timestamp;
}

void BlockMsg::setTimestamp(int timestamp)
{
    this->timestamp = timestamp;
}

const char * BlockMsg::getMinerId() const
{
    return this->minerId.c_str();
}

void BlockMsg::setMinerId(const char * minerId)
{
    this->minerId = minerId;
}

const char * BlockMsg::getCurrentHash() const
{
    return this->currentHash.c_str();
}

void BlockMsg::setCurrentHash(const char * currentHash)
{
    this->currentHash = currentHash;
}

size_t BlockMsg::getTransactionsArraySize() const
{
    return transactions_arraysize;
}

const char * BlockMsg::getTransactions(size_t k) const
{
    if (k >= transactions_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)transactions_arraysize, (unsigned long)k);
    return this->transactions[k].c_str();
}

void BlockMsg::setTransactionsArraySize(size_t newSize)
{
    omnetpp::opp_string *transactions2 = (newSize==0) ? nullptr : new omnetpp::opp_string[newSize];
    size_t minSize = transactions_arraysize < newSize ? transactions_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        transactions2[i] = this->transactions[i];
    delete [] this->transactions;
    this->transactions = transactions2;
    transactions_arraysize = newSize;
}

void BlockMsg::setTransactions(size_t k, const char * transactions)
{
    if (k >= transactions_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)transactions_arraysize, (unsigned long)k);
    this->transactions[k] = transactions;
}

void BlockMsg::insertTransactions(size_t k, const char * transactions)
{
    if (k > transactions_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)transactions_arraysize, (unsigned long)k);
    size_t newSize = transactions_arraysize + 1;
    omnetpp::opp_string *transactions2 = new omnetpp::opp_string[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        transactions2[i] = this->transactions[i];
    transactions2[k] = transactions;
    for (i = k + 1; i < newSize; i++)
        transactions2[i] = this->transactions[i-1];
    delete [] this->transactions;
    this->transactions = transactions2;
    transactions_arraysize = newSize;
}

void BlockMsg::appendTransactions(const char * transactions)
{
    insertTransactions(transactions_arraysize, transactions);
}

void BlockMsg::eraseTransactions(size_t k)
{
    if (k >= transactions_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)transactions_arraysize, (unsigned long)k);
    size_t newSize = transactions_arraysize - 1;
    omnetpp::opp_string *transactions2 = (newSize == 0) ? nullptr : new omnetpp::opp_string[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        transactions2[i] = this->transactions[i];
    for (i = k; i < newSize; i++)
        transactions2[i] = this->transactions[i+1];
    delete [] this->transactions;
    this->transactions = transactions2;
    transactions_arraysize = newSize;
}

int BlockMsg::getBlockHeight() const
{
    return this->blockHeight;
}

void BlockMsg::setBlockHeight(int blockHeight)
{
    this->blockHeight = blockHeight;
}

class BlockMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_index,
        FIELD_previousHash,
        FIELD_merkleRoot,
        FIELD_timestamp,
        FIELD_minerId,
        FIELD_currentHash,
        FIELD_transactions,
        FIELD_blockHeight,
    };
  public:
    BlockMsgDescriptor();
    virtual ~BlockMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(BlockMsgDescriptor)

BlockMsgDescriptor::BlockMsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(BlockMsg)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

BlockMsgDescriptor::~BlockMsgDescriptor()
{
    delete[] propertyNames;
}

bool BlockMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<BlockMsg *>(obj)!=nullptr;
}

const char **BlockMsgDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *BlockMsgDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int BlockMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 8+base->getFieldCount() : 8;
}

unsigned int BlockMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_index
        FD_ISEDITABLE,    // FIELD_previousHash
        FD_ISEDITABLE,    // FIELD_merkleRoot
        FD_ISEDITABLE,    // FIELD_timestamp
        FD_ISEDITABLE,    // FIELD_minerId
        FD_ISEDITABLE,    // FIELD_currentHash
        FD_ISARRAY | FD_ISEDITABLE | FD_ISRESIZABLE,    // FIELD_transactions
        FD_ISEDITABLE,    // FIELD_blockHeight
    };
    return (field >= 0 && field < 8) ? fieldTypeFlags[field] : 0;
}

const char *BlockMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "index",
        "previousHash",
        "merkleRoot",
        "timestamp",
        "minerId",
        "currentHash",
        "transactions",
        "blockHeight",
    };
    return (field >= 0 && field < 8) ? fieldNames[field] : nullptr;
}

int BlockMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "index") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "previousHash") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "merkleRoot") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "timestamp") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "minerId") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "currentHash") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "transactions") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "blockHeight") == 0) return baseIndex + 7;
    return base ? base->findField(fieldName) : -1;
}

const char *BlockMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_index
        "string",    // FIELD_previousHash
        "string",    // FIELD_merkleRoot
        "int",    // FIELD_timestamp
        "string",    // FIELD_minerId
        "string",    // FIELD_currentHash
        "string",    // FIELD_transactions
        "int",    // FIELD_blockHeight
    };
    return (field >= 0 && field < 8) ? fieldTypeStrings[field] : nullptr;
}

const char **BlockMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *BlockMsgDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int BlockMsgDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    BlockMsg *pp = omnetpp::fromAnyPtr<BlockMsg>(object); (void)pp;
    switch (field) {
        case FIELD_transactions: return pp->getTransactionsArraySize();
        default: return 0;
    }
}

void BlockMsgDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    BlockMsg *pp = omnetpp::fromAnyPtr<BlockMsg>(object); (void)pp;
    switch (field) {
        case FIELD_transactions: pp->setTransactionsArraySize(size); break;
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'BlockMsg'", field);
    }
}

const char *BlockMsgDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    BlockMsg *pp = omnetpp::fromAnyPtr<BlockMsg>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string BlockMsgDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    BlockMsg *pp = omnetpp::fromAnyPtr<BlockMsg>(object); (void)pp;
    switch (field) {
        case FIELD_index: return long2string(pp->getIndex());
        case FIELD_previousHash: return oppstring2string(pp->getPreviousHash());
        case FIELD_merkleRoot: return oppstring2string(pp->getMerkleRoot());
        case FIELD_timestamp: return long2string(pp->getTimestamp());
        case FIELD_minerId: return oppstring2string(pp->getMinerId());
        case FIELD_currentHash: return oppstring2string(pp->getCurrentHash());
        case FIELD_transactions: return oppstring2string(pp->getTransactions(i));
        case FIELD_blockHeight: return long2string(pp->getBlockHeight());
        default: return "";
    }
}

void BlockMsgDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    BlockMsg *pp = omnetpp::fromAnyPtr<BlockMsg>(object); (void)pp;
    switch (field) {
        case FIELD_index: pp->setIndex(string2long(value)); break;
        case FIELD_previousHash: pp->setPreviousHash((value)); break;
        case FIELD_merkleRoot: pp->setMerkleRoot((value)); break;
        case FIELD_timestamp: pp->setTimestamp(string2long(value)); break;
        case FIELD_minerId: pp->setMinerId((value)); break;
        case FIELD_currentHash: pp->setCurrentHash((value)); break;
        case FIELD_transactions: pp->setTransactions(i,(value)); break;
        case FIELD_blockHeight: pp->setBlockHeight(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'BlockMsg'", field);
    }
}

omnetpp::cValue BlockMsgDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    BlockMsg *pp = omnetpp::fromAnyPtr<BlockMsg>(object); (void)pp;
    switch (field) {
        case FIELD_index: return pp->getIndex();
        case FIELD_previousHash: return pp->getPreviousHash();
        case FIELD_merkleRoot: return pp->getMerkleRoot();
        case FIELD_timestamp: return pp->getTimestamp();
        case FIELD_minerId: return pp->getMinerId();
        case FIELD_currentHash: return pp->getCurrentHash();
        case FIELD_transactions: return pp->getTransactions(i);
        case FIELD_blockHeight: return pp->getBlockHeight();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'BlockMsg' as cValue -- field index out of range?", field);
    }
}

void BlockMsgDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    BlockMsg *pp = omnetpp::fromAnyPtr<BlockMsg>(object); (void)pp;
    switch (field) {
        case FIELD_index: pp->setIndex(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_previousHash: pp->setPreviousHash(value.stringValue()); break;
        case FIELD_merkleRoot: pp->setMerkleRoot(value.stringValue()); break;
        case FIELD_timestamp: pp->setTimestamp(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_minerId: pp->setMinerId(value.stringValue()); break;
        case FIELD_currentHash: pp->setCurrentHash(value.stringValue()); break;
        case FIELD_transactions: pp->setTransactions(i,value.stringValue()); break;
        case FIELD_blockHeight: pp->setBlockHeight(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'BlockMsg'", field);
    }
}

const char *BlockMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr BlockMsgDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    BlockMsg *pp = omnetpp::fromAnyPtr<BlockMsg>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void BlockMsgDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    BlockMsg *pp = omnetpp::fromAnyPtr<BlockMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'BlockMsg'", field);
    }
}

Register_Class(GossipMsg)

GossipMsg::GossipMsg(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

GossipMsg::GossipMsg(const GossipMsg& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

GossipMsg::~GossipMsg()
{
}

GossipMsg& GossipMsg::operator=(const GossipMsg& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void GossipMsg::copy(const GossipMsg& other)
{
    this->type = other.type;
    this->originatorIp = other.originatorIp;
    this->msgId = other.msgId;
    this->payload = other.payload;
    this->hopCount = other.hopCount;
}

void GossipMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->type);
    doParsimPacking(b,this->originatorIp);
    doParsimPacking(b,this->msgId);
    doParsimPacking(b,this->payload);
    doParsimPacking(b,this->hopCount);
}

void GossipMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->type);
    doParsimUnpacking(b,this->originatorIp);
    doParsimUnpacking(b,this->msgId);
    doParsimUnpacking(b,this->payload);
    doParsimUnpacking(b,this->hopCount);
}

int GossipMsg::getType() const
{
    return this->type;
}

void GossipMsg::setType(int type)
{
    this->type = type;
}

const char * GossipMsg::getOriginatorIp() const
{
    return this->originatorIp.c_str();
}

void GossipMsg::setOriginatorIp(const char * originatorIp)
{
    this->originatorIp = originatorIp;
}

int GossipMsg::getMsgId() const
{
    return this->msgId;
}

void GossipMsg::setMsgId(int msgId)
{
    this->msgId = msgId;
}

const char * GossipMsg::getPayload() const
{
    return this->payload.c_str();
}

void GossipMsg::setPayload(const char * payload)
{
    this->payload = payload;
}

int GossipMsg::getHopCount() const
{
    return this->hopCount;
}

void GossipMsg::setHopCount(int hopCount)
{
    this->hopCount = hopCount;
}

class GossipMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_type,
        FIELD_originatorIp,
        FIELD_msgId,
        FIELD_payload,
        FIELD_hopCount,
    };
  public:
    GossipMsgDescriptor();
    virtual ~GossipMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(GossipMsgDescriptor)

GossipMsgDescriptor::GossipMsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(GossipMsg)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

GossipMsgDescriptor::~GossipMsgDescriptor()
{
    delete[] propertyNames;
}

bool GossipMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<GossipMsg *>(obj)!=nullptr;
}

const char **GossipMsgDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *GossipMsgDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int GossipMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 5+base->getFieldCount() : 5;
}

unsigned int GossipMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_type
        FD_ISEDITABLE,    // FIELD_originatorIp
        FD_ISEDITABLE,    // FIELD_msgId
        FD_ISEDITABLE,    // FIELD_payload
        FD_ISEDITABLE,    // FIELD_hopCount
    };
    return (field >= 0 && field < 5) ? fieldTypeFlags[field] : 0;
}

const char *GossipMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "type",
        "originatorIp",
        "msgId",
        "payload",
        "hopCount",
    };
    return (field >= 0 && field < 5) ? fieldNames[field] : nullptr;
}

int GossipMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "type") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "originatorIp") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "msgId") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "payload") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "hopCount") == 0) return baseIndex + 4;
    return base ? base->findField(fieldName) : -1;
}

const char *GossipMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_type
        "string",    // FIELD_originatorIp
        "int",    // FIELD_msgId
        "string",    // FIELD_payload
        "int",    // FIELD_hopCount
    };
    return (field >= 0 && field < 5) ? fieldTypeStrings[field] : nullptr;
}

const char **GossipMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *GossipMsgDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int GossipMsgDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    GossipMsg *pp = omnetpp::fromAnyPtr<GossipMsg>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void GossipMsgDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    GossipMsg *pp = omnetpp::fromAnyPtr<GossipMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'GossipMsg'", field);
    }
}

const char *GossipMsgDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    GossipMsg *pp = omnetpp::fromAnyPtr<GossipMsg>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string GossipMsgDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    GossipMsg *pp = omnetpp::fromAnyPtr<GossipMsg>(object); (void)pp;
    switch (field) {
        case FIELD_type: return long2string(pp->getType());
        case FIELD_originatorIp: return oppstring2string(pp->getOriginatorIp());
        case FIELD_msgId: return long2string(pp->getMsgId());
        case FIELD_payload: return oppstring2string(pp->getPayload());
        case FIELD_hopCount: return long2string(pp->getHopCount());
        default: return "";
    }
}

void GossipMsgDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    GossipMsg *pp = omnetpp::fromAnyPtr<GossipMsg>(object); (void)pp;
    switch (field) {
        case FIELD_type: pp->setType(string2long(value)); break;
        case FIELD_originatorIp: pp->setOriginatorIp((value)); break;
        case FIELD_msgId: pp->setMsgId(string2long(value)); break;
        case FIELD_payload: pp->setPayload((value)); break;
        case FIELD_hopCount: pp->setHopCount(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'GossipMsg'", field);
    }
}

omnetpp::cValue GossipMsgDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    GossipMsg *pp = omnetpp::fromAnyPtr<GossipMsg>(object); (void)pp;
    switch (field) {
        case FIELD_type: return pp->getType();
        case FIELD_originatorIp: return pp->getOriginatorIp();
        case FIELD_msgId: return pp->getMsgId();
        case FIELD_payload: return pp->getPayload();
        case FIELD_hopCount: return pp->getHopCount();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'GossipMsg' as cValue -- field index out of range?", field);
    }
}

void GossipMsgDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    GossipMsg *pp = omnetpp::fromAnyPtr<GossipMsg>(object); (void)pp;
    switch (field) {
        case FIELD_type: pp->setType(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_originatorIp: pp->setOriginatorIp(value.stringValue()); break;
        case FIELD_msgId: pp->setMsgId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_payload: pp->setPayload(value.stringValue()); break;
        case FIELD_hopCount: pp->setHopCount(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'GossipMsg'", field);
    }
}

const char *GossipMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr GossipMsgDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    GossipMsg *pp = omnetpp::fromAnyPtr<GossipMsg>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void GossipMsgDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    GossipMsg *pp = omnetpp::fromAnyPtr<GossipMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'GossipMsg'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

