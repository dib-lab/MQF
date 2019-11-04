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
}
