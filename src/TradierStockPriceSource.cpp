/**
 * Copyright 2017, SonicRED
 * Original Author: Travis Cammack
 * Contributors:
 */

#include <TradierStockPriceSource.hpp>
#include <DataSource.hpp>
#include <Configuration.hpp>
#include <Logger.hpp>
#include <TradierAccessor.hpp>

#include <Poco/Exception.h>
#include <Poco/DateTime.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeParser.h>
#include <Poco/Timespan.h>
#include <Poco/URI.h>
#include <json/json.hpp>

#include <vector>
#include <iostream>
#include <map>
#include <memory>
#include <utility>

using namespace std;
using namespace Poco;
using namespace nlohmann;


// TODO: There needs to be something better than storing a "position" index
// because the way it is now will lead to inconsistencies when you go and try
// to get trailing data
// TODO: This will probs lead to having to store the prices in some type of
// map where the key is the datetime object (it might be possible, but maybe
// unneccessary, to convert the datetime objects into integer timestamps for
// increased efficiency


TradierStockPriceSource::TradierStockPriceSource (
		const Configuration& config,
		Logger& logger,
		const TradierAccessor& accessor
	) : DataSource<float>(config, logger), _accessor(accessor) {}

pair<bool, float> TradierStockPriceSource::get (
		const DateTime& timePoint,
        const Timespan& interval,
		const string& instrument) {
	if (instrument == "") {
		throw InvalidArgumentException("Error: instrument cannot be empty");
	}
    auto instPtr = _cache.emplace(
            instrument,
            make_shared<map<DateTime, float>>()
        );
    map<DateTime, float>& instrumentCache = *(instPtr.first->second);
    //size_t& position = p.first->second.second;
    //if (position == instrumentCache.size() - 1) {
    auto cachePtr = instrumentCache.find(timePoint);
    if (cachePtr == instrumentCache.end()) {
        /* cache miss */
        cout << "CACHE MISS" << endl;
        URI uri("/markets/history");
        uri.addQueryParameter("symbol", instrument);
        uri.addQueryParameter("interval", "daily");
        string end = DateTimeFormatter::format(timePoint, "%Y-%m-%d");
        cout << "timePoint " << end << endl;
        uri.addQueryParameter("end", end);
        DateTime start(timePoint);
        for (int i = 0; i < PREFETCH_SIZE; ++i) { // 100 is arbitrary
            start -= interval;
        }
        string begin = DateTimeFormatter::format(start, "%Y-%m-%d");
        uri.addQueryParameter("start", begin);
        response_t response = _accessor.get(uri);
        int attempt = 3;
        int returnValue;
        do {
            returnValue = loadResponseDataToCache(response, instrumentCache);
        } while (returnValue == FAIL && attempt > 0);
        if (!attempt) {
            _logger << "WARNING: failed to retrieve historical price data ";
            _logger << "with HTTP status: " << response.first << endl;
            return make_pair(false, 0);
        }
        if (returnValue == NO_HIST) {
            _logger << "WARNING: no available Tradier history for '";
            _logger << instrument << "'" << endl;
            return make_pair(false, 0);
        }
        cachePtr = instrumentCache.find(timePoint);
    } else {
        cout << "CACHE HIT" << endl;
    }
    return make_pair(true, cachePtr->second);
}

int TradierStockPriceSource::loadResponseDataToCache (
        response_t response,
        map<DateTime, float>& instrumentCache) {
    if (response.first == 200) {
        json j = json::parse(response.second);
        if (j["history"].is_null()) {
            return NO_HIST;
        }
        json days = j["history"]["day"];
        int timeZoneDiff;
        for (auto it = days.rbegin(); it != days.rend(); ++it) {
            cout << (*it)["date"] << " " << (*it)["close"] << endl;
            DateTime d = DateTimeParser::parse(
                    (*it)["date"],
                    timeZoneDiff
                );
            instrumentCache.emplace(d, (*it)["close"]);
        }
        return SUCCESS;
    } else {
        return FAIL;
    }
}
