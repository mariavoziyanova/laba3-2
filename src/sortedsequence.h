#pragma once

#include "dynamicarray.h"
#include "sequence.h"

// Является модификацией интерфейса Sequence, описывающей отсортированные последовательности.
// Основное отличие состоит в том, что методы Append, Prepend и InsertAt заменены одним методом Add
template <typename TElement>
struct SortedSequence {  // : public Sequence<TElement>
 private:
  DynamicArray<TElement> data;  // Массив данных
 public:
  // Длина последовательности (количество элементов)
  [[nodiscard]] int getLength() const {
    return data.getSize();
  }
  // Признак того, является ли последовательность пустой
  [[nodiscard]] int getIsEmpty() const {
    return data.getSize() == 0;
  }
  // Получение элемента по индексу
  TElement get(int index) const {
    return data[index];
  }
  // Перегруженные операторы чтобы можно было обращаться к элементу как в
  // обычном массиве
  TElement operator[](size_t index) const {  // Получение значения
    return get(index);
  }
  // Получить первый элемент последовательности
  TElement getFirst() const {
    return data[0];
  }
  // Получить последний элемент последовательности
  TElement getLast() const {
    return data[data.getSize() - 1];
  }
  // Получить индекс элемента последовательности.
  // Если указанного элемента в последовательности не содержится,
  // возвращается значение -1
  int indexOf(TElement element) const {
    for (int i = 0; i < data.getSize(); i++) {
      if (data[i] == element) return i;
    }
    return -1;
  }
  // Получить подпоследовательность:
  // начиная с элемента с индексом startIndex и заканчивая элементом
  // с индексом endIndex
  SortedSequence<TElement> getSubsequence(int startIndex, int endIndex) const {
    SortedSequence<TElement> res;
    for (int i = startIndex; i <= endIndex; i++) {
      res.data.append(data[i]);
    }
    return res;
  }
  // Добавить элемент в последовательность. Элемент автоматически вставляется так,
  // что итоговая последовательность остается отсортированной
  void add(TElement element) {
    // Ищем место куда вставить новый элемент
    int index = 0;
    while (index < data.getSize() && data[index] < element) index++;
    data.insertAt(element, index);
  }
};