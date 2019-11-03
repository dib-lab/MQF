#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

#[cfg(test)]
mod tests {
    use super::*;
    use std::ffi::CString;
    use std::ptr;

    #[test]
    fn simple_counting_test() {
        //except first item is inserted 5 times to full test _insert1
        let mut qf: QF = QF {
            mem: ptr::null_mut(),
            metadata: ptr::null_mut(),
            blocks: ptr::null_mut(),
        };

        let counter_size = 2;
        let qbits = 5;
        let num_hash_bits = qbits + 8;
        let maximum_count = (1u64 << counter_size) - 1;
        let mut count = 0;
        let mut fixed_counter = 0;

        let s = CString::new("").unwrap();

        //INFO("Counter size = "<<counter_size<<" max count= "<<maximum_count);
        unsafe {
            qf_init(
                &mut qf,
                1u64 << qbits,
                num_hash_bits,
                0,
                counter_size,
                0,
                true,
                s.as_ptr(),
                2038074761,
            );
        }

        /*
        for i in 0..=10 {
            qf_insert(&qf, 100, 1, false, false);
            count = qf_count_key(&qf, 100);
            //fixed_counter=qf_get_fixed_counter(&qf,100);
            dbg!((count, fixed_counter));
            assert_eq!(count, 1 + i);
        }

        qf_insert(&qf,1500,50,false,false);

        count = qf_count_key(&qf, 1500);
        //  fixed_counter=qf_get_fixed_counter(&qf,1500);
        INFO("Counter = "<<count<<" fixed counter = "<<fixed_counter)
        CHECK(count == (50));

        qf_insert(&qf,1600,60,false,false);
        count = qf_count_key(&qf, 1600);
        //  fixed_counter=qf_get_fixed_counter(&qf,1600);
        INFO("Counter = "<<count<<" fixed counter = "<<fixed_counter)
        CHECK(count == (60));



        qf_insert(&qf,2000,4000,false,false);
        count = qf_count_key(&qf, 2000);
        //  fixed_counter=qf_get_fixed_counter(&qf,2000);
        INFO("Counter = "<<count<<" fixed counter = "<<fixed_counter)
        CHECK(count == (4000));
        */
    }
}
