#pragma once

#include "types.h"
#include "dancer.h"

namespace disco {

class Dancer;

class Party {
public:
    void AddDancer( const Dancer& d );
    typedef vector< string > PartyGoers;
    PartyGoers WhoIsAtThisParty() const;
    
private:
    std::vector< Dancer > mDancers;
};

}
