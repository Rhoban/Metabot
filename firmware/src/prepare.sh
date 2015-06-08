#!/bin/sh

if [ -d "RobotsWar" ]; then
    cd RobotsWar &&
    git pull
    cd ../Rhock &&
    git pull
else
    git clone https://github.com/RobotsWar/RobotsWar.git RobotsWar --depth=1
    git clone https://github.com/Rhoban/Rhock.git RhockGit --depth=1
    ln -s $PWD/RhockGit RobotsWar/Rhock
fi
