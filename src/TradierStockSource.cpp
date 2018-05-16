/**
 * Copyright 2017, SonicRED
 * Original Author: Travis Cammack
 * Contributors:
 */

#include <Configuration.hpp>
#include <Logger.hpp>
#include <TradierStockSource.hpp>
#include <InstrumentSource.hpp>
#include <TradierAccessor.hpp>

#include <Poco/URI.h>
#include <Poco/Net/NetException.h>
#include <json/json.hpp>

#include <iostream>
#include <unordered_set>
#include <utility>
#include <string>
#include <regex>

using namespace std;
using namespace Poco;
using namespace Poco::Net;
using namespace nlohmann;

TradierStockSource::TradierStockSource(
		const Configuration& config,
		Logger& logger,
		const TradierAccessor& accessor
	) : InstrumentSource(config, logger), _accessor(accessor) {}

void TradierStockSource::fetch (unordered_set<string>& names) {
    regex r("[A-Z]+");
	for (const char& c : string("A")) {//"ABCDEFGHIJKLMNOPQRSTUVWXYZ") {
		URI uri("markets/lookup");
		uri.addQueryParameter("q", string(1, c));
        /* hardcode NASDAQ (Q) and NYSE (N) */
		uri.addQueryParameter("exchanges", "Q,N");
		uri.addQueryParameter("types", "stock");
		response_t response = _accessor.get(uri);
		if (response.first == 200) {
			json j = json::parse(response.second);
			for (const json& security : j["securities"]["security"]) {
                string name = security["symbol"];
                if (regex_match(name, r)) {
                    names.emplace(name);
                }
			}
		} else {
            _logger << "WARNING: failed to retrieve stock symbols starting "
                    "with '" + string(1, c) + "' with HTTP "
                    "status " + to_string(response.first);
		}
	}
}
