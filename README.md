![scalene](https://github.com/emeryberger/scalene/raw/master/docs/scalene-image.png)

# scalene: a high-performance CPU and memory profiler for Python

by [Emery Berger](https://emeryberger.com)

------------

# About Scalene

Scalene is a high-performance CPU *and* memory profiler for Python that does a few things that other Python profilers do not and cannot do.  It runs orders of magnitude faster than other profilers while delivering far more detailed information.

1. Scalene is _fast_. It uses sampling instead of instrumentation or relying on Python's tracing facilities. Its overhead is typically no more than 10-20% (and often less).
1. Scalene is _precise_. Unlike most other Python profilers, Scalene performs CPU profiling _at the line level_, pointing to the specific lines of code that are responsible for the execution time in your program. This level of detail can be much more useful than the function-level profiles returned by most profilers.
1. Scalene separates out time spent running in Python from time spent in native code (including libraries). Most Python programmers aren't going to optimize the performance of native code (which is usually either in the Python implementation or external libraries), so this helps developers focus their optimization efforts on the code they can actually improve.
1. Scalene _profiles memory usage_. In addition to tracking CPU usage, Scalene also points to the specific lines of code responsible for memory growth. It accomplishes this via an included specialized memory allocator.

## Installation

Scalene is distributed as a `pip` package and works on Linux and Mac OS X platforms. You can install it as follows:
```
  % pip install scalene
```

or
```
  % python -m pip install scalene
```

_NOTE_: Currently, installing Scalene in this way does not install its memory profiling library, so you will only be able to use it to perform CPU profiling. To take advantage of its memory profiling capability, you will need to download this repository.

**NEW**: You can now install the memory profiling part on Mac OS X using Homebrew.

```
  % brew tap emeryberger/scalene
  % brew install --head libscalene
```

This will install a `scalene` script you can use (see below).

# Usage

The following command will run Scalene to only perform line-level CPU profiling on a provided example program.

```
  % python -m scalene test/testme.py
```

If you have installed the Scalene library with Homebrew, you can just invoke `scalene` to perform both line-level CPU and memory profiling:

```
  % scalene test/testme.py
```

Otherwise, you first need to build the specialized memory allocator by running `make`:

```
  % make
```

Profiling on a Mac OS X system (without using Homebrew):
```
  % DYLD_INSERT_LIBRARIES=$PWD/libscalene.dylib PYTHONMALLOC=malloc python -m scalene test/testme.py
``` 

Profiling on a Linux system:
```
  % LD_PRELOAD=$PWD/libscalene.so PYTHONMALLOC=malloc python -m scalene test/testme.py
``` 

To see all the options, run with `--help`.

    % python3 -m scalene --help
    usage: scalene [-h] [-o OUTFILE] [--profile-interval PROFILE_INTERVAL]
                   [--wallclock]
                   prog
    
    Scalene: a high-precision CPU and memory profiler.
                https://github.com/emeryberger/Scalene
    
                    for CPU profiling only:
                % python -m scalene yourprogram.py
                    for CPU and memory profiling (Mac OS X):
                % DYLD_INSERT_LIBRARIES=$PWD/libscalene.dylib PYTHONMALLOC=malloc python -m scalene yourprogram.py
                    for CPU and memory profiling (Linux):
                % LD_PRELOAD=$PWD/libscalene.so PYTHONMALLOC=malloc python -m scalene yourprogram.py
    
    positional arguments:
      prog                  program to be profiled
    
    optional arguments:
      -h, --help            show this help message and exit
      -o OUTFILE, --outfile OUTFILE
                            file to hold profiler output (default: stdout)
      --profile-interval PROFILE_INTERVAL
                            output profiles every so many seconds.
      --wallclock           use wall clock time (default: virtual time)

# Comparison to Other Profilers

## Performance and Features

Below is a table comparing the **performance** of various profilers to scalene, running on an example Python program (`benchmarks/julia1_nopil.py`) from the book _High Performance Python_, by Gorelick and Ozsvald. All of these were run on a 2016 MacBook Pro.


| Profiler                           | Time | Slowdown |
| :--- | ---: | ---: |
| _original program_ | 6.71s | 1.0x |
|                    |     |        |
| `cProfile`      | 11.04s  | 1.65x  |
| `Profile`       | 202.26s | 30.14x |
| `pyinstrument`  | 9.83s   | 1.46x  |
| `line_profiler` | 78.0s   | 11.62x |
| `pprofile` _(deterministic)_ | 403.67s | 60.16x |
| `pprofile` _(statistical)_ | 7.47s | 1.11x |
| `yappi` _(CPU)_ | 127.53s | 19.01x |
| `yappi` _(wallclock)_ | 21.45s | 3.2x |
| `py-spy` | 7.25s | 1.08x |
| `memory_profiler`     | _> 2 hours_ | **>1000x**|
|               |     |        |                    | |  | |
| `scalene` _(CPU only)_     | 6.98s | **1.04x** |
| `scalene` _(CPU + memory)_ | 7.68s | **1.14x** |

And this table compares the **features** of other profilers vs. Scalene.

| Profiler | Line-level?    | CPU? | Wall clock vs. CPU time? | Python vs. native? | Memory? | Unmodified code? | Threads? |
| ---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| | | | | | | |
| | | | | | | |
| `cProfile`                   |   | ✔ | wall clock  |   |   | ✔ |   |
| `Profile`                    |   | ✔ | CPU time    |   |   | ✔ |   |
| `pyinstrument`               |   | ✔ | wall clock  |   |   | ✔ |   |
| `line_profiler`              | ✔ | ✔ | wall clock  |   |   |   |   |
| `pprofile` _(deterministic)_ | ✔ | ✔ | wall clock  |   |   | ✔ | ✔ | 
| `pprofile` _(statistical)_   | ✔ | ✔ | wall clock  |   |   | ✔ | ✔ |
| `yappi` _(CPU)_              |   | ✔ | CPU time    |   |   | ✔ | ✔ |
| `yappi` _(wallclock)_        |   | ✔ | wall clock  |   |   | ✔ | ✔ |
| `py-spy`                     | ✔ | ✔ | **both**    |   |   | ✔ | ✔ |
| `memory_profiler`            | ✔ |   |             |   | ✔ |   |   |
|                              |   |   |             |   |   |   |   |
| `scalene` _(CPU only)_       | ✔ | ✔ | **both**    | ✔ |   | ✔ | ✔ |
| `scalene` _(CPU + memory)_   | ✔ | ✔ | **both**    | ✔ | ✔ | ✔ | ✔ |


## Output

Scalene prints annotated source code for the program being profiled and any modules it uses in the same directory or subdirectories. Here is a snippet from `pystone.py`, just using CPU profiling:

```
    benchmarks/pystone.py: % of CPU time = 100.00% out of   3.66s.
          	 |     CPU % |     CPU % |   
      Line	 |  (Python) |  (native) |  [benchmarks/pystone.py]
    --------------------------------------------------------------------------------
    [... lines omitted ...]
       137	 |     0.27% |     0.14% | def Proc1(PtrParIn):
       138	 |     1.37% |     0.11% |     PtrParIn.PtrComp = NextRecord = PtrGlb.copy()
       139	 |     0.27% |     0.22% |     PtrParIn.IntComp = 5
       140	 |     1.37% |     0.77% |     NextRecord.IntComp = PtrParIn.IntComp
       141	 |     2.47% |     0.93% |     NextRecord.PtrComp = PtrParIn.PtrComp
       142	 |     1.92% |     0.78% |     NextRecord.PtrComp = Proc3(NextRecord.PtrComp)
       143	 |     0.27% |     0.17% |     if NextRecord.Discr == Ident1:
       144	 |     0.82% |     0.30% |         NextRecord.IntComp = 6
       145	 |     2.19% |     0.79% |         NextRecord.EnumComp = Proc6(PtrParIn.EnumComp)
       146	 |     1.10% |     0.39% |         NextRecord.PtrComp = PtrGlb.PtrComp
       147	 |     0.82% |     0.06% |         NextRecord.IntComp = Proc7(NextRecord.IntComp, 10)
       148	 |           |           |     else:
       149	 |           |           |         PtrParIn = NextRecord.copy()
       150	 |     0.82% |     0.32% |     NextRecord.PtrComp = None
       151	 |           |           |     return PtrParIn
```

And here is an example with memory profiling enabled, running the Julia benchmark.
The top line is a "spark line" summarizing memory consumption over time.

```
    Memory usage: ▁▁▄▇█▇▇▇█▇█▇█▇█▇█▇▇▇▇█▇▇█▇█▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇█ (max: 105.73MB)
    benchmarks/julia1_nopil.py: % of CPU time = 100.00% out of   9.11s.
          	 |     CPU % |     CPU % | Avg memory  | Memory      | 
      Line	 |  (Python) |  (native) | growth (MB) | usage (%)   | [benchmarks/julia1_nopil.py]
    --------------------------------------------------------------------------------
         1	 |           |           |             |             | import sys
    [... lines omitted ...]
        30	 |           |           |             |             | def calculate_z_serial_purepython(maxiter, zs, cs):
        31	 |           |           |             |             |     """Calculate output list using Julia update rule"""
        32	 |           |           |          18 |       0.74% |     output = [0] * len(zs)
        33	 |     0.44% |     0.06% |          16 |       1.32% |     for i in range(len(zs)):
        34	 |           |           |             |             |         n = 0
        35	 |     0.22% |     0.04% |         -16 |             |         z = zs[i]
        36	 |     0.22% |     0.07% |             |             |         c = cs[i]
        37	 |    26.12% |     5.57% |             |             |         while abs(z) < 2 and n < maxiter:
        38	 |    36.04% |     7.74% |          16 |      85.09% |             z = z * z + c
        39	 |    12.01% |     2.70% |         -16 |       3.96% |             n += 1
        40	 |     0.33% |     0.10% |             |             |         output[i] = n
        41	 |           |           |             |             |     return output
        42	 |           |           |             |             | 
```

Positive memory numbers indicate total memory allocation in megabytes;
negative memory numbers indicate memory reclamation. Memory usage
means how much of the total memory allocation activity a particular
line represents.

# Acknowledgements

Logo created by [Sophia Berger](https://www.linkedin.com/in/sophia-berger/).
