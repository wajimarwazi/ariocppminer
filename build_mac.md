# Requirements
* Tested on MacOS 10.13.3 (High Sierra)
* Should work on older versions too, but they need to be 64bits

# Dependencies
* Install latest XCode
* Install XCode commandline tools 
	* xcode-select --install
* Install brew
		* see https://brew.sh/
* Install gmp & openssl
	* brew install gmp openssl
* Install premake5 / rapidjson / argon:
	* ./setup_mac.sh

# Build
* Building all binaries (normal, avx, avx2):
	* ./make_release_mac.sh
	* resulting binaries are in rel/ folder
* Building a specific version:
	* cd prj
	* make help

