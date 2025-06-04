use std::{ffi, path::Path, str::FromStr};

unsafe extern "C" {
    fn qbeEmit(input: *const ffi::c_char, output: *const ffi::c_char);
}

pub fn emit<A: AsRef<Path>, B: AsRef<Path>>(input: A, output: B) {
    let input = input.as_ref().to_str().expect("input path is invalid");
    let output = output.as_ref().to_str().expect("output path is invalid");

    let input = ffi::CString::from_str(input).unwrap();
    let output = ffi::CString::from_str(output).unwrap();

    unsafe {
        qbeEmit(input.as_ptr(), output.as_ptr());
    }
}

