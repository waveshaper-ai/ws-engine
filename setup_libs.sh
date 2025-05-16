#!/bin/bash
# Script to set up symbolic links for libraries and update library cache

echo "Setting up library symbolic links..."

# Function to properly create a symbolic link
create_symlink() {
    local target=$1
    local link_name=$2
    
    # Remove existing file/link if it exists
    if [ -e "$link_name" ] || [ -L "$link_name" ]; then
        rm "$link_name"
    fi
    
    # Create the symbolic link
    ln -s "$target" "$link_name"
    echo "Created symlink: $link_name -> $target"
}

# Set up Linux libraries if we're on Linux
if [ -d "lib/linux" ]; then
    cd lib/linux
    
    # TLcore library
    if [ -f "libTLcore.so.1.0.685" ]; then
        create_symlink "libTLcore.so.1.0.685" "libTLcore.so.1"
        create_symlink "libTLcore.so.1" "libTLcore.so"
    fi
    
    # TLscript library
    if [ -f "libTLscript.so.1.0.202" ]; then
        create_symlink "libTLscript.so.1.0.202" "libTLscript.so.1"
        create_symlink "libTLscript.so.1" "libTLscript.so"
    fi
    
    # WSai library
    if [ -f "libWSai.so.1.0.704" ]; then
        create_symlink "libWSai.so.1.0.704" "libWSai.so.1"
        create_symlink "libWSai.so.1" "libWSai.so"
    fi
    
    # Update the library cache
    echo "Updating library cache..."
    if [ "$(id -u)" -eq 0 ]; then
        # Running as root
        ldconfig $(pwd)
    else
        # Running as non-root user
        echo "Not running as root, skipping system-wide ldconfig."
        echo "You may need to set LD_LIBRARY_PATH manually:"
        echo "  export LD_LIBRARY_PATH=$(pwd):\$LD_LIBRARY_PATH"
    fi
    
    cd ../..
    echo "Linux library setup complete."
fi

# Set up Windows libraries if we're on Windows
if [ -d "lib/windows" ] && command -v lib.exe &> /dev/null; then
    echo "Setting up Windows libraries..."
    cd lib/windows
    
    # For each DLL, create a .lib file if it doesn't exist yet
    for dll in *.dll; do
        base=${dll%.dll}
        if [ ! -f "${base}.lib" ]; then
            echo "Creating import library for ${dll}..."
            
            # Extract exports from DLL
            echo "LIBRARY ${dll}" > ${base}.def
            echo "EXPORTS" >> ${base}.def
            dumpbin /exports ${dll} | grep -A 100 "ordinal hint" | grep -B 100 " Summary" | grep -v "ordinal hint" | grep -v " Summary" | awk '{print $4}' | grep -v "^$" >> ${base}.def
            
            # Generate .lib file
            lib /def:${base}.def /out:${base}.lib /machine:x64
            
            echo "Created ${base}.lib"
        fi
    done
    
    cd ../..
    echo "Windows library setup complete."
fi

echo "Library setup completed successfully."