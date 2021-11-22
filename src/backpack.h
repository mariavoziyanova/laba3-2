#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <vector>

#include "arraysequence.h"
#include "common.hpp"
#include "sequence.h"

using namespace std;

// Пусть имеется двухмерный рюкзак (см. пример на рис. ниже) и набор предметов
// (каждый из которых занимает в рюкзаке сколько-то место.
// Каждый предмет, кроме размеров, характеризуется весом и стоимостью.
// Требуется найти такой вариант наполнения рюкзака, чтобы:
// a) суммарная стоимость предметов в рюкзаке была максимальной,
//   а суммарный объем – не превосходил заданной величины объема рюкзака
//   (считается, что форма предметов игнорируется)
// b) суммарная стоимость предметов в рюкзаке была максимальной,
// а объем и масса не превосходят заданных величин
// c) суммарная стоимость предметов в рюкзаке была максимальной,
//   но ограничена грузоподъемность (суммарный вес не должен превосходить заданной величины)
//   и следует заполнить рюкзак как можно более эффективно (Δ𝑉5=0 или Δ𝑉≤𝜀)
// d) суммарная стоимость предметов в рюкзаке была максимальной,
//   заполнение рюкзака должно быть максимальным (при ограниченной вместимости),
//   но ограничена грузоподъемность (суммарный вес не должен превосходить заданной величины)
// e) суммарная стоимость предметов в рюкзаке была максимальной,
//   а суммарный вес – минимальным (при ограниченной вместимости)
// При этом все предметы должны полностью умещаться в рюкзаке.

// Предмет для рюкзака
struct Item {
  int weight;            // Вес предмета
  int price;             // Стоимость предмета
  vector<string> shape;  // Форма предмета
  Item(int weight, int price) : weight(weight), price(price) {}

  friend wostream &operator<<(wostream &os, const Item &x);
};

wostream &operator<<(wostream &os, const Item &x) {
  os << L"Item weight " << x.weight << L" price " << x.price << endl;
  for (auto &s : x.shape) {
    os << toWS(s) << endl;
  }
  return os;
}

// Рюкзак (пустой/частично заполненный/полностью)
struct BackPack {
  vector<string> shape;  // Форма рюкзака
  int weight = 0;        // Вес
  int price = 0;         // Стоимость

  BackPack() : shape(), weight(0), price(0) {}

  BackPack(const vector<string> &shape, int weight, int price) : shape(shape), weight(weight), price(price) {}

  ~BackPack() = default;

  inline friend bool operator<(const BackPack &a, const BackPack &b) {
    if (a.price == b.price)
      return a.weight < b.weight;
    else
      return a.price < b.price;
  }

  friend wostream &operator<<(wostream &os, const BackPack &x);
};

wostream &operator<<(wostream &os, const BackPack &x) {
  os << L"BackPack weight " << x.weight << L" price " << x.price << endl;
  for (auto &s : x.shape) {
    os << toWS(s) << endl;
  }
  return os;
}

// Форма фигуры - вектор строк
typedef vector<string> Shape;

void printShape(const Shape &shape) {
  for (auto &s : shape) {
    wcout << toWS(s) << endl;
  }
}

Shape rotate(const vector<string> &box, int width, int height) {
  // #
  // ###
  //
  // ##
  // #
  // #
  Shape rot(width);
  for (int i = 0; i < rot.size(); i++) {  // проходим по оси x, новая колонка это бывшая строка
    rot[i].resize(height);
    for (int j = 0; j < height; j++) rot[i][j] = box[height - j - 1][i];
  }

  return rot;
};

Shape mirror(const vector<string> &box, int width, int height) {
  // ##     ##
  // #   ->  #
  // #       #
  Shape rot(height);
  for (int i = 0; i < height; i++) {  // проходим по оси x, новая колонка это бывшая строка
    rot[i].resize(width);
    for (int j = 0; j < width; j++) rot[i][j] = box[i][width - j - 1];
  }
  return rot;
};

