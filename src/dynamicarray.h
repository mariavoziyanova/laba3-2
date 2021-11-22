#pragma once

#include <cstring>
#include <iostream>

#include "common.hpp"

// АТД контейнер: динамический массив
// Массив состоит из элементов типа T
// DynamicArray<int> da1(2);
// DynamicArray<char> da2(2);
template <class T>
class DynamicArray {
  int size;       // Количество элементов массива
  T *data;        // Данные массива
  bool *defined;  // Задан ли элемент массива?
  // Проверка - является ли индекс допустимым? Если нет => генерируется исключение
  void checkIndex(int index) const {
    // Может выбрасывать исключения:
    // − IndexOutOfRange (если индекс отрицательный, больше/равен числу элементов, или указывает на не заданный элемент)
    if (index < 0 || index >= size) {
      throw IndexOutOfRange(string("Index ") + to_string(index) + " out of range 0.." + to_string(size - 1));
    }
  }

 public:  // Делаем доступными извне класса конструкторы и методы-операции
  // == Создание объекта - конструкторы ==
  // Копировать элементы из переданного массива
  // - data - массив значений типа T для инициализации динамического массива
  // - count - количество этих значений
  DynamicArray(T *items, int count) : size(count) {  // Записываем количество элементов в поле size
    if (size < 0) throw IndexOutOfRange("Size < 0");
    // new MyClass - динамическая память для одного экземляра данного класса
    // new MyClass[10] - динамическая память для 10 экземпляров класса
    data = new T[size];  // Создаём в динамической памяти массив заданного размера
    defined = new bool[size];
    memcpy(data, items, sizeof(T) * size);  // Копируем как кусочек памяти: data <- data
    // Считаем что изначально все элементы заданы
    for (int i = 0; i < size; i++) {
      // data[i] = data[i];
      defined[i] = true;  // Элемент задан
    }
  };
  // Создать массив заданной длины count
  explicit DynamicArray(int count = 0) : size(count) {
    if (size < 0) throw IndexOutOfRange("Count < 0");
    data = new T[size];  // Создаём в динамической памяти массив заданного размера
    // Считаем что изначально все элементы "не заданы"
    defined = new bool[size];
    for (int i = 0; i < size; i++) {
      defined[i] = false;  // Элемент "не задан"
    }
  }
  explicit DynamicArray(initializer_list<T> list) : DynamicArray(list.size()) {
    int i = 0;
    for (T x : list) {
      set(i, x);
      i++;
    }
  }
  // Копирующий конструктор - создаёт обьект-копию другого обьекта
  // Цель: менять новый обьект не затрагивая старый
  DynamicArray(const DynamicArray<T> &dynamicArray) {
    size = dynamicArray.size;  // Тот же размер у нового массива что у исходного
    // Копируем элементы
    data = new T[dynamicArray.size];
    memcpy(data, dynamicArray.data, dynamicArray.size * sizeof(T));
    // Копируем какие элементы определены
    defined = new bool[dynamicArray.size];
    memcpy(defined, dynamicArray.defined, dynamicArray.size * sizeof(bool));
  }
  // == Деструктор - очистка памяти ==
  ~DynamicArray() {
    delete[] data;
    data = nullptr;  // NULL - нулевой указатель, nullptr - нулевой указатель, который не будет автоматически
    // приводиться к другим типам
    delete[] defined;
    defined = nullptr;
  }
  // == Декомпозиция ==
  T &get(int index) const {  // Получить элемент по индексу
    checkIndex(index);  // Проверяем индекс и генерируем исключение если он неверный
    if (!defined[index]) {
      throw IndexOutOfRange(string("Element with index ") + to_string(index) + " not defined");
    }
    return data[index];
  }
  [[nodiscard]] int getSize() const {  // Получить размер массива
    return size;
  }
  // == Операции ==
  // Задать значение элемента по индексу
  // index - индекс изменяемого элемента
  // value - новое значение элемента
  void set(int index, T value) {
    checkIndex(index);  // Может выбросить IndexOutOfRange
    data[index] = value;
    // Если элемент был "не задан" => он становится задан
    defined[index] = true;
  }
  // Перегруженные операторы чтобы можно было обращаться к элементу как в
  // обычном массиве
  T operator[](size_t index) const {  // Получение значения
    return get(index);
  }
  T &operator[](size_t index) {  // Чтобы делать присваивание так: dynamicArray[1] = 1233;
    checkIndex(index);
    defined[index] = true;
    return data[index];
  }
  // Изменить размер массива (уменьшить или увеличить)
  void resize(int newSize) {
    if (newSize < 0) {
      throw bad_array_new_length();
    }
    T *newData = new T[newSize];
    bool *newDefined = new bool[newSize];
    // Если размер увеличивается, все элементы копируются в начало новой памяти
    // Если уменьшается – элементы, которые не помещаются, отбрасываются
    for (int i = 0; i < min(size, newSize); i++) {
      newData[i] = data[i];
      newDefined[i] = defined[i];
    }
    memcpy(newData, data, sizeof(T) * min(size, newSize));
    // Копируем какие элементы определены
    memcpy(newDefined, defined, sizeof(bool) * min(size, newSize));
    // Оставшиеся заполняем false - будет работать только если newSize > size
    for (int i = size; i < newSize; i++) {
      newDefined[i] = false;
    }
    // Обновляем количество элементов, сами элементы и какие определены
    size = newSize;
    delete[] data;  // Очищаем память
    data = newData;
    delete[] defined;  // Очищаем память
    defined = newDefined;
  }
  // Меняем элементы i и j местами
  // Может быть использовано для сортировки и других алгоритмов
  void swap(int i, int j) {
    T temp = get(i);
    set(i, get(j));
    set(j, temp);
  }
  void append(T item) {
    resize(size + 1);  // Увеличиваем массив на 1
    set(size - 1, item);
  }
  // Добавляем элемент в начало массива
  void prepend(T item) {
    resize(size + 1);  // Увеличиваем массив на 1
    // Сдвигаем все элементы вправо (в сторону увеличения индексов)
    for (int i = size - 1; i >= 1; i--) {
      data[i] = data[i - 1];
      defined[i] = defined[i - 1];
    }
    // Вставляем новый элемент как первый
    data[0] = item;
    defined[0] = true;
  }
  // Вставляет элемент в заданную позицию
  // Может выбрасывать исключения:
  // − IndexOutOfRange (если индекс отрицательный или больше/равен числу элементов)
  void insertAt(T item, int index) {
    resize(size + 1);  // Увеличиваем размер на 1
    checkIndex(index);  // Проверяем индекс и генерируем исключение если он неверный
    // Сдвигаем все элементы вправо
    for (int i = size - 1; i > index; i--) {
      data[i] = data[i - 1];
      defined[i] = defined[i - 1];
    }
    set(index, item);
  };
  // Удаление элемента по индексу
  void removeAt(const int index) {
    checkIndex(index);  // Проверяем индекс и генерируем исключение если он неверный
    for (int i = index + 1; i < size; i++) {  // Сдвигаем все элементы начиная с index+1 влево на один
      data[i - 1] = data[i];  // Сдвигаем значение
      defined[i - 1] = defined[i];
    }
    resize(size - 1);  // Уменьшаем размер массива
  }
  // Печать динамического массива
  void print() {
    wcout << L"DynamicArray size = " << size << L":";
    for (int i = 0; i < size; i++) {
      if (defined[i])
        wcout << L" " << data[i];
      else
        wcout << L" *";  // Если элемент "не задан" => печатается звёздочка *
    }
    wcout << endl;
  }
};
