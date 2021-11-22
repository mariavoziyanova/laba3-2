// Консольная программа для демонстрации

#include <fcntl.h>

#include <chrono>
#include <complex>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <fstream>
#include <random>

#include "backpack.h"
#include "menu.h"

using namespace std::chrono;

void main_menu1() {
  auto printSolve = []()
  {
    const char *fileName = "../backpack_a.txt";
    solveBackpack(fileName);
  };

  MenuItem menu[] = {
    {L"Стоимость предметов максимальная, а суммарный объем не превосходит заданной величины", printSolve },
      {L"", }
  };
  menuLoop(L"Возможные операции", _countof(menu), menu);
}

struct AllSolutions
{
  vector<BackPack> sol1; // Вывести все уникальные решения (различная цена и вес), отсортированные по цене
  vector<BackPack> sol2; // Стоимость предметов максимальная, вес не превосходит заданной величины и эффективное заполнение
  BackPack sol3; // Стоимость предметов в рюкзаке была максимальной, а суммарный вес минимальный
  vector<BackPack> sol4; // Стоимость предметов максимальная, заполнение максимальное, вес не превосходит заданной величины
  int time_milliseconds;

  explicit AllSolutions()
  {
    Config cfg("../input.txt");
    int max_weight = cfg.maxWeight;
    SolutionTree tree(cfg.backPack);
    auto begin = chrono::steady_clock::now();
    auto solutions = tree.solve(cfg.items);
    auto end = chrono::steady_clock::now();
    auto time = chrono::duration_cast<chrono::milliseconds>(end - begin);
    time_milliseconds = time.count();
    wcout << "Time: " << time.count() << " mls\n";
    wcout << "Number of solutions: " << solutions.size() << "\n";

    sol1.resize(solutions.size());
    int size = 0;
    for (auto &backpack: solutions)
      sol1[size++] = backpack;

    for (int i = sol1.size() - 1; i >= 0; i--)
    {
      if (sol1[i].weight <= max_weight)
      {
        int j = i;
        while (j >= 0 && sol1[i].price == sol1[j].price)
        {
          sol2.emplace_back(sol1[j]);
          j--;
        }
        break;
      }
    }

    size = sol1.size() - 2;
    while (size > 0 && sol1.back().price == sol1[size].price)
      size--;
    sol3 = sol1[size + 1];

    int min_vol = INT32_MAX;
    for (auto &backpack: sol2)
    {
      int vol = 0;
      for (auto &str: backpack.shape)
      {
        vol += count(str.begin(), str.end(), '_');
      }
      if (vol < min_vol)
        min_vol=vol;
    }

    for (auto &backpack: sol2) {
      int vol = 0;
      for (auto &str: backpack.shape)
      {
        vol += count(str.begin(), str.end(), '_');
      }
      if (vol == min_vol)
        sol4.emplace_back(backpack);
    }
  }

  ~AllSolutions() = default;
};

void main_menu2()
{
  AllSolutions ans;

  auto sol1 = [&ans]()
  {
    for (auto &backpack: ans.sol1)
    {
      wcout << backpack << "\n";
    }
  };

  auto sol2 = [&ans]()
  {
    for (auto &backpack: ans.sol2)
    {
      wcout << backpack << "\n";
    }
  };

  auto sol3 = [&ans]()
  {
    wcout << ans.sol3;
  };

  auto sol4 = [&ans]()
  {
    for (auto &backpack: ans.sol4)
    {
      wcout << backpack << "\n";
    }
  };

  MenuItem menu[] = {
      {L"Вывести все уникальные решения (различная цена и вес), отсортированные по цене",sol1},
      {L"Стоимость максимальная, вес не превосходит заданной величины",sol2},
      {L"Стоимость максимальная, а суммарный вес минимальный", sol3},
      {L"Стоимость максимальная, заполнение максимальное, вес не превосходит заданной величины", sol4},
  };
  menuLoop(L"Возможные операции", _countof(menu), menu);
}


// Основная программа
int main()
{
  // Задаём кодировку UTF-16 для всего вывода в программе
  // Все символы и строки будут wchar_t
#if WIN32 || WIN64
  _setmode(_fileno(stdout), _O_U16TEXT);
  _setmode(_fileno(stdin), _O_U16TEXT);
  _setmode(_fileno(stderr), _O_U16TEXT);
#endif
  wprintf(L"== Задача о рюкзаке ==\n");

  // Сделать меню и какие варианты
  MenuItem menu[] = {
      {L"Не задана форма рюкзака и фигуры", main_menu1},
      {L"Задана форма рюкзака и фигуры",    main_menu2},
  };
  try
  {
    menuLoop(L"Выберите вариант задачи", _countof(menu), menu);
  } catch (IndexOutOfRange &ex)
  {
    wcout << L"Exception: " << ex.what() << endl << endl;
  }
}
