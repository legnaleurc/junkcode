#! /bin/sh

getSite() {
	if [ "x`echo "$1" | grep '^\(sid\|squeeze\|lenny\)$'`" = "x$1" ] ; then
		echo 'http://ftp.twaren.net/debian/'
	elif [ "x`echo "$1" | grep '^\(gutsy\|hardy\|intrepid\|jaunty\|karmic\)$'`" = "x$1" ] ; then
		echo 'http://tw.archive.ubuntu.com/ubuntu/'
	else
		echo "unknown distro: $1"
		exit 1
	fi
}

# $1 is arch, $2 is distro
create() {
	arch=$1
	distro=$2
	dir="$distro-$arch"
	[ -d "$dir" ] || mkdir "$dir"
	debootstrap --arch "$arch" "$distro" "$dir" "`getSite $distro`" || exit 1
	[ -d "${dir}/dev/input" ] || mkdir "${dir}/dev/input"
	cat >> "${dir}/root/.bashrc" <<EOF

export DEBFULLNAME="Wei-Cheng Pan"
export DEBEMAIL="legnaleurc@gmail.com"
EOF
}

enter() {
	mount --bind /var/cache/apt/archives "$1/var/cache/apt/archives"
	mount --bind /dev/pts "$1/dev/pts"
	mount --bind /dev/input "$1/dev/input"
	mount --bind /tmp "$1/tmp"
	mount --bind /proc "$1/proc"

	chroot "$1" env -i HOME=/root TERM=$TERM $SHELL --login

	umount "$1/proc"
	umount "$1/tmp"
	umount "$1/dev/input"
	umount "$1/dev/pts"
	umount "$1/var/cache/apt/archives"
}

usage() {
	cat <<EOF
usage: `basename $0` <create|enter> <option...>
	create <arch> <distro>
	enter <directory>
EOF
}

case "x$1" in
'xcreate'	)
	shift 1
	if [ $# -ge 2 ] ; then
		create "$1" "$2"
	else
		usage
	fi
	;;
'xenter'	)
	shift 1
	if [ $# -ge 1 ] ; then
		enter "$1"
	else
		usage
	fi
	;;
*	)
	usage
	;;
esac
