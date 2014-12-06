#!/bin/sh

if [ -d "RobotsWar" ]; then
    cd RobotsWar &&
    git pull
else
    git clone https://github.com/RobotsWar/RobotsWar.git RobotsWar
fi
