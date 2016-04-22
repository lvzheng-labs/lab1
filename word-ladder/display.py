#!/usr/bin/env python3

import sys

status = int(input())
if status == -1:
    print("error: " + input())
    sys.exit(1)
s = input();
for i in range(status - 1):
    s = s + "->" + input()
print(s)
