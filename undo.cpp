/*
 * Based on "Inheritance is the Base Class of Evil"
 * https://www.youtube.com/watch?v=bIhUE5uUFOA
 */

#include "undo.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

template <>
void draw(const int &x, std::ostream &out, size_t position) {
  out << std::string(position, ' ') << x << std::endl;
}

template <>
void draw(const std::string &x, std::ostream &out, size_t position) {
  out << std::string(position, ' ') << x << std::endl;
}

int main() {
  history_t h(12);

  current(h).emplace_back(0);
  current(h).emplace_back(std::string("Hello"));

  draw(current(h), std::cout, 0);
  std::cout << "-----------" << std::endl;

  commit(h);

  current(h).emplace_back(current(h));
  current(h).emplace_back(1);
  current(h)[1] = std::string("World!");

  draw(current(h), std::cout, 0);
  std::cout << "-----------" << std::endl;

  undo(h);

  draw(current(h), std::cout, 0);
  std::cout << "-----------" << std::endl;
}
