//
// Created by Markus on 24.9.2020.
//

#ifndef UPFW_JSON_H
#define UPFW_JSON_H

#include <nlohmann/json.hpp>
#include <utility>
#include "Connector.h"
#include "data.h"

using json = nlohmann::json;

class Processor {
public:
    const std::string TAG = "Processor";

    Processor(std::shared_ptr<Connector> _connector) {
        connector = std::move(_connector);
        processServerList();
    }

    ~Processor() {

    }

    void inspect(const std::string &_server);
    void getDstIP(const std::string &);

private:
    std::shared_ptr<Connector> connector;
    std::vector<Server> vm;
    std::vector<Server>::iterator vmIter;

    void processServerList();
    std::vector<Server>::iterator findServer(const std::string &);
    retVals checkIfPreviousEntryExists(const std::string &);
    std::string getSrcIP();
    std::string generateFwJson(std::vector<Server>::iterator, const std::string &);

    json jsonEntry;
    static Server serverJsonToStruct(const json &, const json &);
    static Access addrJsonToStruct(const json &);
};

#endif //UPFW_JSON_H
