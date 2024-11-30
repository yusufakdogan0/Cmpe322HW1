# Check and create mount directory if it doesn't exist
if [ ! -d "mount" ]; then
    mkdir mount
fi

# Check if the loop device is already in use and detach if necessary
if losetup | grep -q "/dev/loop0"; then
    sudo losetup -d /dev/loop0
fi

# Set up the loop device and mount the image
sudo losetup /dev/loop0 storage_vgc.img
sudo mount -o loop storage_vgc.img mount

# Remove existing symbolic link (if any) and create a new one
if [ -L /dev/device_file ]; then
    sudo rm /dev/device_file
fi
sudo ln -sf /dev/loop0 /dev/device_file

# Copy the compiled binaries to the mounted directory
[ -d mount/bin ] || mkdir mount/bin
cp bin/* mount/bin/

# Make the binaries executable
sudo chmod +x mount/bin/*
