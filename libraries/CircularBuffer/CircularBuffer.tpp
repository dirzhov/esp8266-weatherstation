/*
 CircularBuffer.tpp - Circular buffer library for Arduino.
 Copyright (c) 2017 Roberto Lo Giacco.  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

template<typename T, __CB_ST__ S> 
CircularBuffer<T,S>::CircularBuffer() :
		head(buffer), tail(buffer), count(0) {
}

template<typename T, __CB_ST__ S> 
CircularBuffer<T,S>::~CircularBuffer() {
}

template<typename T, __CB_ST__ S> 
bool CircularBuffer<T,S>::unshift(T value) {
	if (head == buffer) {
		head = buffer + S;
	}
	*--head = value;
	if (count == S) {
		if (tail-- == buffer) {
			tail = buffer + S - 1;
		}
		return false;
	} else {
		if (count++ == 0) {
			tail = head;
		}
		return true;
	}
}

template<typename T, __CB_ST__ S> 
bool CircularBuffer<T,S>::push(T value) {
	if (++tail == buffer + S) {
		tail = buffer;
	}
	*tail = value;
	if (count == S) {
		if (++head == buffer + S) {
			head = buffer;
		}
		return false;
	} else {
		if (count++ == 0) {
			head = tail;
		}
		return true;
	}
}

template<typename T, __CB_ST__ S> 
T CircularBuffer<T,S>::shift() {
	T result = *head++;
	if (head == buffer + S) {
		head = buffer;
	}
	count--;
	return result;
}

template<typename T, __CB_ST__ S> 
T CircularBuffer<T,S>::pop() {
	T result = *tail--;
	if (tail == buffer) {
		tail = buffer + S - 1;
	}
	count--;
	return result;
}

template<typename T, __CB_ST__ S> 
T inline CircularBuffer<T,S>::first() {
	return *head;
}

template<typename T, __CB_ST__ S> 
T inline CircularBuffer<T,S>::last() {
	return *tail;
}

template<typename T, __CB_ST__ S> 
T CircularBuffer<T,S>::operator [](__CB_ST__ index) {
	return *(buffer + ((head - buffer + index) % S));
}

template<typename T, __CB_ST__ S> 
T CircularBuffer<T,S>::update(__CB_ST__ index, T value) {
	//*(buffer + ((head - buffer + index) % S)) = value;
	return value;
}

template<typename T, __CB_ST__ S> 
__CB_ST__ inline CircularBuffer<T,S>::size() {
	return count;
}

template<typename T, __CB_ST__ S> 
__CB_ST__ inline CircularBuffer<T,S>::available() {
	return S - count;
}

template<typename T, __CB_ST__ S> 
bool inline CircularBuffer<T,S>::isEmpty() {
	return count == 0;
}

template<typename T, __CB_ST__ S> 
bool inline CircularBuffer<T,S>::isFull() {
	return count == S;
}

template<typename T, __CB_ST__ S> 
void inline CircularBuffer<T,S>::clear() {
	memset(buffer, 0, S);
	head = tail = buffer;
	count = 0;
}
