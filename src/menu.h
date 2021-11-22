#pragma once

#include <cassert>
#include <cwchar>
#include <iostream>
#include <functional>

struct MenuItem {
  const wchar_t *name;

//  void (*f)();
  function<void()> f;
};

// Вывод меню и выбор из меню + вызов функций
// На входе:
//  title - заголовок меню
//  N - количество пунктов меню (вариантов для выбора)
//  ... (varargs) - пункты меню (массив строк)
// На выходе: выбор пользователя
void menuLoop(const wchar_t *title, int N, MenuItem menu[]) {
  assert(N >= 2);  // Хотя бы 2 варианта должно быть для выбора
  while (true) {
    wprintf(L"\n%ls (0 - выход):\n", title);  // Печатаем заголовок меню ls - строчка в формате юникод
    // Печатаем меню
    for (int no = 1; no <= N; no++) {  // no=1,2,3...N - номер пункта меню
      wprintf(L"  %d. %ls\n", no, menu[no - 1].name);
    }
    // Печатаем предложение о выборе
    wprintf(L"Выберите (1..%d): ", N);
    int choice;  // Выбор пользователя
    wcin >> choice;
    if (choice >= 1 && choice <= N) {
      int index = choice - 1;
      std::wcout << L"Вы выбрали: " << menu[index].name << std::endl;
      menu[index].f();  // Вызываем функцию
      std::wcout << std::endl;
    } else {
      wprintf(L"Пункта меню %d нет => выход\n", choice);
      return;
    }
  }
}