// Функция будет генерировать все повороты и отражения
set<Shape> genAllRotations(const Shape &shape) {
  // Считаем размеры фигуры (описывающий прямоугольник)
  int height = shape.size();
  int width = 0;
  for (auto &s : shape) width = std::max(width, int(s.size()));

  Shape box(height);  // описывающий прямоугольник
  for (int i = 0; i < height; i++) {
    box[i].resize(width, ' ');
    for (int j = 0; j < shape[i].size(); j++) box[i][j] = shape[i][j];
  }

  set<Shape> res;  // удалить дубликаты

  // Копируем фигуру как есть
  res.insert(Shape(box));

  Shape rot(box);
  Shape mirror_rot = mirror(box, width, height);
  res.insert(mirror_rot);
  int rot_width = width, rot_height = height;
  for (int idx = 0; idx < 3; idx++) {
    rot = rotate(rot, rot_width, rot_height);
    mirror_rot = rotate(mirror_rot, rot_width, rot_height);
    res.insert(rot);
    res.insert(mirror_rot);
    swap(rot_width, rot_height);
  }

  return res;
}

// Пытаемся положить предмет в рюкзак со смещением
// относительно верхнего левого угла рюкзака
// item - изображение предмета
// bp - состояние рюкзака
// rowOffset - смещение по строчкам внутри рюкзака
// colOffset - смещение по столбцам внутри рюкзака
// symbol - символ, которым рисовать фигуру
bool tryPutItem(const vector<string> &item, vector<string> &bp, int rowOffset, int colOffset, char symbol) {
  for (int r = 0; r < item.size(); r++) {
    for (int c = 0; c < item[r].size(); c++) {
      // Если в этом месте есть пиксель у предмета
      if (item[r][c] == '@') {
        // Проверяем, есть ли пустое место в рюкзаке
        int row = rowOffset + r;
        int col = colOffset + c;
        if (row >= bp.size() || col >= bp[row].size() || bp[row][col] != '_') {
          return false;  // Не получилось разместить потому что клетка занята
        }
        bp[row][col] = symbol;
      }
    }
  }
  return true;  // Удалось разместить
}

class BackPackSearch {
 public:
  BackPack bp;
  bool leaf = false;  // является ли узел листом или нет
  BackPackSearch(const BackPack &backPack, bool leaf) : bp(backPack), leaf(leaf) {}
};

// Дерево решений
class SolutionTree {
 public:
  BackPack backPack;

 private:
  struct Node {
    BackPack bp;
    Node *parent;
    vector<Node *> child;  /// указатели на детей
    vector<int> keys;  /// индексы предметов, которые содержаться в рюкзаке, обязательно по порядку
    bool leaf = false;  /// является ли узел листом или нет

    /// конструктор специально для root
    Node(const BackPack &backpack, set<BackPack> &solutions, const vector<Item *> &items)
        : bp(backpack), parent(nullptr), keys() {
      child.reserve((items.size() - keys.size()) * 5);
      solve(solutions, items);
    }

    /// Деструктор
    ~Node() {
      for (Node *n : child) {
        delete n;
      }
    }

    Node(const BackPack &backpack, Node *parent_, const vector<int> &parent_keys, int new_key, set<BackPack> &solutions,
         const vector<Item *> &items)
        : bp(backpack), parent(parent_), keys(parent_keys) {
      auto it = lower_bound(keys.begin(), keys.end(), new_key);
      keys.insert(it, new_key);
      if (!is_sorted(keys.begin(), keys.end())) throw runtime_error("Error");

      if (keys.size() == items.size()) {
        leaf = true;
        solutions.insert(bp);
      } else {
        child.reserve((items.size() - keys.size()) * 5);
        solve(solutions, items);
      }
    }

    void solve(set<BackPack> &solutions, const vector<Item *> &items) {
      int idx = 0;
      for (int i = 0; i < items.size(); i++) {
        if (idx < keys.size() && i == keys[idx]) {
          idx++;
          continue;
        }

        set<Shape> shapes = genAllRotations(items[i]->shape);
        for (auto &shape : shapes) {
          for (int row = 0; row < bp.shape.size(); row++) {
            for (int col = 0; col < bp.shape[row].size(); col++) {
              if (bp.shape[row][col] == '#') continue;

              // Пробуем положить фигуру начиная с клетки рюкзака row col
              vector<string> newBP(bp.shape);  // Копируем изображение
              if (tryPutItem(shape, newBP, row, col, char('1' + i))) {
                BackPack nbp = BackPack(newBP, bp.weight + items[i]->weight, bp.price + items[i]->price);

                Node *chd = new Node(nbp, this, keys, i, solutions, items);
                child.emplace_back(chd);
              }
            }
          }
        }
      }
      if (child.empty()) {
        leaf = true;
        solutions.insert(bp);
      }
    }

