use std::env;
use std::path::PathBuf;

extern crate cmake;
use cmake::Config;

fn main() {
    let dst = Config::new(".").define("BUILD_STATIC_LIBS", "ON").build();

    let target = env::var("TARGET").unwrap();
    if target.contains("apple") {
        println!("cargo:rustc-link-lib=dylib=c++");
    } else if target.contains("linux") {
        println!("cargo:rustc-link-lib=dylib=stdc++");
    } else {
        unimplemented!();
    }

    // TODO: static libs are being generated in lib too, but should use
    // something like this instead:
    /*
    println!("cargo:rustc-link-search=native={}/build/src", dst.display());
    println!(
        "cargo:rustc-link-search=native={}/build/ThirdParty/stxxl/lib",
        dst.display()
    );
    */
    println!("cargo:rustc-link-search=native=lib");

    println!("cargo:rustc-link-lib=static=MQF");
    println!("cargo:rustc-link-lib=static=stxxl");

    let bindings = bindgen::Builder::default()
        .clang_arg("-I./include")
        //.clang_arg("-I./ThirdParty/stxxl/include/stxxl")
        //.clang_arg("-I./ThirdParty/stxxl/include")
        .clang_arg("-x")
        .clang_arg("c++")
        .clang_arg("-std=c++11")
        .header("include/gqf.h")
        .whitelist_type("QF")
        .whitelist_function("qf_init")
        .whitelist_function("qf_insert")
        .whitelist_function("qf_count_key")
        .blacklist_type("std::*")
        //.opaque_type("QF")
        .generate()
        .expect("Unable to generate bindings");

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("couldn't write bindings!");
}
