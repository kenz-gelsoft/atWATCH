#!/bin/sh

set -e

pebble build
pebble install --phone 192.168.0.2
#pebble install #--phone 192.168.13.2
