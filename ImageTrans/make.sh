export LD_LIBRARY_PATH=/opt/opencv4.x/lib:$LD_LIBRARY_PATH
g++ -o send send.cpp -I/opt/opencv4.x/include/opencv4/ -L/opt/opencv4.x/lib -lopencv_photo -lopencv_world -lpthread
g++ -o recv recv.cpp -I/opt/opencv4.x/include/opencv4/ -L/opt/opencv4.x/lib -lopencv_photo -lopencv_world -lpthread