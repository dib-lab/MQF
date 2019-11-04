mod raw;

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
