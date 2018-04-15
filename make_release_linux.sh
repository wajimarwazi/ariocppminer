#!/bin/bash
set -e

# get version number
VERSION_NUM=v$(grep 'VERSION = "[^"]*' src/main.cpp | cut -d'"' -f 2)
VERSION="$VERSION_NUM$1"
OUT_DIR="rel/$VERSION"

echo
echo "-- Building ArioCppMiner $VERSION --"

# Cleanup
mkdir -p rel
rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

# Clean and Build all release versions
make -C prj/ config=rel_x64 clean ariocppminer
make -C prj/ config=relavx_x64 clean ariocppminer
make -C prj/ config=relavx2_x64 clean ariocppminer

# Copy to output folder
if ! [ -f "bin/ariocppminer" ]; then
	echo "Cannot find exe, compilation probably failed..."
	exit 1
fi

if ! [ -f "bin/ariocppminer_avx" ]; then
	echo "Cannot find AVX exe, compilation probably failed..."
	exit 1
fi

if ! [ -f "bin/ariocppminer_avx2" ]; then
	echo "Cannot find AVX2 exe, compilation probably failed..."
	exit 1
fi

cp "bin/ariocppminer" "$OUT_DIR"
cp "bin/ariocppminer_avx" "$OUT_DIR"
cp "bin/ariocppminer_avx2" "$OUT_DIR"
cp readme.md "$OUT_DIR/readme_${VERSION_NUM}.txt"

# tar
pushd "$OUT_DIR"
if [ -z "$1" ]; then
	tar -zcvf "../ariocppminer_${VERSION}_linux64.tar.gz" *
else
	tar -zcvf "../ariocppminer_${VERSION}.tar.gz" *
fi
popd

echo
echo "Build ok, result in $OUT_DIR"

exit 0
