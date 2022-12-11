#!/bin/bash

apt-get update -m
apt-get install -y cmake libglu1-mesa-dev libxrandr-dev ninja-build pip python xorg-dev

pip install jinja2
