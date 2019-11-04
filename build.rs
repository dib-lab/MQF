use std::env;
use std::path::PathBuf;

extern crate cmake;
use cmake::Config;

fn main() {
    let dst = Config::new(".").define("BUILD_STATIC_LIBS", "ON").build();

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
    println!(
        "cargo:rustc-link-search=native={}/build/ThirdParty/stxxl/lib",
        dst.display()
    );
    // TODO: static libs are being generated in lib too,
    // cmake seems to be just copying it from the right locations.
    // But not sure we should be using them...
    // println!("cargo:rustc-link-search=native=lib");

    println!("cargo:rustc-link-lib=static=MQF");
    // TODO: there are two names for stxxl, depending on being built on release
    // or debug mode...
    //println!("cargo:rustc-link-lib=static=stxxl");
    println!("cargo:rustc-link-lib=static=stxxl_debug");

    let bindings = bindgen::Builder::default()
        .clang_arg("-I./include")
        .clang_arg("-x")
        .clang_arg("c++")
        .clang_arg("-std=c++11")
        .header("include/gqf.h")
        .whitelist_type("QF")
        .whitelist_function("qf_init")
        .whitelist_function("qf_insert")
        .whitelist_function("qf_count_key")
        .whitelist_function("qf_destroy")
        .whitelist_function("qf_copy")
        .blacklist_type("std::*")
        .generate()
        .expect("Unable to generate bindings");

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("couldn't write bindings!");
}
