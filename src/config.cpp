#include "miningConfig.h"
#include "config.h"
#include "args.h"

#ifdef _MSC_VER
#include "windows/procinfo_windows.h"
#endif

#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <cstring>

extern std::string s_configDir;

const std::string CONFIG_FILE_NAME = "config.cfg";

std::string configFilePath()
{
	return s_configDir + CONFIG_FILE_NAME;
}

enum {
	MODE = 0,
	POOL,
	ADDRESS,
	NTHREADS,
	HASHER,
	COLORS,
	WORKER_NAME,
	N_PARAMS
};

const std::string RELAX_STR = "relax";

bool configFileExists() {
	std::ifstream fs(configFilePath());
	if (!fs.is_open())
		return false;
	return true;
}

bool loadConfigFile(std::string& log) {	
	//
	MiningConfig newCfg = miningConfig();

	// read javaminer like config lines
	std::ifstream fs(configFilePath());
	const size_t BUFLEN = 256;
	char params[N_PARAMS][BUFLEN];
	for (int i = 0; i < N_PARAMS; i++) {
		if (!fs.getline(params[i], BUFLEN)) {
			log = "not enough lines";
			return false;
		}
		if (strlen(params[i]) <= 0) {
			log = "empty param";
			return false;
		}
	}

	newCfg.address = params[ADDRESS];

	if (sscanf(params[NTHREADS], "%d", &newCfg.nThreads) != 1) {
		log = "invalid number of threads";
		return false;
	}
	
	newCfg.poolUrl = params[POOL];

	// if there is an extra line, it contains cpp miner specific params
	char cppMinerParamLine[BUFLEN];
	if (fs.getline(cppMinerParamLine, BUFLEN)) {
		std::string cppMinerParams(cppMinerParamLine);
		if (cppMinerParams.find(RELAX_STR) != std::string::npos) {
			newCfg.relaxMode = true;
		}
	}
	
	setMiningConfig(newCfg);
	return true;
}

bool createConfigFile(std::string &log) {
	MiningConfig newCfg = miningConfig();

	std::ofstream fs(configFilePath());
	if (!fs.is_open()) {
		log = "Cannot open config file for writing";
		return false;
	}
	
	std::cout << std::endl << "-- Configuration File creation --" << std::endl;
	
	bool addressOk = false;
	while (!addressOk) {
		std::cin.clear();
		std::cout << "Enter your wallet address (if empty dev address): ";
		std::getline(std::cin, newCfg.address);
		if (newCfg.address.size() == 0)
			newCfg.address = miningConfig().devAddress;
		addressOk = isValidBase58(newCfg.address);
		if (!addressOk) {
			std::cout << "this is not a valid base58 address sorry ..." << std::endl;
		}
	}

	std::string defaultPool = miningConfig().poolUrl;
	bool poolUrlOk = false;
	while (!poolUrlOk) {
		std::cin.clear();
		std::cout << "Enter pool url (if empty " << defaultPool << "): ";
		std::getline(std::cin, newCfg.poolUrl, '\n');
		if (newCfg.poolUrl.size() == 0) {
			newCfg.poolUrl = defaultPool;
			poolUrlOk = true;
		}
		else {
			if (!(newCfg.poolUrl.find("http://") == 0 || newCfg.poolUrl.find("https://") == 0)) {
				std::cout << "Pool url must start with http:// or https://" << std::endl;
			}
			else {
				poolUrlOk = true;
			}
		}
	}

	bool nThreadsOk = false;
	while (!nThreadsOk) {
		std::cin.clear();
		std::cout << "Enter number of threads to use, 0/empty for auto (" << std::thread::hardware_concurrency() << " cores detected): ";
		std::string nThreadsStr;
		std::getline(std::cin, nThreadsStr);
		if (nThreadsStr.size() == 0) {
			nThreadsOk = true;
			newCfg.nThreads = miningConfig().nThreads;
		}
		else {
			int nThreads = 0;
			nThreadsOk = sscanf(nThreadsStr.c_str(), "%d", &nThreads) == 1;
			if (nThreads >= 0) {
				newCfg.nThreads = (uint32_t)nThreads;
			}
			else {
				nThreadsOk = false;
			}
		}
		if (!nThreadsOk) {
			std::cout << "invalid number of threads" << std::endl;
		}
	}

	bool relaxModeOk = false;
	while (!relaxModeOk) {
		std::cin.clear();
		std::cout << "Enable relax mode ?" << std::endl << "Relax mode makes computer more responsive while mining, but you may lose 1 to 2% hash rate (y/n): ";
		std::string relaxStr;
		std::getline(std::cin, relaxStr);
		if (relaxStr == "y") {
			relaxModeOk = true;
			newCfg.relaxMode = true;
		}
		else if (relaxStr == "n") {
			relaxModeOk = true;
			newCfg.relaxMode = false;
		}
		else {
			std::cout << "Please answer y or n." << std::endl;
		}
	}

	fs << "pool" << std::endl;
	fs << newCfg.poolUrl << std::endl;
	fs << newCfg.address << std::endl;
	fs << newCfg.nThreads << std::endl;
	fs << "standard" << std::endl;
	fs << "true" << std::endl;
	fs << miningConfig().workerID << std::endl;
	if (newCfg.relaxMode)
		fs << RELAX_STR << std::endl;

	fs.close();
	if (!fs) {
		std::cout << "cannot write to " << CONFIG_FILE_NAME << std::endl;
		return false;
	}

	std::cout << std::endl << "-- Configuration written to " << CONFIG_FILE_NAME << " --" << std::endl;
	std::cout << "To change config later, either edit " << CONFIG_FILE_NAME << " or delete it and relaunch the miner" << std::endl;
	std::cout << std::endl;

	setMiningConfig(newCfg);

	return true;
}
