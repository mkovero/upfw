//
// Created by Markus on 24.9.2020.
//
#include <regex>
#include "Processor.h"

std::string Processor::generateFWJSON(std::vector<Server>::iterator _srv, const std::string &_index) {
    jsonEntry["firewall_rule"] = {
            {"position",                  _index},
            {"direction",                 "in"},
            {"family",                    "IPv4"},
            {"protocol",                  "tcp"},
            {"source_address_start",      getSrcIP()},
            {"source_address_end",        getSrcIP()},
            {"destination_address_start", _srv->net.pub.IPv4.address},
            {"destination_address_end",   _srv->net.pub.IPv4.address},
            {"destination_port_start",    "22"},
            {"destination_port_end",      "22"},
            {"action",                    "accept"},
            {"comment",                   "SSHOpenedMagically"}};
    return jsonEntry.dump();
}

void Processor::inspect(const std::string &_server) {

    auto srv = findServer(_server);
    if (srv->hostname != "DUMMY") {

        auto[previousExists, index] = checkIfPreviousEntryExists(srv->uuid);
        if (!previousExists) {
            std::cout << _server << " got previous did not exist, index is " << index << std::endl;

            connector->updateFw(srv->uuid, generateFWJSON(srv, index));
        } else if (previousExists && index != "0") {
            std::cout << _server << " got previous exists, index is " << index << std::endl;
            connector->deleteFwRule(srv->uuid, index);
            connector->updateFw(srv->uuid, generateFWJSON(srv, index));
        }
    } else {
        std::cout << _server << " not found." << std::endl;
    }
}

retVals Processor::checkIfPreviousEntryExists(const std::string &_uuid) {

    json result = json::parse(connector->fetchFw(_uuid));
    json outerArray = result["firewall_rules"];
    json innerArray = outerArray["firewall_rule"];
    bool previousExists = false;
    std::string index = "0";
    std::string ourAddress = getSrcIP();

    for (auto&[innerKey, innerValue] : innerArray.items()) {
        json entry = innerValue;

        if (entry["comment"] == "SSHOpenedMagically") {
            previousExists = true;
            if (entry["source_address_start"] == ourAddress) {
                std::cout << "No need to update our address." << std::endl;
            } else {
                index = entry["position"];
                std::cout << "Previous entry exists with index " << index << std::endl;
            }
            break;
        }

    }
    if (!previousExists) {
        index = "1";
        std::cout << "No old entry found, using index " << index << std::endl;
    }
    return retVals{previousExists, index};
}

void Processor::processServerList() {

    Server dummyEntry;
    dummyEntry.hostname = "DUMMY";
    vm.push_back(dummyEntry);

    json restResponse = json::parse(connector->fetchServerList());
    json outerServerArray = restResponse["servers"];
    json serverListResult = outerServerArray["server"];

    json ipAddrResponse = json::parse(connector->fetchIPAddr());
    json ipAddrOuterArray = ipAddrResponse["ip_addresses"];
    json ipAddrListResult = ipAddrOuterArray["ip_address"];

    vm.resize(serverListResult.size() + ipAddrListResult.size());
    for (auto&[innerServerKey, innerServerArray] : serverListResult.items()) {
        vm.push_back(serverJsonToStruct(innerServerArray, ipAddrListResult));
    }
}

Server Processor::serverJsonToStruct(const json &_innerServerArray, const json &_ipAddrListResult) {
    Server p;

    _innerServerArray.at("uuid").get_to(p.uuid);

    _innerServerArray.at("hostname").get_to(p.hostname);
    _innerServerArray.at("title").get_to(p.title);
    _innerServerArray.at("memory_amount").get_to(p.memory_amount);

    _innerServerArray.at("plan").get_to(p.plan);
    _innerServerArray.at("plan_ipv4_bytes").get_to(p.plan_ipv4_bytes);
    _innerServerArray.at("plan_ipv6_bytes").get_to(p.plan_ipv6_bytes);

    _innerServerArray.at("simple_backup").get_to(p.simple_backup);
    _innerServerArray.at("state").get_to(p.state);

    _innerServerArray.at("zone").get_to(p.zone);
    _innerServerArray.at("core_number").get_to(p.core_number);
    _innerServerArray.at("license").get_to(p.license);
    _innerServerArray.at("created").get_to(p.created);

    for (auto&[innerNetKey, innerNetArray] : _ipAddrListResult.items()) {
        if (_innerServerArray["uuid"] == innerNetArray["server"]) {
            p.net = addrJsonToStruct(innerNetArray);
        }
    }
    return p;
}


