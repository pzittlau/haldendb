#pragma once

#include "BeTreeLRUCache.hpp"
#include "BeTreeMessage.hpp"
#include "ErrorCodes.h"
#include <cstdint>
#include <iosfwd>
#include <memory>
#include <utility>
#include <vector>

// Forward declarations
template <typename KeyType, typename ValueType> class Message;
template <typename KeyType, typename ValueType> class BeTreeLRUCache;

template <typename KeyType, typename ValueType>
class BeTreeNode : public std::enable_shared_from_this<BeTreeNode<KeyType, ValueType>> {
public:
    using MessagePtr = std::unique_ptr<Message<KeyType, ValueType>>;
    using BeTreeNodePtr = std::shared_ptr<BeTreeNode<KeyType, ValueType>>;
    using CachePtr = std::shared_ptr<BeTreeLRUCache<KeyType, ValueType>>;

    uint16_t fanout; // maximum amount of children an internal node can have
    std::vector<KeyType> keys;
    KeyType lowestSearchKey;

    uint64_t id;
    uint64_t parent;
    uint64_t leftSibling;
    uint64_t rightSibling;

    CachePtr cache;

    enum ChildChangeType {
        None,
        Split,
        RedistributeLeft,
        RedistributeRight,
        MergeLeft,
        MergeRight,
    };

    struct ChildChange {
        KeyType key;
        uint64_t node;
        ChildChangeType type;
    };

    virtual ~BeTreeNode() = default;
    BeTreeNode(uint16_t fanout, CachePtr cache, uint64_t parent = 0)
        : fanout(fanout), parent(parent), cache(cache), id(0), leftSibling(0), rightSibling(0) {}

    virtual ErrorCode applyMessage(MessagePtr message, uint16_t indexInParent, ChildChange& oldChild) = 0;
    virtual ErrorCode insert(MessagePtr message, ChildChange& newChild) = 0;
    virtual ErrorCode remove(MessagePtr message, uint16_t indexInParent, ChildChange& oldChild) = 0;
    virtual std::pair<ValueType, ErrorCode> search(MessagePtr message) = 0;

    // Helper functions
    virtual bool isLeaf() const = 0;
    bool isRoot() const { return this->parent == 0; }
    uint16_t size() const { return this->keys.size(); }
    bool isUnderflowing() const { return size() < (this->fanout - 1) / 2; }
    bool isMergeable() const { return size() == this->fanout / 2; }
    bool isBorrowable() const { return size() > (this->fanout - 1) / 2; }
    bool isSplittable() const { return size() >= this->fanout - 1; }
    bool isOverflowing() const { return size() >= this->fanout; }

    auto getIndex(KeyType key) {
        return std::lower_bound(keys.begin(), keys.end(), key);
    }

    virtual size_t getSerializedSize() const = 0;
    virtual size_t serialize(char*& buf) const = 0; // returns the number of bytes written
    virtual void serialize(std::ostream& os) const = 0;
    virtual size_t deserialize(char*& buf, size_t bufferSize) = 0; // returns the number of bytes read
    virtual void deserialize(std::istream& is) = 0;

    virtual void printNode(std::ostream& out) const = 0;
};
