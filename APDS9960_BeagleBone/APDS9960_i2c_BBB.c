/******************************************************
 * APDS9960_i2c_BBB.c
 * Handles i2c communication between the Beaglebone Black and
 * the APDS-9960 Gesture Sensor
 * This Code is a translation of the Arduino Library made for the APDS-9960,
 * rewritten for the Beaglebone, with Gesture Sensing functionality added.
 * Sara Fagin
 * EC535
 * Created: 4/22/21
 *
 * Usage: ./APDS9960_i2c_BBB
 *
 * Current Functionality:
 * * Initializes I2C communication and reads the id value from the register
 * * * Successfully reads 0xab from register 0x92.
 * * * Moved read to seperate function outside of main.
 * * * Change arguments so target register is included.
 * * * Confirmed SetMode Functionality works
 * * * * This also confirms write works as well as read
 * * Initialization Process complete and tested.
 * * Gesture Sensor Enabled, Appears to be working. Compiles.
 * * Delay function added
 * * Actual Gesture Sensing Functions added, compiles but not tested.
 * * Added everything except interrupt handling.
 * * Outputs to a text file
 * * Compiled but haven't tried running yet.
 * * Added GPIO Functionality, compiles but doesn't handdle loops properly
 * * * Does react to gestures over the sensor though.
 * Set interrupt function to correctly react to Active Low
 * Adjusted GESTURE_SENSITIVITY_1 and GESTURE_SENSITIVITY_2 to reduce the
 * * minimum deltas needed to detect direction.
 * Currently predicts direction fairly accurately, though it tends to be
 * biased to Right (motion value 2).
 * 1 and 2 (left & right) are easier to reproduce than 3 and 4 (Up and Down)
 *
 * Code Sources & Reference Material:
 * * Delay function: https://www.geeksforgeeks.org/time-delay-c/
 * * Arduino C++ Library: https://github.com/sparkfun/APDS-9960_RGB_and_Gesture_Sensor.git
 * * APDS-9960 Datasheet: https://cdn.sparkfun.com/datasheets/Sensors/Proximity/apds9960.pdf
 *****************************************************/

 //Library Includes
 #include <errno.h>
 #include <string.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <linux/i2c-dev.h>
 #include <sys/ioctl.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <time.h>

 //interrupt pin
 #define APDS9960_INT            49 // GPIO_49 = Pin P9_23
 #define SYSFS_GPIO_DIR          "/sys/class/gpio"

 /* Debug */
 #define DEBUG                   0

 /* APDS-9960 I2C address */
 #define APDS9960_I2C_ADDR       0x39

 /* Gesture parameters */
 #define GESTURE_THRESHOLD_OUT   10
 #define GESTURE_SENSITIVITY_1   50
 #define GESTURE_SENSITIVITY_2   0 //Changed from 20 to 0 to prevent triggering

 /* Error code for returned values */
 #define ERROR                   0xFF

 /* Acceptable device IDs */
 #define APDS9960_ID_1           0xAB
 #define APDS9960_ID_2           0x9C
 #define APDS9960_ID_3           0xA8

 /* Misc parameters */
 #define FIFO_PAUSE_TIME         100      // Wait period (ms) between FIFO reads
                                          // Increased from 30 to 100 ms

 /* APDS-9960 register addresses */
 #define APDS9960_ENABLE         0x80
 #define APDS9960_ATIME          0x81
 #define APDS9960_WTIME          0x83
 #define APDS9960_AILTL          0x84
 #define APDS9960_AILTH          0x85
 #define APDS9960_AIHTL          0x86
 #define APDS9960_AIHTH          0x87
 #define APDS9960_PILT           0x89
 #define APDS9960_PIHT           0x8B
 #define APDS9960_PERS           0x8C
 #define APDS9960_CONFIG1        0x8D
 #define APDS9960_PPULSE         0x8E
 #define APDS9960_CONTROL        0x8F
 #define APDS9960_CONFIG2        0x90
 #define APDS9960_ID             0x92
 #define APDS9960_STATUS         0x93
 #define APDS9960_CDATAL         0x94
 #define APDS9960_CDATAH         0x95
 #define APDS9960_RDATAL         0x96
 #define APDS9960_RDATAH         0x97
 #define APDS9960_GDATAL         0x98
 #define APDS9960_GDATAH         0x99
 #define APDS9960_BDATAL         0x9A
 #define APDS9960_BDATAH         0x9B
 #define APDS9960_PDATA          0x9C
 #define APDS9960_POFFSET_UR     0x9D
 #define APDS9960_POFFSET_DL     0x9E
 #define APDS9960_CONFIG3        0x9F
 #define APDS9960_GPENTH         0xA0
 #define APDS9960_GEXTH          0xA1
 #define APDS9960_GCONF1         0xA2
 #define APDS9960_GCONF2         0xA3
 #define APDS9960_GOFFSET_U      0xA4
 #define APDS9960_GOFFSET_D      0xA5
 #define APDS9960_GOFFSET_L      0xA7
 #define APDS9960_GOFFSET_R      0xA9
 #define APDS9960_GPULSE         0xA6
 #define APDS9960_GCONF3         0xAA
 #define APDS9960_GCONF4         0xAB
 #define APDS9960_GFLVL          0xAE
 #define APDS9960_GSTATUS        0xAF
 #define APDS9960_IFORCE         0xE4
 #define APDS9960_PICLEAR        0xE5
 #define APDS9960_CICLEAR        0xE6
 #define APDS9960_AICLEAR        0xE7
 #define APDS9960_GFIFO_U        0xFC
 #define APDS9960_GFIFO_D        0xFD
 #define APDS9960_GFIFO_L        0xFE
 #define APDS9960_GFIFO_R        0xFF

 /* Bit fields */
 #define APDS9960_PON            0b00000001
 #define APDS9960_AEN            0b00000010
 #define APDS9960_PEN            0b00000100
 #define APDS9960_WEN            0b00001000
 #define APSD9960_AIEN           0b00010000
 #define APDS9960_PIEN           0b00100000
 #define APDS9960_GEN            0b01000000
 #define APDS9960_GVALID         0b00000001

 /* On/Off definitions */
 #define OFF                     0
 #define ON                      1

 /* Acceptable parameters for setMode */
 #define POWER                   0
 #define AMBIENT_LIGHT           1
 #define PROXIMITY               2
 #define WAIT                    3
 #define AMBIENT_LIGHT_INT       4
 #define PROXIMITY_INT           5
 #define GESTURE                 6
 #define ALL                     7

 /* LED Drive values */
 #define LED_DRIVE_100MA         0
 #define LED_DRIVE_50MA          1
 #define LED_DRIVE_25MA          2
 #define LED_DRIVE_12_5MA        3

 /* Proximity Gain (PGAIN) values */
 #define PGAIN_1X                0
 #define PGAIN_2X                1
 #define PGAIN_4X                2
 #define PGAIN_8X                3

 /* ALS Gain (AGAIN) values */
 #define AGAIN_1X                0
 #define AGAIN_4X                1
 #define AGAIN_16X               2
 #define AGAIN_64X               3

 /* Gesture Gain (GGAIN) values */
 #define GGAIN_1X                0
 #define GGAIN_2X                1
 #define GGAIN_4X                2
 #define GGAIN_8X                3

 /* LED Boost values */
 #define LED_BOOST_100           0
 #define LED_BOOST_150           1
 #define LED_BOOST_200           2
 #define LED_BOOST_300           3

 /* Gesture wait time values */
 #define GWTIME_0MS              0
 #define GWTIME_2_8MS            1
 #define GWTIME_5_6MS            2
 #define GWTIME_8_4MS            3
 #define GWTIME_14_0MS           4
 #define GWTIME_22_4MS           5
 #define GWTIME_30_8MS           6
 #define GWTIME_39_2MS           7

 /* Default values */
 #define DEFAULT_ATIME           219     // 103ms
 #define DEFAULT_WTIME           246     // 27ms
 #define DEFAULT_PROX_PPULSE     0x87    // 16us, 8 pulses
 #define DEFAULT_GESTURE_PPULSE  0x89    // 16us, 10 pulses
 #define DEFAULT_POFFSET_UR      0       // 0 offset
 #define DEFAULT_POFFSET_DL      0       // 0 offset
 #define DEFAULT_CONFIG1         0x60    // No 12x wait (WTIME) factor
 #define DEFAULT_LDRIVE          LED_DRIVE_100MA
 #define DEFAULT_PGAIN           PGAIN_4X
 #define DEFAULT_AGAIN           AGAIN_4X
 #define DEFAULT_PILT            0       // Low proximity threshold
 #define DEFAULT_PIHT            50      // High proximity threshold
 #define DEFAULT_AILT            0xFFFF  // Force interrupt for calibration
 #define DEFAULT_AIHT            0
 #define DEFAULT_PERS            0x11    // 2 consecutive prox or ALS for int.
 #define DEFAULT_CONFIG2         0x01    // No saturation interrupts or LED boost
 #define DEFAULT_CONFIG3         0       // Enable all photodiodes, no SAI
 #define DEFAULT_GPENTH          40      // Threshold for entering gesture mode
 #define DEFAULT_GEXTH           30      // Threshold for exiting gesture mode
 #define DEFAULT_GCONF1          0x40    // 4 gesture events for int., 1 for exit
 #define DEFAULT_GGAIN           GGAIN_4X
 #define DEFAULT_GLDRIVE         LED_DRIVE_100MA
 #define DEFAULT_GWTIME          GWTIME_2_8MS
 #define DEFAULT_GOFFSET         0       // No offset scaling for gesture mode
 #define DEFAULT_GPULSE          0xC9    // 32us, 10 pulses
 #define DEFAULT_GCONF3          0       // All photodiodes active during gesture
 #define DEFAULT_GIEN            0       // Disable gesture interrupts

 /* Direction definitions */
