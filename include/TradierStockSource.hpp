/**
 * Copyright 2017, SonicRED
 * Original Author: Travis Cammack
 * Contributors:
 */

#ifndef TRADIER_STOCK_SOURCE_H
#define TRADIER_STOCK_SOURCE_H

#include <InstrumentSource.hpp>
#include <Configuration.hpp>
#include <Logger.hpp>
#include <TradierAccessor.hpp>

#include <unordered_set>
#include <string>

using namespace std;

class TradierStockSource : public InstrumentSource {

private:

	TradierAccessor _accessor;

public:

	TradierStockSource (
			const Configuration& config,
			Logger& logger,
			const TradierAccessor& accessor
		);

	/**
	 *
	 */
	void fetch (unordered_set<string>& names);

};

#endif
