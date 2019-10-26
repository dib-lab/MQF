#include <inttypes.h>
#include <stdbool.h>
#include <pthread.h>
#include "gqf.h"
#include "bufferedMQF.h"
#include <iostream>

using namespace std;


bufferedMQFIterator::bufferedMQFIterator(QFi* bit,onDiskMQF_Namespace::onDiskMQFIterator* dit){
    bufferIt=bit;
    diskIt=dit;
    next();
}

int bufferedMQFIterator::get(uint64_t *key, uint64_t *value, uint64_t *count){
    *key=currentKey;
    *value=currentLabel;
    *count=currentCount;


}


/* Advance to next entry.  Returns whether or not another entry is
     found.  */
int bufferedMQFIterator::next()
{
    if(end()) return 1;
    if(diskIt->end())
    {
        qfi_get(bufferIt,&currentKey,&currentLabel,&currentCount);
        qfi_next(bufferIt);
        return 0;
    }

    if(qfi_end(bufferIt))
    {
        diskIt->get(&currentKey,&currentLabel,&currentCount);
        diskIt->next();
        return 0;
    }
    uint64_t diskKey, diskLabel, diskCount, memoryKey, memoryLabel, memoryCount;
    diskIt->get(&diskKey,&diskLabel,&diskCount);
    qfi_get(bufferIt,&memoryKey,&memoryLabel,&memoryCount);
    if(diskKey < memoryKey)
    {
		currentKey=diskKey;
		currentLabel=diskLabel;
		currentCount=diskCount;
		diskIt->next();
        return 0;
    }
    else if(diskKey > memoryKey){
        currentKey=memoryKey;
        currentLabel=memoryLabel;
        currentCount=memoryCount;
        qfi_next(bufferIt);
        return 0;
    } else{
        currentKey=diskKey;
        currentLabel=diskLabel;
        currentCount=diskCount+memoryCount;

        qfi_next(bufferIt);
        diskIt->next();
        return 0;
    }

}

/* Check to see if the if the end of the QF */
int bufferedMQFIterator::end(){
    if(diskIt->end() && qfi_end(bufferIt) ) return 1;
    return 0;
}


void bufferedMQF_init(bufferedMQF *qf, uint64_t nslots_buffer ,uint64_t nslots
	, uint64_t key_bits, uint64_t value_bits,uint64_t fixed_counter_size, const char *path){

		if(qf==NULL)
		{
			qf= new bufferedMQF();
		}

		qf_init(qf->memoryBuffer,nslots_buffer,key_bits,value_bits,fixed_counter_size,0,true,"",2038074761);

		onDiskMQF_Namespace::onDiskMQF::init(qf->disk,nslots,key_bits,value_bits,fixed_counter_size,path);
}

void bufferedMQF_reset(bufferedMQF *qf){
	qf_reset(qf->memoryBuffer);
	//onDiskMQF_reset(qf->disk);
	qf->disk->reset();
}

void bufferedMQF_destroy(bufferedMQF *qf){
//	qf_destroy(qf->memoryBuffer);
//	delete qf->disk;
}

void bufferedMQF_copy(bufferedMQF *dest, bufferedMQF *src){
	qf_copy(dest->memoryBuffer,src->memoryBuffer);
	src->disk->copy(dest->disk);
}

/* Increment the counter for this key/value pair by count. */

bool bufferedMQF_insert(bufferedMQF *qf, uint64_t key, uint64_t count,
							 bool lock, bool spin){
  key=key%qf->memoryBuffer->metadata->range;
    try {
        qf_insert(qf->memoryBuffer, key, count, lock, spin);
    }
    catch (exception& e)
    {
        bufferedMQF_syncBuffer(qf);
        qf_insert(qf->memoryBuffer, key, count, lock, spin);
    }
	if(qf_space(qf->memoryBuffer)>80)
	{
		bufferedMQF_syncBuffer(qf);
	}
	return true;



}

/* Remove count instances of this key/value combination. */
bool bufferedMQF_remove(bufferedMQF *qf, uint64_t hash, uint64_t count,  bool lock, bool spin){
	bool res=false;
	res|=qf_remove(qf->memoryBuffer,hash,count,lock,spin);
	res|=qf->disk->remove(hash,count,lock,spin);
	return res;
}




/* Return the number of times key has been inserted, with any value,
	 into qf. */
uint64_t bufferedMQF_count_key(const bufferedMQF *qf, uint64_t key){
	return qf->disk->count_key(key)+qf_count_key(qf->memoryBuffer,key);
}

int bufferedMQF_space(bufferedMQF *qf){
	uint64_t occupied_slots=qf->disk->metadata->noccupied_slots+qf->memoryBuffer->metadata->noccupied_slots;
	return (int)(((double)occupied_slots/
							 (double)qf->disk->metadata->xnslots
						 )* 100.0);

}
void bufferedMQF_syncBuffer(bufferedMQF *qf){
	qf->disk->migrateFromQF(qf->memoryBuffer);
	qf_reset(qf->memoryBuffer);
}

void bufferedMQF_BatchQuery( bufferedMQF* qf,QF* Batch){
	QFi source_i;
	if (qf_iterator(Batch, &source_i, 0)) {
		do {
			uint64_t key = 0, value = 0, count = 0;
			qfi_get(&source_i, &key, &value, &count);
			uint64_t diskCount=qf->disk->count_key(key);
			uint64_t memCount=qf_count_key(qf->memoryBuffer,key);
			qf_setCounter(Batch,key,diskCount+memCount);
		} while (!qfi_next(&source_i));
	}
}


//
// /* Initialize an iterator */
bufferedMQFIterator* bufferedMQF_iterator(bufferedMQF *qf, uint64_t position){
// 	if(qfi !=NULL)
// 		delete qfi;
 	bool res=false;
 	onDiskMQF_Namespace::onDiskMQFIterator* dit=new onDiskMQF_Namespace::onDiskMQFIterator();
 	QFi* bit=new QFi();

 	res|=qf_iterator(qf->memoryBuffer,bit,position);
 	res|=qf->disk->getIterator(dit,position);
	return new bufferedMQFIterator(bit,dit);

 }

void bufferedMQF_migrate(bufferedMQF* source, bufferedMQF* dest){
    bufferedMQFIterator* source_i=bufferedMQF_iterator(source,0);
    do {
        uint64_t key = 0, value = 0, count = 0;
        source_i->get(&key, &value, &count);
        try {
            bufferedMQF_insert(dest, key, count, false, false);
        }
        catch(exception& e)
        {
            cout << e.what() << '\n';
        }


        uint64_t  tmpcount=bufferedMQF_count_key(dest,key);
        tmpcount++;

//        bufferedMQF_add_label((const bufferedMQF*)dest,key,value);
    } while (!source_i->next());
    delete source_i;
}
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
