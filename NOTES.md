How to build and run tests:

```
cmake -B build -S . -DBUILD_TESTING=ON && make -C build -j9 && (cd build && ctest --output-on-failure)
```
