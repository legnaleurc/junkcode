#! /bin/sh

set -e -x

BUILD_DIR='/tmp/llvm-clang'
INSTALL_DIR='~/local/opt/llvm-clang'
LLVM_VERSION='5.0.0'
DARWIN_VERSION='17.2.0'


prepare_source () {
  build_dir="$1"
  version="$2"
  llvm_dir="llvm-$version.src"
  llvm_tarball="$llvm_dir.tar.xz"
  llvm_url="https://releases.llvm.org/$version/$llvm_tarball"
  clang_dir="cfe-$version.src"
  clang_tarball="$clang_dir.tar.xz"
  clang_url="https://releases.llvm.org/$version/$clang_tarball"
  libcxx_dir="libcxx-$version.src"
  libcxx_tarball="$libcxx_dir.tar.xz"
  libcxx_url="https://releases.llvm.org/$version/$libcxx_tarball"
  libcxxabi_dir="libcxxabi-$version.src"
  libcxxabi_tarball="$libcxxabi_dir.tar.xz"
  libcxxabi_url="https://releases.llvm.org/$version/$libcxxabi_tarball"
  lldb_dir="lldb-$version.src"
  lldb_tarball="$lldb_dir.tar.xz"
  lldb_url="https://releases.llvm.org/$version/$lldb_tarball"
  cwd="$(pwd)"

  cd "$build_dir"

  # Download a release clang release from the website
  curl -L -O "$llvm_url"
  tar -xvf "$llvm_tarball"

  # Get the same release of clang
  curl -L -O "$clang_url"
  tar -xvf "$clang_tarball"
  mv "$clang_dir" "$llvm_dir/tools/clang"

  # Get the same release of libcxx and libcxxabi
  curl -L -O "$libcxx_url"
  tar -xvf "$libcxx_tarball"
  mv "$libcxx_dir" "$llvm_dir/projects/libcxx"
  curl -L -O "$libcxxabi_url"
  tar -xvf "$libcxxabi_tarball"
  mv "$libcxxabi_dir" "$llvm_dir/projects/libcxxabi"

  # Get the same release of lldb
  curl -L -O "$lldb_url"
  tar -xvf "$lldb_tarball"
  mv "$lldb_dir" "$llvm_dir/tools/lldb"

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

  make -j$(get_cpu_cores)
  make install

  cd "$cwd"
}


get_cpu_cores () {
  if which nproc > /dev/null ; then
    nproc --all
  else
    sysctl -n hw.ncpu
  fi
}

# Find/make a parent dir
mkdir -p "$BUILD_DIR"

prepare_source "$BUILD_DIR" "$LLVM_VERSION"

build_source "$BUILD_DIR" "$LLVM_VERSION" "$DARWIN_VERSION" "$INSTALL_DIR"
