# Cache System - Class Diagram

## System Design

```mermaid
classDiagram
direction TB

class EvictionPolicy~K~ {
    <<interface>>
    +keyToEvict() K
    +keyAccessed(K key)
    +removeKey(K key)
}

class LRUEvictionPolicy~K~ {
    -Node head
    -Node tail
    -Map~K,Node~ mapper
    +keyToEvict() K
    +keyAccessed(K key)
    +removeKey(K key)
}

class Node {
    +K key
    +Node prev
    +Node next
}

class CacheStorage~K,V~ {
    <<interface>>
    +put(K key, V value)
    +get(K key)
    +remove(K key)
}

class InMemoryCache~K,V~ {
    -Map~K,V~ storage
    +put(K key, V value)
    +get(K key)
    +remove(K key)
}

class Cache~K,V~ {
    -CacheStorage~K,V~ storage
    -EvictionPolicy~K~ evictionPolicy
    -int capacity
    +put(K key, V value)
    +get(K key)
}

LRUEvictionPolicy ..|> EvictionPolicy
InMemoryCache ..|> CacheStorage
Cache --> CacheStorage
Cache --> EvictionPolicy
LRUEvictionPolicy --> Node
```

## Design Patterns Used
1. Strategy Pattern: For different eviction policies
2. Factory Pattern: Could be used for creating different types of caches
3. Template Method: For implementing cache operations
4. Adapter Pattern: Could be used for adapting different storage systems

## Implementation Steps
1. Define the EvictionPolicy interface with key operations
2. Implement LRUEvictionPolicy with doubly linked list
3. Create CacheStorage interface for storage operations
4. Implement InMemoryCache with HashMap storage
5. Build main Cache class with eviction and storage
6. Add capacity management and eviction logic
7. Implement thread-safety mechanisms
8. Add error handling for edge cases
9. Implement cache statistics tracking
10. Add support for different value types
