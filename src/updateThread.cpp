#include <rapidjson/document.h>
#include <curl/curl.h>

#include <string>
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <chrono>
#include <assert.h>

#include "miningConfig.h"
#include "miner.h"
#include "updateThread.h"
#include "http.h"
#include "log.h"

using namespace rapidjson;

using std::chrono::high_resolution_clock;

const uint64_t SEND_INITIAL_HASH_RATE_AFTER_S = 30;
const uint64_t HASH_RATE_SEND_INTERVAL_S = 10 * 60;
const uint64_t POLL_INTERVAL_MS = 5 * 1000;

static std::mutex s_hashParams_mutex;
static HashParams s_hashParams;
static bool s_bUpdateThreadRun = true;
bool s_initialHashRateSent = false;

const char* UPDATE_THREAD_LOG_PREFIX = "UPDT";

uint64_t updateThreadPollIntervalMs() {
	return POLL_INTERVAL_MS;
}

static bool updatePoolParams(const MiningConfig& config, HashParams &hashParams, uint32_t hashRate) {
	// construct url
	std::ostringstream oss;
	oss << config.poolUrl << "/mine.php?q=info";
	oss << "&worker=" << config.workerID;
	if (hashRate > 0) {
		oss << "&address=" << config.address;
		oss << "&hashrate=" << hashRate;
	}
	std::string url = oss.str();
	std::string miningInfoJson;

	// perform request
	bool res = httpGetString(url, miningInfoJson);
	if (!res) {
		return false;
	}

	// parse result
	Document miningInfo;
	miningInfo.Parse(miningInfoJson.c_str());
	if (!miningInfo.IsObject()) {
		return false;
	}

	// check if status ok
	const char* STATUS = "status";
	if (!miningInfo.HasMember(STATUS))
		return false;
	auto status = miningInfo[STATUS].GetString();
	if (!strstr(status, "ok"))
		return false;

	// JSON helper
	auto checkAndSetStr = [](rapidjson::GenericValue<rapidjson::UTF8<>>& parent, const std::string &name, std::string &out) -> bool {
		if (!parent.HasMember(name.c_str()) || !parent[name.c_str()].IsString())
			return false;
		out = parent[name.c_str()].GetString();
		return true;
	};

	// coin must be arionum
	std::string coin;
	if (!checkAndSetStr(miningInfo, "coin", coin)) return false;
	if (coin != "arionum")
		return false;

	// read "data" struct, which contains the mining parameters
	const char* DATA = "data";
	if (!miningInfo.HasMember(DATA))
		return false;

	if (!checkAndSetStr(miningInfo[DATA], "difficulty", hashParams.difficulty)) return false;
	if (!checkAndSetStr(miningInfo[DATA], "block", hashParams.block)) return false;
	if (!checkAndSetStr(miningInfo[DATA], "public_key", hashParams.publicKey)) return false;

	const char* LIMIT = "limit";
	if (!miningInfo[DATA].HasMember(LIMIT) || !miningInfo[DATA][LIMIT].IsInt())
		return false;
	hashParams.limit = miningInfo[DATA][LIMIT].GetUint();

	const char* HEIGHT = "height";
	if (!miningInfo[DATA].HasMember(HEIGHT) || !miningInfo[DATA][HEIGHT].IsInt())
		return false;
	hashParams.height = miningInfo[DATA][HEIGHT].GetUint();

	return true;
}

HashParams currentHashParams() {
	HashParams ret;
	s_hashParams_mutex.lock();
	{
		ret = s_hashParams;
	}
	s_hashParams_mutex.unlock();
	return ret;
}

// regularly polls the pool to get new HashParams when block changes
void updateThreadFn() {	
	logLine(UPDATE_THREAD_LOG_PREFIX, "Pool update thread launched");
	auto tStart = high_resolution_clock::now();
	uint32_t nHashes = getTotalHashes();

	while (s_bUpdateThreadRun) {
		HashParams newHashParams;

		uint32_t nHashesNow = getTotalHashes();
		auto tNow = high_resolution_clock::now();
		std::chrono::duration<float> durationSinceLast = tNow - tStart;
		bool recomputeHashRate = false;
		if (!s_initialHashRateSent) {
			if (durationSinceLast.count() >= SEND_INITIAL_HASH_RATE_AFTER_S) {
				s_initialHashRateSent = true;
				recomputeHashRate = true;
			}
		}
		else {
			if (durationSinceLast.count() >= HASH_RATE_SEND_INTERVAL_S) {
				recomputeHashRate = true;
			}
		}

		uint32_t hashRate = 0;
		if (recomputeHashRate) {
			hashRate = (nHashesNow - nHashes) / (uint32_t)(durationSinceLast.count());
			tStart = tNow;
			nHashes = nHashesNow;
		}

		bool ok = updatePoolParams(miningConfig(), newHashParams, hashRate);
		if (!ok) {
			logLine(UPDATE_THREAD_LOG_PREFIX, "Cannot get pool info (%s), retrying in %.2fs",
				miningConfig().poolUrl.c_str(),
				POLL_INTERVAL_MS/1000.f);
		}
		else {
			if (s_hashParams.height != newHashParams.height) {
				s_hashParams_mutex.lock();
				{
					s_hashParams = newHashParams;
				}
				s_hashParams_mutex.unlock();
				logLine(UPDATE_THREAD_LOG_PREFIX, "New block height: %u difficulty: %s",
					newHashParams.height,
					newHashParams.difficulty.c_str());
			}
#ifdef _DEBUG
			else {
				logLine(UPDATE_THREAD_LOG_PREFIX, "Got pool info: height: %u difficulty: %s",
					newHashParams.height,
					newHashParams.difficulty.c_str());
			}
#endif
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(POLL_INTERVAL_MS));
	}
}

std::thread* s_pThread = nullptr;
void startUpdateThread() {
	if (s_pThread) {
		assert(0);
		return;
	}
	s_pThread = new std::thread(updateThreadFn);
}

void stopUpdateThread() {
	if (s_pThread) {
		assert(s_bUpdateThreadRun);
		s_bUpdateThreadRun = false;
		s_pThread->join();
		delete s_pThread;
	}
	else {
		assert(0);
	}
}