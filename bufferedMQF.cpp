#include <inttypes.h>
#include <stdbool.h>
#include <pthread.h>
#include "gqf.h"
#include "bufferedMQF.h"
#include <iostream>

using namespace std;
void bufferedMQF_init(bufferedMQF *qf, uint64_t nslots_buffer ,uint64_t nslots
	, uint64_t key_bits, uint64_t value_bits,uint64_t fixed_counter_size,uint64_t memSize, const char *path){

		if(qf==NULL)
		{
			qf= new bufferedMQF();
		}

		qf_init(qf->memoryBuffer,nslots_buffer,key_bits,value_bits,fixed_counter_size,true,"",2038074761);
		onDiskMQF_init(qf->disk,nslots,key_bits,value_bits,fixed_counter_size,memSize,path);
}

void bufferedMQF_reset(bufferedMQF *qf){
	qf_reset(qf->memoryBuffer);
	onDiskMQF_reset(qf->disk);
}

void bufferedMQF_destroy(bufferedMQF *qf){
	qf_destroy(qf->memoryBuffer);
	onDiskMQF_destroy(qf->disk);
}

void bufferedMQF_copy(bufferedMQF *dest, bufferedMQF *src){
	qf_copy(dest->memoryBuffer,src->memoryBuffer);
	onDiskMQF_copy(dest->disk,src->disk);
}

/* Increment the counter for this key/value pair by count. */

bool bufferedMQF_insert(bufferedMQF *qf, uint64_t key, uint64_t count,
							 bool lock, bool spin){

	qf_insert(qf->memoryBuffer,key,count,lock,spin);
	if(qf_space(qf->memoryBuffer)>90)
	{
		bufferedMQF_syncBuffer(qf);
	}
	return true;



}

/* Remove count instances of this key/value combination. */
bool bufferedMQF_remove(bufferedMQF *qf, uint64_t hash, uint64_t count,  bool lock, bool spin){
	bool res=false;
	res|=qf_remove(qf->memoryBuffer,hash,count,lock,spin);
	res|=onDiskMQF_remove(qf->disk,hash,count,lock,spin);
	return res;
}




/* Return the number of times key has been inserted, with any value,
	 into qf. */
uint64_t bufferedMQF_count_key(const bufferedMQF *qf, uint64_t key){
	return onDiskMQF_count_key(qf->disk,key)+qf_count_key(qf->memoryBuffer,key);
}

int bufferedMQF_space(bufferedMQF *qf){
	uint64_t occupied_slots=qf->disk->metadata->noccupied_slots+qf->memoryBuffer->metadata->noccupied_slots;
	return (int)(((double)occupied_slots/
							 (double)qf->disk->metadata->xnslots
						 )* 100.0);

}
void bufferedMQF_syncBuffer(bufferedMQF *qf){
	onDiskMQF_migrate(qf->memoryBuffer,qf->disk);
	qf_reset(qf->memoryBuffer);
}
//
// /* Initialize an iterator */
// bool bufferedMQF_iterator(bufferedMQF *qf, bufferedMQFIterator* qfi, uint64_t position){
// 	if(qfi ==NULL)
// 		qfi=new bufferedMQFIterator();
// 	bool res=false;
// 	res|=qf_iterator(qf->memoryBuffer,qfi->firstLayerIterator,position);
// 	res|=qf_iterator(qf->disk,qfi->diskIterator,position);
// 	return res;
// }
//
// /* Returns 0 if the iterator is still valid (i.e. has not reached the
// 	 end of the QF. */
// int bufferedMQF_qfi_get(bufferedMQFIterator*qfi, uint64_t *key, uint64_t *value, uint64_t *count){
//
// }
//
// /* Advance to next entry.  Returns whether or not another entry is
// 	 found.  */
// int bufferedMQF_qfi_next(bufferedMQFIterator*qfi);
//
// /* Check to see if the if the end of the QF */
// int bufferedMQF_qfi_end(bufferedMQFIterator*qfi);
//
// /* For debugging */
// void bufferedMQF_dump(const bufferedMQF *);
//
// /* write data structure of to the disk */
// void bufferedMQF_serialize(const bufferedMQF *qf, const char *filename);
//
// /* read data structure off the disk */
// void bufferedMQF_deserialize(bufferedMQF *qf, const char *filename);
//
// /* mmap the QF from disk. */
// void bufferedMQF_read(bufferedMQF *qf, const char *path);

/* merge two QFs into the third one. */
//void bufferedMQF_merge(bufferedMQF *bufferedMQFa, bufferedMQF *bufferedMQFb, QF *qfc);

/* merge multiple QFs into the final QF one. */
//void qf_multi_merge(QF *qf_arr[], int nqf, QF *qfr);

/* find cosine similarity between two QFs. */
//	uint64_t qf_inner_product(QF *qfa, QF *qfb);

/* magnitude of a QF. */
//uint64_t qf_magnitude(QF *qf);
