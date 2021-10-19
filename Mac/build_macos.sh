
if [ ! -d build  ];then
  mkdir build
else
  echo build dir exist
fi

cd build 

cmake ..

cmake --build .