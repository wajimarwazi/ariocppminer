#include "args.h"
#include "inputParser.h"
#include "miningConfig.h"
#include "log.h"

const std::string OPT_NTHREADS = "-t";
const std::string OPT_ADDRESS = "-a";
const std::string OPT_POOL_URL = "-p";
const std::string OPT_RELAX = "-relax";
const std::string OPT_USAGE = "-h";

static const std::string s_usageMsg = R"(
--- Usage ---
ArioCppMiner.exe    [-a address] [-t nThreads] [-p poolUrl] [-relax] [-h]
  -a address          : your arionum address (if not specified, dev address used as default)
  -t nThreads         : number of threads to use (if not specified will use maximum logical threads available)
  -p poolUrl          : pool url (if not specified, will use http://aropool.com)
  -h                  : display this help message and exit
  -relax              : enable relax mode, freezes less computer at the cost of reducing a bit the hashrate (-1% to -2% less H/s)

--- Notes ---
* The first share you find goes to dev, 
  after that 0.5% of the shares goes to the dev (so ~1 share on 200, randomly)
* Solo mining is not supported yet
* Mining with more threads than the number of physical cores on your CPU 
  will only give you a marginal improvement. For example, if your CPU has 4 cores 
  and hyperthreading is ON then miner will detect 4 physical cores and 
  8 logical cores, the difference in hash rate between 8 and 4 threads 
  is around 5% (using 8 threads may cause your computer to lag much more than 4 threads)
)";

void printUsage()
{
	printf("%s\n", s_usageMsg.c_str());
}

bool isValidBase58(const std::string& address) {
	const char B58_CHARS[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
	const size_t N_CHARS = sizeof(B58_CHARS);
	for (size_t i = 0; i < address.size(); i++) {
		bool found = false;
		size_t j = 0;
		for (; j < N_CHARS; j++) {
			if (B58_CHARS[j] == address[i])
				break;
		}
		if (j >= N_CHARS)
			return false;
	}
	return true;
}

bool parseArgs(const char* prefix, int argc, char** argv)
{
	InputParser ip(argc, argv);
	MiningConfig cfg = miningConfig();
	
	if (ip.cmdOptionExists(OPT_USAGE)) {
		printUsage();
		return false;
	}

	if (ip.cmdOptionExists(OPT_NTHREADS)) {
		const auto& nThreadsStr = ip.getCmdOption(OPT_NTHREADS);
		int n = sscanf(nThreadsStr.c_str(), "%u", &cfg.nThreads);
		if (n<1) {
			logLine(prefix, "Warning: invalid value for number of threads (%d), reverting to default", cfg.nThreads);
			cfg.nThreads = 0;
		}
	}

	if (ip.cmdOptionExists(OPT_ADDRESS)) {
		cfg.address = ip.getCmdOption(OPT_ADDRESS);
		if (!isValidBase58(cfg.address)) {
			logLine(prefix, "Warning: wallet address (%s) seems invalid, reverting to default (dev) address", cfg.address.c_str());
			cfg.address = cfg.devAddress;
		}
	}

	if (ip.cmdOptionExists(OPT_POOL_URL)) {
		std::string prevUrl = cfg.poolUrl;
		cfg.poolUrl = ip.getCmdOption(OPT_POOL_URL);
		if (cfg.poolUrl.size() <= 1) {
			logLine(prefix, "Warning: pool url (%s) seems invalid, reverting to default pool", cfg.poolUrl.c_str());
			cfg.poolUrl = prevUrl;
		}
	}

	if (ip.cmdOptionExists(OPT_RELAX)) {
		cfg.relaxMode = true;
	}

	setMiningConfig(cfg);

	return true;
}
