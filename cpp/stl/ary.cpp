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
};

template<typename T> 
class Matrix 
{ 
    public: 
        Matrix(unsigned r=1, unsigned c = 1) : row(r) 
        {for (int i=0; i < r; i++) row[i] = new Vector<T>(c);} 
        ~Matrix() {for (int i=0; i < row.size(); i++) delete row[i];} 
        Vector<T>& operator[] (unsigned i) const { return *row[i];} 
        unsigned rows() {return row.size();} 
        unsigned columns() { return row[0] -> size();} 
    protected: 
        Vector<Vector<T>* > row; 
};

template <typename T>
class Iterator
{
	public:
	virtual void reset() = 0;
	virtual T operator() () = 0;
	virtual void operator=(T t) = 0;
	virtual bool operator!() = 0;
	virtual int operator ++() = 0; 
	
};

template <typename T>
class ListNode
{
//	friend class List<T>;
	public:
	ListNode(T& t, ListNode<T>*p ) : data(t), next(p){}
	ListNode<T>* getNext()
	{
		return next;
	}
	protected:
	T data;
	ListNode * next;
};


template <typename T>
class List
{
	public:
	List() : first(0){}
	~List();
	void insert(T t);
	int shift(T& t);
	bool isEmpty(){ return (first == 0); }
	void print();
	protected:
	ListNode<T> * first;
	ListNode<T> * newNode( T& t, ListNode<T> *p )
	{
		ListNode<T>* q = new ListNode<T>(t,p); 
		return q; 
	}
};
template <typename T>
List<T>::~List()
{
	ListNode<T> *temp;
	for(ListNode<T>*p = first; p; ){
		temp = p;
		p = p->getNext();
		delete temp;
	}
}

template <typename T>
void List<T>::insert( T t)
{
	ListNode<T> *p = newNode(t, first );
	first = p;
}

template <typename T>
int List<T>::shift(T& t)
{
	if( isEmpty()) return 0;
	t = first->data;
	ListNode<T> *p = first;
	first = first->next();
	delete p;
	return 1;
}



template <typename T>
class ListIter : public Iterator<T>
{
	public:
	ListIter<T> (ListIter<T>& l ): list(l)
	{
		reset(); 
	}
	virtual void reset()
	{
		previous = 0; current = list.first;
	}
	virtual T operator()() 
	{
		return current->data; 
	}
	virtual void operator=(T t)
	{
		current->data = t;
	}
	virtual bool operator!();
	virtual int operator++();
	void insert(T t);
	void preInsert( T t);
	void shift();
	
	protected:
	ListNode<T> * current;
	ListNode<T> * previous;
	ListNode<T>& list;
};

template <typename T>
bool ListIter<T>::operator!()
{
	if( current == 0 ){
		if( previous == 0 ) {
			current = list.first;
		} else {
			current = previous->next;
		}
	}
	return ( current != 0 );
}
template <typename T>
int ListIter<T>::operator++()
{
	if( current == 0 ){
		if( previous == 0 ){
			current = list.first;
		} else {
			current = previous->next;
		}
	} else {
		previous = current;
		current = current->next;
	}
	return (current != 0 );
}
template <typename T>
void ListIter<T>::insert(T t)
{
	ListNode<T>*p = list.newNode(t,0);
	if( list.isEmpty()){
		list.first = p;
	} else {
		p->next = current->next;
		current->next = p;
	}
}

template <typename T>
void ListIter<T>::preInsert( T t)
{
	ListNode<T>*p = list.newNode( t, current );
	if( current == list.first ){
		list.first = previous = p;
	} else {
		previous->next = p;
	}
}

template <typename T>
void ListIter<T>::shift()
{
	if(current==list.first){
		list.first  = current->next;
	} else {
		previous->next = current->next;
	}
	delete current;
	current = 0;
}


int main()
{
	Vector<short> v;
	v[5] = 1234;
	Vector<short> w = v, x(3);
	cout << w.size() << endl;
	
	//
	Matrix<float> a(2,3);
	a[0][0] = 0.0f;
	a[1][0] = 1.0f;
	
	cout << a.columns() << endl;
	
	//
	List<string> f;
	f.insert( "row, tom");
	f.insert( "hack them");
	f.insert( "poo foo" );
	f.insert( "ccmcmc" );

	ListIter<string> it(f);
	for(it.reset();!it; ++it ){
		it = "{" + it() + "}";
		cout << "it:" << it() << endl;
	}

	
	
	// Array x(1,3);
	// x[1] = 99;
	// x[2] = 111;
	// x[3] = 1234;
	// cout << x.size() << endl;
	// cout << x.firstSubscript() << endl;
	// cout << x.lastSubscript() << endl;
	// for(int i= 1;i<=3;i++){
	// 	cout << x[i] << endl;
	// }
	return 0;
}