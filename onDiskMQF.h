#ifndef onDiskMQF_H
#define onDiskMQF_H

#include <inttypes.h>
#include <stdbool.h>
#include <pthread.h>
#include <map>
#include <vector>
#include "gqf.h"
#include <fstream>
#include <stxxl/vector>


namespace onDiskMQF_Namespace{
/* Can be
	0 (choose size at run-time),
	8, 16, 32, or 64 (for optimized versions),
	or other integer <= 56 (for compile-time-optimized bit-shifting-based versions)
	*/
	#define SLOTS_PER_BLOCK (64)
	#define METADATA_WORDS_PER_BLOCK 1

	struct __attribute__ ((__packed__)) qfblock;
	typedef struct qfblock qfblock;

	typedef struct diskParameters {
		uint64_t nBlocksOnMemory;
		uint64_t sizeOnMemory;
		uint8_t nBlocksPerPointer;
		uint64_t memoryBufferPos;
		uint64_t nBlocksPerIOBatch;
		uint64_t blocksPointersLen;
	} diskParameters;


//	const uint8_t bitsPerSlot=15;
	template<uint64_t bitsPerSlot=64>
	class onDisk_qfblock{
	public:
		uint8_t offset;
		uint64_t occupieds[METADATA_WORDS_PER_BLOCK];
		uint64_t runends[METADATA_WORDS_PER_BLOCK];
		uint8_t slots[8*bitsPerSlot];
		onDisk_qfblock(){
			offset=0;
			memset(occupieds,0,METADATA_WORDS_PER_BLOCK*sizeof(uint64_t));
			memset(runends,0,METADATA_WORDS_PER_BLOCK*sizeof(uint64_t));
			memset(slots,0,8*bitsPerSlot);
		}
		// onDisk_qfblock(const onDisk_qfblock &obj) noexcept
		// {
		// 	offset=obj.offset;
		// 	memcpy(occupieds, obj.occupieds, METADATA_WORDS_PER_BLOCK*sizeof(uint64_t) );
		// 	memcpy(runends, obj.runends, METADATA_WORDS_PER_BLOCK*sizeof(uint64_t) );
		// 	memcpy(slots,obj.slots,8*bitsPerSlot);
		// }
		// onDisk_qfblock& operator=( onDisk_qfblock const& obj) noexcept {
		// 	this->offset=obj.offset;
		// 	memcpy(this->occupieds, obj.occupieds, METADATA_WORDS_PER_BLOCK*sizeof(uint64_t) );
		// 	memcpy(this->runends, obj.runends, METADATA_WORDS_PER_BLOCK*sizeof(uint64_t) );
		// 	memcpy(this->slots,obj.slots,8*bitsPerSlot);
	 	// 	return *this;
 		// }
	};
	typedef struct onDiskMQF {
		qfmem *mem;
		qfmetadata *metadata;
		stxxl::vector<onDisk_qfblock<> > blocks;
		uint64_t* blocksFilePos;
		uint64_t reverseBlocksPointer;
		std::fstream diskMQFStream;
		qfblock **blocksPointers;
		diskParameters* diskParams;
	} onDiskMQF;

	typedef struct onDiskMQFIterator {
		onDiskMQF *qf;
		uint64_t run;
		uint64_t current;
		uint64_t cur_start_index;
		uint16_t cur_length;
		uint32_t num_clusters;
		cluster_data *c_info;
	} onDiskMQFIterator;




	/*!
	@breif initialize mqf .

	@param Qf* qf : pointer to the Filter.
	@param uint64_t nslots : Number of slots in the filter. Maximum number of items to be inserted depends on this number.
	@param uint64_t key_bits: Number of bits in the hash values. This number should equal log2(nslots) +r. Accuracy depends on r.
	@param uint64_t tag_bits: Number of bits in tag value.
	@param uint64_t fixed_counter_size: Fixed counter size. must be > 0.
	@param bool mem: Flag to create the filter on memeory. IF false, mmap is used.
	@param const char * path: In case of mmap. Path of the file used to pack the filter.
	@param uint32_t seed: useless value. To be removed
		  */

