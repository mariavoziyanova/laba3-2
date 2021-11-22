#pragma once

#include <cwchar>
#include <iostream>

// == АТД (абстрактные типы данных) ==

// n-арное дерево - целевой АТД, указанный в варианте задания
// T - тип данных которые мы храним в дереве
template <class T, int N>
class Tree {
  // Количество детей у каждого узла - N
  // int N;
  struct Node {      // Узел дерева
    T value;         // Значение в узле
    Node *child[N];  // Дети данного узла (их N)
    Node *parent;
    explicit Node(T value) : value(value) {
      for (int i = 0; i < N; i++) child[i] = nullptr;
    }
    bool find(T v) {
      if (v == this->value) {
        return true;
      }
      for (int i = 0; i < N; i++) {
        if (child[i] && child[i]->find(v)) {
          return true;
        }
      }
      return false;
    }
  };
  Node *root = nullptr;  // Корень дерева
 public:
  explicit Tree() = default;
  // Вставка элемента
  void insert(T value) {
    // Создаём новый узел дерева
    auto *n = new Node(value);
    if (root == nullptr) {  // Если дерево пустое => новый узел становится корнем
      root = n;
    } else {
      root = insert(root, n);
    }
  }
  Node *insert(Node *r, Node *newNode) {
    // Вставляем на ближайшее пустое место
    int idx = 0;
    for (int i = 0; i < N; i++) {
      if (r->child[i]) {
        idx = i;
      } else {
        r->child[i] = newNode;
        return r;
      }
    }
    r->child[idx] = insert(r->child[idx], newNode);
    return r;
  }
  // Поиск элемента по значению
  bool find(T value) {
    if (root == nullptr) {
      return false;
    }
    return root->find(value);
  }
  // map - применение функции к каждому элементу дерево
  // Создаётся новое дерево
  Tree<T, N> *map(T (*f)(T)) {
    auto *res = new Tree<T, N>;
    // for (T x : *this) {
    //   res->insert(f(x));
    // }
    return res;
  }
  // where фильтрует значения из списка l с помощью функции-фильтра h
  Tree<T, N> *where(bool (*h)(T)) {
    auto *res = new Tree<T, N>;
    // for (T x : *this) {
    //   if (h(x)) {
    //     res->insert(x);
    //   }
    // }
    return res;
  }
  // reduce - применяем к каждой паре значений пока не получим одно значение
  T reduce(T (*f)(T, T)) {
    return reduce(root, f);
  }
  T reduce(Node *n, T (*f)(T, T)) {
    T value = n->value;
    for (int i = 0; i < N; i++) {
      if (n->child[i]) {
        value = f(value, reduce(n->child[i], f));
      }
    }
    return value;
  }
  // Ввод элементов дерева
  // Конструктор для ввода элементов стека
  explicit Tree(const wchar_t *string) {
    std::wcout << string << std::endl;
    int M;
    wprintf(L"Введите количество элементов: ");
    std::wcin >> M;
    // Вводим элементы по одному
    for (int i = 0; i < M; i++) {
      wprintf(L"Введите элемент с индексом %d: ", i);
      T element;
      std::wcin >> element;
      insert(element);  // Добавляем элемент в дерево
      // print(); // Текущее состояние стека
    }
  }
  void print() {
    print(root);
  }
  void print(Node *n) {
    std::wcout << n->value << " ";
    for (int i = 0; i < N; i++) {
      if (n->child[i]) {
        print(n->child[i]);
      }
    }
    std::wcout << std::endl;
  }
};

//// Функции для работы со стеком
// map - применение функции f к каждому элементу стека
template <class T, int N>
Tree<T, N> *map(T (*f)(T), Tree<T, N> &l) {
  return l.map(f);
}

// where фильтрует значения из списка l с помощью функции-фильтра h
template <class T, int N>
Tree<T, N> *where(bool (*h)(T), Tree<T, N> &l) {
  return l.where(h);
}

// Применение операции к элементам пока
template <class T, int N>
T reduce(T (*f)(T, T), Tree<T, N> &l) {
  return l.reduce(f);
}
