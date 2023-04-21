// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#pragma once

#include "../Strings/StringInMemoryPool.hpp"
#include "MemoryPool.hpp"

#include <stdlib.h>  // malloc, free

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnon-virtual-dtor"
#elif defined(__GNUC__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif

namespace ARDUINOJSON_NAMESPACE {
class DefaultAllocator {
 public:
  void* allocate(size_t size) {
    return malloc(size);
  }
  void deallocate(void* pointer) {
    free(pointer);
  }
};

template <typename TAllocator>
class DynamicMemoryPoolBase : public MemoryPool {
  struct Block;
  struct EmptyBlock {
    Block* next;
    size_t capacity;
    size_t size;
  };
  struct Block : EmptyBlock {
    uint8_t data[1];
  };

 public:
  enum { EmptyBlockSize = sizeof(EmptyBlock) };

  DynamicMemoryPoolBase(size_t initialSize = ARDUINOJSON_DEFAULT_POOL_SIZE)
      : _head(NULL), _nextBlockCapacity(initialSize) {}

  ~DynamicMemoryPoolBase() {
    clear();
  }

  void reserve(size_t capacity) {
    _nextBlockCapacity = capacity;
  }

  // Gets the number of bytes occupied in the memoryPool
  size_t size() const {
    size_t total = 0;
    for (const Block* b = _head; b; b = b->next) total += b->size;
    return total;
  }

  // Allocates the specified amount of bytes in the memoryPool
  virtual void* alloc(size_t bytes) {
    alignNextAlloc();
    return canAllocInHead(bytes) ? allocInHead(bytes) : allocInNewBlock(bytes);
  }

  // Resets the memoryPool.
  // USE WITH CAUTION: this invalidates all previously allocated data
  void clear() {
    Block* currentBlock = _head;
    while (currentBlock != NULL) {
      _nextBlockCapacity = currentBlock->capacity;
      Block* nextBlock = currentBlock->next;
      _allocator.deallocate(currentBlock);
      currentBlock = nextBlock;
    }
    _head = 0;
  }

  class StringBuilder {
   public:
    explicit StringBuilder(DynamicMemoryPoolBase* parent)
        : _parent(parent), _start(NULL), _length(0) {}

    void append(char c) {
      if (_parent->canAllocInHead(1)) {
        char* end = static_cast<char*>(_parent->allocInHead(1));
        *end = c;
        if (_length == 0) _start = end;
      } else {
        char* newStart =
            static_cast<char*>(_parent->allocInNewBlock(_length + 1));
        if (_start && newStart) memcpy(newStart, _start, _length);
        if (newStart) newStart[_length] = c;
        _start = newStart;
      }
      _length++;
    }

    StringInMemoryPool complete() {
      append(0);
      return _start;
    }

   private:
    DynamicMemoryPoolBase* _parent;
    char* _start;
    size_t _length;
  };

  StringBuilder startString() {
    return StringBuilder(this);
  }

 private:
  void alignNextAlloc() {
    if (_head) _head->size = this->round_size_up(_head->size);
  }

  bool canAllocInHead(size_t bytes) const {
    return _head != NULL && _head->size + bytes <= _head->capacity;
  }

  void* allocInHead(size_t bytes) {
    void* p = _head->data + _head->size;
    _head->size += bytes;
    return p;
  }

  void* allocInNewBlock(size_t bytes) {
    size_t capacity = _nextBlockCapacity;
    if (bytes > capacity) capacity = bytes;
    if (!addNewBlock(capacity)) return NULL;
    _nextBlockCapacity *= 2;
    return allocInHead(bytes);
  }

  bool addNewBlock(size_t capacity) {
    size_t bytes = EmptyBlockSize + capacity;
    Block* block = static_cast<Block*>(_allocator.allocate(bytes));
    if (block == NULL) return false;
    block->capacity = capacity;
    block->size = 0;
    block->next = _head;
    _head = block;
    return true;
  }

  TAllocator _allocator;
  Block* _head;
  size_t _nextBlockCapacity;
};

// Implements a MemoryPool with dynamic memory allocation.
// You are strongly encouraged to consider using StaticMemoryPool which is much
// more suitable for embedded systems.
typedef DynamicMemoryPoolBase<DefaultAllocator> DynamicMemoryPool;
}  // namespace ARDUINOJSON_NAMESPACE

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
#endif
