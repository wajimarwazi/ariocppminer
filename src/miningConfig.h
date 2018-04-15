#pragma once

#include <string>

struct MiningConfig {
	std::string poolUrl;
	std::string address;
	std::string devAddress;
	uint32_t nThreads;
	std::string workerID;
	bool relaxMode;
};

void initMiningConfig();
const MiningConfig& miningConfig();

// do not call that during mining, only during init !
void setMiningConfig(const MiningConfig& cfg);