Access Processor::addrJsonToStruct(const json &_innerNetArray) {
    Access p;
    if (_innerNetArray.at("access") == "public") {
        if (_innerNetArray.at("family") == "IPv4") {
            _innerNetArray.at("address").get_to(p.pub.IPv4.address);
            _innerNetArray.at("ptr_record").get_to(p.pub.IPv4.ptr_record);
            _innerNetArray.at("mac").get_to(p.pub.IPv4.mac);
        }
        if (_innerNetArray.at("family") == "IPv6") {
            _innerNetArray.at("address").get_to(p.pub.IPv6.address);
            _innerNetArray.at("ptr_record").get_to(p.pub.IPv6.ptr_record);
            _innerNetArray.at("mac").get_to(p.pub.IPv6.mac);
        }
    }

    if (_innerNetArray.at("access") == "utility") {
        if (_innerNetArray.at("family") == "IPv4") {
            _innerNetArray.at("address").get_to(p.util.IPv4.address);
            _innerNetArray.at("ptr_record").get_to(p.util.IPv4.ptr_record);
            _innerNetArray.at("mac").get_to(p.util.IPv4.mac);
        }
        if (_innerNetArray.at("family") == "IPv6") {
            _innerNetArray.at("address").get_to(p.util.IPv6.address);
            _innerNetArray.at("ptr_record").get_to(p.util.IPv6.ptr_record);
            _innerNetArray.at("mac").get_to(p.util.IPv6.mac);
        }
    }
    return p;
}

std::string Processor::getSrcIP() {
    std::string addressResponse = connector->fetchOurAddress();
    std::regex pattern(R"((\d{1,3}(\.\d{1,3}){3}))");
    std::smatch match;
    if (std::regex_search(addressResponse, match, pattern)) {
        return match[1];
    } else {
        return nullptr;
    }
}

void Processor::getDstIP(const std::string &_param) {
    auto cursrv = findServer(_param);
    if ((cursrv->uuid.length()) > 1) {
        std::cout << "IPv4 public Addresses for " << cursrv->hostname << " (" << cursrv->uuid << ") @ "
                  << cursrv->zone;
        for (auto &address : cursrv->net.pub.IPv4.address) {
            std::cout << address << "\n";
        }
        std::cout << "IPv6 public Addresses for " << cursrv->hostname << " (" << cursrv->uuid << ") @ "
                  << cursrv->zone;

        for (auto trooler = cursrv->net.pub.IPv6.address.begin();
             trooler != cursrv->net.pub.IPv4.address.end(); ++trooler) {
            std::cout << *trooler << "\n";
        }
        std::cout << "IPv4 utility Addresses for " << cursrv->hostname << " (" << cursrv->uuid << ") @ "
                  << cursrv->zone;
        for (auto trooler = cursrv->net.util.IPv4.address.begin();
             trooler != cursrv->net.pub.IPv4.address.end(); ++trooler) {
            std::cout << *trooler << "\n";
        }
        std::cout << "IPv6 utility Addresses for " << cursrv->hostname << " (" << cursrv->uuid << ") @ "
                  << cursrv->zone;
        for (auto trooler = cursrv->net.util.IPv6.address.begin();
             trooler != cursrv->net.pub.IPv4.address.end(); ++trooler) {
            std::cout << *trooler << "\n";
        }
    } else {
        std::cout << "not found" << "\n";
    }
}

std::vector<Server>::iterator Processor::findServer(const std::string &_param) {
    std::cout << "Searching for " << _param;
    for (auto trooler = vm.begin(); trooler != vm.end(); ++trooler) {
        if (trooler->hostname == _param) {
            std::cout << "Found!" << std::endl;
            vmIter = trooler;
            break;
        } else if (trooler->uuid == _param) {
            std::cout << "Found!" << std::endl;
            vmIter = trooler;
            break;
        } else {
            vmIter = vm.begin();
        }
        std::cout << "..";

    }
    std::cout << std::endl;
    return vmIter;
}
