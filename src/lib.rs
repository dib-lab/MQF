mod raw;

use std::ffi::CString;
use std::ptr;

pub struct MQF {
    inner: raw::QF,
}

impl Default for MQF {
    fn default() -> MQF {
        MQF {
            inner: raw::QF {
                mem: ptr::null_mut(),
                metadata: ptr::null_mut(),
                blocks: ptr::null_mut(),
            },
        }
    }
}

impl Drop for MQF {
    fn drop(&mut self) {
        unsafe { raw::qf_destroy(&mut self.inner) };
    }
}

impl Clone for MQF {
    fn clone(&self) -> Self {
        let mut new_qf = MQF::default();
        unsafe {
            raw::qf_copy(&mut new_qf.inner, &self.inner);
        };
        new_qf
    }
}

impl MQF {
    pub fn new(counter_size: u64, qbits: u64) -> MQF {
        let mut mqf = MQF::default();

        let num_hash_bits = qbits + 8;
        let maximum_count = (1u64 << counter_size) - 1;

        let s = CString::new("").unwrap();

        unsafe {
            raw::qf_init(
                &mut mqf.inner,
                1u64 << qbits, // nslots
                num_hash_bits, // key_bits
                0,             // label_bits
                counter_size,  // fixed_counter_size
                0,             // blocksLabelSize
                true,          // mem
                s.as_ptr(),    // path
                0,             // seed (doesn't matter)
            );
        };

        mqf
    }

    pub fn insert(&mut self, key: u64, count: u64) {
        unsafe { raw::qf_insert(&mut self.inner, key, count, false, false) };
    }

    pub fn count_key(&self, key: u64) -> u64 {
        unsafe { raw::qf_count_key(&self.inner, key) }
    }

    pub fn iter(&mut self) -> MQFIter {
        let mut cfi = raw::QFi {
            qf: ptr::null_mut(),
            run: 0,
            current: 0,
            cur_start_index: 0,
            cur_length: 0,
            num_clusters: 0,
            c_info: ptr::null_mut(),
        };

        // TODO: treat false
        let result = unsafe { raw::qf_iterator(&mut self.inner, &mut cfi, 0) };

        MQFIter { inner: cfi }
    }
}

pub struct MQFIter {
    inner: raw::QFi,
}

impl Iterator for MQFIter {
    type Item = (u64, u64, u64);

    fn next(&mut self) -> Option<Self::Item> {
        if unsafe { raw::qfi_end(&mut self.inner) } != 0 {
            None
        } else {
            let mut key = 0;
            let mut value = 0;
            let mut count = 0;

            unsafe {
                raw::qfi_get(&mut self.inner, &mut key, &mut value, &mut count);
                raw::qfi_next(&mut self.inner)
            };
            Some((key, value, count))
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn simple_counting_test_api() {
        //except first item is inserted 5 times to full test _insert1
        let counter_size = 2;
        let qbits = 5;
        let mut qf: MQF = MQF::new(counter_size, qbits);

        let mut count = 0;
        let mut fixed_counter = 0;

        for i in 0..=10 {
            qf.insert(100, 1);
            count = qf.count_key(100);
            dbg!((count, fixed_counter));
            assert_eq!(count, 1 + i);
        }

        qf.insert(1500, 50);

        count = qf.count_key(1500);
        dbg!((count, fixed_counter));
        assert_eq!(count, 50);

        qf.insert(1600, 60);
        count = qf.count_key(1600);
        dbg!((count, fixed_counter));
        assert_eq!(count, 60);

        qf.insert(2000, 4000);
        count = qf.count_key(2000);
        dbg!((count, fixed_counter));
        assert_eq!(count, 4000);
    }

    #[test]
    fn big_count() {
        let mut qf = MQF::new(4, 5);
        qf.insert(100, 100000);
        let mut count = qf.count_key(100);
        assert_eq!(count, 100000);
    }

    #[test]
    fn iter_next() {
        let mut qf = MQF::new(4, 5);
        qf.insert(100, 100000);
        qf.insert(101, 10000);
        qf.insert(102, 1000);
        qf.insert(103, 100);

        let vals: Vec<(u64, u64, u64)> = qf.iter().collect();
        dbg!(&vals);
        assert_eq!(vals.len(), 4);
    }
}
