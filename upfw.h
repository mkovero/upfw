//
// Created by Markus on 23.9.2020.
//

#ifndef UPFW_MGR_H
#define UPFW_MGR_H

#include "Processor.h"


class UpFW {
    const std::string TAG = "UpFW";

public:
    UpFW(const std::string &_server) {
        addRule(_server);
    }

    ~UpFW() {
    }

private:
    inline void addRule(const std::string &_server) {
        processor->inspect(_server);
    }

    std::shared_ptr<Connector> connector = std::make_shared<Connector>();
    std::shared_ptr<Processor> processor = std::make_shared<Processor>(connector);
};


#endif //UPFW_MGR_H
