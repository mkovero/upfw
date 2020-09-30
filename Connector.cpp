//
// Created by Markus on 24.9.2020.
//

#include "Connector.h"

std::string Connector::basicGet(const std::string &_uri) {
    sendHeadersForGet();
    RestClient::Response r = conn->get(_uri);
    return r.body;
}


void Connector::basicPost(const std::string &uri, const std::string& _payload) {
    sendHeadersForPost();
    RestClient::Response p = conn->post(uri, _payload);
}

void Connector::basicDelete(const std::string &_uri) {
    sendHeadersForPost();
    RestClient::Response p = conn->del(_uri);
}

void Connector::sendHeadersForGet() {
    RestClient::HeaderFields headers;
    headers["Accept"] = "application/json";
    conn->SetHeaders(headers);
}

void Connector::sendHeadersForPost() {
    RestClient::HeaderFields headers;
    headers["Accept"] = "application/json";
    headers["Content-Type"] = "application/json";
    conn->SetHeaders(headers);
}

std::string Connector::getEnv(const std::string &var) {
    const char *val = std::getenv(var.c_str());
    if (val == nullptr) { // invalid to assign nullptr to std::string
        return "";
    } else {
        return val;
    }
}

std::string Connector::fetchFw(const std::string &_uuid) {
    std::string _uri = "/1.3/server/" + _uuid + "/firewall_rule";
    return basicGet(_uri);
}

std::string Connector::fetchServerList() {
    std::string _uri = "/1.3/server";
    return basicGet(_uri);
}

std::string Connector::fetchIPAddr() {
    std::string _uri = "/1.3/ip_address";
    return basicGet(_uri);
}

void Connector::updateFw(const std::string &_uuid, const std::string _payload) {
    std::string _uri = "/1.3/server/" + _uuid + "/firewall_rule";
    basicPost(_uri, _payload);
}

void Connector::deleteFwRule(const std::string &_uuid, const std::string &_position) {
    std::string _uri = "/1.3/server/" + _uuid + "/firewall_rule/" + _position;
    basicDelete(_uri);
}

std::string Connector::fetchOurAddress() {
    RestClient::Connection IPconn = RestClient::Connection("http://ip.nebula.fi/");
    IPconn.SetTimeout(5);
    RestClient::HeaderFields headers;
    headers["Accept"] = "text/html";
    IPconn.SetHeaders(headers);
    IPconn.FollowRedirects(true);

    RestClient::Response IPr = IPconn.get("/");
    return IPr.body;
}


