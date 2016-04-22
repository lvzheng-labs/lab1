#!/usr/bin/env python3

import os
import sys

STATE_NULL = 0
STATE_PATH = 1
STATE_CONTENT = 2

state = STATE_NULL
for line in sys.stdin:
    line = line.strip()
    if len(line) == 0:
        continue

    if state == STATE_PATH:
        delim = line.rfind(os.sep)
        if delim != -1:
            os.makedirs(line[:delim], exist_ok = True)
        f = open(line, 'w')
        state = STATE_CONTENT
        continue

    if line == "===":
        if state == STATE_CONTENT:
            f.close()
        state = STATE_PATH
        continue

    if state == STATE_NULL:
        continue

    f.write(line)
    f.write('\n')