    vector<BackPackSearch> search(int price) {
      vector<BackPackSearch> res;
      for (auto node : child) {
        // Если найденный ключ равен k, возвращаем этот узел
        if (node->bp.price == price) res.emplace_back(BackPackSearch(node->bp, node->leaf));
        if (node->bp.price > price) break;
        auto childRes = node->search(price);
        res.insert(std::end(res), std::begin(childRes), std::end(childRes));
      }
      return res;
    }
  };

 public:
  Node *root;

  explicit SolutionTree(const BackPack &bp) : backPack(bp) {}

  set<BackPack> solve(const vector<Item *> &items) {
    set<BackPack> solutions;
    root = new Node(backPack, solutions, items);
    return solutions;
  }

  ~SolutionTree() { delete root; }

  vector<BackPackSearch> search(int price) const {
    if (root) {
      return root->search(price);
    }
    return vector<BackPackSearch>();
  }
};

/// Чтение рюкзака и предметов из файла
class Config {
 public:
  BackPack backPack;
  vector<Item *> items;
  int maxWeight = 0;

  // Загрузка параметров рюкзака и предметов в рюкзаке
  explicit Config(const char *fileName) {
    ifstream input(fileName);
    if (!input.is_open()) {
      wcout << L"Can't open file " << fileName << endl;
      throw string("Can't open file");
    }
    // Считываем рюкзак
    string s;
    input >> maxWeight;
    getline(input, s);  // Skip "end of line"
    getline(input, s);
    while (!s.empty()) {
      backPack.shape.push_back(s);
      getline(input, s);
    }
    // Считываем предметы для укладки
    int weight, price;
    while (input >> weight >> price) {  // Пока не кончился файл
      Item *item = new Item(weight, price);
      item->weight = weight;
      item->price = price;
      getline(input, s);
      getline(input, s);           // Считываем очередную строку рисунка фигуры
      while (!s.empty()) {         // Если она не пустая
        item->shape.push_back(s);  // Добавляем в образ фигуры
        getline(input, s);         // И читаем следующую строчку
      }
      items.emplace_back(item);  // Добавляем
      // wcout << *item << endl; // Вывод для отладки
      input.peek();
    }
  }
};

int solveBackpack(const char *fileName) {
  ifstream input(fileName);
  if (!input.is_open()) {
    wcout << L"Can't open file " << fileName << endl;
    throw string("Can't open file");
  }
  // Считываем рюкзак

  int n, W;
  input >> n >> W;
  wcout << "Number of items: " << n << ", max weight of backpack: " << W << "\nItems:\n";
  string s;
  getline(input, s);
  getline(input, s);


  vector<int> w(n); //вес
  vector<int> c(n); //цена
  for (int i = 0; i < n; i++) {
    input >> w[i] >> c[i];
    wcout << i+1 << ": weight = " << w[i] << ", price = " << c[i] << "\n";
  }
  input.close();

  vector<vector<int>> d(n);
  for (int i = 0; i < n; i++) {
    d[i].resize(W+1, 0);
  }

  d[0][w[0]] = c[0];
  for (int i = 1; i < n; i++) {
    for (int j = 0; j <= W; j++) {
      if (j + w[i] <= W)
        d[i][j + w[i]] = max(d[i][j + w[i]], d[i - 1][j] + c[i]);
      d[i][j] = max(d[i][j], d[i - 1][j]);
    }
  }

  int ans = 0;
  for (int i = 0; i <= W; i++)
    ans = max(ans, d[n - 1][i]);
  wcout << "Answer: " << ans<<"\n";

  return ans;
}