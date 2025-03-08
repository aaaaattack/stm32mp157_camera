#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "icm20608g.h"
// 假设的 SPI 设备节点，根据实际情况修改
#define SPI_DEVICE "/dev/spidev0.0"

// 全局变量，用于保存 SPI 设备文件描述符

int spi_fd;
static ICM20608G_GYRO_ACC_adc icm20608g_get;
// 初始化 SPI 设备
int spi_init() {

    spi_fd = open(SPI_DEVICE, O_RDWR);
    if (spi_fd < 0) {
        perror("Failed to open SPI device");
        return -1;
    }

    // 设置 SPI 模式
    unsigned char mode = SPI_MODE_0;
    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) < 0) {
        perror("Failed to set SPI mode");
        close(spi_fd);
        return -1;
    }

    // 设置每个字的位数
    unsigned char bits = 8;
    if (ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) {
        perror("Failed to set bits per word");
        close(spi_fd);
        return -1;
    }

    // 设置 SPI 时钟速度
    int speed = 1000000; // 1 MHz
    if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, (int*)&speed) < 0) {
        perror("Failed to set max speed");
        close(spi_fd);
        return -1;
    }

    return 0;
}

// 进行 SPI 数据传输
int spi_transfer(unsigned char *tx_buf, unsigned char *rx_buf, size_t len) {
    struct spi_ioc_transfer tr = {
       .tx_buf = (unsigned long)tx_buf,
       .rx_buf = (unsigned long)rx_buf,
       .len = len,
       .speed_hz = 1000000,
       .bits_per_word = 8,
    };

    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr) < 0) {
        perror("Failed to transfer data");
        return -1;
    }

    return 0;
}


// 读取指定地址的数据
unsigned char icm20608g_read_addr(unsigned char uc_addr) {
    unsigned char uc_read = 0;
    unsigned char tx_buf[2];
    unsigned char rx_buf[2];

    uc_addr |= 0x80; /* 地址最高位为 1 表示读取 */
    tx_buf[0] = uc_addr;
    tx_buf[1] = 0xff;

    if (spi_transfer(tx_buf, rx_buf, 2) < 0) {
        return 0;
    }

    uc_read = rx_buf[1];
    return uc_read;
}

// 向指定地址写入数据
int icm20608g_write_addr(unsigned char uc_addr, unsigned char uc_data) {
    unsigned char tx_buf[2];
    unsigned char rx_buf[2];

    uc_addr &= 0x7F; /* 地址最高位为 0 表示写入 */
    tx_buf[0] = uc_addr;
    tx_buf[1] = uc_data;

    if (spi_transfer(tx_buf, rx_buf, 2) < 0) {
        return -1;
    }
    for(int i = 0; i < 2; i++)
    {
        printf("rxbuf: %d\n",rx_buf[i]);
    }
    return 0;
}

unsigned char icm20608g_init()
{
    // icm20608g_write_addr(ICM20608G_PWR_MGMT_1,0x80);
    icm20608g_write_addr(ICM20608G_PWR_MGMT_1,0x01);//设备退出复位
    unsigned char uc_dev_id = icm20608g_read_addr(ICM20608G_WHO_AM_I);
	printf("read icm20608g id is 0x%x\n\r",uc_dev_id);
	if(uc_dev_id!=0xAF)
	{
		printf("read id fail\n\r");
		return -1;
	}
    icm20608g_write_addr(ICM20608G_SMPLRT_DIV, 0x00);// 1k采样频率
    icm20608g_write_addr(ICM20608G_CONFIG, 0x00);// 
    icm20608g_write_addr(ICM20608G_GYRO_CONFIG, 0x00);
    icm20608g_write_addr(ICM20608G_ACC_CONFIG, 0x00);
    icm20608g_write_addr(ICM20608G_ACC_CONFIG2, 0x00);
    icm20608g_write_addr(ICM20608G_LP_MODE_CFG, 0x00);
    icm20608g_write_addr(ICM20608G_FIFO_EN, 0x00);
    icm20608g_write_addr(ICM20608G_PWR_MGMT_2, 0x00);
    printf("icm init over\n");
    return 0;
}
// 关闭 SPI 设备
void spi_close() {
    close(spi_fd);
}

