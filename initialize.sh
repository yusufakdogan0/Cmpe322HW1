# Check if disk image already exists and delete it
if [ -f "storage_vgc.img" ]; then
    rm "storage_vgc.img"
fi

# Create the disk image (100MB in this example)
dd if=/dev/zero of="storage_vgc.img" bs=1M count=100 status=progress

# Create bin directory if it doesn't exist and ensure it's empty
mkdir -p bin
rm -rf bin/*  # Ensure bin is empty before compiling

# Compile C source files
gcc src/game_snake.c -o bin/game_snake
gcc src/game_blackjack.c -o bin/game_blackjack
gcc src/game_platform.c -o bin/game_platform
gcc src/main_screen.c -o bin/main_screen

