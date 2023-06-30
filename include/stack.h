#pragma once
#include <iostream>
using namespace std;

#define MAX_VECTOR_SIZE 1000		//максимальный размер вектора

#define FACTOR 2	//множитель
#define FACTOR_LOW 3	//коэффициент, который нужен чтобы понять, насколько большое значение capacity относительно size 

template <class ValType = int>
class Vector
{
protected:
	ValType* data;
	int size;	//сколько элементов лежит в векторе с точки зрения пользователя
	int capacity;	//реальльный размер вектора, вместе с запасной вместимостью
public:
	Vector();					//конструктор по умолчанию
	Vector(int passed_size);	//конструктор с параметром
	Vector(const Vector& v);	//конструктор копирования
	~Vector();					//деструктор

	void push_back(ValType elem);	//добавить в конец вектора ещё один элемент - elem
	void pop_back();	//удалить последний элемент из вектора
	void push_front(ValType elem);	//добавить в конец вектора ещё один элемент - elem
	void pop_front();	//удалить последний элемент из вектора

	void resize(int n);	//Оставляет в векторе n первых элементов. Если вектор содержит больше элементов, то его размер усекается до n элементов
						//Если размер вектора меньше n, то добавляются недостающие элементы и инициализируются значением по умолчанию
	void free_up_space();	//Если capacity в процессе работы стало намного больше, чем size, уменьшает значение capacity, освобождает неиспользуемую память

	int GetSize() { return size; }
	int GetCapacity() { return capacity; }

	ValType& operator[] (int index);	//возвращает само число, то есть ссылку на него, изменять будем само значение в ячейке. Например, v[5]=4;
	ValType operator[] (int index) const;	//возвращает значение хранящегося в ячейке числа. Чтобы, например, int k=v[5];

	void insert(ValType elem, int index);		// вставляет элемент elem на позицию, на которую указывает итератор index
	void erase(int index);		//удаляет элемент, на который указывает итератор index

	  // ввод-вывод
	friend istream& operator>>(istream& in, Vector& v)
	{
		for (int i = 0; i < v.size; i++)
			in >> v.data[i];
		return in;
	}
	friend ostream& operator<<(ostream& out, const Vector& v)
	{
		if (v.data != nullptr)
			for (int i = 0; i < v.size; i++)
				out << v.data[i] << ' ';
		return out;
	}
};

template <class ValType>	//конструктор по умолчанию
Vector<ValType>::Vector()
{
	data = nullptr;	//указатель равен нулю
	size = 0;
	capacity = 0;
}

template <class ValType>	//конструктор с параметром
Vector<ValType>::Vector(int passed_size)
{
	if (passed_size > 0 && passed_size<=MAX_VECTOR_SIZE)
	{
		data = new ValType[passed_size * FACTOR];
		size = passed_size;
		capacity = passed_size * FACTOR;
		for (int i = 0; i < size; i++)
			data[i] = 0;
	}
	else throw "Incorrect Vector's Size";
}

template <class ValType>	//конструктор копирования
Vector<ValType>::Vector(const Vector& v)
{
	size = v.size;
	capacity = v.capacity;
	data = new ValType[v.capacity];
	if (data != nullptr)
		for (int i = 0; i < v.size; i++)
			data[i] = v.data[i];
}

template <class ValType>	//деструктор
Vector<ValType>::~Vector()
{
	delete[] data;
	data = nullptr;	//остальные поля удалятся сами
}

template <class ValType>
void Vector<ValType>::push_back(ValType elem)
{
	resize(size + 1);		//размерность вектора стала size+1, самая правая ячейка - data[(size+1)-1]. Т.к. стало size=size+1, то самая правая ячейка - это data[size-1]
	data[size - 1] = elem;
	/* рабочее! но без проверки на size+1<=MAX_VECTOR_SIZE
	if (size < capacity)	//если есть ещё место в конце нашего вектора, то дописываем туда
	{
		data[size] = elem;		//т.к. до сих пор заняты места от нуля до size-1 - всего ровно size штук.
		size++;
	}
	else
	{
		capacity = size * FACTOR;
		ValType* tmp = new ValType[capacity];
		for (int i = 0; i < size; i++)
			tmp[i] = data[i];
		delete[] data;
		data = tmp;
		data[size] = elem;
		size++;
	}
	*/
}

