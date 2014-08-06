#/bin/bash
echo "Building GLFW"
cd extern/glfw
cmake .
make -j8
cd ../..
echo "Building GLFW successful"

echo "Building Bullet"
cd extern/bullet
/bin/bash install.sh
cd ../..
echo "Building Bullet successful"

echo "Building Jedi"
cmake .
make -j8
echo "Building Jedi successful"
echo "Jedi can now be started by executing ./run_jedi.sh"
