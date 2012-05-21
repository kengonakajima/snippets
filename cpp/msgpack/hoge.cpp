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

    msgpack::unpacker unpac;
    unpac.reserve_buffer(32*1024);
    assert( unpac.buffer_capacity() > outsz );
    memcpy( unpac.buffer(), out, outsz );
    unpac.buffer_consumed(outsz);

    msgpack::unpacked result;

    while( unpac.next( &result ) ) {
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


    // packer
    {
        msgpack::sbuffer sb;
        msgpack::packer< msgpack::sbuffer > pk( & sb );
        pk.pack_array(3);
        pk.pack(10);
        pk.pack(20);
        pk.pack(10000000000);

        char *packed = sb.data();
        size_t packedsz = sb.size();

        std::cerr << "packedsz:" << packedsz << std::endl;
        msgpack::unpacker unp;
        unp.reserve_buffer(32*1024);
        memcpy( unp.buffer(), packed, packedsz );
        unp.buffer_consumed(packedsz);

        std::cerr << "before-next: offset:" << unp.off << " parsed:" << unp.parsed << std::endl;
        
        msgpack::unpacked result;
        assert( unp.next(&result) );

        std::cerr << "after-next: offset:" << unp.off << " parsed:" << unp.parsed << std::endl;
        msgpack::object obj = result.get();
        

        assert( obj.type == MSGPACK_OBJECT_ARRAY );
        assert( obj.via.array.size == 3 );

        msgpack::object e0, e1, e2;
        e0 = obj.via.array.ptr[0];
        e1 = obj.via.array.ptr[1];
        e2 = obj.via.array.ptr[2];
        assert( e0.type == MSGPACK_OBJECT_POSITIVE_INTEGER );
        assert( e1.type == MSGPACK_OBJECT_POSITIVE_INTEGER );
        assert( e2.type == MSGPACK_OBJECT_POSITIVE_INTEGER );
        assert( e0.as<int>() == 10 );
        assert( e1.as<int>() == 20 );
        assert( e2.as<long long>() == 10000000000 );
    }
}
    
