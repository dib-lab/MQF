use std::env;
use std::path::PathBuf;

fn main() {
    cc::Build::new()
        .cpp(true) // Switch to C++ library compilation.
        .flag_if_supported("-std=c++11")
        .include("include/")
        .file("src/gqf.cpp")
        .file("src/utils.cpp")
        .compile("libmqf.a");

    let bindings = bindgen::Builder::default()
        .clang_arg("-I./include")
        .clang_arg("-x")
        .clang_arg("c++")
        .clang_arg("-std=c++11")
        .header("include/gqf.h")
        .whitelist_type("QF")
        .whitelist_type("QFi")
        .whitelist_function("qf_init")
        .whitelist_function("qf_insert")
        .whitelist_function("qf_count_key")
        .whitelist_function("qf_destroy")
        .whitelist_function("qf_copy")
        .whitelist_function("qf_serialize")
        .whitelist_function("qf_deserialize")
        .whitelist_function("qf_migrate")
        .whitelist_function("qf_iterator")
        .whitelist_function("qfi_get")
        .whitelist_function("qfi_next")
        .whitelist_function("qfi_end")
        .blacklist_type("std::*")
        .generate()
        .expect("Unable to generate bindings");

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("couldn't write bindings!");
}