	void onDiskMQF_init(onDiskMQF *qf, uint64_t nslots, uint64_t key_bits, uint64_t tag_bits,uint64_t fixed_counter_size,
							  uint64_t memorySizeinMB,const char * path);
	void onDiskMQF_reset(onDiskMQF*qf);

	void onDiskMQF_destroy(onDiskMQF*qf);

	void onDiskMQF_copy(onDiskMQF*dest, onDiskMQF*src);

	/*!
	 	@breif Increment the counter for this item by count.

		@param Qf* qf : pointer to the Filter
		@param uint64_t key : hash of the item to be insertedItems
		@param uint64_t count: Count to be added
		@param bool lock: For Multithreading, Lock the slot used by the current thread so that other threads can't change the value
		@param bool spin: For Multithreading, If there is a lock on the target slot. wait until the lock is freed and insert the count.

		@return bool: True if the item is inserted correctly.
	 */
	bool onDiskMQF_insert(onDiskMQF*qf, uint64_t key, uint64_t count,
								 bool lock=false, bool spin=false);


	/* Remove all instances of this key/value pair. */
	//void onDiskMQF_delete_key_value(onDiskMQF*qf, uint64_t key, uint64_t value);

	/* Remove all instances of this key. */
	//void onDiskMQF_delete_key(onDiskMQF*qf, uint64_t key);

	/* Replace the association (key, oldvalue, count) with the association
		 (key, newvalue, count). If there is already an association (key,
		 newvalue, count'), then the two associations will be merged and
		 their counters will be summed, resulting in association (key,
		 newvalue, count' + count). */
	void onDiskMQF_replace(onDiskMQF*qf, uint64_t key, uint64_t oldvalue, uint64_t newvalue);

	/* Lookup the value associated with key.  Returns the count of that
		 key/value pair in the QF.  If it returns 0, then, the key is not
		 present in the QF. Only returns the first value associated with key
		 in the QF.  If you want to see others, use an iterator. */
	uint64_t onDiskMQF_query(onDiskMQF*qf, uint64_t key, uint64_t *value);

	/*!
	@breif Return the number of times key has been inserted, with any value,
		 into qf.

	@param Qf* qf : pointer to the Filter.
	@param uint64_t key : hash of the item.

	@return uint64_t the count associated with the input key.
		  */
	uint64_t onDiskMQF_count_key(onDiskMQF*qf, uint64_t key);

	/*!
	@breif Decrement the counter for this item by count.

	@param	Qf* qf : pointer to the Filter
	@param	uint64_t key : hash of the item to be removed
	@param	uint64_t count: Count to be removed
	@param	bool lock: For Multithreading, Lock the slot used by the current thread so that other threads can't change the value
	@param	bool spin: For Multithreading, If there is a lock on the target slot. wait until the lock is freed and insert the count.

	@return bool: Returns true if the item is removed successfully.
	 */
	bool onDiskMQF_remove(onDiskMQF*qf, uint64_t hash, uint64_t count,  bool lock=false, bool spin=false);


	/*!
		@breif Add Tag to item.

		@param Qf* qf : pointer to the Filter
		@param uint64_t key : hash of the item to be insertedItems
		@param uint64_t tag: tag to be added
		@param bool lock: For Multithreading, Lock the slot used by the current thread so that other threads can't change the value
		@param bool spin: For Multithreading, If there is a lock on the target slot. wait until the lock is freed and insert the count.

		@return bool: True if the item is inserted correctly.
	 */
	uint64_t onDiskMQF_add_tag(onDiskMQF*qf, uint64_t key, uint64_t tag, bool lock=false, bool spin=false);
	/*!
	@breif Return the tag associated with a given item.

	@param Qf* qf : pointer to the Filter.
	@param uint64_t key : hash of the item.

	@return uint64_t the tag associated with the input key.
			*/
	uint64_t onDiskMQF_get_tag(onDiskMQF*qf, uint64_t key);
	/*!
	@breif delete the tag associated with a given item.

	@param Qf* qf : pointer to the Filter.
	@param uint64_t key : hash of the item.

	@return bool: Returns true if the item is removed successfully.
			*/
	uint64_t onDiskMQF_remove_tag(onDiskMQF*qf, uint64_t key, bool lock=false, bool spin=false);

