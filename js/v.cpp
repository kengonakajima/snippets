#include <iostream>
class Vector3 {
 public:
    float x,y,z;
    Vector3(float _x,float _y,float _z) : x(_x),y(_y),z(_z) {
    }
    Vector3* translate( Vector3 *v ) {
        this->x += v->x;
        this->y += v->y;
        this->z += v->z;
        return this;
    }
};

static const int NLOOP=10000;
static const int NVEC=10000;
Vector3 *ary[NVEC];
Vector3 *ary1[NVEC];

int main(int argc,char**argv){
    for(int i=0;i<NVEC;i++){
        ary[i] = new Vector3(0,0,0);
        ary1[i] = new Vector3(1,1,1);
    }
    for(int i=0;i<NLOOP;i++){
        for(int j=0;j<NVEC;j++){
            ary[j]->translate( ary1[j] );
        }
    }
    Vector3*v = ary[NLOOP/2];
    std::cerr << v->x << v->y << v->z << std::endl;
}
