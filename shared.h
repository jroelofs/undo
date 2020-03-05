#ifndef SHARED_H
#define SHARED_H

#include <cassert>
#include <cstddef>
#include <utility>
#include <cstdio>

namespace sandbox {

#if DEBUG_RETAIN
static int cb_count = 0;
#endif

template <typename T>
struct shared_ptr {
  struct control_block {
    template<typename U>
    control_block(U *u_, size_t c_) : t(u_), strong_count(u_)
    {
#if DEBUG_RETAIN
      printf("CB #%d ctor\n", id = ++cb_count);
#endif
    }

    control_block(T *t_, size_t c_) : t(t_), strong_count(c_) {
#if DEBUG_RETAIN
      printf("CB #%d ctor\n", id = ++cb_count);
#endif
    }

    ~control_block() {
#if DEBUG_RETAIN
      printf("CB #%d dtor\n", id = cb_count--);
#endif
    }

#if DEBUG_RETAIN
    int id;
#endif
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
#if DEBUG_RETAIN
    printf("release %d --> %zu\n", cb->id, cb->strong_count);
#endif

    if (--cb->strong_count)
      return;

    assert(cb->strong_count == 0 && "releasing, but there are still referers");

    delete cb->t;
    cb->t = nullptr;

    delete cb;
    cb = nullptr;
  }

  void retain(control_block *cb) {
    cb->strong_count++;
#if DEBUG_RETAIN
    printf("retain %d --> %zu\n", cb->id, cb->strong_count);
#endif
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