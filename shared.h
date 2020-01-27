#ifndef SHARED_H
#define SHARED_H

#include <cassert>
#include <cstddef>
#include <utility>
#include <cstdio>

namespace sandbox {

static int retain_count = 0;
static int release_count = 0;

template <typename T>
struct shared_ptr {
  struct control_block {
    template<typename U>
    control_block(U *u_, size_t c_) : t(u_), strong_count(u_) {}

    control_block(T *t_, size_t c_) : t(t_), strong_count(c_) {}
    T *t;
    size_t strong_count;
  };

  template<class U> explicit shared_ptr(U *u)
    : cb(new control_block(static_cast<T*>(u), 0)) {
    retain(cb);
  }

  shared_ptr(const shared_ptr &rhs) : cb(rhs.cb) {
    retain(cb);
  }

  template<class U>
  shared_ptr(shared_ptr<U> &&rhs) {
    // FIXME: this seems like an ugly hack / potential UB.
    cb = reinterpret_cast<control_block*>(rhs.cb);
    retain(cb);
  }

  shared_ptr &operator=(const shared_ptr &rhs) {
    if (this != &rhs) {
      release(cb);
      cb = rhs.cb;
      retain(cb);
    }
    return *this;
  }

  ~shared_ptr() {
    release(cb);
  }

  void release(control_block *&cb) {
    //printf("release %d\n", release_count++);

    if (cb->strong_count--)
      return;

    assert(cb->strong_count == 0 && "releasing, but there are still referers");

    delete cb->t;
    cb->t = nullptr;

    delete cb;
    cb = nullptr;
  }

  void retain(control_block *cb) {
    //printf("retain %d\n", retain_count++);

    cb->strong_count++;
  }

  T *operator->() {
    assert(cb && cb->strong_count && "dereferencing without a pointee");
    return cb->t;
  }

  const T *operator->() const {
    assert(cb && cb->strong_count && "dereferencing without a pointee");
    return cb->t;
  }

  control_block *cb;
};

template <typename T, typename ...Args>
shared_ptr<T> make_shared(Args... args) {
  return shared_ptr<T>(new T(std::forward<Args>(args)...));
}

}

#endif
