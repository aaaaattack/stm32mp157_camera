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
#include <fcntl.h>
#include <pthread.h>



class ImgRecv
{
public:
    ImgRecv(int m_port):port(m_port)
    {
        MaxSize = 65536;
        CreateSocket();
        SetNonBlock();
        Bind();
    }

    ~ImgRecv()
    {
        close(sock);
    }

    void CreateSocket();
    void SetNonBlock();
    void Bind();
    void SaveImage();
    std::string Path();

public:
    std::string ImgPath;
    int path_cnt;
    int MaxSize;
    int port;
    int sock;
    int flag;
    int bind_status;
    struct sockaddr_in socket_addr;
    std::vector<unsigned char> ImgData;
    cv::Mat Image;

};


void ImgRecv::CreateSocket()
{
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        std::cout << "socket创建失败" << std::endl;
        return;
    }
    std::cout << "socket创建成功" << std::endl;
}

void ImgRecv::SetNonBlock()
{
    flag = fcntl(sock, F_GETFL, 0);
    if(flag < 0)
    {
        std::cout << "获取套接字失败" << std::endl;
        return;
    }

    flag |= O_NONBLOCK;
    if(fcntl(sock, F_SETFL, flag) < 0)
    {
        std::cout << "设置非阻塞失败" << std::endl;
    }

}

void ImgRecv::Bind()
{
    memset(&socket_addr, 0, sizeof(socket_addr));
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_port = htons(port);
    socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind_status = bind(
                    sock, 
                    (struct sockaddr*)&socket_addr,
                    sizeof(socket_addr));
    if (bind_status < 0) {
        std::cout << "绑定失败" << std::endl;
        return;
    }
}

void ImgRecv::SaveImage()
{
    char buffer[MaxSize];
    struct sockaddr remote_addr;
    socklen_t addrlen = sizeof(remote_addr);

    while(1)
    {
        const int num_bytes = recvfrom(
                                    sock,
                                    buffer,
                                    MaxSize,
                                    0,
                                    (struct sockaddr*)&remote_addr,
                                    &addrlen);

        if(num_bytes > 0)
        {
            ImgData.insert(ImgData.end(), &buffer[0], &buffer[num_bytes]);
        }

        if(!ImgData.empty())
        {
            path_cnt++;
            Image = cv::imdecode(ImgData, cv::IMREAD_COLOR);
            if (Image.empty()) 
            {
                std::cerr << "Error: Image data is empty." << std::endl;
                return;
            }
            ImgPath = Path();
            // std::cout << ImgPath << std::endl;
            bool result = cv::imwrite(ImgPath, Image);
            if(result)
            {
                std::cout << "图像已保存" << std::endl;
            }
            else
            {
                std::cout << "保存失败" << std::endl;
            }

            ImgData.clear();
        }
        usleep(1000);
    }
}


std::string ImgRecv::Path()
{
    std::string p;

    p.append(std::to_string(path_cnt));
    std::stringstream s_path;
    s_path << "./xiewei_" << p << ".jpg";
    s_path >> p;

    return p;
}

int main()
{
    ImgRecv receiver(2323);

    receiver.SaveImage();

    return 0;
}