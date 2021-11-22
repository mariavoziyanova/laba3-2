#pragma once

#include <cwchar>

#include "dynamicarray.h"
#include "sequence.h"

// АТД последовательность на основе динамического массива
template <class T>
class ArraySequence : public Sequence<T> {
 private:
  DynamicArray<T> data;  // Массив данных
 public:
  // == Создание объекта ==
  // Копировать элементы из переданного массива
  ArraySequence(T *data, int count) : data(data, count) {}
  // Конструктор в который передаётся только размер массива
  explicit ArraySequence(int count) : data(count) {}
  // Создать пустой список на основе массива
  ArraySequence() : data() {};
  // Копирующий конструктор
  explicit ArraySequence(const DynamicArray<T> &array) : data(array) {};
  // == Декомпозиция ==
  // Получить первый элемент в списке
  // Может выбрасывать исключения: IndexOutOfRange (если список пуст)
  T getFirst() const override {
    return data.get(0);
  };
  // Получить последний элемент в списке
  // Может выбрасывать исключения: IndexOutOfRange (если список пуст)
  T getLast() const override {
    return data.get(data.getSize() - 1);
  };
  // Получить элемент по индексу
  // Может выбрасывать исключения: IndexOutOfRange (если индекс отрицательный или больше/равен числу элементов)
  T get(int index) const override {
    return data.get(index);
  };
  // Перегруженные операторы
  T operator[](int i) const override {
    return data[i];  // Получаем значение a = obj[1]
  }
  T &operator[](int i) override {
    return data[i];  // Получаем ссылку, чтобы сделать присваивание obj[1] = 12 + 3232
  }
  // Получить список из всех элементов, начиная с startIndex и заканчивая endIndex
  // Может выбрасывать исключения:
  // - IndexOutOfRange (если хотя бы один из индексов отрицательный или больше/равен числу элементов)
  Sequence<T> *getSubsequence(int startIndex, int endIndex) const override {
    if (startIndex > endIndex) {  // Проверяем корректность индексов
      throw IndexOutOfRange(string("Index startIndex <= endIndex"));
    }
    int size = endIndex - startIndex + 1;  // Размер последовательности
    DynamicArray<T> da(size);
    for (int i = 0; i < size; i++) {
      da[i] = get(startIndex + i);
    }
    return new ArraySequence<T>(da);
  }
  // Получить длину списка
  // Может выбрасывать исключения: IndexOutOfRange (если индекс отрицательный или больше/равен числу элементов)
  [[nodiscard]] int getLength() const override {
    return data.getSize();
  }
  // == Операции ==
  // Добавляет элемент в конец списка
  void append(T item) override {
    data.append(item);
  };
  // Добавляет элемент в начало списка
  void prepend(T item) override {
    data.prepend(item);
  };
  // Вставляет элемент в заданную позицию
  // Может выбрасывать исключения:
  // − IndexOutOfRange (если индекс отрицательный или больше/равен числу элементов)
  void insertAt(T item, int index) override {
    data.insertAt(item, index);
  };
  // Сцепляет два списка
  Sequence<T> *concat(Sequence<T> *list) override {
    // Сначала копируем наш массив в результат
    auto *result = new ArraySequence<T>(this->data);
    // Общая длина = сумма длин первого и второго списка
    result->data.resize(getLength() + list->getLength());
    for (int i = 0; i < list->getLength(); i++) {
      result->data.set(getLength() + i, list->get(i));
    }
    return result;
  };
  // Удаление элемента по индексу
  void removeAt(int index) override {
    data.removeAt(index);
  }
  // Печать на экран всех элементов
  void print() override {
    data.print();
  }
  // == Виртуальный деструктор ==
  virtual ~ArraySequence<T>() = default;
  Sequence<T> *map(T (*f)(T)) const override {
    Sequence<T> *res = new ArraySequence<T>();
    for (int i = 0; i < getLength(); i++) {
      res->append(f(data.get(i)));
    }
    return res;
  }
  Sequence<T> *where(bool (*h)(T)) const override {
    auto *res = new ArraySequence<T>;
    for (int i = 0; i < getLength(); i++) {
      T item = data.get(i);
      if (h(item)) {  // Если h возвращает true - добавляем элемент в результат
        res->append(item);
      }
    }
    return res;
  }
  T reduce(T (*f)(T, T)) const override {
    T result = data.get(0);
    for (int i = 1; i < data.getSize(); i++) {
      result = f(result, data.get(i));
    }
    return result;
  }
};
