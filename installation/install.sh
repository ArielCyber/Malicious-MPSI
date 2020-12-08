#update
sudo apt-get update
sudo apt-get upgrade


#install libraries
sudo apt install make
sudo apt install libgmp-dev
sudo apt install libssl-dev
sudo apt install gcc
sudo apt install gcc-8
sudo apt install g++
sudo apt install g++-8
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 80 --slave /usr/bin/g++ g++ /usr/bin/g++-8

#install cmake v3.15
wget https://github.com/Kitware/CMake/releases/download/v3.15.2/cmake-3.15.2.tar.gz
tar -zxvf cmake-3.15.2.tar.gz
rm cmake-3.15.2.tar.gz
cd cmake-3.15.2
./bootstrap
make
sudo make install
cd ..

#install libboost v1.69
wget https://dl.bintray.com/boostorg/release/1.69.0/source/boost_1_69_0.tar.bz2
tar xfj boost_1_69_0.tar.bz2
mv boost_1_69_0 boost
rm boost_1_69_0.tar.bz2
cd boost
./bootstrap.sh --with-libraries=all --with-toolset=gcc
./b2 toolset=gcc
sudo ./b2 install --prefix=/usr
sudo ldconfig
cd ..

#install libOTe
git clone --recursive https://github.com/osu-crypto/libOTe.git
cd libOTe/cryptoTools/thirdparty/linux
bash miracl.get
cd ../../..
mkdir extern
cd extern
git clone --recursive https://github.com/encryptogroup/ENCRYPTO_utils.git
cd ..
cmake . -DENABLE_MIRACL=ON -DENABLE_KOS=ON -DENABLE_SIMPLESTOT=ON
make

#cp src directory to ~/libOTe , main0.cpp file to ~/libOTe/frontend as main.cpp, CMakeLists.txt file to ~/libOTe , CMakeLists_main.txt to ~/libOTe/frontend as CMakeLists.txt , and run 'make' command on ~/libOTe directory