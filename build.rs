use std::fs;

use build_target::{Arch, Os};
use tempdir::TempDir;

fn main() {
    let config_dir = TempDir::new("config_dir").unwrap();
    let config_file = config_dir.path().join("config.h");

    // TODO: test whether all of these work
    let deftgt = match build_target::target_os().unwrap() {
        Os::MacOs => match build_target::target_arch().unwrap() {
            Arch::AARCH64 => "T_arm64_apple",
            Arch::X86_64 => "T_amd64_apple",
            arch => panic!("macos arch {arch} unsupported"),
        },
        _ => match build_target::target_arch().unwrap() {
            Arch::AARCH64 => "T_arm64",
            Arch::RISCV => "T_rv64",
            // i doubt this is correct
            Arch::X86_64 => "T_amd64_sysv",
            arch => panic!("arch {arch} unsupported"),
        },
    };

    let config_def = format!("#define Deftgt {deftgt}");
    fs::write(&config_file, config_def).unwrap();

    cc::Build::new()
        .debug(false)
        .opt_level(3)
        .files(&[
            "qbe/abi.c",
            "qbe/alias.c",
            "qbe/cfg.c",
            "qbe/copy.c",
            "qbe/emit.c",
            "qbe/fold.c",
            "qbe/gcm.c",
            "qbe/gvn.c",
            "qbe/live.c",
            "qbe/load.c",
            "src/lib.c", // lib.c instead of main.c
            "qbe/mem.c",
            "qbe/parse.c",
            "qbe/rega.c",
            "qbe/simpl.c",
            "qbe/spill.c",
            "qbe/ssa.c",
            "qbe/util.c",
            // amd64
            "qbe/amd64/emit.c",
            "qbe/amd64/isel.c",
            "qbe/amd64/sysv.c",
            "qbe/amd64/targ.c",
            // arm64
            "qbe/arm64/abi.c",
            "qbe/arm64/emit.c",
            "qbe/arm64/isel.c",
            "qbe/arm64/targ.c",
            // rv64
            "qbe/rv64/abi.c",
            "qbe/rv64/emit.c",
            "qbe/rv64/isel.c",
            "qbe/rv64/targ.c",
        ])
        .include(config_dir.path())
        .include("qbe/")
        .compile("qbe");

    println!("cargo::rerun-if-changed=src/lib.c");
}
