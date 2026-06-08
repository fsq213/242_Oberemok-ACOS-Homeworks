#!/bin/bash

echo -n "Enter your name: "
read name

if [ -n "$name" ]; then
    echo "Hello, $name!"
else
    echo "Hello, tmpuser?"
fi