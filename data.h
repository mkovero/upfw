//
// Created by Markus on 24.9.2020.
//

#ifndef UPFW_DATA_H
#define UPFW_DATA_H

struct IPAddr {
public:
    std::string address;
    std::string ptr_record;
    std::string mac;
};
struct Family {
public:
    IPAddr IPv4;
    IPAddr IPv6;
};
struct Access {
public:
    Family pub;
    Family util;
};


class Server {

public:
    std::string uuid;
    std::string hostname;
    std::string title;
    std::string memory_amount;
    std::string plan;
    std::string plan_ipv4_bytes;
    std::string plan_ipv6_bytes;
    std::string simple_backup;
    std::string state;
    std::string tags[255];
    std::string zone;
    std::string core_number;
    int license;
    int created;
    Access net;

};

struct retVals {
    bool b;
    std::string s;
};
#endif //UPFW_DATA_H
