#!/bin/bash

# Create the mount directory if it doesn't exist
[ -d mount ] || mkdir mount

# Set up the loop device and mount the image
sudo losetup /dev/loop0 storage_vgc.img
sudo mount /dev/loop0 mount

# Create a symbolic link to the loop device in the current directory
ln -sf /dev/loop0 <device-file>

# Add the files
[ -d mount/bin ] || mkdir mount/bin
cp bin/* mount/bin/