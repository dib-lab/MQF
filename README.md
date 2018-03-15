# MQF
[![Build Status](https://travis-ci.org/shokrof/MQF.svg?branch=mqfDevelopmenet)](https://travis-ci.org/shokrof/MQF)
[![codecov](https://codecov.io/gh/shokrof/MQF/branch/mqfDevelopmenet/graph/badge.svg)](https://codecov.io/gh/shokrof/MQF)

MQF, Mixed Quotient Filter, is approximate membership query data structure that supports many useful functions. MQF is a variant of [CQF](https://github.com/splatlab/cqf). MQF has lower bits per element than Bloom filter and Count-min sketches. MQF also has good data locality which makes it efficient when running on secondary storage. Moreover. It supports removing, iterating, merging ,and resizing.

## Documentation
### Initialization

### Functions Supported
1. Insert :
Increment the counter for this item by count.
  ```c++
  bool qf_insert(QF *qf, uint64_t key,
     uint64_t count,bool lock, bool spin);
  ```

  * Qf* qf : pointer to the Filter
  * uint64_t key : hash of the item to be inserted.
  * uint64_t count: Count to be added
  * bool lock: For Multithreading, Lock the * slot used by the current thread so that other threads can't change the value
  * bool spin: For Multithreading, If there is a lock on the target slot. wait until the lock is freed and insert the count.
  * returns True if the insertion succeeded.

2. Count:
 Return the number of times key has been inserted, with any value, into qf.
 ```c++
 uint64_t qf_count_key(const QF *qf, uint64_t key);
 ```
 * Qf* qf : pointer to the Filter
 * uint64_t key : hash of the item to be counted.
 * returns the number of times the item is inserted.
3. Remove:
Decrement the counter for this item by count.
```c++
bool qf_remove(QF *qf, uint64_t hash, uint64_t count, bool lock, bool spin);
```
  * Qf* qf : pointer to the Filter
  * uint64_t key : hash of the item to be removed
  * uint64_t count: Count to be removed
  * bool lock: For Multithreading, Lock the slot used by the current thread so that other threads can't change the value
  * bool spin: For Multithreading, If there is a lock on the target slot. wait until the lock is freed and insert the count.

4. Add/Remove tag to elements
```c++
uint64_t qf_add_tag(const QF *qf, uint64_t key, uint64_t tag, bool lock, bool spin);
uint64_t qf_get_tag(const QF *qf, uint64_t key);
uint64_t qf_remove_tag(const QF *qf, uint64_t key, bool lock, bool spin);
```
* Qf* qf : pointer to the Filter
* uint64_t key : hash of the item.
* uint64_t tag: tag for the item.
* bool lock: For Multithreading, Lock the slot used by the current thread so that other threads can't change the value
* bool spin: For Multithreading, If there is a lock on the target slot. wait until the lock is freed and insert the count.

5. Resize
6. Merge
7. Intersect
8. Compare
9. Unique

### Miscellaneous Functions
1. Capacity
2. Copy
3. Serialize/ Deserialize
4. MMap read
