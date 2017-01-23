# pyenv
pe () {
    export PATH="$HOME/.pyenv/bin:$PATH"
    eval "$(pyenv init -)"
    eval "$(pyenv virtualenv-init -)"
}

# nvm
ne () {
    export NVM_DIR="$HOME/.nvm"
    [ -s "$NVM_DIR/nvm.sh" ] && . "$NVM_DIR/nvm.sh"  # This loads nvm
}


firefox_build_static () {
    MOZCONFIG=.mozconfig_static ./mach build "$@"
}

firefox_build_llvm () {
    PATH="$HOME/local/opt/shim:$PATH" ./mach build "$@"
}

firefox_run_wait () {
    MOZ_DEBUG_CHILD_PROCESS=1 ./mach run "$@"
}


firefox_run_remote () {
    ./mach run --remote --start-debugger-server 6000 "$@"
}

firefox_run_leak () {
    XPCOM_MEM_LEAK_LOG=1 XPCOM_MEM_REFCNT_LOG=/tmp/refcnt.log XPCOM_MEM_LOG_CLASSES=nsMIMEInputStream ./mach run --debug
}


b2g_build_spark () {
    GAIA_DISTRIBUTION_DIR=distros/spark GAIA_KEYBOARD_LAYOUTS=en,jp-kanji,zh-Hant-Zhuyin ./build.sh "$@"
}

b2g_build_mulet () {
    MOZCONFIG=mozconfig.mulet ./mach build "$@"
}

b2g_run_mulet () {
    MOZCONFIG=mozconfig.mulet ./mach run -profile "$HOME/local/src/gaia/profile" "$@"
}

