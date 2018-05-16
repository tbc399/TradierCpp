/**
 * Copyright 2017, SonicRED
 * Original Author: Travis Cammack
 * Contributors:
 */

#ifndef TRADIER_H
#define TRADIER_H

#include <MarketOrder.hpp>
#include <LimitOrder.hpp>
#include <StopOrder.hpp>
#include <StopLimitOrder.hpp>
#include <Account.hpp>
#include <ActivePosition.hpp>
#include <OrderStatus.hpp>
#include <Configuration.hpp>
#include <Broker.hpp>
#include <TradierAccessor.hpp>
//#include <OpenOrder.hpp>

#include <string>
#include <vector>

using namespace std;

/**
 * An implementation of the Broker abstract class using the Tradier
 * brokerage platform.
 */
class Tradier : public Broker {

private:

	/**
	 * The configuration of the system
	 */
	const Configuration _config;

	/**
	 * An alphanumeric identifier for a Tradier brokerage account.
	 */
	string _accountId;

	/**
	 * The particular accessor/endpoint to use when interacting with the
	 * Tradier API. This will most likely just be either their sandox
	 * environment or their production environment.
	 */
	const TradierAccessor _accessor;

	/**
	 * Use this flag to help in making sensible use of the network
	 * calls to retrieve account information
	 */
	bool _accountModified;

	/**
	 * Store a snapshot of the trading account as a way to reduce the
	 * number of network calls that are unnecessary
	 */
	Account _cachedAccount;

private:

	/**
	 * This guys wraps up the bulk of the stuff used to place an order
	 */
	string orderHelper (const string& requestBody);

public:

	/**
	 * Construct an instance of the Tradier broker class that represents
	 * an account with this brokerage.
	 */
	Tradier (const Configuration& config, const TradierAccessor& accessor);

	/**
	 * Return the commissions on an equities order which at the
	 * time of writting this is a per order flat rate.
	 */
	float commission (size_t size);

	/**
	 * Place a market order to buy and return a string representing
	 * the id of the submitted order
	 */
	string placeBuyOrder (const MarketOrder& order);

	/**
	 * Place a limit order to buy and return a string representing
	 * the id of the submitted order
	 */
	string placeBuyOrder (const LimitOrder& order);

	/**
	 * Place a stop order to buy and return a string representing
	 * the id of the submitted order
	 */
	string placeBuyOrder (const StopOrder& order);

	/**
	 * Place a stop-limit order to buy and return a string representing
	 * the id of the submitted order
	 */
	string placeBuyOrder (const StopLimitOrder& order);

	/**
	 * Place a market order to sell and return a string representing
	 * the id of the submitted order
	 */
	string placeSellOrder (const MarketOrder& order);

	/**
	 * Place a limit order to sell and return a string representing
	 * the id of the submitted order
	 */
	string placeSellOrder (const LimitOrder& order);

	/**
	 * Place a stop order to sell and return a string representing
	 * the id of the submitted order
	 */
	string placeSellOrder(const StopOrder& order);

	/**
	 * Place a stop-limit order to sell and return a string representing
	 * the id of the submitted order
	 */
	string placeSellOrder (const StopLimitOrder& order);

	/**
	 * Get the various "standard" balances on an account. The account id
	 * or number would probably best be passed in via the construction
	 * of the derived subclass of Broker
	 */
	Account getAccountBalances ();

	/**
	 * Return a listing of all current positions for this account
	 */
	vector<ActivePosition> getPositions ();

	/**
	 * Return all currently open orders in the account
	 */
	//vector<OpenOrder> getOpenOrders ();

	/**
	 * Return the status of a submitted order using a string that represents
	 * the order's identifier.
	 */
	OrderStatus checkOrderStatus (const string& id);

};

#endif
