#!/bin/bash

# original credit goes to kporceil

if [[ $(cat $(find . \( -name "*.c" -o -name "*.h" \) -type f) | grep -c "TODO:") -ne 0 || $(cat $(find . \( -name "*.c" -o -name "*.h" \) -type f) | grep -c "FIXME:") -ne 0 ]]; then
	false
fi
