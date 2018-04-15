#TODO v0.9.6
* better handling of cli params errors
	* ex: ./ariocppminer -a12345 -paro.cool
* node stats reporting (compatible with java miner)

#TODO v0.9.7
* for all platforms 
	* proper logical / physical cores detection
	* proper ctrl+c handler
* cpu affinity

#TODO, later versions
* optimizations
	* BUILD_XY for all archs
* gpu mining 
	* Cuda, OpenCL or both ?
* avx512 support
* 32 bit versions

#NOTES
* it seems that PHP is not providing a fully random salt to Arion
	* PHP generates random bytes and then encodes them to base64 and send that to Arion
	* so PHP is not using the full space of possible Argon salts, only the base64 space
	* a base64 string will be longer than the random bytes it was generated from, so we could suppose this compensates ...

* Nonce size in default miner:
	* 32 bytes randomly generated
	* after base64 encoding, becomes 32 * 3 / 4 = 42.66 => so 43 bytes max
	* but then "invalid" base64 chars are removed : '=', '/', '+' ...
	* so in the end, the nonce lenght will be between 0 and 43 bytes, from actual tests the lenght is usually in [41-43]
	* note that nonce length does not seem to change Argon2i execution time

* HTTP post/get usefull links
	* https://stackoverflow.com/questions/5725430/http-test-server-that-accepts-get-post-calls
	* https://raw.githubusercontent.com/curl/curl/master/docs/examples/postinmemory.c
	* https://raw.githubusercontent.com/curl/curl/master/docs/examples/post-callback.c
