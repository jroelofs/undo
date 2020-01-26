#ifndef UNDO_H
#define UNDO_H

/*
 * Based on "Inheritance is the Base Class of Evil"
 * https://www.youtube.com/watch?v=bIhUE5uUFOA
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>

template <typename T>
void draw(const T &t, std::ostream &out, size_t position);

class object_t {
public:
  template <typename T>
  object_t(T x) : self_(std::make_shared<model<T>>(std::move(x))) {}

  friend void draw(const object_t &x, std::ostream &out, size_t position) {
    x.self_->draw_(out, position);
  }

private:
  struct concept_t {
    virtual ~concept_t() = default;
    virtual void draw_(std::ostream &out, size_t position) const = 0;
  };

  template <typename T>
  struct model : concept_t {
    model(T x) : data_(std::move(x)) {}
    void draw_(std::ostream &out, size_t position) const {
      draw(data_, out, position);
    }

    T data_;
  };

  std::shared_ptr<const concept_t> self_;
};

using document_t = std::vector<object_t>;

using history_t = std::vector<document_t>;

void commit(history_t &x) {
  assert(x.size() && "no history to commit yet");
  x.push_back(x.back());
}

void undo(history_t &x) {
  assert(x.size() && "no history to undo yet");
  x.pop_back();
}

document_t &current(history_t &x) {
  assert(x.size());
  return x.back();
}

template <>
void draw(const document_t &x, std::ostream &out, size_t position) {
  out << std::string(position, ' ') << "<document>" << std::endl;
  for (const auto &e : x) draw(e, out, position + 2);
  out << std::string(position, ' ') << "</document>" << std::endl;
}

#endif
