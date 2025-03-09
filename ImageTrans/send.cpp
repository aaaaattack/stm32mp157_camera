#include <iostream>
#include <vector>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <unistd.h>
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

#define MAXSIZE = 65535
#define port 2323
#define ip_addr "192.168.5.11"

vector<unsigned char> image_buffer;

const std::vector<int> compression_params = {
        cv::IMWRITE_JPEG_QUALITY,
        60
};

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        cout << "./send <image_path>\n" << endl;
        return 0;
    }
    Mat Image;
    Image = imread(argv[1], 1);
    imencode(".jpg", Image, image_buffer, compression_params);

    //dgram和data stream的区别
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in skaddr;
    skaddr.sin_family = AF_INET;
    skaddr.sin_port = htons(port);
    skaddr.sin_addr.s_addr = inet_addr(ip_addr);
    
    sendto(
            sock, 
            (const char *)image_buffer.data() ,
            image_buffer.size(), 
            0, 
            (struct sockaddr *)&skaddr, 
            sizeof(skaddr));
}