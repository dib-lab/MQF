#ifndef bufferedMQF_H
#define bufferedMQF_H

#include <inttypes.h>
#include <stdbool.h>
#include <pthread.h>
#include "gqf.h"
#include "onDiskMQF.h"
#ifdef __cplusplus
extern "C" {
#endif


typedef class bufferedMQFIterator {
public:
    onDiskMQF_Namespace::onDiskMQFIterator* diskIt;
	QFi* bufferIt;
	uint64_t currentKey;
	uint64_t currentLabel;
	uint64_t currentCount;
	bool finished;
    bufferedMQFIterator(){
    	diskIt=new onDiskMQF_Namespace::onDiskMQFIterator();
    	bufferIt=new QFi();
    };
	bufferedMQFIterator(QFi* bit,onDiskMQF_Namespace::onDiskMQFIterator* dit);


	int get(uint64_t *key, uint64_t *value, uint64_t *count);

	/* Advance to next entry.  Returns whether or not another entry is
         found.  */
	int next();

	/* Check to see if the if the end of the QF */
	int end();

	void update();
	~bufferedMQFIterator(){
		delete diskIt;
		delete bufferIt;
	}


} bufferedMQFIterator;


typedef class bufferedMQF {
	public:
		QF* memoryBuffer;
		onDiskMQF_Namespace::onDiskMQF* disk;
		string filename;
		bufferedMQF(){
			memoryBuffer=new QF();
			//disk=new onDiskMQF_Namespace::onDiskMQF();
		}
		~bufferedMQF()
		{
		    if(memoryBuffer!=NULL)
            {
                qf_destroy(memoryBuffer);
                delete memoryBuffer;
            }

			delete disk;
		}
	} bufferedMQF;

    void bufferedMQF_deleteMemoryBuffer(bufferedMQF *qf);
	void bufferedMQF_init(bufferedMQF *qf, uint64_t nslots_buffer ,uint64_t nslots, uint64_t key_bits, uint64_t value_bits,uint64_t fixed_counter_size,const char *path);

	void bufferedMQF_reset(bufferedMQF *qf);

	void bufferedMQF_destroy(bufferedMQF *qf);

	void bufferedMQF_copy(bufferedMQF *dest, bufferedMQF *src);
	void bufferedMQF_syncBuffer(bufferedMQF *qf);

	/* Increment the counter for this key/value pair by count. */
	bool bufferedMQF_insert(bufferedMQF *qf, uint64_t key, uint64_t count,
								 bool lock, bool spin);

	/* Remove count instances of this key/value combination. */
	bool bufferedMQF_remove(bufferedMQF *qf, uint64_t hash, uint64_t count,  bool lock=false, bool spin=false);


		/*!
			@breif Add label to item.

			@param Qf* qf : pointer to the Filter
			@param uint64_t key : hash of the item to be insertedItems
			@param uint64_t label: label to be added
			@param bool lock: For Multithreading, Lock the slot used by the current thread so that other threads can't change the value
			@param bool spin: For Multithreading, If there is a lock on the target slot. wait until the lock is freed and insert the count.

			@return bool: True if the item is inserted correctly.
		 */
		uint64_t bufferedMQF_add_label(const QF *qf, uint64_t key, uint64_t label, bool lock=false, bool spin=false);
		/*!
		@breif Return the label associated with a given item.

		@param Qf* qf : pointer to the Filter.
		@param uint64_t key : hash of the item.

		@return uint64_t the label associated with the input key.
				*/
		uint64_t bufferedMQF_get_label(const QF *qf, uint64_t key);
		/*!
		@breif delete the label associated with a given item.

		@param Qf* qf : pointer to the Filter.
		@param uint64_t key : hash of the item.

		@return bool: Returns true if the item is removed successfully.
				*/
		uint64_t bufferedMQF_remove_label(const QF *qf, uint64_t key, bool lock=false, bool spin=false);



	/* Return the number of times key has been inserted, with any value,
		 into qf. */
	uint64_t bufferedMQF_count_key(const bufferedMQF *qf, uint64_t key);


	void bufferedMQF_BatchQuery( bufferedMQF* qf,QF* input);


	/* For debugging */
	void bufferedMQF_dump(const bufferedMQF *);

	/* write data structure of to the disk */
	void bufferedMQF_serialize(bufferedMQF *qf);

	/* read data structure off the disk */
	void bufferedMQF_deserialize(bufferedMQF *qf, const char *filename);

	/* mmap the QF from disk. */
	void bufferedMQF_read(bufferedMQF *qf, const char *path);

	/* merge two QFs into the third one. */
	//void bufferedMQF_merge(bufferedMQF *bufferedMQFa, bufferedMQF *bufferedMQFb, QF *qfc);

	/* merge multiple QFs into the final QF one. */
	//void qf_multi_merge(QF *qf_arr[], int nqf, QF *qfr);

	/* find cosine similarity between two QFs. */
//	uint64_t qf_inner_product(QF *qfa, QF *qfb);

	/* magnitude of a QF. */
	//uint64_t qf_magnitude(QF *qf);

	int bufferedMQF_space(bufferedMQF *qf);

	void bufferedMQF_migrate(bufferedMQF* source, bufferedMQF* destination);

    bool bufferedMQF_find(bufferedMQF*,bufferedMQFIterator *qfi, uint64_t key);

    bufferedMQFIterator*  bufferedMQF_iterator(bufferedMQF *qf, uint64_t position);
#ifdef __cplusplus
}
#endif

#endif /* bufferedMQF_H */
