#!/bin/bash

cd kitlus
make -j install

cd ../agents
make -j install

echo "\n** You might need to add '$HOME/.local/lib' to your LD_LIBRARY_PATH.\n"
