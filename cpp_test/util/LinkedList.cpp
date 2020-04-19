
#ifndef LinkedList_t
#define LinkedList_t

#include <stddef.h>
#include <iostream>
#include <typeinfo>
namespace List{
template<class T>
class Link{
		public:
		T * data;
		Link<T> *next = NULL;
		Link(T* data, Link<T> *next){
			this->data = data;
			this->next = next;
		};
	} ;
}

template <class T>
class LinkedList{
	List::Link<T> * firstItem = new List::Link<T>(NULL, NULL);
	
	List::Link<T> *getLink(int index, const char *error);
	
	public:
	T* get(int index, const char *error);
	void add(T*item, int index, const char * error);
	T* remove(int index, const char *error);
	int size();
};

template <class T> 
T * LinkedList<T>::get(int index, const char *error){
	return this->getLink(index, error)->data;
}

template <class T>
void LinkedList<T>::add(T*item , int index, const char *error){
	if(item == NULL){ 
		std:: cout<<"null pointer exception when adding item to linked list of type:"
				  <<typeid(T).name()
				  <<" at index:"
				  <<index
				  <<"\n";
		throw -78;
	}
	if(index == 0){
		this->firstItem =  new List::Link<T>(item, this->firstItem);
	}
	List::Link<T> *previous = this->getLink(index-1, error);
	previous->next = new List::Link<T>(item, previous->next);
}

template <class T>
T* LinkedList<T>::remove(int index, const char *error){
	if(index == 0){
		if(this->firstItem->next == NULL){//if this is an array with one item
			T * retu = this->firstItem->data;
			this->firstItem->data = NULL;
			return retu;
		}
		List::Link<T> newFirst = this->firstItem->next;//this is an array with more then one item
		T *retu = this->firstItem->data;
		delete (this->firstItem);
		this->firstItem = newFirst;
		return retu;
	}
	else{
		List::Link<T> *beforeDeleted = this->getLink(index-1, error);
		List::Link<T> *toBeDeleted = this->getLink(index, error);
		T *retu = toBeDeleted->data;
		List::Link<T> *next = toBeDeleted->next;
		beforeDeleted->next = next;
		delete toBeDeleted;
		return retu;
	}
}


template<class T>
int LinkedList<T>::size(){
	int retu = 0;
	List::Link<T> *item = this->firstItem;
	while(item != NULL){
		item = item->next;
		retu ++;
	}
	return retu;
}

template <class T>
List::Link<T> *LinkedList<T>::getLink(int index, const char *error){
	List::Link<T> *retu = this->firstItem;
	for(int i = 0; i<index; i++){
		retu = retu->next;
		if(retu == NULL){
			std::cout<<"index out of bounds exception. attempted to retrieve element with larged index than length\n";
			std::cout<<"attempted to get index:"<<index<<" of an array of size:"<<this->size()<<"\n";
			std::cout<<"stack trace clue:"<<error;
			throw -45;
		}
	}
	return retu;
}



#endif
