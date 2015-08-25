PATH=$PATH:/usr/local/cross/bin
cd loader
if make ; then
	cd ../kernel
	if make ; then
		cd ..
		./run_qemu.sh
	else
		cd ..
	fi
else
	cd ..
fi
