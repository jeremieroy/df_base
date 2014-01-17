#pragma once
#include <assert.h>
#include <string.h>
#include <stdint.h>

namespace df
{

//#define alignof __alignof
/// a generic vector like container that guarantee move semantic on reserve, resize, insert and remove
/// constructor are only called when a new object is created, and destructor when objects are destroyed
template<class T, uint32_t ALIGNMENT = 4> //= alignof(T)>
class Array
{
	static const uint32_t MINIMAL_SIZE = 8;
public:
	Array();
	Array(uint32_t reserved_size);
	~Array();
	Array(const Array& src);
	Array& operator=(const Array& other);

	uint32_t size() const { return _size ; }
	uint32_t reserved_size() const { return _reserved_size; }
	
	/// Makes sure that the array has at least the specified capacity. (If not, the array is grown.)
	void reserve(uint32_t reserved_size);

	/// Changes the size of the array (does not reallocate memory unless necessary).
	void resize(uint32_t new_size);
	void resize(uint32_t new_size, const T& default_value);
	
	//set size to 0 and free memory (reserved size = 0)
	void clear();	
	/// Trims the array so that its capacity matches its size.
	void trim();

	// *** element operations ***	
	T& operator[](uint32_t idx)             { assert(idx < _size); return _data[idx]; }
	const T& operator[](uint32_t idx) const { assert(idx < _size); return _data[idx]; }
	//just for convenience
	T& operator[](int idx)             { assert(idx >= 0); return operator[](uint32_t(idx)); }
	const T& operator[](int idx) const { assert(idx >= 0); return operator[](uint32_t(idx)); }
	
	/// Used to iterate over the array.
	T* begin() { return _data; }
	T* end()   { return _data + _size; }
	const T* begin() const  { return _data; }
	const T* end() const    { return _data + _size; }
	  		
	/// Pushes the item to the end of the array.
	void push_back(const T& value);
	/// Pops the last item from the array. The array cannot be empty.
	void pop_back();

	//doesn't call destructor and only call constructor on newly created element
	void insert(const T& value, uint32_t idx);
	void insert(const T& value, uint32_t idx, uint32_t count);
	void insert(const T* values, uint32_t idx, uint32_t count);

	void remove(uint32_t idx);
	//remove count elements, starting from idx
	void remove(uint32_t idx, uint32_t count);
	//Swaps element index with the last element and shrink by 1
	void unsorted_remove(uint32_t idx);
	
protected:
	void* allocate(uint32_t size);
	void deallocate(void* alignedPtr);

