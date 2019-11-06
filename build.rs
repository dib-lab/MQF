use std::env;
use std::path::PathBuf;

extern crate cmake;
use cmake::Config;

fn main() {
    let dst = Config::new(".")
        .define("BUILD_STATIC_LIBS", "ON")
        .define("SUPRESS_BIN", "ON")
        .define("SUPRESS_TESTS", "ON")
        .build();

    // TODO: there are probably better ways to do this...
    let target = env::var("TARGET").unwrap();
    if target.contains("apple") {
        println!("cargo:rustc-link-lib=dylib=c++");
    } else if target.contains("linux") {
        println!("cargo:rustc-link-lib=dylib=stdc++");
    } else {
        unimplemented!();
    }

    println!("cargo:rustc-link-search=native={}/build/src", dst.display());
    println!("cargo:rustc-link-lib=static=MQF");

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