	/* Initialize an iterator */
	bool onDiskMQF_iterator(onDiskMQF *qf, onDiskMQFIterator *qfi, uint64_t position);

	/* Returns 0 if the iterator is still valid (i.e. has not reached the
		 end of the QF. */
	int onDiskMQFIterator_get(onDiskMQFIterator *qfi, uint64_t *key, uint64_t *value, uint64_t *count);

	/* Advance to next entry.  Returns whether or not another entry is
		 found.  */
	int onDiskMQFIterator_next(onDiskMQFIterator *qfi);

	/* Check to see if the if the end of the QF */
	int onDiskMQFIterator_end(onDiskMQFIterator *qfi);

	/* For debugging */
	void onDiskMQF_dump(onDiskMQF *);

	/*! write data structure of to the disk */
	void onDiskMQF_serialize(onDiskMQF *qf, const char *filename);

	/* read data structure off the disk */
	void onDiskMQF_deserialize(onDiskMQF *qf, const char *filename);

	/* mmap the QF from disk. */
	void onDiskMQF_read(onDiskMQF*qf, const char *path);

	/* merge two QFs into the third one. */
	void onDiskMQF_merge(onDiskMQF*qfa, onDiskMQF*qfb, onDiskMQF*qfc);

	void onDiskMQF_intersect(onDiskMQF *qfa, onDiskMQF *qfb, onDiskMQF *qfc);

	void onDiskMQF_subtract(onDiskMQF *qfa, onDiskMQF *qfb, onDiskMQF*qfc);
	/* merge multiple QFs into the final QF one. */
	void onDiskMQF_multi_merge(onDiskMQF *qf_arr[], int nqf, onDiskMQF *qfr);


	/*! @breif Invertiable merge function adds tag for each key and creates index structure. The index is map of an integer and vector of integers where the integer is the value of the tags and vector on integers is the ids of the source filters.

	@param Qf* qf_arr : input array of filters
	@param int nqf: number of filters
	@param QF* qfr: pointer to the output filter.
	@param std::map<uint64_t, std::vector<int> > *inverted_index_ptr: Pointer to the output index.
	*/
	void onDiskMQF_invertable_merge(onDiskMQF *qf_arr[], int nqf, onDiskMQF *qfr);
	void onDiskMQF_invertable_merge_no_count(onDiskMQF *qf_arr[], int nqf, onDiskMQF *qfr);


	/*! @breif Resize the filter into a bigger or smaller one

	@param Qf* qf : pointer to the Filter
	@param uint64_t newQ: new number of slots(Q). the slot size will be recalculated to keep the range constant.
	@param string originalFilename(optional): dump the current filter to the disk to free space for the new filter. Filename is provided as the content of the string.
	@param string newFilename(optional): the new filter is created on disk. Filename is provided as the content of the string.

	@return QF: New Quotient Filter.
	*/
	onDiskMQF* onDiskMQF_resize(onDiskMQF* qf, int newQ, const char * originalFilename=NULL, const char * newFilename=NULL);
	/* find cosine similarity between two QFs. */
	uint64_t onDiskMQF_inner_product(onDiskMQF *qfa, onDiskMQF *qfb);

	/* magnitude of a QF. */
	uint64_t onDiskMQF_magnitude(onDiskMQF *qf);
	/* return the filled space(percent) */
	int onDiskMQF_space(onDiskMQF *qf);

	bool onDiskMQF_equals(onDiskMQF *qfa, onDiskMQF *qfb);

	bool onDiskMQF_general_lock(onDiskMQF* qf, bool spin);
	void onDiskMQF_general_unlock(onDiskMQF* qf);

	void onDiskMQF_migrate(onDiskMQF* source, onDiskMQF* destination);
	void onDiskMQF_migrate(QF* source, onDiskMQF* destination);


}
#endif /* onDiskMQF_H */
