# Requirements
* 64 bit linux
* Tested on Ubuntu, Lubuntu & Xubuntu

# Dependencies
* Install required packages:
	* sudo apt-get install curl git make gcc g++ libgmp-dev libssl-dev libcurl4-gnutls-dev 
	* (can also use libcurl4-openssl-dev instead of gnutls)
* Run this script which installs premake, argon & rapijson:
	* ./setup_linux.sh

# Build
* Building all binaries (normal, avx, avx2):
	* ./make_release_linux.sh
	* resulting binaries are in rel/ folder
* Building a specific version:
	* cd prj
	* make help
