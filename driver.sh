#! /bin/bash

# Comparing the extension to find out the type of file given 
# "*.c", "*.o" and <module_name>

stringZ=$1
size=${#stringZ}
extension=${stringZ:$size-2:$size-1}		#saves the extension of the file

if [[ $extension = ".c" ]]; then

	file=${stringZ:0:$size-2}.o
	module=${stringZ:0:$size-2}.ko

elif [[ $extension = ".o" ]]; then

	file=$1
	module=${stringZ:0:$size-2}.ko

else

	file=$1.o
	module=$1.ko

fi

echo "**********************************************************************************
			BUILDING $file MODULE
**********************************************************************************
"
rm -rf *~ 

make --quiet default obj-m=$file

if [[ $? -eq 0 ]]; then
	
	#If make succedded. 
	timestamp="<$(($(date +%N)/1000000))>"
	echo "$timestamp : Make $file was successful"
	sudo dmesg --clear

	timestamp="<$(($(date +%N)/1000000))>"
	echo "$timestamp : Loading $file"
	sudo insmod ./$module
	sleep 1

	timestamp="<$(($(date +%N)/1000000))>"
	echo "$timestamp : Removing $file"
	sudo rmmod $module
	
	dmesg > data.txt
	subl data.txt 

	timestamp="<$(($(date +%N)/1000000))>"
	echo "$timestamp : Cleanup"
	make --quiet clean

else
	timestamp="<$(($(date +%N)/1000000))>"
	echo "
	$timestamp : ERROR	:	Make of $file did not succed. Exiting
		"
	exit
fi

	


