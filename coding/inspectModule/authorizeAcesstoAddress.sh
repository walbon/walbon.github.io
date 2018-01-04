#!/bin/bash

value=$(cat /proc/sys/kernel/kptr_restrict)

printf "Actual value for %%pk is %d." ${value}

if [ "${value}" == "0" ]; then
    echo
    exit 0
fi

printf "\nReseting to 0"
sudo sh -c "echo 0  > /proc/sys/kernel/kptr_restrict" && \
printf "...OK\n" || printf "...NOK\n"

value=$(cat /proc/sys/kernel/kptr_restrict)
exit ${value}



