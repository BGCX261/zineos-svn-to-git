# constants
VMNAME="ZineOS"
DSKIMG="$(cd `dirname $0` && pwd)/bin/floppy.img"
STORAGE="FloppyImg"
VBOXMGR="VBoxManage"

# clean up any VirtualBox stuff that has been loaded
# discard any output
function vb_error {
	# remove controller
	$VBOXMGR storagectl $VMNAME			\
		--name $STORAGE				\
		--remove				\
		> /dev/zero
	# unbind the medium
	$VBOXMGR closemedium floppy $DSKIMG		\
		> /dev/zero
	# remove VM
	$VBOXMGR unregistervm $VMNAME --delete		\
		> /dev/zero
	# tata
	exit 0
}

# uninstall ?
if [ "$1" = "remove" ] ; then
	echo "Remove $VMNAME from VirtualBox"
	vb_error
fi;

# Copy files
./copy_img.sh

# check if virtual machine exists
if ! ($VBOXMGR list vms | grep $VMNAME) ; then
	echo "-----------------------------------------------------------------"
	echo "Create VirtualMachine $VMNAME"
	echo "-----------------------------------------------------------------"
	if ! ( $VBOXMGR createvm			\
			--name $VMNAME			\
			--register
	) ; then
		exit 0;
	fi;

	echo "-----------------------------------------------------------------"	
	echo "Configure it"
	echo "-----------------------------------------------------------------"
	if ! ( $VBOXMGR modifyvm $VMNAME		\
			--ostype Other			\
			--memory 64			\
			--vram 8			\
			--acpi on			\
			--ioapic on			\
			--cpus 4			\
			--pae on			\
			--hpet on			\
			--hwvirtex on			\
			--hwvirtexexcl on		\
			--nestedpaging on		\
			--largepages on			\
			--boot1 floppy
	) ; then
		vb_error;
	fi;

	echo "-----------------------------------------------------------------"	
	echo "Add kernel image to VirtualBox"
	echo "-----------------------------------------------------------------"
	if ! ( $VBOXMGR openmedium floppy $DSKIMG ) ; then
		vb_error;
	fi;
	
	echo "-----------------------------------------------------------------"	
	echo "Add controller"
	echo "-----------------------------------------------------------------"
	if ! ( $VBOXMGR storagectl $VMNAME		\
			--name $STORAGE			\
			--add floppy
	) ; then
		vb_error;
	fi;
		
	echo "-----------------------------------------------------------------"	
	echo "Bind kernel image"
	echo "-----------------------------------------------------------------"
	if ! ( $VBOXMGR storageattach $VMNAME		\
			--storagectl $STORAGE		\
			--port 0			\
			--device 0			\
			--type fdd			\
			--medium $DSKIMG
	) ; then
		vb_error;
	fi;
		
fi;

# Execute the VM
echo "-----------------------------------------------------------------"	
echo "Starting $VMNAME"
echo "-----------------------------------------------------------------"
$VBOXMGR startvm $VMNAME

