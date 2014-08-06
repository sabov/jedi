#/bin/bash
echo "Bullet physics engine"
mkdir build2
cd build2
echo "Configuring Bullet..."
cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX:PATH=../build2 -DINCLUDE_INSTALL_DIR:PATH=include -DLIB_DESTINATION=lib -DBUILD_DEMOS=OFF -DBUILD_CPU_DEMOS=OFF
echo "Compiling Bullet..."
make -j8
echo "Installing Bullet..."
make install
