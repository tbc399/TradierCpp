/**
 * Copyright 2017, SonicRED
 * Original Author: Travis Cammack
 * Contributors:
 */

#ifndef TRADIER_SANDBOX_ACCESSOR_H
#define TRADIER_SANDBOX_ACCESSOR_H

#include <TradierAccessor.hpp>
#include <Configuration.hpp>

/**
 * A Tradier accessor that hooks into the sandbox API
 */
class TradierSandboxAccessor : public TradierAccessor {

public:

	/**
	 * Default construction
	 */
	TradierSandboxAccessor (const Configuration& config);

};

#endif
