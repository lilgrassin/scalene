#ifndef SPLITHEAP_H
#define SPLITHEAP_H

#include <dlfcn.h>
#include <execinfo.h>
#include <string.h>

#include "common.hpp"
#include "repoman.hpp"
#include "tprintf.h"

#define N_FRAMES 4

#define PYNAME "python"
#define PYNAME_LEN 6

template <class SuperHeap>
class SplitHeap : public SuperHeap {
 public:
  SplitHeap() : _libheap(), _nested(false) {}

  ~SplitHeap() {}

  ATTRIBUTE_ALWAYS_INLINE inline void * malloc(size_t sz) {
    return isPyCall() ? SuperHeap::malloc(sz) : _libheap.malloc(sz);
  }

  ATTRIBUTE_ALWAYS_INLINE inline size_t free(void * ptr) {
    return SuperHeap::inBounds(ptr) ? SuperHeap::free(ptr) : _libheap.free(ptr);
  }

  ATTRIBUTE_ALWAYS_INLINE inline size_t getSize(void * ptr) {
    return SuperHeap::inBounds(ptr) ? SuperHeap::getSize(ptr)
                                    : _libheap.getSize(ptr);
  }

 private:
  SuperHeap _libheap;
  bool _nested;

  ATTRIBUTE_ALWAYS_INLINE inline bool isPyCall() {
    void * stack[N_FRAMES];
    Dl_info libscalene;
    int nframes;

    // On GNU Linux the first call to backtrace causes libgcc to be dynamically
    // loaded, which triggers a nested call to malloc.
    // TODO: preload libgcc on Linux?
#ifdef __gnu_linux__
    if (_nested) { return false; }
    _nested = true;
#endif
    nframes = backtrace(stack, N_FRAMES);
#ifdef __gnu_linux__
    _nested = false;
#endif

    if (likely(nframes > 1) && likely(dladdr(stack[0], &libscalene))) {
      return hasPyFrame(nframes, stack, libscalene.dli_fname);
    }
    tprintf::tprintf(
        "isPyCall: failed to get info for libscalene or have @ frames\n",
        nframes);
    return false;
  }

  ATTRIBUTE_ALWAYS_INLINE inline bool hasPyFrame(int nframes,
                                                 void * stack[N_FRAMES],
                                                 const char * libname) {
    Dl_info info;

    for (int i = 1; i < nframes; ++i) {
      // If it's not our frame and has info...
      if (dladdr(stack[i], &info) && strcmp(libname, info.dli_fname)) {
        // check if it's python
        return !strncmp(&info.dli_fname[strlen(info.dli_fname) - PYNAME_LEN],
                        PYNAME, PYNAME_LEN);
      }
    }
    // note: calls from dlopen on macOS often have stack frames with no info
    return false;
  }
};

#endif