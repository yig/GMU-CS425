#include <iostream>

// Need a forward reference, because RoomManager is declared before Organizer.
class Organizer;

class RoomManager {
public:
    // Can't do this, because curly braces are too late. We need to initialize
    // the reference in our initializer list.
    // RoomManager( Organizer& anOrg ) { org = anOrg; }
    // This works:
    RoomManager( Organizer& anOrg ) : org( anOrg ) {}
    
    Organizer& org;
};

class Organizer {
public:
    RoomManager rooms;
    
    Organizer() : rooms( *this ) {}
};

int main(int argc, char *argv[]) {
    Organizer o;
}
