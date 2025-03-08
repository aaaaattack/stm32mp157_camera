#include <sys/types.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/fs.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include "ap_sensor.h"

#define DEV_FILE "/dev/i2c-0"

static int fd = -1;

static void msleep(unsigned int time)
{
    struct timespec sleeper, temp;

    sleeper.tv_sec = (time_t)(time/1000);
    sleeper.tv_nsec = (long)(time%1000)*1000000;
    nanosleep(&sleeper, &temp);
}

// write cmd
static int ap_write_reg(unsigned char reg_addr, unsigned char cmd)
{
    int              ret = -1;
    unsigned char    buf[2] = {0};

    buf[0] = reg_addr;
    buf[1] = cmd;

    ret = write(fd, buf, 2);
    if( ret < 0 )
    {
        printf("write cmd to ap3216c register failure.\n");
        return -1;
    }

    return 0;
}

int ap_init()
{
    fd = open(DEV_FILE, O_RDWR);
    if(fd < 0)
    {
        printf("fd open err\n");
        close(fd);
        return -1;
    }

    ioctl(fd, I2C_TENBIT, 0);
    ioctl(fd, I2C_SLAVE_FORCE, slave_addr);

    ap_write_reg(config_reg, reset_cmd);
    msleep(2);
    ap_write_reg(config_reg, enable_cmd);
    msleep(2);

    return 0;


}

// read data
static int ap_read_byte(unsigned char reg_addr, unsigned char *val)
{
    int           ret = -1;
    unsigned char buf[1] = {0};

    buf[0] = reg_addr;           //send register address
    ret = write( fd, buf, 1);
    if( ret < 0 )
    {
        printf("write cmd to ap3216c register failure.\n");
        return -1;
    }

    ret = read(fd, buf, 1);  //read data from the register
    if( ret < 0 )
    {
        printf("get the humidy failure.\n");
        return -1;
    }
    *val = buf[0];

    return 0;
}




int ap_read_data(ap3216c_data *pdata)
{
    // unsigned char *buf = (unsigned char *)malloc(6*sizeof(unsigned char));
    // for(unsigned int i = 0; i < 6; i++)
    // {
    //     ap_read_byte(0x0a, (buf + i));
    // }

    // if(*(buf) & 0x80)
    // {
    //     data->ir = 0;
    // }
    // else
    // {
    //     data->ir = (unsigned short)(*(buf+1) << 2 | (*buf & 0x03));
    // }

    unsigned char i =0;
    unsigned char buf[6], val = 0;

    /* read all sensor‘ data */
    for( i = 0; i < 6; i++)
    {
        ap_read_byte( 0x0a + i, &val);
        buf[i] = val;
    }

     /* IR   */
    if(buf[0] & 0X80){  /* IR_OF位为1,则数据无效 */
        pdata->ir = 0;
    }
    else {
        pdata->ir = ((unsigned short)buf[1] << 2) | (buf[0] & 0X03);
    }

    /* ALS  */
    pdata->als = ((unsigned short)buf[3] << 8) | buf[2];

    /* PS */
    if(buf[4] & 0x40){    /* IR_OF位为1,则数据无效 */
        pdata->ps = 0;
    }
    else{
        pdata->ps = ((unsigned short)(buf[5] & 0X3F) << 4) | (buf[4] & 0X0F);
    }

}

void ap3216c_release( void )
{
    close(fd);
}

