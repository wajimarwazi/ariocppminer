#!/bin/bash
set -e

# get version number
VERSION="v$(grep -Po 'VERSION = "\K[^"]*' src/main.cpp)"
OUT_DIR="rel/${VERSION}_win64"
ZIP_PATH="rel/ariocppminer_${VERSION}_win64.zip"

# get 7zip path
SEVEN_ZIP_X86="${PROGRAMFILES} (x86)/7-Zip/7z.exe"
SEVEN_ZIP_X64="${PROGRAMFILES}/7-Zip/7z.exe"
if [ -f "$SEVEN_ZIP_X86" ]; then
	SEVEN_ZIP="$SEVEN_ZIP_X86"
elif [ -f "$SEVEN_ZIP_X64" ]; then
	SEVEN_ZIP="$SEVEN_ZIP_X64"
else
	echo "Cannot find 7zip, please install it"
	exit 1
fi

#get msbuild (vs2015) path
MSBUILD_VS2015="${PROGRAMFILES} (x86)\\MSBuild\\14.0\\Bin\\MSBuild.exe"
if ! [ -f "$MSBUILD_VS2015" ]; then
	echo "Cannot find msbuild, please install visual studio 2015"
	exit 1
fi

echo
echo "-- Building ArioCppMiner Win64 $VERSION --"

# Cleanup
mkdir -p rel
rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"
rm -f "$ZIP_PATH"

# Clean and Build AVX/AVX2
cd prj
"$MSBUILD_VS2015" 'ArioCppMiner.vcxproj' '//t:Clean;Build' '//p:Configuration=Rel' '//p:Platform=x64'
"$MSBUILD_VS2015" 'ArioCppMiner.vcxproj' '//t:Clean;Build' '//p:Configuration=RelAvx' '//p:Platform=x64'
"$MSBUILD_VS2015" 'ArioCppMiner.vcxproj' '//t:Clean;Build' '//p:Configuration=RelAvx2' '//p:Platform=x64'
cd ..

# Copy to output folder
if ! [ -f "bin/ariocppminer_avx.exe" ]; then
	echo "Cannot find AVX exe, compilation probably failed..."
	exit 1
fi

if ! [ -f "bin/ariocppminer_avx2.exe" ]; then
	echo "Cannot find AVX2 exe, compilation probably failed..."
	exit 1
fi

if ! [ -f "bin/ariocppminer.exe" ]; then
	echo "Cannot find exe, compilation probably failed..."
	exit 1
fi

cp "bin/ariocppminer.exe" "$OUT_DIR"
cp "bin/ariocppminer_avx.exe" "$OUT_DIR"
cp "bin/ariocppminer_avx2.exe" "$OUT_DIR"
unix2dos -n readme.md "$OUT_DIR/readme_${VERSION}.txt"

# zip
"$SEVEN_ZIP" a -tzip "$ZIP_PATH" "./$OUT_DIR/*"

echo
echo "Build ok, result in $OUT_DIR"

exit 0
