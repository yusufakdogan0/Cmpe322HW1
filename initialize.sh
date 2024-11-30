# Check if disk image already exists and delete it
if [ -f "storage_vgc.img" ]; then
    echo "Disk image storage_vgc.img already exists. Overriding it..."
    rm "storage_vgc.img"
fi

# Create the disk image (100MB in this example)
dd if=/dev/zero of="storage_vgc.img" bs=1M count=100

# Compile C source files
mkdir -p bin
rm -rf bin/*  # Ensure bin is empty before compiling
gcc src/game_snake.c -o bin/game_snake
gcc src/game_blackjack.c -o bin/game_blackjack
gcc src/game_platform.c -o bin/game_platform
gcc src/main_screen.c -o bin/main_screen