enum {
  DIR_NONE,
  DIR_LEFT,
  DIR_RIGHT,
  DIR_UP,
  DIR_DOWN,
  DIR_NEAR,
  DIR_FAR,
  DIR_ALL
};

/* State definitions */
enum {
  NA_STATE,
  NEAR_STATE,
  FAR_STATE,
  ALL_STATE
};

 /* Container for gesture data */
typedef struct gesture_data_type {
    int u_data[32];
    int d_data[32];
    int l_data[32];
    int r_data[32];
    int index;
    int total_gestures;
    int in_threshold;
    int out_threshold;
} gesture_data_type;

 //Global Variables
 int isr_flag = 0;              // Interrupt flag

 void interruptRoutine(){
   isr_flag = 1;
 }
 void i2c_read(int file, char *buf, int bufsize, int target_reg){
   buf[0] = target_reg;
   if (write(file, buf, 1) != 1) {
     printf("Write Function Failed\n");
   }
   if (read(file, buf, bufsize) != bufsize) {
     printf("Read Function Failed\n");
   }
 }

 void i2c_write(int file, char *buf, int bufsize, int target_reg, int value){
   buf[0] = target_reg;
   buf[1] = value;
   if (write(file, buf, bufsize) != bufsize) {
     printf("Write Function Failed\n");
   }
 }

 void delay(int ms){
   int milli_seconds = ms;
   clock_t start_time = clock();
   while (clock() < start_time + milli_seconds);
 }

