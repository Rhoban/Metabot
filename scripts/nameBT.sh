#!/bin/bash

# Simple script to give name to bluetooth devices in the udev 

# Ensuring the default rules are copied
cp rules/* /etc/udev/rules

devices=`ls /dev/rfcom* 2>/dev/null`

if [ "$devices" == "" ]; then
    echo "No devices plugged in (searching for /dev/ttyUSB* and /dev/ttyACM* and /dev/rfcom*)"
else
    for i in $devices; do
        address=`udevadm info -a -n $i|grep "{address}"|head -n 1|cut -d"\"" -f2`
        file="/etc/udev/rules.d/45-$address.rules"
        name=""

        echo ""
        echo "How would you name $i (address: $address) ? (Leave blank to keep the name)"

        if [ -e $file ]; then
            name=`head -n 1 $file|cut -d" " -f2`
            echo "Note: This device is already recognized by a rule $file (name: $name)"
        fi

        read deviceName
        
        if [ "$deviceName" != "" ]; then
            echo "# $deviceName" > $file
            echo "# File generated using nameBTs.sh" >> $file
            echo "ATTRS{address}==\"$address\", SYMLINK+=\"$deviceName\"" >> $file
            echo "OK, creating udev rule to name it /dev/$deviceName"
            echo "Rule file name: $file"
        else
            if [ "$name" != "" ]; then
                echo "OK, let's it named $name for now"
            else
                echo "OK, let's let it unnamed for now"
            fi
        fi
    done
fi
