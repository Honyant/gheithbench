#! /bin/bash

echo "BUILDING \`010.dir\`..."

# reset
rm -rf 010.dir
mkdir 010.dir

# build executable
gcc -m32 -static -nostartfiles -nostdlib -fPIE -o src src.c

# hexdump
hexdump -C src

# move into 010.dir
mv src 010.dir/int
cp src.c 010.dir/src.c

# readelf
readelf -S 010.dir/int