int main(int argc, char *argv[]) {
  //Local Variables
  FILE *fp;
  int file;
  int addr = APDS9960_I2C_ADDR;  // The I2C address of the APDS-9960: 0x39
  char *filename = "/dev/i2c-2"; // I2C bus
  char buf[10];                  // Buffer for register read/write
  int enable, mode, reg_val;     // For setting functions
  int reg_valL, reg_valH;
  int gain, drive, time, boost;
  int reg_dump, val_dump;
  //GPIO Export/Dir/Read/UnExport Variables
  int fd, len;
  char ch;
  char exPin_buf[64];
  char dirPin_buf[64];
  char pinVal_buf[64];
  char uexPin_buf[64];
  //For readGesture functionality
  //int fifo_level = 0;
  //char fifo_data[128];
  //int gstatus;
  //int bytes_read = 0;
  //int motion, i;
  gesture_data_type gesture_data_; //Container for Gesture Data
  int gesture_ud_delta_;
  int gesture_lr_delta_;
  int gesture_ud_count_;
  int gesture_lr_count_;
  int gesture_near_count_;
  int gesture_far_count_;
  int gesture_state_;
  int gesture_motion_;

  //Verify Pin is not already in use
  len = snprintf(uexPin_buf, sizeof(uexPin_buf), "%d", APDS9960_INT);
  fd = open("/sys/class/gpio/unexport", O_WRONLY);
  if(!write(fd, uexPin_buf, len)){
    printf("Pin was not in use\n");
  }
  close(fd);

  //Export GPIO Pin for Interrupt
  len = snprintf(exPin_buf, sizeof(exPin_buf), "%d", APDS9960_INT);
  fd = open("/sys/class/gpio/export", O_WRONLY);
  write(fd, exPin_buf, len);
  close(fd);

  //Set direction of GPIO Interrupt Pin
  snprintf(dirPin_buf, sizeof(dirPin_buf), "/sys/class/gpio/gpio%d/direction", APDS9960_INT);
  fd = open(dirPin_buf, O_WRONLY);
  write(fd, "in", 3);
  close(fd);

  //Open the I2C Bus
  if ((file = open(filename, O_RDWR)) < 0) {
    /* ERROR HANDLING: you can check errno to see what went wrong */
    printf("Failed to open the i2c bus\n");
    exit(1);
  }

  //Initiating Communication with device at address 0x39
  if (ioctl(file, I2C_SLAVE, addr) < 0) {
    printf("Failed to acquire bus access and/or talk to slave device.\n");
    /* ERROR HANDLING; you can check errno to see what went wrong */
    exit(1);
  }

  //APDS9960_ID = 0x92 is the target register to read from
  i2c_read(file, buf, 1, APDS9960_ID);
  printf("Device ID read from Register 0x%x: %x\n", APDS9960_ID, buf[0]); //Debug Print Statement

  /*****************************************************
   * SetMode Portion of Initialization
   *****************************************************/
  i2c_read(file, buf, 1, APDS9960_ENABLE);
  reg_val = buf[0];
  if (reg_val == ERROR){
    printf("Error with fetching Enable value\n");
  }
  enable = OFF & 0x01;
  if (enable) {
    reg_val = 0x7F;
  } else {
    reg_val = 0x00;
  }
  //Writing value back to Enable register
  i2c_write(file, buf, 2, APDS9960_ENABLE, reg_val);

  printf("Checking result of SetMode\n");
  //i2c_read(file, buf, 1, APDS9960_ENABLE);
  printf("Value of Enable read from Register 0x%x: %x\n", APDS9960_ENABLE, buf[0]);

  /*****************************************************
   * Setting Default Values for Ambient Light and
   * Proximity registers
   *****************************************************/
   i2c_write(file, buf, 2, APDS9960_ATIME, DEFAULT_ATIME);
   i2c_write(file, buf, 2, APDS9960_WTIME, DEFAULT_WTIME);
   i2c_write(file, buf, 2, APDS9960_PPULSE, DEFAULT_PROX_PPULSE);
   i2c_write(file, buf, 2, APDS9960_POFFSET_UR, DEFAULT_POFFSET_UR);
   i2c_write(file, buf, 2, APDS9960_POFFSET_DL, DEFAULT_POFFSET_DL);
   i2c_write(file, buf, 2, APDS9960_CONFIG1, DEFAULT_CONFIG1);
   //setLEDDrive(DEFAULT_LDRIVE)
   i2c_read(file, buf, 1, APDS9960_CONTROL);
   reg_val = buf[0];
   drive = DEFAULT_LDRIVE & 0b00000011;
   drive = drive << 6;
   reg_val = reg_val & 0b00111111;
   reg_val = reg_val | drive;
   i2c_write(file, buf, 2, APDS9960_CONTROL, reg_val);
   //setProximityGain(DEFAULT_PGAIN)
   i2c_read(file, buf, 1, APDS9960_CONTROL);
   reg_val = buf[0];
   drive = DEFAULT_PGAIN & 0b00000011;
   drive = drive << 2;
   reg_val = reg_val & 0b11110011;
   reg_val = reg_val | drive;
   i2c_write(file, buf, 2, APDS9960_CONTROL, reg_val);
   //setAmbientLightGain(DEFAULT_AGAIN)
   i2c_read(file, buf, 1, APDS9960_CONTROL);
   reg_val = buf[0];
   drive = DEFAULT_AGAIN & 0b00000011;
   reg_val = reg_val & 0b11111100;
   reg_val = reg_val | drive;
   i2c_write(file, buf, 2, APDS9960_CONTROL, reg_val);
   //setProxIntLowThresh(DEFAULT_PILT)
   i2c_write(file, buf, 2, APDS9960_PILT, DEFAULT_PILT);
   //setProxIntHighThresh(DEFAULT_PIHT)
   i2c_write(file, buf, 2, APDS9960_PIHT, DEFAULT_PIHT);
   //setLightIntLowThreshold(DEFAULT_AILT)
   reg_valL = DEFAULT_AILT & 0x00FF;
   reg_valH = (DEFAULT_AILT & 0xFF00) >> 8;
   i2c_write(file, buf, 2, APDS9960_AILTL, reg_valL);
   i2c_write(file, buf, 2, APDS9960_AILTH, reg_valH);
   //setLightIntHighThreshold(DEFAULT_AIHT)
   reg_valL = DEFAULT_AIHT & 0x00FF;
   reg_valH = (DEFAULT_AIHT & 0xFF00) >> 8;
   i2c_write(file, buf, 2, APDS9960_AIHTL, reg_valL);
   i2c_write(file, buf, 2, APDS9960_AIHTH, reg_valH);
   //
   i2c_write(file, buf, 2, APDS9960_PERS, DEFAULT_PERS);
   i2c_write(file, buf, 2, APDS9960_CONFIG2, DEFAULT_CONFIG2);
   i2c_write(file, buf, 2, APDS9960_CONFIG3, DEFAULT_CONFIG3);

   /*****************************************************
    * Setting Default Values for Gesture Sensor registers
    *****************************************************/
    //setGestureEnterThresh(DEFAULT_GPENTH)
    i2c_write(file, buf, 2, APDS9960_GPENTH, DEFAULT_GPENTH);
    //setGestureExitThresh(DEFAULT_GEXTH)
    i2c_write(file, buf, 2, APDS9960_GEXTH, DEFAULT_GEXTH);
    //
    i2c_write(file, buf, 2, APDS9960_GCONF1, DEFAULT_GCONF1);
    //setGestureGain(DEFAULT_GGAIN)
    i2c_read(file, buf, 1, APDS9960_GCONF2);
    reg_val = buf[0];
    gain = DEFAULT_GGAIN & 0b00000011;
    gain = gain << 5;
    reg_val = reg_val & 0b10011111;
    reg_val = reg_val | gain;
    i2c_write(file, buf, 2, APDS9960_GCONF2, reg_val);
    //setGestureLEDDrive(DEFAULT_GLDRIVE)
    i2c_read(file, buf, 1, APDS9960_GCONF2);
    reg_val = buf[0];
    drive = DEFAULT_GLDRIVE & 0b00000011;
    drive = drive << 3;
    reg_val = reg_val & 0b11100111;
    reg_val = reg_val | drive;
    i2c_write(file, buf, 2, APDS9960_GCONF2, reg_val);
    //setGestureWaitTime(DEFAULT_GWTIME)
    i2c_read(file, buf, 1, APDS9960_GCONF2);
    reg_val = buf[0];
    time = DEFAULT_GWTIME & 0b00000111;
    reg_val = reg_val & 0b11111000;
    reg_val = reg_val | time;
    i2c_write(file, buf, 2, APDS9960_GCONF2, reg_val);
    //
    i2c_write(file, buf, 2, APDS9960_GOFFSET_U, DEFAULT_GOFFSET);
    i2c_write(file, buf, 2, APDS9960_GOFFSET_D, DEFAULT_GOFFSET);
    i2c_write(file, buf, 2, APDS9960_GOFFSET_L, DEFAULT_GOFFSET);
    i2c_write(file, buf, 2, APDS9960_GOFFSET_R, DEFAULT_GOFFSET);
    i2c_write(file, buf, 2, APDS9960_GPULSE, DEFAULT_GPULSE);
    i2c_write(file, buf, 2, APDS9960_GCONF3, DEFAULT_GCONF3);
    //setGestureIntEnable(DEFAULT_GIEN)
    i2c_read(file, buf, 1, APDS9960_GCONF4);
    reg_val = buf[0];
    enable = DEFAULT_GIEN & 0b00000001;
    enable = enable << 1;
    reg_val = reg_val & 0b11111101;
    reg_val = reg_val | enable;
    i2c_write(file, buf, 2, APDS9960_GCONF4, reg_val);

    /*****************************************************
     * Gesture Config Register Dump
     *****************************************************/

     for (reg_dump = 0x80; reg_dump <= 0xAF; reg_dump++){
       if((reg_dump != 0x82) && (reg_dump != 0x8A) && (reg_dump != 0x91) && (reg_dump != 0xA8) && (reg_dump != 0xAC) && (reg_dump != 0xAD)){
         i2c_read(file, buf, 1, reg_dump);
         val_dump = buf[0];
         printf("0x%x: 0x%x\n", reg_dump, val_dump); //Debug Print Statement
       }
     }

     for (reg_dump = 0xE4; reg_dump <= 0xE7; reg_dump++){
       i2c_read(file, buf, 1, reg_dump);
       val_dump = buf[0];
       printf("0x%x: 0x%x\n", reg_dump, val_dump); //Debug Print Statement
     }

     /*****************************************************
      * Initialization Complete
      *****************************************************/

     printf("APDS-9960 initialization complete\n"); //Debug Print Statement

     /*****************************************************
      * Enabling Gesture Sensor
      *****************************************************/

    //Reset all gesture data parameters
    gesture_data_.index = 0;
    gesture_data_.total_gestures = 0;
    gesture_ud_delta_ = 0;
    gesture_lr_delta_ = 0;
    gesture_ud_count_ = 0;
    gesture_lr_count_ = 0;
    gesture_near_count_ = 0;
    gesture_far_count_ = 0;
    gesture_state_ = 0;
    gesture_motion_ = DIR_NONE;

    i2c_write(file, buf, 2, APDS9960_WTIME, 0xFF);
    i2c_write(file, buf, 2, APDS9960_PPULSE, DEFAULT_GESTURE_PPULSE);
    //setLEDBoost(LED_BOOST_300)
    i2c_read(file, buf, 1, APDS9960_CONFIG2);
    reg_val = buf[0];
    boost = LED_BOOST_300 & 0b00000011;
    boost = boost << 4;
    reg_val = reg_val & 0b11001111;
    reg_val = reg_val | boost;
    i2c_write(file, buf, 2, APDS9960_CONFIG2, reg_val);
    //setGestureIntEnable(1)
    i2c_read(file, buf, 1, APDS9960_GCONF4);
    reg_val = buf[0];
    enable = 1 & 0b00000001;
    enable = enable << 1;
    reg_val = reg_val & 0b11111101;
    reg_val = reg_val | enable;
    i2c_write(file, buf, 2, APDS9960_GCONF4, reg_val);
    //setGestureMode(1)
    i2c_read(file, buf, 1, APDS9960_GCONF4);
    reg_val = buf[0];
    mode = 1 & 0b00000001;
    reg_val = reg_val & 0b11111110;
    reg_val = reg_val | mode;
    i2c_write(file, buf, 2, APDS9960_GCONF4, reg_val);
    //enablePower()
   i2c_read(file, buf, 1, APDS9960_ENABLE);
   reg_val = buf[0];
   if (reg_val == ERROR){
     printf("Error with fetching Enable value\n");
   }
   enable = 1 & 0x01;
   if (enable) {
     reg_val = reg_val | (1 << POWER);
   } else {
     reg_val = reg_val & (~(1 << POWER));
   }
   //Writing value back to Enable register
   i2c_write(file, buf, 2, APDS9960_ENABLE, reg_val);
    //setMode(WAIT, 1)
   i2c_read(file, buf, 1, APDS9960_ENABLE);
   reg_val = buf[0];
   if (reg_val == ERROR){
     printf("Error with fetching Enable value\n");
   }
   enable = 1 & 0x01;
   if (enable) {
     reg_val = reg_val | (1 << WAIT);
   } else {
     reg_val = reg_val & (~(1 << WAIT));
   }
   //Writing value back to Enable register
   i2c_write(file, buf, 2, APDS9960_ENABLE, reg_val);
    //setMode(PROXIMITY, 1)
   i2c_read(file, buf, 1, APDS9960_ENABLE);
   reg_val = buf[0];
   if (reg_val == ERROR){
     printf("Error with fetching Enable value\n");
   }
   enable = 1 & 0x01;
   if (enable) {
     reg_val = reg_val | (1 << PROXIMITY);
   } else {
     reg_val = reg_val & (~(1 << PROXIMITY));
   }
   //Writing value back to Enable register
   i2c_write(file, buf, 2, APDS9960_ENABLE, reg_val);
    //setMode(GESTURE, 1)
   i2c_read(file, buf, 1, APDS9960_ENABLE);
   reg_val = buf[0];
   if (reg_val == ERROR){
     printf("Error with fetching Enable value\n");
   }
   enable = 1 & 0x01;
   if (enable) {
     reg_val = reg_val | (1 << GESTURE);
   } else {
     reg_val = reg_val & (~(1 << GESTURE));
   }
   //Writing value back to Enable register
   i2c_write(file, buf, 2, APDS9960_ENABLE, reg_val);

    printf("Gesture Sensor Engine is Enabled and Running\n"); //Debug Print Statement

    /*****************************************************
     * Gesture Sensor Enabled
     *****************************************************/

    do {
      //Reading the interrupt pin at the beginning of every loop
      snprintf(pinVal_buf, sizeof(pinVal_buf), "/sys/class/gpio/gpio%d/value", APDS9960_INT);
      fd = open(pinVal_buf, O_RDONLY);
      read(fd, &ch, 1);
      close(fd);
      //printf("Reading pin %d\n", APDS9960_INT);
      //Interrupt Pin active LOW
      if (ch == '0'){
        interruptRoutine();
        printf("Interrupt Routine Triggered\n"); //Debug Print Statement
      }
      if (isr_flag == 1) {
        printf("isr_flag = %d\n", isr_flag); //Debug Print Statement
        //!!FILL THIS IN!!detachInterrupt
        /*****************************************************
         * Begin Gesture Handling
         *****************************************************/
        //Is Gesture Available?
        i2c_read(file, buf, 1, APDS9960_GSTATUS);
        reg_val = buf[0];
        reg_val = reg_val & APDS9960_GVALID;
        //If reg_val == 1, then Gesture is available
        if (reg_val == 1){
          //Reading Gesture
          printf("Gesture is available, reg_val = %d\n", reg_val); //Debug Print Statement
          printf("Reading Gesture\n"); //Debug Print Statement
          int fifo_level = 0;
          char fifo_data[128]; //changed from int for i2c_read
          int gstatus;
          int bytes_read = 0;
          int motion, i;
          int u_first = 1;
          int d_first = 1;
          int l_first = 1;
          int r_first = 1;
          int u_last = 1;
          int d_last = 1;
          int l_last = 1;
          int r_last = 1;
          int ud_ratio_first;
          int lr_ratio_first;
          int ud_ratio_last;
          int lr_ratio_last;
          int ud_delta;
          int lr_delta;
          int processGesture = 0;
          int decodeGesture = 1;
          while(1){ //Might need to move this back into the outer loop. !!IMPORTANT!!
            printf("Entering Nested Loop\n"); //Debug Print Statement
            delay(FIFO_PAUSE_TIME);
            i2c_read(file, buf, 1, APDS9960_GSTATUS);
            gstatus = buf[0];
            printf("Checking if data is valid\n"); //Debug Print Statement
            if((gstatus & APDS9960_GVALID) == APDS9960_GVALID){
              i2c_read(file, buf, 1, APDS9960_GFLVL);
              fifo_level = buf[0];

              printf("Data is valid, fifo_level is %d\n", fifo_level); //Debug Print Statement
              if(fifo_level > 0){
                printf("There is data in the FIFO (level > 0)\n"); //Debug Print Statement
                i2c_read(file, fifo_data, fifo_level * 4, APDS9960_GFIFO_U);

                bytes_read = fifo_level * 4;
                printf("Read in %d bytes\n", bytes_read); //Debug Print Statement
                printf("The value of those bytes is 1: %d\n", fifo_data[0]); //Debug Print Statement

                if (bytes_read >= 4){
                  for(i = 0; i < bytes_read; i += 4){
                    gesture_data_.u_data[gesture_data_.index] =fifo_data[i + 0];
                    gesture_data_.d_data[gesture_data_.index] =fifo_data[i + 1];
                    gesture_data_.l_data[gesture_data_.index] =fifo_data[i + 2];
                    gesture_data_.r_data[gesture_data_.index] =fifo_data[i + 3];
                    gesture_data_.index++;
                    gesture_data_.total_gestures++;
                  }
                  printf("Total gestures tallied: %d\n", gesture_data_.total_gestures); //Debug Print Statement
                  //This is being triggered when it shouldn't so total_gestures probably isn't being incremented correctly
                  if((gesture_data_.total_gestures <= 4) || (gesture_data_.total_gestures >= 32)){
                    processGesture = 0;
                  } else {
                    for(i = 0; i < gesture_data_.total_gestures; i++ ){
                      if((gesture_data_.u_data[i] > GESTURE_THRESHOLD_OUT) &&
                      (gesture_data_.d_data[i] > GESTURE_THRESHOLD_OUT) ||
                      (gesture_data_.l_data[i] > GESTURE_THRESHOLD_OUT) &&
                      (gesture_data_.r_data[i] > GESTURE_THRESHOLD_OUT)){
                        u_first = gesture_data_.u_data[i];
                        d_first = gesture_data_.d_data[i];
                        l_first = gesture_data_.l_data[i];
                        r_first = gesture_data_.r_data[i];
                        break;
                      }
                    }
                    for(i = gesture_data_.total_gestures - 1; i >= 0; i--){
                      if((gesture_data_.u_data[i] > GESTURE_THRESHOLD_OUT) &&
                      (gesture_data_.d_data[i] > GESTURE_THRESHOLD_OUT) ||
                      (gesture_data_.l_data[i] > GESTURE_THRESHOLD_OUT) &&
                      (gesture_data_.r_data[i] > GESTURE_THRESHOLD_OUT)){
                        u_last = gesture_data_.u_data[i];
                        d_last = gesture_data_.d_data[i];
                        l_last = gesture_data_.l_data[i];
                        r_last = gesture_data_.r_data[i];
                        break;
                      }
                    }

                    /* Calculate the first vs. last ratio of up/down and left/right */
                    printf("Calculating first vs last ratio of up/down and left/right\n"); //Debug Print Statement
                    ud_ratio_first = ((u_first - d_first) * 100) / (u_first + d_first);
                    lr_ratio_first = ((l_first - r_first) * 100) / (l_first + r_first);
                    ud_ratio_last = ((u_last - d_last) * 100) / (u_last + d_last);
                    lr_ratio_last = ((l_last - r_last) * 100) / (l_last + r_last);
                    printf("ud_ratio_first: %d\n", ud_ratio_first); //Debug Print Statement
                    printf("lr_ratio_first: %d\n", lr_ratio_first); //Debug Print Statement
                    printf("ud_ratio_last: %d\n", ud_ratio_last); //Debug Print Statement
                    printf("lr_ratio_last: %d\n", lr_ratio_last); //Debug Print Statement

                    /* Determine the difference between the first and last ratios */
                    ud_delta = ud_ratio_last - ud_ratio_first;
                    lr_delta = lr_ratio_last - lr_ratio_first;
                    printf("ud_delta: %d\n", ud_delta); //Debug Print Statement
                    printf("lr_delta: %d\n", lr_delta); //Debug Print Statement

                    /* Accumulate the UD and LR delta values */
                    gesture_ud_delta_ += ud_delta;
                    gesture_lr_delta_ += lr_delta;
                    printf("gesture_ud_delta_: %d\n", gesture_ud_delta_); //Debug Print Statement
                    printf("gesture_lr_delta_: %d\n", gesture_lr_delta_); //Debug Print Statement

                    //calibrating sensor values
                    if( abs(gesture_ud_delta_) > abs(gesture_lr_delta_) ){
                      gesture_ud_delta_ = gesture_ud_delta_ * 10;
                    } else if(abs(gesture_ud_delta_) < abs(gesture_lr_delta_)){
                      gesture_lr_delta_ = gesture_lr_delta_ * 10;
                    }

                    printf("GESTURE_SENSITIVITY_1: %d\n", GESTURE_SENSITIVITY_1); //Debug Print Statement
                    /* Determine U/D gesture */
                    printf("Determing Up/Down Gesture\n"); //Debug Print Statement
                    if( gesture_ud_delta_ >= GESTURE_SENSITIVITY_1 ) {
                      gesture_ud_count_ = 1;
                      printf("Detected Gesture UP exceeded threshold\n"); //Debug Print Statement
                    } else if( gesture_ud_delta_ <= -GESTURE_SENSITIVITY_1 ) {
                      gesture_ud_count_ = -1;
                      printf("Detected Gesture DOWN exceeded threshold\n"); //Debug Print Statement
                    } else {
                      gesture_ud_count_ = 0;
                      printf("No Up or Down gesture exceeded threshold\n"); //Debug Print Statement
                    }

                    /* Determine L/R gesture */
                    printf("Determing Left/Right Gesture\n"); //Debug Print Statement
                    if( gesture_lr_delta_ >= GESTURE_SENSITIVITY_1 ) {
                      gesture_lr_count_ = 1;
                      printf("Detected Gesture LEFT exceeded threshold\n"); //Debug Print Statement
                    } else if( gesture_lr_delta_ <= -GESTURE_SENSITIVITY_1 ) {
                      gesture_lr_count_ = -1;
                      printf("Detected Gesture RIGHT exceeded threshold\n"); //Debug Print Statement
                    } else {
                      gesture_lr_count_ = 0;
                      printf("No LEFT or RIGHT gesture exceeded threshold\n"); //Debug Print Statement
                    }

                    /* Determine Near/Far gesture */
                    printf("Determining Near/Far gesture\n"); //Debug Print Statement
                    if((gesture_ud_count_ == 0) && (gesture_lr_count_ == 0)){
                      printf("No Up/Down or Left/Right gestures detected\n"); //Debug Print Statement
                      if((abs(ud_delta) < GESTURE_SENSITIVITY_2) && (abs(lr_delta) < GESTURE_SENSITIVITY_2)){
                        if((ud_delta == 0) && (lr_delta == 0)){
                          gesture_near_count_++;
                          printf("Detected Nearness exceeded threshold\n"); //Debug Print Statement
                        } else if((ud_delta != 0) || (lr_delta != 0)){
                          gesture_far_count_++;
                          printf("Detected Farness exceeded threshold\n"); //Debug Print Statement
                        }
                        if((gesture_near_count_ >= 10) && (gesture_far_count_ >= 2)){
                          if((ud_delta == 0) && (lr_delta == 0)){
                            gesture_state_ = NEAR_STATE;
                          } else if((ud_delta != 0) && (lr_delta != 0)){
                            gesture_state_ = FAR_STATE;
                          }
                          processGesture = 1;
                          printf("Line 781: Process Gesture = %d\n", processGesture); //Debug Print Statement
                        }
                      }
                    } else {
                      if((abs(ud_delta) < GESTURE_SENSITIVITY_2) && (abs(lr_delta) < GESTURE_SENSITIVITY_2)){
                        if((ud_delta == 0) && (lr_delta == 0)){
                          gesture_near_count_++;
                        }
                        if(gesture_near_count_ >= 10){
                          gesture_ud_count_ = 0;
                          gesture_lr_count_ = 0;
                          gesture_ud_delta_ = 0;
                          gesture_lr_delta_ = 0;
                        }
                      }
                    }

                    printf("UD_CT: %d\n", gesture_ud_count_); //Debug Print Statement
                    printf("LR_CT: %d\n", gesture_lr_count_); //Debug Print Statement
                    printf("NEAR_CT: %d\n", gesture_near_count_); //Debug Print Statement
                    printf("FAR_CT: %d\n", gesture_far_count_); //Debug Print Statement
                    printf("------------\n"); //Debug Print Statement
                  }
                  printf("processGesture = %d\n", processGesture); //Debug Print Statement
                  printf("Checking Process Gesture Value\n"); //Debug Print Statement
                  if(processGesture){
                    printf("Within Process Gesture Checking\n"); //Debug Print Statement
                    /***** Beginning of DecodeGesture Function *****/
                    /* Return if near or far event is detected */
                    if(gesture_state_ == NEAR_STATE){
                      gesture_motion_ = DIR_NEAR;
                    } else if(gesture_state_ == FAR_STATE){
                      gesture_motion_ = DIR_FAR;
                    }

                    /* Determine swipe direction */
                    if((gesture_ud_count_ == -1) && (gesture_lr_count_ == 0)){
                      gesture_motion_ = DIR_UP;
                    } else if ((gesture_ud_count_ == 1) && (gesture_lr_count_ == 0)){
                      gesture_motion_ = DIR_DOWN;
                    } else if ((gesture_ud_count_ == 0) && (gesture_lr_count_ == 1)){
                      gesture_motion_ = DIR_RIGHT;
                    } else if ((gesture_ud_count_ == 0) && (gesture_lr_count_ == -1)){
                      gesture_motion_ = DIR_LEFT;
                    } else if ((gesture_ud_count_ == -1) && (gesture_lr_count_ == 1)){
                      if(abs(gesture_ud_delta_) > abs(gesture_lr_delta_)){
                        gesture_motion_ = DIR_UP;
                      } else{
                        gesture_motion_ = DIR_RIGHT;
                      }
                    } else if ((gesture_ud_count_ == 1) && (gesture_lr_count_ == -1)){
                      if(abs(gesture_ud_delta_) > abs(gesture_lr_delta_)){
                        gesture_motion_ = DIR_DOWN;
                      } else {
                        gesture_motion_ = DIR_LEFT;
                      }
                    } else if((gesture_ud_count_ == -1) && (gesture_lr_count_ == -1)){
                      if(abs(gesture_ud_delta_) > abs(gesture_lr_delta_)){
                        gesture_motion_ = DIR_UP;
                      } else {
                        gesture_motion_ = DIR_LEFT;
                      }
                    } else if((gesture_ud_count_ == 1) && (gesture_lr_count_ == 1)){
                      if(abs(gesture_ud_delta_) > abs(gesture_lr_delta_)){
                        gesture_motion_ = DIR_DOWN;
                      } else {
                        gesture_motion_ = DIR_RIGHT;
                      }
                    } else {
                      decodeGesture = 0;
                    }
                    /***** End of DecodeGesture Function *****/
                    if(decodeGesture){
                      printf("Gesture Motion: %d", gesture_motion_); //Debug Print Statement
                    }
                  }
                //reset Data
                gesture_data_.index = 0;
                gesture_data_.total_gestures = 0;
                }
              }
            } else {
              delay(FIFO_PAUSE_TIME);
              /***** Beginning of DecodeGesture Function *****/
              /* Return if near or far event is detected */
              if(gesture_state_ == NEAR_STATE){
                gesture_motion_ = DIR_NEAR;
              } else if(gesture_state_ == FAR_STATE){
                gesture_motion_ = DIR_FAR;
              }

              /* Determine swipe direction */
              if((gesture_ud_count_ == -1) && (gesture_lr_count_ == 0)){
                gesture_motion_ = DIR_UP;
              } else if ((gesture_ud_count_ == 1) && (gesture_lr_count_ == 0)){
                gesture_motion_ = DIR_DOWN;
              } else if ((gesture_ud_count_ == 0) && (gesture_lr_count_ == 1)){
                gesture_motion_ = DIR_RIGHT;
              } else if ((gesture_ud_count_ == 0) && (gesture_lr_count_ == -1)){
                gesture_motion_ = DIR_LEFT;
              } else if ((gesture_ud_count_ == -1) && (gesture_lr_count_ == 1)){
                if(abs(gesture_ud_delta_) > abs(gesture_lr_delta_)){
                  gesture_motion_ = DIR_UP;
                } else{
                  gesture_motion_ = DIR_RIGHT;
                }
              } else if ((gesture_ud_count_ == 1) && (gesture_lr_count_ == -1)){
                if(abs(gesture_ud_delta_) > abs(gesture_lr_delta_)){
                  gesture_motion_ = DIR_DOWN;
                } else {
                  gesture_motion_ = DIR_LEFT;
                }
              } else if((gesture_ud_count_ == -1) && (gesture_lr_count_ == -1)){
                if(abs(gesture_ud_delta_) > abs(gesture_lr_delta_)){
                  gesture_motion_ = DIR_UP;
                } else {
                  gesture_motion_ = DIR_LEFT;
                }
              } else if((gesture_ud_count_ == 1) && (gesture_lr_count_ == 1)){
                if(abs(gesture_ud_delta_) > abs(gesture_lr_delta_)){
                  gesture_motion_ = DIR_DOWN;
                } else {
                  gesture_motion_ = DIR_RIGHT;
                }
              } else {
                decodeGesture = 0;
              }
              /***** End of DecodeGesture Function *****/

              motion = gesture_motion_;
              printf("Gesture motion to return is: %d\n", motion); //Debug Print Statement

              /***** Resetting Gesture Parameters *****/
              gesture_data_.index = 0;
              gesture_data_.total_gestures = 0;
              gesture_ud_delta_ = 0;
              gesture_lr_delta_ = 0;
              gesture_ud_count_ = 0;
              gesture_lr_count_ = 0;
              gesture_near_count_ = 0;
              gesture_far_count_ = 0;
              gesture_state_ = 0;
              gesture_motion_ = DIR_NONE;
              /***** End of Gesture Parameters Reset *****/
              printf("Exiting Inner Nested Loop\n");
              break;
            }
          } //End of Nested Loop
          fp = fopen("move.txt", "w+");
          switch(motion){
            case DIR_UP:
              printf("UP\n");
              //fprintf(fp, "U\n");
              fprintf(fp, "D\n");
              break;
            case DIR_DOWN:
              printf("DOWN\n");
              //fprintf(fp, "D\n");
              fprintf(fp, "U\n");
              break;
            case DIR_LEFT:
              printf("LEFT\n");
              //fprintf(fp, "L\n");
              fprintf(fp, "R\n");
              break;
            case DIR_RIGHT:
              printf("RIGHT\n");
              //fprintf(fp, "R\n");
              fprintf(fp, "L\n");
              break;
            case DIR_NEAR:
              printf("NEAR\n");
              //fprintf(fp, "N\n");
              break;
            case DIR_FAR:
              printf("FAR\n");
              //fprintf(fp, "F\n");
              break;
            default:
              printf("NONE\n");
              //fprintf(fp, "X\n");
          }
          fclose(fp);
        }
        isr_flag = 0;
        //!!FILL THIS IN!!attachInterrupt(0, interruptRoutine, FALLING)
      }
    } while(1);

    //Unexporting the pin, not needed but added anyways.
    printf("Code exiting\n"); //Debug Print Statement
    len = snprintf(uexPin_buf, sizeof(uexPin_buf), "%d", APDS9960_INT);
    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    write(fd, uexPin_buf, len);
    close(fd);

  return 0;
}
