#include <chrono>
#include <complex>
#include <cstdlib>
#include "backpack.h"
#include "btree.h"
#include "gtest/gtest.h"
#include "sortedsequence.h"

TEST(BackPack, loadConfiguration) {
  // Загружаем начальные условия к задаче
  Config cfg("../input.txt");
  SolutionTree tree(cfg.backPack);
  auto begin = chrono::steady_clock::now();
  auto solutions = tree.solve(cfg.items);
  auto end = chrono::steady_clock::now();
  auto time = chrono::duration_cast<chrono::microseconds>(end - begin);
  cout << "Time: " << time.count() / 1e3 << "\n";
  cout << "Number of solution: " << solutions.size() << "\n";
  for (auto &backpack : solutions) {
    wcout << backpack << "\n";
  }

  for (auto &x : tree.search(10)) {
    wcout << x.bp << " is leaf " << x.leaf << endl;
  }

  //         Начальное состояние рюкзака (пустой)
  //         Берём самую маленькую и ценную вещь
  //
  //        /    |     |     |
}

TEST(SortedSequence, basic) {
  SortedSequence<int> s;
  ASSERT_EQ(0, s.getLength());
  ASSERT_TRUE(s.getIsEmpty());
  // Добавим первый элемент
  s.add(10);
  ASSERT_EQ(1, s.getLength());
  ASSERT_EQ(10, s.get(0));
  ASSERT_FALSE(s.getIsEmpty());
  // Добавим второй элемент больше первого чтобы он встал в конец
  s.add(20);
  ASSERT_EQ(2, s.getLength());
  ASSERT_EQ(10, s.get(0));
  ASSERT_EQ(20, s.get(1));
  // Добавим третий элемент чтобы он встал в начало
  s.add(5);
  ASSERT_EQ(3, s.getLength());
  ASSERT_EQ(5, s.get(0));
  ASSERT_EQ(10, s.get(1));
  ASSERT_EQ(20, s.get(2));
  // Проверим правильность поиска индекса
  ASSERT_EQ(0, s.indexOf(5));
  ASSERT_EQ(1, s.indexOf(10));
  ASSERT_EQ(2, s.indexOf(20));
  ASSERT_EQ(5, s.getFirst());
  ASSERT_EQ(20, s.getLast());
  // Получаем подпоследовательность меньшей длины
  SortedSequence<int> sub = s.getSubsequence(0, 1);
  ASSERT_EQ(2, sub.getLength());
  ASSERT_EQ(5, sub.get(0));
  ASSERT_EQ(10, sub.get(1));
  ASSERT_EQ(5, sub[0]);
  ASSERT_EQ(10, sub[1]);
}

TEST(BTree, int_basic) {
  BTree<int> t(3);  // A B-Tree with minimum degree 3

  ASSERT_FALSE(t.found(10));
  t.insert(10);
  ASSERT_TRUE(t.found(10));
  t.insert(20);
  ASSERT_TRUE(t.found(20));
  t.insert(5);
  ASSERT_TRUE(t.found(5));
  t.insert(6);
  ASSERT_TRUE(t.found(6));
  t.insert(12);
  ASSERT_TRUE(t.found(12));
  t.insert(30);
  ASSERT_TRUE(t.found(30));
  t.insert(7);
  ASSERT_TRUE(t.found(7));
  t.insert(17);
  ASSERT_TRUE(t.found(17));

  cout << "Traversal of the constructed tree is ";
  t.traverse();

  int k = 6;
  (t.search(k) != nullptr) ? cout << "\nPresent" : cout << "\nNot Present";

  k = 15;
  (t.search(k) != nullptr) ? cout << "\nPresent" : cout << "\nNot Present";

  // Output:
  //  Traversal of the constructed tree is  5 6 7 10 12 17 20 30
  //  Present
  //      Not Present
}

TEST(BTree, string_basic) {
  BTree<string> t(4);  // A B-Tree with minimum degree 3

  ASSERT_FALSE(t.found(string("test")));
  t.insert(string("test"));
  ASSERT_TRUE(t.found(string("test")));
  t.insert(string("XXX"));
  ASSERT_TRUE(t.found(string("XXX")));

  cout << "Traversal of the constructed tree is ";
  t.traverse();

  string k = "6";
  (t.search(k) != nullptr) ? cout << "\nPresent" : cout << "\nNot Present";

  k = "15";
  (t.search(k) != nullptr) ? cout << "\nPresent" : cout << "\nNot Present";
}

//
// 4 способа укладки:
//          #        #
//    #     #        #
//  #####  ##  ##### ##
//          #    #   #
//          #        #
TEST(Item, genAllRotations_square) {
  // Симметричный по всем осям (квадрат):
  // ##
  // ##
  Shape square = {"##", "##"};
  auto res = genAllRotations(square);
  ASSERT_EQ(1, res.size());  // Одно положение
}
TEST(Item, genAllRotations_rectangle) {
  // 2 способа укладки:
  //  #####   ##
  //  #####   ##
  //          ##
  //          ##
  //          ##
  Shape rect = {"#####", "#####"};
  auto res = genAllRotations(rect);
  ASSERT_EQ(2, res.size());
}

// Кочерга - 8 способов укладки
// #       ##       #      ##
// #        #       #      #
// #     #  # ####  # #### #  #
// ## ####  # #    ##    # #  ####
TEST(Item, genAllRotations_kocherga) {
  // 8 способов укладки кочерги:
  Shape rect = {"#", "####"};
  auto res = genAllRotations(rect);
  ASSERT_EQ(8, res.size());
}

TEST(Item, genAllRotations_krivulya) {
  // 8 способов укладки большой кривой фигуры:
  //  #
  // ####
  //  # ###
  Shape rect = {" #", "####", " # ###"};
  auto res = genAllRotations(rect);
  ASSERT_EQ(8, res.size());
  auto r = *res.begin();
  ASSERT_EQ(3, r.size());
  ASSERT_EQ("    # ", r[0]);
  ASSERT_EQ("  ####", r[1]);
  ASSERT_EQ("### # ", r[2]);
}

TEST(Backpack, solveBackpack) {
  ASSERT_EQ(solveBackpack("../backpack_a.txt"), 40);
  ASSERT_EQ(solveBackpack("../backpack_Aa.txt"), 58638);
}