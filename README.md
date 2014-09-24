
Kitlus
==============

The kitlus library (in the `kitlus` subdirectory) must be installed before the
agents can be built and installed:

```bash
cd kitlus
make && make install
cd ../agents
make && make install
```

Default install prefix is `$HOME/.local`.  You will probably need to add
`$HOME/.local/lib` to your `LD_LIBRARY_PATH`.

