#ifndef DRV_H
#define DRV_H
// 设备地址
#define slave_addr      0x1e
// 寄存器地址
#define config_reg      0x00
#define ir_reg_low      0x0a
#define ir_reg_high     0x0b
#define als_reg_low     0x0c
#define als_reg_high    0x0d
#define ps_reg_low      0x0e
#define ps_reg_high     0x0f
// 指令
#define reset_cmd       0x04
#define enable_cmd      0x03

typedef struct {
   unsigned short ir;
   unsigned short als;
   unsigned short ps;
}ap3216c_data;

int ap_init(void);
int ap_read_data(ap3216c_data *data);
void ap3216c_release( void );

#endif
