#ifndef ARENA_H
#define ARENA_H

#ifndef ALIGNMENT
#define ALIGNMENT 8
#endif

#include <vector>
#include <memory>
#include <new>
#include <utility>
#include <cstdint>

namespace memory {

    struct ArenaBlock {
        void* mem;
        void* startRef;
        uint32_t used;
        uint32_t cap;
        ArenaBlock* next;
    };

    class Arena {
        friend class PoolAllocator;
    public:
        ~Arena();
        void blockRelease(ArenaBlock *b);
        ArenaBlock *newArenaBlock(uint64_t size, uint64_t align);
        void clear();
    private:
        void freeBlocks();
        uint32_t blockCap;
        uint32_t inUse;
        ArenaBlock* head;
        ArenaBlock* tail;
    };

    class PoolAllocator {
        public:
        explicit PoolAllocator(uint32_t blockCap);

        PoolAllocator(const PoolAllocator&) = delete;
        PoolAllocator& operator=(const PoolAllocator&) = delete;
        PoolAllocator(PoolAllocator&&) = delete;
        PoolAllocator& operator=(PoolAllocator&&) = delete;

        ~PoolAllocator();
        void memoryDump();

        template<typename T>
        T* alloc(std::size_t count = 1){
            return static_cast<T*>(allocRaw(sizeof(T) * count, alignof(T)));
        }

        void* allocRaw(std::size_t size, uint32_t alignment);
        private:
        Arena* arena;

        void* alignPointer(void* ptr, uint32_t align);
        uintptr_t alignAddr(uintptr_t ptr, uint32_t align);
    };

    template<typename T>
    class STLAllocator {
    public:
        using value_type = T;

        STLAllocator() = delete;

        explicit STLAllocator(PoolAllocator& arena) noexcept : m_arena(&arena) {}

        template<typename U>
        STLAllocator(const STLAllocator<U>& other) noexcept : m_arena(other.m_arena) {}

        T* allocate(std::size_t n) {
            return static_cast<T*>(m_arena->allocRaw(sizeof(T) * n, alignof(T)));
        }

        void deallocate(T*, std::size_t) noexcept {
            //nop
        }

        template<typename U>
        bool operator==(const STLAllocator<U>& other) const noexcept {
            return m_arena == other.m_arena;
        }

        template<typename U>
        bool operator!=(const STLAllocator<U>& other) const noexcept {
            return !(*this == other);
        }

        PoolAllocator* getArena() const { return m_arena; }

    private:
        PoolAllocator* m_arena;
        template<typename U> friend class STLAllocator;
    };

    template<typename T>
    struct ArenaDeleter {
        void operator()(T* ptr) const {
            if (ptr) {
                ptr->~T(); 
            }
        }
    };

    template<typename T, typename... Args>
    std::unique_ptr<T, ArenaDeleter<T>> make_arena_unique(PoolAllocator& arena, Args&&... args) {
        T* memory = arena.alloc<T>();
        new (memory) T(std::forward<Args>(args)...);
        return std::unique_ptr<T, ArenaDeleter<T>>(memory);

    }

} // namespace memory

#endif
