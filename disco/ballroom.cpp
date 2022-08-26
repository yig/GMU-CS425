#include "ballroom.h"

namespace disco {

void Party::AddDancer( const Dancer& d ) {
    mDancers.push_back( d );
}
Party::PartyGoers Party::WhoIsAtThisParty() const {
    // Declare our output
    std::vector< std::string > result;
    result.reserve( mDancers.size() );
    
    // Iterate over the dancers
    for( const auto& d : mDancers ) {
        // Add their names to the result
        result.push_back( d.name );
    }
    
    return result;
}

}
