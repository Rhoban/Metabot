#!/bin/sh

if [ -d "Maple" ]; then
    cd Maple &&
    git pull &&
    cd Rhock &&
    git pull
else
    git clone https://github.com/Rhoban/Maple.git --depth=1 &&
    cd Maple &&
    git clone https://github.com/Rhoban/Rhock.git --depth=1
fi
