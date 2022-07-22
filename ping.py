#!/usr/bin/python
import pyping
from pyping.core import ping

response = pyping.ping('192.168.32.205')

if response.ret_code == 0:
    print("reachable")
else:
    print("unreachable")