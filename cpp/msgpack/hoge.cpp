#include <iostream>
#include <assert.h>

#include <msgpack.hpp>

int main() {
    msgpack::sbuffer sb;
    msgpack::pack( &sb, (int)10);
    msgpack::pack( &sb, (int)20);
    msgpack::pack( &sb, (int)20000000);    

    char out[1000];
    size_t outsz = sb.size();
    memcpy( out, sb.data(), outsz );
    assert(outsz==(1+1+5));
    std::cerr << "outsz:" << outsz << std::endl;


    // 7バイトしかないのに20バイト読むと例外が発生するのでダメ
    // msgpack::zone z;
    // msgpack::object obj = msgpack::unpack( out, 20, z );

    msgpack::unpacker pac;
    pac.reserve_buffer(32*1024);
    assert( pac.buffer_capacity() > outsz );
    memcpy( pac.buffer(), out, outsz );
    pac.buffer_consumed(outsz);

    msgpack::unpacked result;

    while( pac.next( &result ) ) {
        std::cerr << "unpacked!" << std::endl;
        msgpack::object obj = result.get();
        std::auto_ptr<msgpack::zone> z = result.zone();
        std::cerr << "obj type:" << obj.type << std::endl;
        switch( obj.type ) {
        case MSGPACK_OBJECT_NIL:
            std::cerr << "nil" << std::endl;
            break;
        case MSGPACK_OBJECT_BOOLEAN:
            std::cerr << ((int)obj.convert()) << std::endl;
            break;
        case MSGPACK_OBJECT_POSITIVE_INTEGER:
        case MSGPACK_OBJECT_NEGATIVE_INTEGER:
            std::cerr << ((long long)obj.convert()) << std::endl;
            break;
        case MSGPACK_OBJECT_DOUBLE:
        case MSGPACK_OBJECT_RAW:
        case MSGPACK_OBJECT_ARRAY:
        case MSGPACK_OBJECT_MAP:
        default:
            assert(false);
        }
            
        
    }

}
    
