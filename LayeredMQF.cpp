#include <inttypes.h>
#include <stdbool.h>
#include <pthread.h>
#include "gqf.h"
#include "LayeredMQF.h"

void layeredMQF_init(layeredMQF *qf, uint64_t nslots_singletons ,uint64_t nslots, uint64_t key_bits, uint64_t value_bits,uint64_t fixed_counter_size, bool mem, const char *path, uint32_t seed){
{
		if(qf==NULL)
		{
			qf= new layeredMQF();
		}
		if(qf->firstLayer_signletons==NULL)
		{
			qf->firstLayer_signletons=new QF();
		}
		if(qf->secondLayer==NULL)
		{
			qf->secondLayer=new QF();
		}
		qf_init(qf->firstLayer_signletons,nslots_singletons,key_bits,value_bits,1,mem,path,seed);
		qf_init(qf->secondLayer,nslots,key_bits,value_bits,fixed_counter_size,mem,path,seed);
}

void layeredMQF_reset(layeredMQF *qf){
	qf_reset(qf->firstLayer_signletons);
	qf_reset(qf->secondLayer);
}

void layeredMQF_destroy(layeredMQF *qf, bool mem){
	qf_destroy(qf->firstLayer_signletons,mem);
	qf_destroy(qf->secondLayer,mem);
}

void layeredMQF_copy(layeredMQF *dest, layeredMQF *src){
	qf_copy(dest->firstLayer_signletons,src->firstLayer_signletons);
	qf_copy(dest->secondLayer,src->secondLayer);
}

/* Increment the counter for this key/value pair by count. */
bool layeredMQF_insert(layeredMQF *qf, uint64_t key, uint64_t value, uint64_t count,
							 bool lock, bool spin){
	if()

}

/* Remove count instances of this key/value combination. */
void layeredMQF_remove(layeredMQF *qf, uint64_t key, uint64_t value, uint64_t count){
	qf_remove(qf->firstLayer_signletons,key,value)
}





/* Return the number of times key has been inserted, with any value,
	 into qf. */
uint64_t layeredMQF_count_key(const layeredMQF *qf, uint64_t key){
	uint64_t res=qf_count_key(qf->secondLayer,key);
	if(res==0)
	{
		res=qf_count_key(qf->firstLayer_signletons,key);
	}
	return res;
}


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
