#!/bin/bash
export OMPI_MCA_btl_vader_single_copy_mechanism=none
apt-get update
apt-get install libglib2.0-dev -y
apt-get install make -y
make clean all clean_o
