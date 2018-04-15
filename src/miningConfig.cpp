#include "miningConfig.h"
#include "uniqid.h"

static MiningConfig s_cfg;

void initMiningConfig() {
	s_cfg.poolUrl = "http://aropool.com";
	s_cfg.devAddress = "419qwxjJLBRdAVttxcJVT84vdmdq3GP6ghXdQdxN6sqbdr5SBXvPU8bvfVzUXWrjrNrJbAJCvW9JYDWvxenus1pK";
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
