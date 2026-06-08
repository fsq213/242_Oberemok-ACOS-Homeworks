8#!/bin/bash

sum() {
    local total=0
    
    if [ $# -eq 0 ]; then
        echo "Error: No arguments provided" > /dev/null
        return 0
    fi
    
    for num in "$@"; do
        if ! expr "$num" + 0 > /dev/null 2>&1; then
            echo "Error: Non-numeric argument '$num'" > /dev/null
            return 0
        fi
        total=$(expr "$total" + "$num")
    done
    
    echo "$total"
    return 0
}

read -r line1
read -r line2

sum1=$(sum $line1)
sum2=$(sum $line2)

if [ "$sum1" -eq "$sum2" ] 2>/dev/null; then
    echo "Equal"
else
    echo "Not equal"
fi
