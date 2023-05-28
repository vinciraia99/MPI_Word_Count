#!/bin/bash
export OMPI_MCA_btl_vader_single_copy_mechanism=none
sudo apt-get update
sudo apt-get install libglib2.0-dev -y
sudo apt-get install make -y
make
