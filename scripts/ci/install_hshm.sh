#!/bin/bash

# CD into git workspace
cd ${GITHUB_WORKSPACE}

set -x
set -e
set -o pipefail

# Set spack env
INSTALL_DIR="${HOME}"
SPACK_DIR=${INSTALL_DIR}/spack
. ${SPACK_DIR}/share/spack/setup-env.sh

mkdir -p "${HOME}/install"
mkdir build
cd build
spack load --only dependencies hermes_shm
cmake ../ \
-DCMAKE_BUILD_TYPE=Debug \
-DHERMES_ENABLE_COVERAGE=ON \
-DHERMES_ENABLE_DOXYGEN=ON \
-BUILD_HSHM_BENCHMARKS=ON \
-BUILD_HSHM_TESTS=ON \
-DCMAKE_INSTALL_PREFIX=${HOME}/install
cmake --build . -- -j4

export CXXFLAGS=-Wall
ctest -VV