unsigned char  icm20608g_read_len(unsigned char uc_addr,unsigned char *buf,unsigned char uc_len)
{
	unsigned char uc_cnt;

    
    uc_addr |= 0x80;
 
    int i = 0;
    if(spi_transfer(&uc_addr, buf, uc_len))
    {
        printf("icm20608g_read_len err\n");
        return -1;
    }
	// for(uc_cnt=0;uc_cnt<uc_len;uc_cnt++)
	// {
    //     spi_transfer(&uc_addr, buf, uc_len);
    //     // spi_transfer((unsigned char *)0xff, buf, uc_len);
	// 	// buf[uc_cnt]
    //     // printf("buf = %d\n",buf[uc_cnt]);
	// }
    // bug:数据和属性不对应
    // 原因:某属性单次读取数据时,内部for循环多次,故读到了别的属性的数据
	
	return 0;
}

void print_x(unsigned char *uc_buf,unsigned char uc_len)
{
	unsigned char uc_cnt;
	for(uc_cnt=0;uc_cnt<uc_len;uc_cnt++)
	{
		printf("read %d : %x \n\r",uc_cnt,uc_buf[uc_cnt]);
	}	
}

unsigned char  icm20608g_read_acc(void)
{
	unsigned char uc_buf[6];
	icm20608g_read_len(0x3b,uc_buf,6);
	icm20608g_get.acc_x_adc = (signed short)((uc_buf[0]<<8)|uc_buf[1]);
	icm20608g_get.acc_y_adc = (signed short)((uc_buf[2]<<8)|uc_buf[3]);
	icm20608g_get.acc_z_adc = (signed short)((uc_buf[4]<<8)|uc_buf[5]);
	printf("@@加速度icm20608g_read_acc \n");
    printf("x %u\ny %u\nz %u\n",icm20608g_get.acc_x_adc, icm20608g_get.acc_y_adc, icm20608g_get.acc_z_adc);
	// print_x(uc_buf,6);
	return 0;
}

unsigned char  icm20608g_read_gyro(void)
{
	unsigned char uc_buf[6];
	icm20608g_read_len(0x43,uc_buf,6);
	icm20608g_get.gyro_x_adc = (signed short)((uc_buf[0]<<8)|uc_buf[1]);
	icm20608g_get.gyro_y_adc = (signed short)((uc_buf[2]<<8)|uc_buf[3]);
	icm20608g_get.gyro_z_adc = (signed short)((uc_buf[4]<<8)|uc_buf[5]);
	printf("###角速度icm20608g_read_gyro \n\r");
    printf("x %d\ny %d\nz %d\n",icm20608g_get.gyro_x_adc, icm20608g_get.gyro_y_adc, icm20608g_get.gyro_z_adc);
	// print_x(uc_buf,6);
	return 0;
}

unsigned char  icm20608g_read_temp(void)
{
	unsigned char uc_buf[2];
	icm20608g_read_len(0x41,uc_buf,2);
	icm20608g_get.temp_adc = (signed short)((uc_buf[0]<<8)|uc_buf[1]);
	printf("$$$温度icm20608g_read_temp \n\r");
    printf("temp = %u\n", icm20608g_get.temp_adc);
	// print_x(uc_buf,2);
	return 0;
}

int main() {
    if (spi_init() < 0) {
        return -1;
    }

    unsigned char uc_cnt;
	icm20608g_init();//初始化传感器ICM-20608-G	
       
    icm20608g_read_acc();
    icm20608g_read_gyro();
    icm20608g_read_temp();

    spi_close();
    return 0;
}