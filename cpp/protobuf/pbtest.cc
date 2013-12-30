#include "game.pb.h"

int main(int argc, char **argv ) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    int n = 1000000;
    
    for(int i=0;i<n;i++){
        UpdatePos u;
        Vec2 *location = new Vec2();
        location->set_x(123+n);
        location->set_y(234+n);
        u.set_allocated_location(location);
        u.set_motion_type(33933+n);
        u.set_char_id(1999+n);

        int sz = u.ByteSize();
        char bytes[1000];
        assert( sizeof(bytes) >= sz );
        bool r = u.SerializeToArray( bytes, sz );
        assert(r);
#if 0
        UpdatePos ud;
        ud.ParseFromArray( bytes, sz );
        
        assert( ud.location().x() == 123+n );
#endif        
    }
}
