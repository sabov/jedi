#/bin/bash
echo "Bullet physics engine"
mkdir build
cd build
echo "Configuring Bullet..."
cmake .. -DCMAKE_INSTALL_PREFIX:PATH=../build -DINCLUDE_INSTALL_DIR:PATH=lib -DBUILD_BULLET2_DEMOS=OFF -DBUILD_BULLET3_DEMOS=OFF -DBUILD_CPU_DEMOS=OFF
echo "Compiling Bullet..."
make
echo "Installing Bullet..."
make install
