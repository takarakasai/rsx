
///* ROM */
///*  read only rom area */ #define RSX_ROM_MODEL_NUM_L   0x00
//#define RSX_ROM_MODEL_NUM_H   0x01
//#define RSX_ROM_FW_VERSION    0x02
//
///*  writable rom area */
//#define RSX_ROM_SERVO_ID      0x04
//#define RSX_ROM_REVERSE       0x05
//#define RSX_ROM_BAUDRATE      0x06
//#define RSX_ROM_RTN_DELAY     0x07
//#define RSX_ROM_CW_ANG_LIM_L  0x08
//#define RSX_ROM_CW_ANG_LIM_H  0x09
//#define RSX_ROM_CCW_ANG_LIM_L 0x0A
//#define RSX_ROM_CCW_ANG_LIM_H 0x0B
//#define RSX_ROM_TEMP_LIM_L    0x0E
//#define RSX_ROM_TEMP_LIM_H    0x0F
//#define RSX_ROM_CW_CMP_MARGN  0x18
//#define RSX_ROM_CCW_CMP_MARGN 0x19
//#define RSX_ROM_CW_CMP_SLOPE  0x1A
//#define RSX_ROM_CCW_CMP_SLOPE 0x1B
//#define RSX_ROM_PUNCH_L       0x1C
//#define RSX_ROM_PUNCH_H       0x1D
//
///* RAM */
///*  writable ram area */
//#define RSX_RAM_GOAL_POS_L    0x1E
//#define RSX_RAM_GOAL_POS_H    0x1F
//#define RSX_RAM_GOAL_TIM_L    0x20
//#define RSX_RAM_GOAL_TIM_H    0x21
//#define RSX_RAM_MAX_TORQUE    0x23
//#define RSX_RAM_TRQ_ENABLE    0x24
//
///*  read only ram area */
//#define RSX_RAM_PRESENT_POS_L 0x2A
//#define RSX_RAM_PRESENT_POS_H 0x2B
//#define RSX_RAM_PRESENT_TIM_L 0x2C
//#define RSX_RAM_PRESENT_TIM_H 0x2D
//#define RSX_RAM_PRESENT_SPD_L 0x2E
//#define RSX_RAM_PRESENT_SPD_H 0x2F
//#define RSX_RAM_PRESENT_CUR_L 0x30
//#define RSX_RAM_PRESENT_CUR_H 0x31
//#define RSX_RAM_PRESENT_TMP_L 0x32
//#define RSX_RAM_PRESENT_TMP_H 0x33
//#define RSX_RAM_PRESENT_VLT_L 0x34
//#define RSX_RAM_PRESENT_VLT_H 0x35

typedef enum {
  /*  read only rom area */
  RSX_ROM_MODEL_NUM_L   = 0x00,
  RSX_ROM_MODEL_NUM_H   = 0x01,
  RSX_ROM_FW_VERSION    = 0x02,
  
  /*  writable rom area */
  RSX_ROM_SERVO_ID      = 0x04,
  RSX_ROM_REVERSE       = 0x05,
  RSX_ROM_BAUDRATE      = 0x06,
  RSX_ROM_RTN_DELAY     = 0x07,
  RSX_ROM_CW_ANG_LIM_L  = 0x08,
  RSX_ROM_CW_ANG_LIM_H  = 0x09,
  RSX_ROM_CCW_ANG_LIM_L = 0x0A,
  RSX_ROM_CCW_ANG_LIM_H = 0x0B,
  RSX_ROM_TEMP_LIM_L    = 0x0E,
  RSX_ROM_TEMP_LIM_H    = 0x0F,
  RSX_ROM_CW_CMP_MARGN  = 0x18,
  RSX_ROM_CCW_CMP_MARGN = 0x19,
  RSX_ROM_CW_CMP_SLOPE  = 0x1A,
  RSX_ROM_CCW_CMP_SLOPE = 0x1B,
  RSX_ROM_PUNCH_L       = 0x1C,
  RSX_ROM_PUNCH_H       = 0x1D,
  
  /* RAM */
  /*  writable ram area */
  RSX_RAM_GOAL_POS_L    = 0x1E,
  RSX_RAM_GOAL_POS_H    = 0x1F,
  RSX_RAM_GOAL_TIM_L    = 0x20,
  RSX_RAM_GOAL_TIM_H    = 0x21,
  RSX_RAM_MAX_TORQUE    = 0x23,
  RSX_RAM_TRQ_ENABLE    = 0x24,
  
  /*  read only ram area */
  RSX_RAM_PRESENT_POS_L = 0x2A,
  RSX_RAM_PRESENT_POS_H = 0x2B,
  RSX_RAM_PRESENT_TIM_L = 0x2C,
  RSX_RAM_PRESENT_TIM_H = 0x2D,
  RSX_RAM_PRESENT_SPD_L = 0x2E,
  RSX_RAM_PRESENT_SPD_H = 0x2F,
  RSX_RAM_PRESENT_CUR_L = 0x30,
  RSX_RAM_PRESENT_CUR_H = 0x31,
  RSX_RAM_PRESENT_TMP_L = 0x32,
  RSX_RAM_PRESENT_TMP_H = 0x33,
  RSX_RAM_PRESENT_VLT_L = 0x34,
  RSX_RAM_PRESENT_VLT_H = 0x35,
} RSX_RS30X_MEM_ADDR;

