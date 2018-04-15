#include "miningConfig.h"
#include "uniqid.h"

static MiningConfig s_cfg;

void initMiningConfig() {
	s_cfg.poolUrl = "http://aropool.com";
	s_cfg.devAddress = "Pqyy9gGmuzgtt1wfKMmxz3hWYoEsxPLHDTHPheZfWGHWsYNMaKTJs7gsyLY4hwo4jMmPr6gh4qDDsyVoPAW1iTW";
	s_cfg.address = s_cfg.devAddress;
	s_cfg.workerID = uniqid();
	s_cfg.nThreads = 0;
	s_cfg.relaxMode = false;
}

void setMiningConfig(const MiningConfig& cfg) {
	s_cfg = cfg;
}

const MiningConfig& miningConfig() {
	return s_cfg;
}