template <class ValType>
void Vector<ValType>::pop_back()	//удаляет последний элемент из вектора
{
	if (size > 0)
	{
		data[size - 1] = 0;	//присвоили значение по умолчанию, но в целом оно больше не используется
		size--;
		free_up_space(); //освободить память, если capacity много больше size
	}
	else throw "Vector size is not pozitive";
}

template <class ValType>
void Vector<ValType>::push_front(ValType elem)	//добавляет элемент в начало вектора
{
	resize(size + 1);
	for (int i = size - 2; i >= 0; i--)
		data[i + 1] = data[i];
	data[0] = elem;
	/*	рабочее!	но без проверки на size+1<=MAX_VECTOR_SIZE
	if (size < capacity)	//если есть ещё место в конце нашего вектора, то дописываем туда
	{
		for(int i=size-1; i>=0; i--)
			data[i + 1] = data[i];
		data[0] = elem;
		size++;
	}
	else
	{
		capacity = size * FACTOR;
		ValType* tmp = new ValType[capacity];
		for (int i = 0; i < size; i++)
			tmp[i+1] = data[i];		//копируем уже со сдвигом, освобождаем нулевую ячейку в tmp. Проблем с записью в tmp[(size-1)+1] не будет,
		tmp[0] = elem;											//т.к. размер у tmp не size, а capacity. Capacity больше size (если FACTOR>1)
		delete[] data;
		data = tmp;
		size++;
	}
	*/
}

template <class ValType>
void Vector<ValType>::pop_front()	//удаляет первый элемент из вектора	
{
	if (size > 1)
	{
		for (int i = 0; i < size - 1; i++)
			data[i] = data[i + 1];
		data[size - 1] = 0;	//присвоили значение по умолчанию, но в целом оно больше не используется
		size--;	//значение capacity сохраняется
		free_up_space();	//освободить память, если capacity много больше size
			//более красиво было бы просто сместить указатель data = (data + 1);  но тогда потери памяти слева
	}
	else if (size == 1)
		data = nullptr;
	else throw "Vector size is already not pozitive";
}

template <class ValType>
void Vector<ValType>::resize(int n)	//Оставляет в векторе n первых элементов. Если вектор содержит больше элементов, то его размер усекается до n элементов
{									//Если размер вектора меньше n, то добавляются недостающие элементы и инициализируются значением по умолчанию (нулём)
	if (n > 0 && n <= MAX_VECTOR_SIZE)
	{
		if (n <= size)	//ничего не добавляем по памяти, просто ограничиваем выводимый размер. Значение capacity сохраняется
			size = n;				//сюда free_up_space не вставляем, т.к. само по себе урезание размера вектора ни о чём не говорит.
		else						//Возможно, далее пользователь будет заново вводить много чисел. Если же будет удалять, сработает проверка в функциях удаления
		{
			if (n <= capacity)	//т.е. если size < n <= capacity. Значение capacity сохраняется
			{
				for (int i = size; i < n; i++)
					data[i] = 0;
				size = n;
			}
			else	//т.е. если  n > capacity
			{
				ValType* tmp = new ValType[n * FACTOR];
				for (int i = 0; i < size; i++)
					tmp[i] = data[i];
				for (int i = size; i < n; i++)
					tmp[i] = 0;
				if (data != nullptr)
					delete[] data;
				data = tmp;
				size = n;
				capacity = n * FACTOR;
			}
		}
	}
	else if (n > MAX_VECTOR_SIZE) throw "Stack Overflow";
		else throw "Negative Vector's Size";
}

template <class ValType>
void Vector<ValType>::free_up_space()	//Если capacity в процессе работы стало намного больше, чем size (в три раза), уменьшает значение capacity
{
	if (capacity >= FACTOR_LOW * size)
	{
		capacity = size * FACTOR;		//Значение size сохраняется
		ValType* tmp = new ValType[capacity];
		for (int i = 0; i < size; i++)
			tmp[i] = data[i];
		delete[] data;
		data = tmp;
	}
}

