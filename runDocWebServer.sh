#!/bin/bash

echo "start doc server in port localhost:6583"
python3 -m http.server 6583 -d ./docs/doxygen/html