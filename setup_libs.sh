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
    
    # WSutil library
    if [ -f "libWSutil.so.1.0.187" ]; then
        create_symlink "libWSutil.so.1.0.187" "libWSutil.so.1"
        create_symlink "libWSutil.so.1" "libWSutil.so"
    fi
    
    # WSai library
    if [ -f "libWSai.so.1.0.888" ]; then
        create_symlink "libWSai.so.1.0.888" "libWSai.so.1"
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

# Set up Linux libraries if we're on Linux
if [ -d "lib/macos" ]; then
    cd lib/macos
    
    # WSutil library
    if [ -f "libWSutil.1.0.187.dylib" ]; then
        create_symlink "libWSutil.1.0.187.dylib" "libWSutil.1.dylib"
        create_symlink "libWSutil.1.dylib" "libWSutil.dylib"
    fi
    
    # WSai library
    if [ -f "libWSai.1.0.888.dylib" ]; then
        create_symlink "libWSai.1.0.888.dylib" "libWSai.1.dylib"
        create_symlink "libWSai.1.dylib" "libWSai.dylib"
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
    echo "MacOS library setup complete."
fi

echo "Library setup completed successfully."
