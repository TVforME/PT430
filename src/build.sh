#!/bin/bash

# Function to show usage
show_usage() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  clean    - Clean build files"
    echo "  version  - Show version information"
    echo "  help     - Show this help"
}

# Function to generate version.h
generate_version_h() {
    echo "Generating version.h..."
    
    # Remove existing version.h if it exists
    if [ -f version.h ]; then
        rm -f version.h
        echo "Removed existing version.h"
    fi
    
    # Generate new version.h
    cat > version.h << EOF
#ifndef VERSION_H
#define VERSION_H

#define VERSION_MAJOR    1
#define VERSION_MINOR    0
#define VERSION_PATCH    0
#define VERSION_STRING   "1.0.0"
#define BUILD_DATE      "$(date '+%Y-%m-%d')"
#define BUILD_TIME      "$(date '+%H:%M:%S')"
#define BUILD_YEAR      "$(date '+%Y')"

#endif // VERSION_H
EOF
    echo "New version.h generated"
}

# Process command line arguments
case "$1" in
    clean)
        make clean
        if [ -f version.h ]; then
            rm -f version.h
            echo "Removed version.h"
        fi
        echo "Build files cleaned"
        exit 0
        ;;
    version)
        make version
        exit 0
        ;;
    help)
        show_usage
        exit 0
        ;;
esac

# Generate version.h
generate_version_h

# Build
echo "Building on Linux..."
make clean
make

# Check build status
if [ $? -eq 0 ]; then
    echo "Build successful"
    make version
else
    echo "Build failed"
    exit 1
fi
