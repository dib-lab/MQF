#ifndef layeredMQF_H
#define layeredMQF_H

#include <inttypes.h>
#include <stdbool.h>
#include <pthread.h>
#include "gqf.h"
#ifdef __cplusplus
extern "C" {
#endif

	
	typedef struct layeredMQF {
		QF* firstLayer_signletons;
		QF* secondLayer;
	} layeredMQF;

	

	

	

	void layeredMQF_init(layeredMQF *qf, uint64_t nslots_singletons ,uint64_t nslots, uint64_t key_bits, uint64_t value_bits,uint64_t fixed_counter_size, bool mem, const char *path, uint32_t seed);

	void layeredMQF_reset(layeredMQF *qf);

	void layeredMQF_destroy(layeredMQF *qf, bool mem);

	void layeredMQF_copy(layeredMQF *dest, layeredMQF *src);

	/* Increment the counter for this key/value pair by count. */
	bool layeredMQF_insert(layeredMQF *qf, uint64_t key, uint64_t value, uint64_t count,
								 bool lock, bool spin);

	/* Remove count instances of this key/value combination. */
	void layeredMQF_remove(layeredMQF *qf, uint64_t key, uint64_t value, uint64_t count);

	/* Remove all instances of this key/value pair. */
	void layeredMQF_delete_key_value(layeredMQF *qf, uint64_t key, uint64_t value);

	/* Remove all instances of this key. */
	void layeredMQF_delete_key(layeredMQF *qf, uint64_t key);

	/* Replace the association (key, oldvalue, count) with the association
		 (key, newvalue, count). If there is already an association (key,
		 newvalue, count'), then the two associations will be merged and
		 their counters will be summed, resulting in association (key,
		 newvalue, count' + count). */
	void layeredMQF_replace(layeredMQF *qf, uint64_t key, uint64_t oldvalue, uint64_t newvalue);

	/* Lookup the value associated with key.  Returns the count of that
		 key/value pair in the QF.  If it returns 0, then, the key is not
		 present in the QF. Only returns the first value associated with key
		 in the QF.  If you want to see others, use an iterator. */
	uint64_t layeredMQF_query(const layeredMQF *qf, uint64_t key, uint64_t *value);

	/* Return the number of times key has been inserted, with any value,
		 into qf. */
	uint64_t layeredMQF_count_key(const layeredMQF *qf, uint64_t key);

	/* Return the number of times key has been inserted, with the given
		 value, into qf. */
	uint64_t layeredMQF_count_key_value(const layeredMQF *qf, uint64_t key, uint64_t value);

	/* Initialize an iterator */
	bool layeredMQF_iterator(layeredMQF *qf, QFi *qfi, uint64_t position);

	/* Returns 0 if the iterator is still valid (i.e. has not reached the
		 end of the QF. */
	int layeredMQF_qfi_get(QFi *qfi, uint64_t *key, uint64_t *value, uint64_t *count);

	/* Advance to next entry.  Returns whether or not another entry is
		 found.  */
	int layeredMQF_qfi_next(QFi *qfi);

	/* Check to see if the if the end of the QF */
	int layeredMQF_qfi_end(QFi *qfi);

	/* For debugging */
	void layeredMQF_dump(const layeredMQF *);

	/* write data structure of to the disk */
	void layeredMQF_serialize(const layeredMQF *qf, const char *filename);

	/* read data structure off the disk */
	void layeredMQF_deserialize(layeredMQF *qf, const char *filename);

	/* mmap the QF from disk. */
	void layeredMQF_read(layeredMQF *qf, const char *path);

	/* merge two QFs into the third one. */
	//void layeredMQF_merge(layeredMQF *layeredMQFa, layeredMQF *layeredMQFb, QF *qfc);

	/* merge multiple QFs into the final QF one. */
	//void qf_multi_merge(QF *qf_arr[], int nqf, QF *qfr);

	/* find cosine similarity between two QFs. */
//	uint64_t qf_inner_product(QF *qfa, QF *qfb);

	/* magnitude of a QF. */
	//uint64_t qf_magnitude(QF *qf);

#ifdef __cplusplus
}
#endif

#endif /* layeredMQF_H */
