#!/bin/bash

# original credit goes to kporceil

if [[ $(cat $(find . \( -name "*.cpp" -o -name "*.hpp" \) -type f) | grep -c "TODO") -ne 0 || $(cat $(find . \( -name "*.cpp" -o -name "*.hppp" \) -type f) | grep -c "FIXME") -ne 0 ]]; then
	false
fi