template <class ValType>
ValType& Vector<ValType>::operator[](int index)
{
	if (index < size && index>0)
		return data[index];
	else throw "Incorrect position number";
}

template <class ValType>
ValType Vector<ValType>::operator[] (int index) const
{
	if (index < size && index>0)
	{
		ValType tmp = data[index];
		return ValType;
	}
	else throw "Incorrect position number";
}

template <class ValType>
void Vector<ValType>::insert(ValType elem, int index)	// вставляет элемент elem на позицию, на которую указывает итератор index
{
	if (index <= size && index >= 0)
	{
		this->resize(size + 1);
		for (int i = size - 1; i >= index; i--)
			data[i + 1] = data[i];
		data[index] = elem;
	}
	else throw "Incorrect Vector's Index";
}

template <class ValType>
void Vector<ValType>::erase(int index)	//удаляет элемент, на который указывает итератор p
{
	if (index < size && index >= 0)		//т.к. сама ячейка data[size] не входит в данный вектор 
	{
		for (int i = index; i < size - 1; i++)
			data[i] = data[i + 1];
		data[size - 1] = 0;//значение по умолчанию
		size--;	//размер уменьшился, элемент data[size - 1] в общем-то не рассматриваем. Capacity сохранилось 
		free_up_space();	//освободить память, если capacity много больше size
	}
	else throw "Incorrect Vector's Index";
}
/*---------------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------------*/
template <class ValType = int>
class Stack : public Vector<ValType>
{
public:
	Stack() : Vector() {};
	Stack(int passed_value) : Vector(passed_value) {}
	Stack(const Stack& v) : Vector(v) {}
	~Stack() {}

	friend ostream& operator<<(ostream& out, const Stack& v)
	{
		if (v.data != nullptr)
			for (int i = v.size-1; i >= 0; i--)		//вывод значений стека будет с конца, то есть с вершины стека к основанию
				out << v.data[i] << ' ';
		return out;
	}

	void repackaging(Stack& s)		//перекладываем из одного стека в другой стек в обратном порядке
	{
		int tmp = size;	//т.к. настоящий size в ходе цикла будет изменяться
		for (int i = 0; i < tmp; i++)
		{
			s.push(top());
			pop();
		}
	}

	ValType top()		//возвращает значение верхнего элемента стека
	{
		if (size > 0)
			return data[size - 1];
		else cout << "Stack is empty now";
	}
	void push(ValType elem) //помещает на верх стека значение elem
	{	
		push_back(elem);
	}	
	void pop() 	//удаляет верхнее значение стека
	{
		pop_back();
	}
	bool empty() 	//возвращает true, если стек пуст
	{
		bool flag = false;
		if (size == 0)
			flag = true;
		return flag;
		//или проще return size==0
	}
	bool full() 	//возвращает true, если стек полон, т.е. если size = MAX_VECTOR_SIZE
	{
		bool flag = false;
		if (size == MAX_VECTOR_SIZE)
			flag = true;
		return flag;
		//или проще return size==MAX_VECTOR_SIZE
	}
	//узнать size и capacity стека можно из соответствующих Get функций вектора

private:	//реализацию функций, которые есть в векторе, но не должно быть в стеке, помещаем в private без инициализации, к ним нельзя будет обратиться
	void push_front(ValType elem) {}
	void pop_front() {}
	ValType& operator[] (int index) {}
	ValType operator[] (int index) const {}
	void insert(ValType elem, int index) {}
	void erase(int index) {}
};
/*---------------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------------*/ 
template <class ValType = int>
class Queue : public Vector<ValType>
{
private:
	int start;		//указывает на начало очереди
	int end;		//указывает на конец очереди. Нужно для циклического буфера

