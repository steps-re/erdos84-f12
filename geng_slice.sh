#!/bin/sh
# geng wrapper: appends res/mod from $SLICE (e.g. 3/1000) so enum_fast can run one slice
exec /usr/bin/nauty-geng "$@" $SLICE
