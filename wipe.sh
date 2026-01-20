#!/bin/bash
set -e

sudo mkdir -p staging
sudo mount /dev/sdb1 staging

sudo rm -r staging/*

sudo umount staging
sudo rm -r staging
