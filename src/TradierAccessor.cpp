/**
 * Copyright 2017, SonicRED
 * Original Author: Travis Cammack
 * Contributors:
 */

#include <TradierAccessor.hpp>
#include <Configuration.hpp>

#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Exception.h>
#include <Poco/URI.h>
#include <Poco/Path.h>

#include <string>
#include <utility>
#include <ostream>
#include <istream>
#include <sstream>
#include <iostream>

using namespace std;
using namespace Poco;
using namespace Poco::Net;

const string TradierAccessor::V1 = "v1";
const string TradierAccessor::BETA = "beta";

TradierAccessor::TradierAccessor (
		const string& domain,
		const string& version
	) : _domain(domain), _version(version) {}


pair<size_t, string> TradierAccessor::get (const URI& uri) const {
    Path versionPath("/");
    versionPath.append(V1);
    versionPath.append(Path(uri.getPath()));
    URI newUri(versionPath.toString());
    newUri.setQuery(uri.getQuery());
	HTTPRequest request(HTTPRequest::HTTP_GET, newUri.toString());
	request.set("Accept", "application/json");
	request.set("Authorization", "Bearer "+_bearer);
	HTTPSClientSession session(_domain);
	session.sendRequest(request);
    //session.setTimeout(Timespan());
	HTTPResponse response;
	istream& stream = session.receiveResponse(response);
	ostringstream oss;
	oss << stream.rdbuf();
	return make_pair((size_t)(response.getStatus()), oss.str());
}

pair<size_t, string> TradierAccessor::post (
		const URI& uri,
		const string& body) const {
    Path versionPath("/");
    versionPath.append(V1);
    versionPath.append(Path(uri.getPath()));
    URI newUri(versionPath.toString());
    newUri.setQuery(uri.getQuery());
	HTTPRequest request(HTTPRequest::HTTP_POST, newUri.toString());
	request.set("Accept", "application/json");
	request.set("Authorization", "Bearer "+_bearer);
	request.setContentLength(body.size());
	HTTPSClientSession session(_domain);
	ostream& requestStream = session.sendRequest(request);
	requestStream << body;
	HTTPResponse response;
	istream& stream = session.receiveResponse(response);
	ostringstream oss;
	oss << stream.rdbuf();
	return make_pair((size_t)(response.getStatus()), oss.str());
}