	void repackaging(int param)
	{
		ValType* tmp = new ValType[param * FACTOR];
		int count = size;
		for (int i = start; i < capacity && count>0; i++, count--)		//условие с count нужно для перепаковки, когда много свободного места. В этом случае i до capacity может и не дойти. Там лежит мусор! 
			tmp[i - start] = data[i];									//Когда, например, очередь вида free 5 7 free free free free free. size=2, start=1. Без условия на count получаем выход за пределы диапозона
		if (start > end)
			for (int i = 0; i <= end && count>0; i++,count--)
				tmp[i + (size - start)] = data[i];
		if (data != nullptr)
			delete[] data;
		data = tmp;
		int raznitsa = param - size;
		size = param;
		capacity = param * FACTOR;
		start = 0;
		end = size - raznitsa - 1;		//т.к. новый элемент ещё не добавляли, только ещё вызвали ресайз. Раньше последний элемент был в ячейке size-1, а после увеличения size до n стал лежать в ячейке size-raznitsa-1
	}

	void resize(int n)	//увеличение размера, когда достигнут максимум из элементов очереди на базе вектора
	{
		if (n > 0 && n <= MAX_VECTOR_SIZE)
		{
			if (n <= size)	//ничего не добавляем по памяти, просто ограничиваем выводимый размер. Значение capacity сохраняется
				size = n;				//сюда free_up_space не вставляем, т.к. само по себе урезание размера вектора ни о чём не говорит.
			else						//Возможно, далее пользователь будет заново вводить много чисел. Если же будет удалять, сработает проверка в функциях удаления
			{
				if (n <= capacity)	//т.е. если size < n <= capacity. Значение capacity сохраняется
				{
					for (int i = size; i < n; i++)
					{
						if (start + i < capacity) data[start + i] = 0;
						else data[start + i - capacity] = 0;
					}
					size = n;	//значения start и end сохраняются
				}
				else	//т.е. если  n > capacity
				{
					repackaging(n);
				}
			}
		}
		else if (n > MAX_VECTOR_SIZE) throw "Stack Overflow";
	}

public:
	Queue() : Vector() { start = 0; end = 0; }
	Queue(int passed_value) : Vector(passed_value) { start = 0; end = passed_value - 1; }			//если написать end=passed_value - 1, то в очереди будет вначале passed_value - 1 нулей. Нам это не нужно, наша задача - просто выделить память под очередь
	Queue(const Queue& v) : Vector(v) { start = v.start; end = v.end; }
	~Queue() {}

	friend ostream& operator<<(ostream& out, const Queue& q)
	{
		if (q.data != nullptr)
			if (q.start <= q.end)
			{
				for (int i = 0; i < q.start; i++)
					out << "free" << ' ';
				for (int i = q.start; i <= q.end; i++)
					out << q.data[i] << ' ';
				for (int i = q.end + 1; i < q.capacity; i++)
					out << "free" << ' ';
			}
			else //если satrt>end, то есть если происходило зацикливание
			{
				for (int i = 0; i <= q.end; i++)
					out << q.data[i] << ' ';
				for (int i = q.end + 1; i < q.start; i++)
					out << "free" << ' ';
				for (int i = q.start; i < q.capacity; i++)
					out << q.data[i] <<' ';
			}
		return out;
	}

	ValType front()		//возвращает значение первого элемента очереди
	{
		if (size > 0)
			return data[start];
		else throw "Queue's Size Must Be Positive";		//если size==0
	}

	ValType back()		//возвращает значение последнего элемента очереди
	{
		if (size > 0)
			return data[end];
		else throw "Queue's Size Must Be Positive";
	}

	void push(ValType elem)	//помещает в конец очереди значение elem
	{
		resize(size + 1);		//size увеличится уже в resize, а дальше добавим элемент, и это станет правдой
		if (end == (capacity-1))
		{
			data[0] = elem;
			end = 0;
		}
		else		//если end не лежит в последней ячейке и не нужно вызывать зацикливания 
		{
			data[end + 1] = elem;
			end++;
		}
	}

