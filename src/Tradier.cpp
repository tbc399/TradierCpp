/**
 * Copyright 2017, SonicRED
 * Original Author: Travis Cammack
 * Contributors:
 */

#include <Poco/Net/NetException.h>
#include <Poco/Exception.h>
#include <Poco/URI.h>
#include <json/json.hpp>

#include <MarketOrder.hpp>
#include <LimitOrder.hpp>
#include <StopOrder.hpp>
#include <StopLimitOrder.hpp>
#include <Account.hpp>
#include <ActivePosition.hpp>
#include <OrderStatus.hpp>
#include <Configuration.hpp>
#include <Tradier.hpp>
#include <TradierAccessor.hpp>

#include <string>
#include <vector>

using namespace std;
using namespace nlohmann;
using namespace Poco::Net;
using namespace Poco;

Tradier::Tradier (
		const Configuration& config,
		const TradierAccessor& accessor
	) : _config(config), _accessor(accessor) {

	_accountId = config("TRADIER_ACCOUNT_ID");
	_accountModified = false;

}


string
Tradier::orderHelper (const string& requestBody) {

	URI uri("accounts/"+_accountId+"/orders");
	response_t response  = _accessor.post(uri, requestBody);

	if (response.first == 200) {

		_accountModified = true;
		json j = response.second;
		return j["order"]["id"];

	} else {

		throw HTTPException(
            "Error: Got HTTP status "+to_string(response.first)+" for: "
        );

	}

}


float
Tradier::commission (size_t size) {
	return stof(_config("TRADIER_COMMISSION"));
}


string
Tradier::placeBuyOrder (const MarketOrder& order) {

	string body = "class=equity"
			"&symbol="+order.symbol()+
			"&duration=gtc"
			"&side=buy"
			"&quantity="+to_string(order.size())+
			"&type=market";

	return orderHelper(body);

}


string
Tradier::placeBuyOrder (const LimitOrder& order) {

	string body = "class=equity"
			"&symbol="+order.symbol()+
			"&duration=gtc"
			"&side=buy"
			"&quantity="+to_string(order.size())+
			"&type=limit"
			"&price="+to_string(order.price());

	return orderHelper(body);

}


string
Tradier::placeBuyOrder (const StopOrder& order) {

	string body = "class=equity"
			"&symbol="+order.symbol()+
			"&duration=gtc"
			"&side=buy"
			"&quantity="+to_string(order.size())+
			"&type=stop"
			"&stop="+to_string(order.stop());

	return orderHelper(body);

}


string
Tradier::placeBuyOrder (const StopLimitOrder& order) {

	string body = "class=equity"
			"&symbol="+order.symbol()+
			"&duration=gtc"
			"&side=buy"
			"&quantity="+to_string(order.size())+
			"&type=stop_limit"
			"&price="+to_string(order.price())+
			"&stop="=order.stop();

	return orderHelper(body);

}


string
Tradier::placeSellOrder (const MarketOrder& order) {

	string body = "class=equity"
			"&symbol="+order.symbol()+
			"&duration=gtc"
			"&side=sell"
			"&quantity="+to_string(order.size())+
			"&type=market";

	return orderHelper(body);

}


string
Tradier::placeSellOrder (const LimitOrder& order) {

	string body = "class=equity"
			"&symbol="+order.symbol()+
			"&duration=gtc"
			"&side=sell"
			"&quantity="+to_string(order.size())+
			"&type=limit"
			"&price="+to_string(order.price());

	return orderHelper(body);

}


string
Tradier::placeSellOrder(const StopOrder& order) {

	string body = "class=equity"
			"&symbol="+order.symbol()+
			"&duration=gtc"
			"&side=sell"
			"&quantity="+to_string(order.size())+
			"&type=stop"
			"&stop="+to_string(order.stop());

	return orderHelper(body);

}


string
Tradier::placeSellOrder (const StopLimitOrder& order) {

	string body = "class=equity"
			"&symbol="+order.symbol()+
			"&duration=gtc"
			"&side=sell"
			"&quantity="+to_string(order.size())+
			"&type=stop_limit"
			"&price="+to_string(order.price())+
			"&stop="+to_string(order.stop());

	return orderHelper(body);

}


Account
Tradier::getAccountBalances () {

	if (!_accountModified && _cachedAccount) {
		_accountModified = false;
		return _cachedAccount;
	}

	URI uri("accounts/"+_accountId+"/balances");
	response_t response  = _accessor.get(uri);

	if (response.first == 200) {

		json j = response.second;
		return _cachedAccount = Account(
				j["total_equity"],
				j["total_cash"],
				j["cash"]["cash_available"],
				j["pending_cash"],
				j["unsettled_funds"]
			);

	} else {

		throw HTTPException(
				"Error: Got HTTP status "+to_string(response.first)+" for:"
			);

	}

}


vector<ActivePosition>
Tradier::getPositions () {

	URI uri("accounts/"+_accountId+"/positions");
	response_t response = _accessor.get(uri);

	if (response.first == 200) {

		vector<ActivePosition> positions;

		json j = response.second;
		json positionsJson = j["positions"];

		for (auto& element : positionsJson) {
			json position = element["position"];
			positions.push_back(
					ActivePosition(
						position["symbol"],
						position["id"],
						position["quantity"],
						position["cost_basis"],
						position["date_acquired"]
					)
				);
		}

		return positions;

	} else {

		throw HTTPException(
				"Error: Got HTTP status "+to_string(response.first)+" for:"
			);

	}

}


//vector<OpenOrder> Tradier::getOpenOrders () {
//
//}


OrderStatus
Tradier::checkOrderStatus (const string& id) {

	URI uri("accounts/"+_accountId+"/orders/"+id);
	response_t response = _accessor.get(uri);

	if (response.first == 200) {

		json j = response.second;
		json order = j["order"];
		string orderStatus = order["id"];

		state_t state;

		if (orderStatus == "submitted") {
			state = SUBMITTED;
		} else if (orderStatus == "open") {
			state = OPEN;
		} else if (orderStatus == "partial") {
			state = PARTIAL;
		} else if (orderStatus == "filled") {
			state = FILLED;
		} else if (orderStatus == "canceled") {
			state = CANCELED;
		} else if (orderStatus == "expired") {
			state = EXPIRED;
		} else if (orderStatus == "rejected") {
			state = REJECTED;
		} else if (orderStatus == "pending") {
			state = PENDING;
		} else {
			throw NotFoundException(
				"Error: "+orderStatus+" is not a valid order status"
			);
		}

		return OrderStatus(
			order["id"],
			state,
			order["symbol"],
			order["avg_fill_price"],
			order["exec_quantity"]
		);

	} else {

		throw HTTPException(
				"Error: Got HTTP status "+to_string(response.first)+" for:"
			);

	}

}
