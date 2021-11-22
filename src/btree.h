#pragma once

#include <iostream>
using namespace std;

template <typename T>
class BTree;

// Узел B-дерева
template <typename T>
class BTreeNode {
  T *keys;        // Массив ключей заданного типа
  int t;          // Диапазон количества ключей
  BTreeNode **C;  // Дочерние узлы
  int n;          // Текущее количество ключей
  bool leaf;
 public:
  // Конструктор
  BTreeNode(int _t, bool _leaf) : t(_t), leaf(_leaf) {
    keys = new T[2 * t - 1];
    C = new BTreeNode *[2 * t];

    // начальное количество ключей 0
    n = 0;
  }

  // Вставка нового ключа в неполное поддерево этого узла
  void insertNonFull(T k);

  // Функция для разделения узла y по индексу i
  void splitChild(int i, BTreeNode *y);

  //Функция для обхода всех узлов поддерева, корни которого находятся в этом узле
  void traverse();

// Функция для поиска ключа в поддереве узла. NULL, если k отсутствует
  BTreeNode *search(T k);

  friend class BTree<T>;
};

// B-дерево
template <typename T>
class BTree {
  BTreeNode<T> *root;  // указатель на корень
  int t;
 public:
  // Конструктор
  BTree(int _t) {
    root = nullptr;
    t = _t;
  }

  // Обход дерева для вывода или какой-то операции
  void traverse() {
    if (root != nullptr) root->traverse();
  }

  // Поиск по ключу k в дереве
  BTreeNode<T> *search(T k) {
    return (root == nullptr) ? nullptr : root->search(k);
  }

  // Есть ли такой ключ в дереве?
  bool found(T k) {
    return search(k) != nullptr;
  }

  // Вставка ключа
  void insert(T k);
};

// Функция для обхода всех узлов поддерева, корни которого находятся в этом узле
template <typename T>
void BTreeNode<T>::traverse() {
  int i;
  for (i = 0; i < n; i++) {
    if (!leaf) C[i]->traverse();
    cout << " " << keys[i];
  }
  if (!leaf) C[i]->traverse();
}

// Поиск ключа k в поддереве начинающемся с данного узла
template <typename T>
BTreeNode<T> *BTreeNode<T>::search(T k) {
  // Ищем первый ключ >= k, i - будет его индексом
  int i = 0;
  while (i < n && k > keys[i]) i++;
  // Если найденный ключ равен k, возвращаем этот узел
  if (keys[i] == k) return this;
  if (leaf) return nullptr;
  return C[i]->search(k);
}

// Вставка нового ключа
template <typename T>
void BTree<T>::insert(T k) {
  if (root == nullptr) {
    root = new BTreeNode<T>(t, true);
    root->keys[0] = k;
    root->n = 1;
  } else {
    // Если корень полный, то дерево увеличивается в высоту
    if (root->n == 2 * t - 1) {
      auto *s = new BTreeNode<T>(t, false);
      s->C[0] = root;
      s->splitChild(0, root);
      int i = 0;
      if (s->keys[0] < k) i++;
      s->C[i]->insertNonFull(k);
      root = s;
    } else //Если корень не полный, то вызываем insertNonFull
      root->insertNonFull(k);
  }
}

//Вставка нового ключа в неполное поддерево этого узла
template <typename T>
void BTreeNode<T>::insertNonFull(T k) {
  int i = n - 1;
  //Если это лист
  if (leaf) {
    // Находим местоположение нового ключа для вставки и смещаем ключи
    while (i >= 0 && keys[i] > k) {
      keys[i + 1] = keys[i];
      i--;
    }
    // Вставляем новый ключ в найденное место
    keys[i + 1] = k;
    n = n + 1;
  } else {
    //Если не лист
    while (i >= 0 && keys[i] > k) i--;

    // Смотрим, полон ли найденный ребенок
    if (C[i + 1]->n == 2 * t - 1) {
      splitChild(i + 1, C[i + 1]);
      if (keys[i + 1] < k) i++;
    }
    C[i + 1]->insertNonFull(k);
  }
}

// Функция для разделения узла y по индексу i
template <typename T>
void BTreeNode<T>::splitChild(int i, BTreeNode<T> *y) {
  auto *z = new BTreeNode<T>(y->t, y->leaf);
  z->n = t - 1;
  // копируем последние (t-1) ключа из y в z
  for (int j = 0; j < t - 1; j++) z->keys[j] = y->keys[j + t];
  // копируем последние t детей из y в z
  if (!y->leaf) {
    for (int j = 0; j < t; j++) z->C[j] = y->C[j + t];
  }
  y->n = t - 1;
  // У этого узла будет новый ребенок, выдаем место
  for (int j = n; j >= i + 1; j--) C[j + 1] = C[j];
  C[i + 1] = z;
  // Находим положение ключа и двигаем другие
  for (int j = n - 1; j >= i; j--) keys[j + 1] = keys[j];
  keys[i] = y->keys[t - 1];
  n = n + 1;
}