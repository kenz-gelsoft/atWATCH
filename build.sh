#!/bin/sh

set -e

pebble build
pebble install --phone 192.168.0.3