	uint32_t _size;
	uint32_t _reserved_size;
	T* _data;
};

template<class T, size_t ALIGNMENT> 
inline void* Array<T, ALIGNMENT>::allocate(uint32_t size)
{
    const size_t pointerSize = sizeof(void*) + sizeof(size_t);
    const size_t requestedSize = size + ALIGNMENT - 1 + pointerSize;
    void* raw = std::malloc(requestedSize);
    void* start = (char*)raw + pointerSize;
    void* aligned = (void*)(((size_t)((char*)start+ALIGNMENT-1)) & ~(ALIGNMENT-1));  
   	*(size_t*)((char*)aligned-pointerSize) = size;
	 *(void**)((char*)aligned-sizeof(void*)) = raw;

	//_allocated_size+=size;
	//++_allocations_count;
	return aligned;	
}

template<class T, size_t ALIGNMENT>
inline void Array<T, ALIGNMENT>::deallocate(void* alignedPtr)
{
	assert(alignedPtr !=NULL);

	//_allocated_size-=allocated_size(aligned);
	//--_allocations_count;
	void* raw = *(void**)((char*)alignedPtr-sizeof(void*));
	std::free(raw);	
}

template<class T, size_t ALIGNMENT> 
inline Array<T, ALIGNMENT>::Array() : _size(0), _reserved_size(0), _data(NULL)
{		
}

template<class T, size_t ALIGNMENT> 
inline Array<T, ALIGNMENT>::Array(uint32_t reserved_size) : _size(0)
{
	reserved_size = (reserved_size > MINIMAL_SIZE) ? reserved_size : MINIMAL_SIZE;
	_data = (T*) allocate(reserved_size * sizeof(T));	
	_reserved_size = reserved_size;
}

template<class T, size_t ALIGNMENT> 
inline Array<T, ALIGNMENT>::Array(const Array& other)  : _size(0), _reserved_size(0), _data(NULL)
{
	resize(other._size);

	T* end_ptr = _data + _size;
	T* other_ptr = other._data;
	for (T* ptr = _data; ptr < end_ptr; ++ptr, ++other_ptr) {
		new (ptr) T(*other_ptr);
	}
}

template<class T, size_t ALIGNMENT> 
inline Array<T, ALIGNMENT>&  Array<T, ALIGNMENT>::operator=(const Array& other) 
{
	// ensure we don't try to assign array to itself  
	assert(this != &other);		
	resize(other._size);
	for (size_t i = 0; i < _size; i++) {
		_data[i] = other._data[i];
    }
    return *this;
}

template<class T, size_t ALIGNMENT> 
inline Array<T, ALIGNMENT>::~Array()
{
	if(_data!=NULL)
	{
		// Invoke the destructors on the elements
		for (size_t i = 0; i < _size; i++) {
			(_data + i)->~T();
		}
		deallocate(_data);
		// Set to 0 in case this Array is global and gets referenced during app exit
		_data = NULL;
		_size = 0;
		_reserved_size = 0;
	}
}

template<class T, size_t ALIGNMENT> 
void Array<T, ALIGNMENT>::reserve(size_t new_size)
{
	// Grow the underlying array if necessary
	if (new_size > _reserved_size) 
	{
		if (_reserved_size == 0) 
		{
			// First allocation; grow to exactly the size requested to avoid wasting space.
			_data = (T*) allocate(new_size * sizeof(T));
			_reserved_size = new_size;
        } else 
		{
			// Increase the underlying size of the array.  Grow aggressively
			// up to 64k, less aggressively up to 400k, and then grow relatively
			// slowly (1.5x per resize) to avoid excessive space consumption.
			// These numbers are tweaked according to performance tests.
			// strategy taken from G3D::Arrayxx

			size_t old_size_bytes = _reserved_size * sizeof(T);
			size_t new_reserved_size = (_reserved_size * 3);
			if (old_size_bytes > 400000) {
				new_reserved_size >>= 1; // Avoid bloat ( *= 1.5)
			} else if (old_size_bytes > 64000) {
				new_reserved_size = _reserved_size << 1; // classic *= 2
			}

			//ensure minimal size
			if(new_reserved_size < MINIMAL_SIZE) new_reserved_size = MINIMAL_SIZE;
			else if(new_reserved_size < new_size) new_reserved_size = new_size;

			//reallocate
			T* old_data = _data;
			_data = (T*) allocate(new_reserved_size * sizeof(T));	
			_reserved_size = new_reserved_size;

			memcpy(_data, old_data, _size * sizeof(T));
		
			//deallocate
			deallocate(old_data);
		}
	}
}


template<class T, size_t ALIGNMENT> 
void Array<T, ALIGNMENT>::resize(size_t new_size)
{
    if (new_size == _size) {
        return;
    }

	if(new_size < _size)
	{	
		// Call destructor on newly hidden elements if there are any
		const T* end_ptr = _data+_size;
		for(T* ptr = _data+new_size; ptr < end_ptr; ++ptr) {
			ptr->~T();
		}
	}else
	{
		//ensure there is enough place for the leftovers
		reserve(new_size);
        // Call the constructors on newly revealed elements.
		const T* end_ptr = _data+new_size;
		for(T* ptr = _data+_size; ptr < end_ptr; ++ptr) {
			new (ptr) T();
		}
	}
	_size = new_size;
}


template<class T, size_t ALIGNMENT> 
void Array<T, ALIGNMENT>::resize(size_t new_size, const T& default_value)
{
    if (new_size == _size) {
        return;
    }

	if(new_size < _size)
	{	
		// Call the destructors on newly hidden elements if there are any
		const T* end_ptr = _data+_size;
		for(T* ptr = _data+new_size; ptr < end_ptr; ++ptr) {		
			ptr->~T();
		}
	}else
	{
		//ensure there is enough place for the leftovers
		reserve(new_size);
        // Call the constructors on newly revealed elements.
		const T* end_ptr = _data+new_size;
		for(T* ptr = _data+_size; ptr < end_ptr; ++ptr)	{
			new (ptr) T(default_value);
		}
	}
	_size = new_size;
}

template<class T, size_t ALIGNMENT> 
void Array<T, ALIGNMENT>::clear()
{	
	const T* end_ptr = _data+_size;
	for(T* ptr = _data; ptr < end_ptr; ++ptr){
		ptr->~T();
	}
	deallocate(_data);
	_data = NULL;
	_size = 0;
	_reserved_size = 0;
}

template<class T, size_t ALIGNMENT> 
void Array<T, ALIGNMENT>::push_back(const T& value)
{
	if (_size < _reserved_size) {
        new (_data + _size) T(value);
    } else if ( (&value >= _data) && (&value < _data + _size) )
	{
		// this is a reference to a data inside the array
		// if we reallocate the array we may invalidate the reference
		// push a copy instead
        T tmp = value;
		reserve(_size + 1);
		new (_data + _size) T(tmp);
    } else { 
        reserve(_size + 1);
		new (_data + _size) T(value);
    }
	++_size;
}

template<class T, size_t ALIGNMENT> 
void Array<T, ALIGNMENT>::pop_back()
{
	assert(_size>0);	
	(_data+_size-1)->~T();
	--_size;
}



template<class T, size_t ALIGNMENT> 
void Array<T, ALIGNMENT>::insert(const T& value, size_t idx)
{
	assert(idx <= _size);
	if ( (&value >= _data) && (&value < _data + _size) )
	{
		// this is a reference to a data inside the array
		// if we reallocate the array we may invalidate the reference
		// push a copy instead
		T tmp = value;
		reserve(++_size);
		memmove(_data+idx+1, _data+idx, (_size-idx) * sizeof(T));
		
		new (_data + idx) T(tmp);
	}else
	{
		reserve(++_size);
		memmove(_data+idx+1, _data+idx, (_size-idx) * sizeof(T));
				
		new (_data + idx) T(value);
	}
    
}

template<class T, size_t ALIGNMENT> 
void Array<T, ALIGNMENT>::insert(const T& value, uint32_t idx, uint32_t count)
{
	assert(idx <= _size);
	//TODO manage inner reference
	
	reserve(_size + count);	
	_size+=count;
	memmove(_data+idx+count, _data+idx, (_size-idx -count) * sizeof(T));
	
	const T* end_ptr = _data+idx+count;
	for(T* ptr = _data+idx; ptr < end_ptr; ++ptr)
	{
			new (ptr) T(value);
	}
	
}

template<class T, size_t ALIGNMENT> 
void Array<T, ALIGNMENT>::insert(const T* values, uint32_t idx, uint32_t count)
{
	assert(idx <= _size);
	//TODO manage inner reference
	
	reserve(_size + count);	
	_size+=count;
	memmove(_data+idx+count, _data+idx, (_size-idx -count) * sizeof(T));
	memcpy(_data+idx, values, count * sizeof(T));
	_size+=count;
}

template<class T, size_t ALIGNMENT> 
void Array<T, ALIGNMENT>::remove(size_t idx)
{
	assert(idx >= 0);
	assert(idx < _size);
	
	(_data+idx)->~T();
	memmove(_data+idx, _data+idx+1, (_size-idx-1) * sizeof(T));
	
	--_size;
}

template<class T, size_t ALIGNMENT> 
void Array<T, ALIGNMENT>::remove(size_t idx, size_t count)
{
	assert((idx >= 0) && (idx < _size));	
	assert((count > 0) && (idx+count <= _size));

	const T* last_ptr = _data+idx+count;
	for(T* ptr = _data+idx ; ptr < last_ptr; ++ptr) {
		ptr->~T();
	}

	memmove(_data+idx, _data+idx+count, (_size-idx-count) * sizeof(T));
		
	_size-=count;
}

template<class T, size_t ALIGNMENT> 
void Array<T, ALIGNMENT>::unsorted_remove(size_t idx)
{
	assert(idx >= 0);
	assert(idx < _size);
	
	(_data+idx)->~T();
	memcpy(_data+idx, _data+_size-1, sizeof(T));	
	--_size;
}



}