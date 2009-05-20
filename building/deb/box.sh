#! /bin/sh

SITE='http://ftp.twaren.net/debian/'

# $1 is arch, $2 is distro
function create {
	arch=$1
	distro=$2
	dir="$distro-$arch"
	[ -d $dir ] || mkdir $dir
	debootstrap --arch $arch $distro $dir $SITE
	[ -d $dir/dev/input ] || mkdir $dir/dev/input
	echo >> $dir/root/.bashrc
	echo 'export DEBFULLNAME="Wei-Cheng Pan"' >> $dir/root/.bashrc
	echo 'export DEBEMAIL="legnaleurc@gmail.com"' >> $dir/root/.bashrc
}

function enter {
	mount --bind /var/cache/apt/archives $1/var/cache/apt/archives
	mount --bind /dev/pts $1/dev/pts
	mount --bind /dev/input $1/dev/input
	mount --bind /tmp $1/tmp
	mount --bind /proc $1/proc

	chroot $1 env -i HOME=/root TERM=$TERM $SHELL --login

	umount $1/proc
	umount $1/tmp
	umount $1/dev/input
	umount $1/dev/pts
	umount $1/var/cache/apt/archives
}

case "$1" in
	'create'	)
		create $2 $3
		;;
	'enter'	)
		enter $2
		;;
	*	)
		echo 'unknow'
		;;
esac
