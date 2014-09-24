
Kitlus
==============

To get started quick, run the `./install.sh` script.  For building yourself,
the kitlus library (in the `kitlus` subdirectory) must be installed before the
agents in the `agents` subdirectory can be built and installed.  The process
goes something like this:

```bash
cd kitlus
make && make install
cd ../agents
make && make install
```

Default install prefix is `$HOME/.local`.  You will probably need to add
`$HOME/.local/lib` to your `LD_LIBRARY_PATH`.

