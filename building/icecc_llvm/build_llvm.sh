#! /bin/sh

set -e -x

BUILD_DIR='/tmp/llvm-clang'
INSTALL_DIR='~/local/opt/llvm-clang'
LLVM_VERSION='3.9.0'
DARWIN_VERSION='16.1.0'


prepare_source () {
  build_dir="$1"
  version="$2"
  llvm_dir="llvm-$version.src"
  llvm_tarball="$llvm_dir.tar.xz"
  llvm_url="http://llvm.org/releases/$version/$llvm_tarball"
  clang_dir="cfe-$version.src"
  clang_tarball="$clang_dir.tar.xz"
  clang_url="http://llvm.org/releases/$version/$clang_tarball"
  libcxx_dir="libcxx-$version.src"
  libcxx_tarball="$libcxx_dir.tar.xz"
  libcxx_url="http://llvm.org/releases/$version/$libcxx_tarball"
  libcxxabi_dir="libcxxabi-$version.src"
  libcxxabi_tarball="$libcxxabi_dir.tar.xz"
  libcxxabi_url="http://llvm.org/releases/$version/$libcxxabi_tarball"
  cwd="$(pwd)"

  cd "$build_dir"

  # Download a release clang release from the website
  curl -O "$llvm_url"
  tar -xvf "$llvm_tarball"

  # Get the same release of clang
  curl -O "$clang_url"
  tar -xvf "$clang_tarball"
  mv "$clang_dir" "$llvm_dir/tools/clang"

  # Get the same release of libcxx and libcxxabi
  curl -O "$libcxx_url"
  tar -xvf "$libcxx_tarball"
  mv "$libcxx_dir" "$llvm_dir/projects/libcxx"
  curl -O "$libcxxabi_url"
  tar -xvf "$libcxxabi_tarball"
  mv "$libcxxabi_dir" "$llvm_dir/projects/libcxxabi"

  cd "$cwd"
}


# install libcxx, NOT OPTIONAL! see https://llvm.org/bugs/show_bug.cgi?id=25044
# check 'xcode-select --install' is installed
build_source () {
  build_dir="$1"
  version="$2"
  darwin_version="$3"
  install_dir="$4"
  llvm_dir="llvm-$version.src"
  cwd="$(pwd)"

  cd "$build_dir"

  mkdir 'build'
  cd 'build'
  # configure. The TRIPLE and the Release build are the important bits here
  cmake \
    "-DLLVM_DEFAULT_TARGET_TRIPLE=x86_64-apple-darwin$darwin_version" \
    "-DCMAKE_INSTALL_PREFIX=$install_dir" \
    -DCMAKE_BUILD_TYPE=Release \
    "$build_dir/$llvm_dir"

  make -j9
  make install

  cd "$cwd"
}


# Find/make a parent dir
mkdir -p "$BUILD_DIR"

prepare_source "$BUILD_DIR" "$LLVM_VERSION"

build_source "$BUILD_DIR" "$LLVM_VERSION" "$DARWIN_VERSION" "$INSTALL_DIR"
