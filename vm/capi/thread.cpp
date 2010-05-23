#include "capi/capi.hpp"
#include "capi/ruby.h"
#include "builtin/thread.hpp"

using namespace rubinius;

extern "C" {
  void rb_thread_schedule() {
    rb_funcall2(rb_cThread, rb_intern("pass"), 0, NULL);
  }

  int rb_thread_select(int max, fd_set* read, fd_set* write, fd_set* except,
                       struct timeval *timeval) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    GlobalLock::UnlockGuard guard(env);

    return select(max, read, write, except, timeval);
  }

  VALUE rb_thread_current(void) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    GlobalLock::UnlockGuard guard(env);
    Thread* thread = env->state()->thread.get();
    
    return env->get_handle(thread);
  }
  

  int rb_thread_alone() {
    return 0;
  }

  VALUE rb_thread_local_aref(VALUE thread, ID id) {
    return rb_funcall(thread, rb_intern("[]"), 1, ID2SYM(id));
  }

  VALUE rb_thread_local_aset(VALUE thread, ID id, VALUE value) {
    return rb_funcall(thread, rb_intern("[]="), 2, ID2SYM(id), value);
  }

  // THAR BE DRAGONS.
  //
  // When venturing through the valleys of the unmanaged, our hero must
  // remain vigilant and disiplined! If she should ever find a VALUE for
  // a reference in her travels: Look away! For these VALUEs are pure
  // death! Our hero must steel herself and continue on her quest, returning
  // as soon as possible to the castle of the managed.
  VALUE rb_thread_blocking_region(rb_blocking_function_t func, void* data,
                                  rb_unblock_function_t ubf, void* ubf_data) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    VALUE ret = Qnil;
    // ubf is ignored entirely.
    {
      GlobalLock::UnlockGuard guard(env);
      ret = (*func)(data);
    }

    return ret;
  }
}
