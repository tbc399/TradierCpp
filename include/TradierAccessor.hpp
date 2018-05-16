/**
 * Copyright 2017, SonicRED
 * Original Author: Travis Cammack
 * Contributors:
 */

#ifndef TRADIER_ACCESSOR_H
#define TRADIER_ACCESSOR_H

#include <Configuration.hpp>

#include <Poco/Net/HTTPRequest.h>
#include <Poco/URI.h>

#include <string>
#include <utility>
#include <utility>

typedef pair<size_t, string> response_t;

using namespace std;
using namespace Poco;
using namespace Poco::Net;

/**
 * This class acts as a base level point of access to the Tradier API.
 * It's biggest advantage is that it can wrap up the sub-domain of the
 * API in order to make it easier to swap back and forth between
 * the sandbox and production environments, for example.
 */
class TradierAccessor {

private:

	/**
	 * The full domain that determines which API environment is used
	 */
	const string _domain;

	/**
	 * The Tradier API verion to use
	 */
	const string _version;

protected:

	/**
	 * The access token
	 */
	string _bearer;

protected:

	/**
	 * The Tradier API version values
	 */
	static const string V1;
	static const string BETA;

protected:

	/**
	 * A protected constructor to specify the basic components for access
	 * to the API
	 */
	TradierAccessor (
			const string& domain,
			const string& version
		);

public:

	/**
	 * Make a GET call to the API and retrieve the response and response code
	 */
	response_t get (const URI& uri) const;

	/**
	 * Make a POST call to the API and retrieve the response and response code
	 */
	response_t post (const URI& uri, const string& body = "") const;

};

#endif
