# Check and create mount directory if it doesn't exist
if [ ! -d "mount" ]; then
    mkdir mount
fi

# Set up the loop device and mount the image
sudo losetup -d /dev/loop0
sudo losetup /dev/loop0 storage_vgc.img

# Check if the image has a file system (e.g., ext4) and format it if necessary
if ! sudo file -sL storage_vgc.img | grep -q "ext4"; then
    sudo mkfs.ext4 storage_vgc.img
fi

sudo mount -o loop,rw storage_vgc.img mount
sudo chmod 777 mount
sudo chown $USER:$USER mount

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
