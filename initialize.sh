# Check if disk image already exists and delete it
if [ -f "storage_vgc.img" ]; then
    rm "storage_vgc.img"
fi

# Create the disk image (100MB in this example)
dd if=/dev/zero of="storage_vgc.img" bs=1M count=2 status=progress

# Set all permissions (777) for .sh files in the current directory
for file in *.sh; do
    if [ -f "$file" ]; then
        chmod 777 "$file"
        echo "Permissions for $file set to 777"
    fi
done

# Create bin directory if it doesn't exist and ensure it's empty
mkdir -p bin
rm -rf bin/*  # Ensure bin is empty before compiling

# Compile C source files
gcc src/game_snake.c -o bin/game_snake
gcc src/game_blackjack.c -o bin/game_blackjack
gcc src/game_platform.c -o bin/game_platform
gcc src/main_screen.c -o bin/main_screen

