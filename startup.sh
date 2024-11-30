# Create the mount directory if it doesn't exist
[ -d mount ] || mkdir mount

# Set up the loop device and mount the image
sudo losetup /dev/loop0 storage_vgc.img
sudo mount /dev/loop0 mount

# Remove existing symbolic link (if any) and create a new one
if [ -L /dev/device_file ]; then
    sudo rm /dev/device_file
fi
sudo ln -sf /dev/loop0 /dev/device_file

# Add the files from bin to the mounted directory
[ -d mount/bin ] || mkdir mount/bin
cp bin/* mount/bin/

# Start the game
echo "Starting game_snake..."
./mount/bin/game_snake
