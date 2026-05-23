#!/bin/sh
find ./src/ -name '*.c' -or -name '*.h' | duplo - -
