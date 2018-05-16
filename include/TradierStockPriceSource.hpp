/**
 * Copyright 2017, SonicRED
 * Original Author: Travis Cammack
 * Contributors:
 */

#ifndef TRADIER_STOCK_PRICE_SOURCE_H
#define TRADIER_STOCK_PRICE_SOURCE_H

#include <DataSource.hpp>
#include <Configuration.hpp>
#include <Logger.hpp>
#include <TradierAccessor.hpp>

#include <Poco/DateTime.h>
#include <Poco/Timespan.h>
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <utility>

/* The size (in days) to grab historical prices for the given name */
#define PREFETCH_SIZE 10

/* return values for loadResponseDataToCache */
#define SUCCESS 0
#define FAIL 1
#define NO_HIST 2

using namespace std;
using namespace Poco;

// TODO: Don't really know how to make this more general
// to anything other than a daily interval
class TradierStockPriceSource : public DataSource<float> {

private:

	TradierAccessor _accessor;

    /**
     * This guy is used as a cache to store prefetched price data
     * to save the network time of going to get every single price point
     */
    map<string, shared_ptr<map<DateTime, float>>> _cache;

private:

    int loadResponseDataToCache (
            response_t response,
            map<DateTime, float>& instrumentCache
        );

public:

	TradierStockPriceSource (
			const Configuration& config,
			Logger& logger,
			const TradierAccessor& accessor
		);

	pair<bool, float> get (
			const DateTime& timePoint,
            const Timespan& interval,
			const string& instrument = ""
		);

};

#endif
