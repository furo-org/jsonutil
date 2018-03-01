#!/bin/sh
#git clone http://github.com/nlohmann/json.git
if [ ! -d json ]; then
    git clone https://github.com/theodelrieu/json.git
fi
if [ ! -d Catch2 ]; then
    git clone https://github.com/catchorg/Catch2.git
fi
cd json
git checkout fix/basic_json_conversion
