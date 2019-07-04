#!/usr/bin/env bash

apt update

apt install -y git gcc g++ libcurl4-openssl-dev libssl-dev make wget

#temporal - installing cmake 3.14.5
version=3.14
build=5
mkdir ~/temp
cd ~/temp
wget https://cmake.org/files/v$version/cmake-$version.$build.tar.gz
tar -xzvf cmake-$version.$build.tar.gz
cd cmake-$version.$build/

./bootstrap
make -j4
make install

PATH=$PATH:/usr/local/bin

cd /
rm -r ~/temp
#####################################


#git clone https://github.com/ravendb/ravendb-cpp-client.git
git clone https://github.com/sashatim125/raven-cpp-client.git
cd raven-cpp-client
git checkout WorkingBranch

git submodule update --init --recursive


cd cmake-build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target install -- -j 4

#"Hello, World!" check
cd ~/RavenDBCppClient

g++ hello_world.cpp -std=c++1z -I ./include/ -I ./external/ -L ./lib -lRaven_CppClient -lstdc++fs -lcurl -lssl -lcrypto -lpthread

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/RavenDBCppClient/lib/
export LD_LIBRARY_PATH

./a.out
rm ./a.out
