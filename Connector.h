//
// Created by Markus on 24.9.2020.
//

#ifndef UPFW_CONNECTOR_H
#define UPFW_CONNECTOR_H

#include "restclient-cpp/connection.h"
#include "restclient-cpp/restclient.h"
#include <iostream>


class Connector {
public:
    const std::string TAG = "Connector";

    Connector() {
        RestClient::init();
        conn = std::make_shared<RestClient::Connection>(getEnv("UPC_HOST"));
        conn->SetBasicAuth(getEnv("UPC_USER"), getEnv("UPC_PWD"));
        conn->SetTimeout(5);
    }

    ~Connector() {
        RestClient::disable();
    }

    static Connector &getInstance() {
        static Connector instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    Connector (Connector const &) = delete;

    void operator=(Connector const &) = delete;

    std::string basicGet(const std::string &);
    void basicPost(const std::string &, const std::string& );
    void basicDelete(const std::string &);

    void sendHeadersForGet();
    void sendHeadersForPost();

    std::string fetchFw(const std::string &);
    void updateFw(const std::string &, const std::string);
    void deleteFwRule(const std::string &, const std::string &);

    std::string fetchServerList();
    std::string fetchIPAddr();

    std::string fetchOurAddress();

private:
    std::shared_ptr<RestClient::Connection> conn;
    static std::string getEnv(const std::string &);
};


#endif //UPFW_CONNECTOR_H
