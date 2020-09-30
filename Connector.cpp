//
// Created by Markus on 24.9.2020.
//

#include "Connector.h"

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
    sendHeadersForGet();
    RestClient::Response r = conn->get("/1.3/server/" + _uuid + "/firewall_rule");
    return r.body;
}

std::string Connector::fetchServerList() {
    sendHeadersForGet();
    RestClient::Response r = conn->get("/1.3/server");
    return r.body;
}

std::string Connector::fetchIPAddr() {
    sendHeadersForGet();
    RestClient::Response r = conn->get("/1.3/ip_address");
    return r.body;
}
void Connector::updateFw(std::string &_uuid, std::string _payload) {
    sendHeadersForPost();
    RestClient::Response p = conn->post("/1.3/server/" + _uuid + "/firewall_rule", _payload);

}
void Connector::deleteFwRule(std::string &_uuid, std::string _position) {
    sendHeadersForPost();
    RestClient::Response d = conn->del("/1.3/server/" + _uuid + "/firewall_rule/" + _position);
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