	void pop()	//удаляет значение из начала очереди
	{
		if (size > 1)
		{
			if (start != (capacity - 1))	//если start лежит ниже самого верхнего значения индекса
				start++;
			else start = 0;
			size--;
			free_up_space();		//проверка на чрезмерное количество свободного места
		}
		else
		{
			size = 0;
			data = nullptr;
			start = 0;
			end = 0;
		}
	}
	void free_up_space()	//Если capacity в процессе работы стало намного больше, чем size (в три раза), уменьшает значение capacity
	{
		if (capacity >= FACTOR_LOW * size)
		{
			repackaging(size);		//будет вызываться перепаковка, то есть после этого метода элементы будут иметь начало очереди в нуле (start=0)
		}
	}
	bool empty() 	//возвращает true, если очередь пуста
	{
		/*bool flag = false;
		if (size == 0)
			flag = true;
		return flag;*/
		return (size == 0);
	}
	bool full() 	//возвращает true, если очередь заполнена, т.е. если size = MAX_VECTOR_SIZE
	{
		/*bool flag = false;
		if (size == MAX_VECTOR_SIZE)
			flag = true;
		return flag;*/
		return (size == MAX_VECTOR_SIZE);
	}

	int GetStart() { return start; }
	int GetEnd() { return end; }
	//узнать size и capacity очереди можно из соответствующих Get функций вектора

private:
	void push_front(ValType elem) {}
	void pop_back() {}
	ValType& operator[] (int index) {}
	ValType operator[] (int index) const {}
	void insert(ValType elem, int index) {}
	void erase(int index) {}
};
/*---------------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------------*/
template <class ValType = int>
class QueueByStacks : public Stack<ValType>
{
public:
	Stack<ValType> first, second;
public:
	QueueByStacks() : first(), second()  {}
	QueueByStacks(int passed_value) : first(passed_value), second() {}
	QueueByStacks(const QueueByStacks& qbs) { first(qbs.first); second(qbs.second); }
	//~QueueByStacks() : ~first(), ~second() {}

	ValType front()		//возвращает значение первого элемента очереди
	{
		if ( (first.GetSize() + second.GetSize()) > 0)		//если очередь не пуста
		{
			if (second.empty())	first.repackaging(second);	//из первого стека перекладываем всё во второй
			return second.top();			//нужный нам элемент находится в любом случае наверху второго стека 
		}
		else throw "Queue's Size Must Be Positive";		//если size==0
	}

	ValType back()		//возвращает значение первого элемента очереди
	{
		if ((first.GetSize() + second.GetSize()) > 0)		//если очередь не пуста
		{
			if (first.empty())	second.repackaging(first);	//из второго стека перекладываем всё в первый
			return first.top();			//нужный нам элемент находится в любом случае наверху первого стека 
		}
		else throw "Queue's Size Must Be Positive";		//если size==0
	}

	void push(ValType elem)
	{
		if (!(this->full()))
			first.push(elem);
		else throw "Queue Overflow";
	}

	void pop()
	{
		if (second.empty()) first.repackaging(second);
			second.pop();		//проверка на пустоту уже есть в push_front. В отличие от push_back. 
	}			//push_back проверяет полноту только первого стека first. Нас же интересует полнота всей очереди, которая равна по размеру полноте одного стека


	bool empty()
	{
		return (first.empty() && second.empty());
	}

	bool full()
	{
		return ( (first.GetSize() + second.GetSize()) == MAX_VECTOR_SIZE );
	}

	int GetSize()
	{
		return first.GetSize() + second.GetSize();
	}
	// ввод-вывод
	friend istream& operator>>(istream& in, QueueByStacks& qbs)
	{
		in >> qbs.first;
		return in;
	}
	friend ostream& operator<<(ostream& out, const QueueByStacks& qbs)
	{
		cout << qbs.second;	//то что нужно: будет выводиться в обратном порядке с верха стека вниз
		Vector<ValType> tmp(qbs.first);		//промежуточный шаг. Вызвать напрямую для qbs дружественный метод из Vector не получилось
		out << tmp;	//то что нужно: будет выводиться в прямом порядке с низа стека наверх
		return out;
	}
};