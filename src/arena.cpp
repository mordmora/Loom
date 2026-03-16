#include "arena.h"
#include <memory>
#include <iostream>
#include <cassert>
#include <cstdint>

namespace memory
{

    PoolAllocator::PoolAllocator(uint32_t size)
    {
        arena = new Arena;
        arena->tail = nullptr;
        arena->blockCap = size;
        arena->inUse = 0;
        arena->head = arena->newArenaBlock(size, ALIGNMENT);
    }

    ArenaBlock *Arena::newArenaBlock(uint64_t size, uint64_t align)
    {
        void *rawMem = ::operator new(size + sizeof(ArenaBlock) + align);
        ArenaBlock *b = static_cast<ArenaBlock *>(rawMem);
        b->cap = size;
        uintptr_t baseAddr = reinterpret_cast<uintptr_t>(rawMem) + sizeof(ArenaBlock);
        b->mem = reinterpret_cast<void *>((baseAddr + (align - 1)) & ~(align - 1));
        b->startRef = rawMem;
        b->used = 0;
        b->next = nullptr;
        return b;
    }

    uintptr_t PoolAllocator::alignAddr(uintptr_t ptr, uint32_t align)
    {
        const uint64_t mask = align - 1;
        assert((mask & align) == 0);
        return (ptr + mask) & ~mask;
    }

    void *PoolAllocator::alignPointer(void *ptr, uint32_t align)
    {
        const uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
        const uintptr_t addrAligned = alignAddr(addr, align);
        return reinterpret_cast<void *>(addrAligned);
    }

    PoolAllocator::~PoolAllocator()
    {
        delete arena;
    }

    Arena::~Arena()
    {
        freeBlocks();
    }

    void Arena::freeBlocks()
    {
        blockRelease(head);
        ArenaBlock *b = tail;
        ArenaBlock *n = nullptr;
        while (b)
        {
            n = b->next;
            blockRelease(b);
            b = n;
        }
    }

    void Arena::blockRelease(ArenaBlock *b)
    {
        if (b)
        {
            ::operator delete(b->startRef);
        }
    }

    void *PoolAllocator::allocRaw(std::size_t size, uint32_t align)
    {

        uintptr_t baseAddr = reinterpret_cast<uintptr_t>(arena->head->mem);
        uintptr_t aligned = alignAddr(baseAddr, align);
        uint32_t pad = static_cast<uint32_t>(aligned - baseAddr);

        uint32_t worstCaseSize = static_cast<uint32_t>(size) + pad;

        if (worstCaseSize > arena->blockCap)
        {
            ArenaBlock *b = arena->newArenaBlock(worstCaseSize, align);
            b->next = arena->tail;
            arena->tail = b;
            b->used = worstCaseSize;
            arena->inUse += worstCaseSize;
            return alignPointer(b->mem, align);
        }
        else
        {
            ArenaBlock *b = arena->head;

            if (b->used + worstCaseSize > b->cap)
            {
                // Si una reserva menor que la capacidad por bloque supera el tamaño,
                // es probable que quede muy poco espacio y no se use nunca más, por lo que
                // se crea un bloque nuevo para mantener simple el sistema.
                ArenaBlock *new_b = arena->newArenaBlock(arena->blockCap, align);
                b->next = arena->tail;
                arena->tail = b;
                arena->head = new_b;
                b = new_b;

                baseAddr = reinterpret_cast<uintptr_t>(b->mem);
                aligned = alignAddr(baseAddr, align);
                pad = static_cast<uint32_t>(aligned - baseAddr);
                worstCaseSize = static_cast<uint32_t>(size) + pad;
            }

            void *mem = b->mem;
            b->used += worstCaseSize;
            b->mem = ((char *)b->mem) + worstCaseSize;
            arena->inUse += worstCaseSize;
            return alignPointer(mem, align);
        }
    }

    void Arena::clear()
    {
        freeBlocks();
        head = newArenaBlock(blockCap, ALIGNMENT);
        tail = nullptr;
        inUse = 0;
    }

    void PoolAllocator::memoryDump()
    {
        std::cout << "Arena dump: " << arena->inUse << " bytes used.\n";
    }
}
