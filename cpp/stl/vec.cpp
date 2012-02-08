#include <iostream>
using namespace std;

template <typename T>
class Vector
{
	public:
	Vector(unsigned n = 8){
		siz = n;
		data = new T[siz];
	}
	Vector(const Vector<T>& v) {
		siz = v.siz;
		data = new T[siz];
		copy(v);
	}
	~Vector(){
		delete[] data;
	}
	Vector<T>& operator= ( const Vector<T>& );
	T& operator[](unsigned i) const {
		return data[i];
	}
	unsigned size(){
		return siz;
	}
	protected:
	T* data;
	unsigned siz;
	void copy(const Vector<T>& );
};

template<typename T>
void Vector<T>::copy(const Vector<T>& v)
{
	unsigned min_siz = (siz < v.siz ? siz: v.siz );
	for( int i = 0;i<min_siz;i++){
		data[i] = v.data[i];
	}
}

int main()
{
	Vector<short> v;
	v[5] = 1234;
	Vector<short> w = v, x(3);
	cout << w.size();
	return 0;
}