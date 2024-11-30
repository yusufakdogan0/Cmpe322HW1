# Unmount the image if it's already mounted
if mount | grep "storage_vgc.img" > /dev/null; then
    sudo umount mount
    sleep 1
fi

# Detach the loop device
sudo losetup -d /dev/loop0

# Remove the symbolic link if it exists
if [ -L /dev/device_file ]; then
    sudo rm /dev/device_file
fi

# Clean up the mount directory
[ -d mount ] && rm -r mount
