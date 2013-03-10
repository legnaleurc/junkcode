#! /bin/bash

if [ -z "${SANDBOX_HOME}" ] || [ -z "${SANDBOX_SOURCE}" ] || [ -z "$1" ] ; then
    exit 1
fi

source "${SANDBOX_SOURCE}"

name="$1"
fake_home="${SANDBOX_HOME}/${name}/environment/root"
key='private.asc'
tmp="${fake_home}/${key}"

# export private key
gpg --export-secret-keys --armor --output "${tmp}"

dpkg_tweak() {
    local path="$1"
    local full_name="$2"
    local email="$3"

    # add deb information
    cat >> "${path}/.bashrc" <<EOF

alias dquilt="quilt --quiltrc=\${HOME}/.quiltrc-dpkg"
export DEBFULLNAME="${full_name}"
export DEBEMAIL="${email}"
EOF
    cat >> "${path}/.quiltrc-dpkg" <<EOF
d=. ; while [ ! -d \$d/debian -a \`readlink -e \$d\` != / ]; do d=\$d/..; done
if [ -d \$d/debian ] && [ -z \$QUILT_PATCHES ]; then
    # if in Debian packaging tree with unset \$QUILT_PATCHES
    QUILT_PATCHES="debian/patches"
    QUILT_PATCH_OPTS="--reject-format=unified"
    QUILT_DIFF_ARGS="-p ab --no-timestamps --no-index --color=auto"
    QUILT_REFRESH_ARGS="-p ab --no-timestamps --no-index"
    QUILT_COLORS="diff_hdr=1;32:diff_add=1;34:diff_rem=1;31:diff_hunk=1;33:diff_ctx=35:diff_cctx=33"
    if ! [ -d \$d/debian/patches ]; then mkdir \$d/debian/patches; fi
fi
EOF

    return 0
}

dpkg_tweak "${fake_home}" 'Wei Cheng Pan' 'legnaleurc@gmail.com'

# make tmp script
tmp="${fake_home}/hook"
cat > "${tmp}" <<EOF
cd
gpg --import "${key}"
rm -f "${key}" 'hook'
EOF

sandbox "${name}" "${SHELL}" '/root/hook'
