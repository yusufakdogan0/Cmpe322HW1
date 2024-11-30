#!/bin/bash

# Create the mount directory if it doesn't exist
[ -d mount ] || mkdir mount

# Mount the image directly using the loop option
sudo mount -o loop storage_vgc.img mount

# Add the files from bin to the mounted directory
[ -d mount/bin ] || mkdir mount/bin
cp bin/* mount/bin/
sudo chmod +x mount/bin/*

# Start the game
./mount/bin/main_screen
