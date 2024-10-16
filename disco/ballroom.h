#pragma once

#include "types.h"
#include "dancer.h"

namespace disco {

struct Dancer;

class Party {
public:
    void AddDancer( const Dancer& d );
    typedef vector< string > PartyGoers;
    PartyGoers WhoIsAtThisParty() const;
    
private:
    vector< Dancer > mDancers;
};

}
