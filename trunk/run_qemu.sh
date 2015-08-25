# Copy the files
./copy_img.sh

# qemu args
CMD="qemu-system-x86_64 -fda bin/floppy.img -m 64 -smp 4"

# disable KVM virtualization if running 32bit mode
if ! ( uname -a | grep "x86_64" ) ; then
	CMD="$CMD -no-kvm";
fi;

# display command line and execute it
echo $CMD
gnome-terminal -e "$CMD -curses"

