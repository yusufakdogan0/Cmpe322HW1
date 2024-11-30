# Delete the disk image
if [ -f storage_vgc.img ]; then
    rm storage_vgc.img
fi
# Delete the bin directory if it exists
if [ -d "bin" ]; then
    rm -rf "bin"
fi
