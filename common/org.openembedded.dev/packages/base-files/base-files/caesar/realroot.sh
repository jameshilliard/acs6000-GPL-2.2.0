#!/bin/bash

root_key="root="
grep -sq "real_root=" /proc/cmdline && root_key="real_root="

real_root=`sed -rn "s/.*($root_key)([^ \t]*)(.*)/\2/p" /proc/cmdline`

unset root_key 

export real_root="$real_root"
