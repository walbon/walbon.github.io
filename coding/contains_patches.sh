#!/bin/bash

# Entry example:
#063b8b1;Fri Apr 20 15:29:48 2018 -0500;powerpc/pseries/memory-hotplug: Only update DT once per memory DLPAR request


FILE=${1:-patches.txt}

awk -F ';' '{print $3}' < ${FILE} | while IFS= read -r line
	do
	git log | \
	grep -m1 -q "${line}" && \
	printf "APPLIED;$(grep -m1 "${line}" "${FILE}")\n" || \
	printf "NOT;$(grep -m1 "${line}" "${FILE}")\n"
       done


# Output:
#NOT;063b8b1;Fri Apr 20 15:29:48 2018 -0500;powerpc/pseries/memory-hotplug: Only update DT once per memory DLPAR request
#APPLIED;063b8b1;Fri Apr 20 15:29:48 2018 -0500;powerpc/pseries/memory-hotplug: Only update DT once per memory DLPAR request

