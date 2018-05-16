/**
 * Copyright 2017, SonicRED
 * Original Author: Travis Cammack
 * Contributors:
 */

#include <TradierSandboxAccessor.hpp>
#include <TradierAccessor.hpp>
#include <Configuration.hpp>

TradierSandboxAccessor::TradierSandboxAccessor(const Configuration& config) :
	TradierAccessor("sandbox.tradier.com", V1) {

	_bearer = config("TRADIER_SANDBOX_BEARER");

}
