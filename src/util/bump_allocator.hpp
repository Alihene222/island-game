#pragma once

#include "std.hpp"
#include "util.hpp"

struct BumpAllocator {
    struct Block {
	BumpAllocator *parent;
	usize size;
	u8 *mem;
	u8 *cur;
	u8 *end;
	Block *next;

	Block(const Block &other) = delete;

	Block(Block &&other) {
	    *this = std::move(other);
	}
	Block &operator=(const Block &other) = delete;

	Block &operator=(Block &&other) {
	    this->mem = other.mem;
	    this->cur = other.cur;
	    this->end = other.end;
	    other.mem = nullptr;
	    other.next = nullptr;
	    return *this;
	}

	Block(BumpAllocator *parent, usize size)
	    : parent(parent), size(size) {
	    this->mem = reinterpret_cast<u8*>(std::malloc(size));
	    this->cur = this->mem;
	    this->end = this->mem + size;
	    this->next = nullptr;
	}

	~Block() {
	    if(this->mem) {
		std::free(this->mem);
	    }

	    if(next) {
		delete next;
	    }
	}

	inline void *alloc(usize n) {
	    usize align = 
		reinterpret_cast<usize>(this->cur) % 16 != 0 ?
		    16 - (reinterpret_cast<usize>(this->cur) % 16) :
		    0;

	    if(this->cur + align + n >= this->end) {
		return nullptr;
	    }

	    void *res = this->cur + align;
	    this->cur += align + n;
	    return res;
	}

	inline void clear() {
	    this->cur = this->mem;
	}
    };

    usize size = 0, block_size = 0, allocated = 0;
    Block *block = nullptr;

    BumpAllocator() = default;

    BumpAllocator(const BumpAllocator &other) = delete;

    BumpAllocator(BumpAllocator &&other) {
	*this = std::move(other);
    }
    BumpAllocator &operator=(const BumpAllocator &other) = delete;

    BumpAllocator &operator=(BumpAllocator &&other) {
	if(other.block) {
	    this->block = other.block;
	    other.block = nullptr;
	}

	this->size = other.size;
	this->block_size = other.block_size;
	return *this;
    }

    explicit BumpAllocator(usize block_size) {
	this->block = nullptr;
	this->size = block_size;
	this->block_size = block_size;
    }

    ~BumpAllocator() {
	if(this->block) {
	    delete this->block;
	}
    }

    inline void *alloc(usize n) {
	this->allocated += n;

	Block *block = this->block, *last = nullptr;
	while(block != nullptr) {
	    void *result = block->alloc(n);

	    if(result) {
		return result;
	    }

	    last = block;
	    block = block->next;
	}

	Block *b;

	if(last) {
	    b = (last->next = new Block(this, this->size));
	} else {
	    b = (this->block = new Block(this, this->size));
	}

	return b->alloc(n);
    }

    template<typename P, typename T = std::remove_pointer_t<P>>
	requires (std::is_pointer_v<P>)
    inline P alloc(usize n) {
	auto *res = this->alloc(n);
	if constexpr (std::is_default_constructible_v<T>) {
	    new (res) T();
	}

	return reinterpret_cast<P>(res);
    }

    inline void clear() {
	if(!this->block) {
	    return;
	}

	usize total = 0;

	Block *block = this->block;
	while(block != nullptr) {
	    total += block->size;
	    block->clear();
	    block = block->next;
	}

	this->size = std::max(this->size, total);
	
	if(this->size > this->block->size) {
	    delete this->block;
	    this->block = new Block(this, this->size);
	}

	this->allocated = 0;
    }
};
