#pragma once
#include <queue>

using namespace std;

template <class T>
	class ShuffleQueue: public queue<T>
		{
		public:
			void shuffle()
				{
				T data;
				if(!this->empty())
					{
					data=this->front();
					this->pop();
					this->push(data);
					}
				}
			T take_front(T defaultdata)
				{
				T data;
				if(!this->empty())
					{
					data=this->front();
					this->pop();
					}
				else
					data=defaultdata;
				return data;
				}
		};
