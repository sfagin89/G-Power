/**************************************************************************
 * temp_io.c kernel module
 * Sara Fagin
 * EC535
 * Submitted: 4/5/2021
 *
 * Current Functionality:
 * On initialization starts in Normal Mode and cycles correctly
 * On first B0 click, enters Red Flash Mode and cycles correctly
 * On next B0 click, enters Yellow Flash Mode and cycles correctly
 * On third B0 click, enters Normal Mode and cycles correctly
 * Write: Writes a number between 1-9 to the /dev/temp_io char dev for Hz
 * * All other written data is gracefully ignored.
 * Read: Using cat /dev/temp_io reads out the desired values
 * Pedestrian Mode: On B1 Click enter Red/Yellow On for 5 cycles on next 'stop'
 * * phase of normal mode (red) then start normal mode
 *
 * Functionality to Add:
 * Light Check Mode
 *
 * Desired Functionality:
 * Normal Mode: GreenOn 3x -> YellowOn 1x -> RedOn 2x -> Start Over
 * * If B0 -> Start RedFlash
 * * If B1, From YellowOn -> Start Pedestrian
 * * * If B0 -> Start RedFlash
 * * Pedestrian Mode: RedOn/YellowOn 5x -> Start Normal
 * Red Flash Mode: RedOn 1x -> RedOff 1x -> Start Over
 * * If B0 -> Start YellowFlash
 * Yellow Flash Mode: YellowOn 1x -> YellowOff 1x -> Start Over
 * * If B0 -> Start Normal
 *************************************************************************/

 #include <linux/init.h>
 #include <linux/module.h>
 #include <linux/fs.h> /* everything... */
 #include <linux/uaccess.h>
 #include <linux/gpio/consumer.h>
 #include <linux/gpio.h>
 #include <linux/interrupt.h>
 #include <linux/kernel.h>
 #include <linux/errno.h>
 #include <linux/timer.h>
 #include <linux/jiffies.h>

 MODULE_LICENSE("Dual BSD/GPL");
 MODULE_AUTHOR("Sara Fagin");
 MODULE_DESCRIPTION("GPIO Consumer for controlling a traffic light system");

 // Hardcoding GPIO LEDs and Buttons
 static unsigned int gpio_BTN0 = 68;     // Hardcoding Up Button 0 gpio
 static unsigned int gpio_BTN1 = 44;     // Hardcoding Right Button 1 gpio
 static unsigned int gpio_BTN2 = 26;     // Hardcoding Down Button 2 gpio
 static unsigned int gpio_BTN3 = 46;     // Hardcoding Left Button 3 gpio
 static unsigned int irq_B0, irq_B1;     // IRQ Numbers for Interrupt Requests
 static unsigned int irq_B2, irq_B3;     // IRQ Numbers for Interrupt Requests
 static unsigned int direction;          // 0=Up, 1=Right, 2=Down, 3=Left

/* Major number */
 static int temp_io_major = 61;
 static unsigned capacity = 512;
 /* Buffer to store data */
 static char *temp_io_buffer;
 static char *temp_io_buffer_end;
 /* Length of the current buffer */
 static int temp_io_len;

 //static void temp_io_init(void);
 //static void temp_io_exit(void);
 static int temp_io_open(struct inode *inode, struct file *filp);
 static int temp_io_release(struct inode *inode, struct file *filp);
 static int temp_io_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
 static int temp_io_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
 static irq_handler_t  gpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);


 struct file_operations temp_io_fops = {
   read: temp_io_read,
   write: temp_io_write,
   open: temp_io_open,
   release: temp_io_release
 };

 // Setting up the GPIO and IRQ
 static int __init gpio_init(void){
   int result = 0;
   int line_0 = 0; // for requesting interrupt line for Button 0
   int line_1 = 0; // for requesting interrupt line for Button 1
   int line_2 = 0; // for requesting interrupt line for Button 2
   int line_3 = 0; // for requesting interrupt line for Button 3
   //printk(KERN_INFO "Initializing the temp_io KM\n");

   result = register_chrdev(temp_io_major, "temp_io", &temp_io_fops);
   if (result < 0){
     printk(KERN_ALERT "temp_io: cannot obtain major number %d\n", temp_io_major);
     return result;
   }

   temp_io_buffer = kmalloc(capacity, GFP_KERNEL);
   if(!temp_io_buffer){
     return -ENOMEM;
   }

   memset(temp_io_buffer, 0, capacity);
   temp_io_len = 0;

   //Test: are the gpio numbers valid?
   if(!gpio_is_valid(gpio_BTN0)){
     printk(KERN_INFO "Invalid GPIO number for Button 0");
     return -ENODEV;
   }
   if(!gpio_is_valid(gpio_BTN1)){
     printk(KERN_INFO "Invalid GPIO number for Button 1");
     return -ENODEV;
   }
   if(!gpio_is_valid(gpio_BTN2)){
     printk(KERN_INFO "Invalid GPIO number for Button 2");
     return -ENODEV;
   }
   if(!gpio_is_valid(gpio_BTN3)){
     printk(KERN_INFO "Invalid GPIO number for Button 3");
     return -ENODEV;
   }

   //Setting up Buttons. Input Mode, debouncing with a delay of 200ms.
   gpio_request(gpio_BTN0, "sysfs");
   gpio_direction_input(gpio_BTN0);
   gpio_set_debounce(gpio_BTN0, 200);

   gpio_request(gpio_BTN1, "sysfs");
   gpio_direction_input(gpio_BTN1);
   gpio_set_debounce(gpio_BTN1, 200);

   gpio_request(gpio_BTN2, "sysfs");
   gpio_direction_input(gpio_BTN2);
   gpio_set_debounce(gpio_BTN2, 200);

   gpio_request(gpio_BTN3, "sysfs");
   gpio_direction_input(gpio_BTN3);
   gpio_set_debounce(gpio_BTN3, 200);

   // Maps IRQ numbers
   irq_B0 = gpio_to_irq(gpio_BTN0);
   irq_B1 = gpio_to_irq(gpio_BTN1);
   irq_B2 = gpio_to_irq(gpio_BTN2);
   irq_B3 = gpio_to_irq(gpio_BTN3);
   //printk(KERN_INFO "Button 0 is mapped to IRQ: %d\n", irq_B0);
   //printk(KERN_INFO "Button 1 is mapped to IRQ: %d\n", irq_B1);

   // Requesting two interrupt lines
   line_0 = request_irq(irq_B0, (irq_handler_t) gpio_irq_handler, IRQF_TRIGGER_RISING, "gpio_handler", NULL);
   line_1 = request_irq(irq_B1, (irq_handler_t) gpio_irq_handler, IRQF_TRIGGER_RISING, "gpio_handler", NULL);
   line_2 = request_irq(irq_B2, (irq_handler_t) gpio_irq_handler, IRQF_TRIGGER_RISING, "gpio_handler", NULL);
   line_3 = request_irq(irq_B3, (irq_handler_t) gpio_irq_handler, IRQF_TRIGGER_RISING, "gpio_handler", NULL);

   // Checking if irq succeeded
   if(line_0 != 0) return line_0;
   if(line_1 != 0) return line_1;
   if(line_2 != 0) return line_2;
   if(line_3 != 0) return line_3;

   //printk(KERN_INFO "interrupt requests successful\n");

   return result;
 }

 static void __exit gpio_exit(void){
   free_irq(irq_B0, NULL);
   free_irq(irq_B1, NULL);
   free_irq(irq_B2, NULL);
   free_irq(irq_B3, NULL);
   gpio_free(gpio_BTN0);
   gpio_free(gpio_BTN1);
   gpio_free(gpio_BTN2);
   gpio_free(gpio_BTN3);

   unregister_chrdev(temp_io_major, "temp_io");

   if (temp_io_buffer){
     kfree(temp_io_buffer);
   }

   //printk(KERN_INFO "Exiting temp_io\n");
 }

 static int temp_io_open(struct inode *inode, struct file *filp){
   //printk(KERN_INFO "open called\n");
   return 0;
 }

 static int temp_io_release(struct inode *inode, struct file *filp){
   //printk(KERN_INFO "release called\n");
   return 0;
 }

 static ssize_t temp_io_read(struct file *filp, char *buf, size_t count, loff_t *f_pos){

   if(*f_pos>=capacity){
     return 0;
   }

   //need the buffer stuff
   char DirectionInput[50];
   //copy to user

   switch(direction){
     case 0:
      strcpy(DirectionInput, "Up\n");
      break;
     case 1:
      strcpy(DirectionInput, "Right\n");
      break;
     case 2:
      strcpy(DirectionInput, "Down\n");
      break;
     case 3:
      strcpy(DirectionInput, "Left\n");
      break;
   }

   strcat(DirectionInput, "\0");

   //if (gpio_get_value(gpio_RED)) strcpy(redStatus, "red on, ");

   memset(temp_io_buffer, 0, capacity);
   strcpy(temp_io_buffer, DirectionInput);

   if(*f_pos + count > capacity){
     count = capacity - *f_pos;
   }

   if (copy_to_user(buf, temp_io_buffer + *f_pos, count) != 0){
     return -EFAULT;
   }

   *f_pos += count;
   return count;
 }

 static ssize_t temp_io_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos){
   int numIn;
   //char temp[256], *tptr = temp;

   //copy from user
   memset(temp_io_buffer, 0, sizeof(*temp_io_buffer));
   if (copy_from_user(temp_io_buffer + *f_pos, buf, count)){
     printk(KERN_ALERT "Copy From User Error!\n");
     return -EFAULT;
   }

   temp_io_buffer_end = kmalloc(capacity, GFP_KERNEL);
   if(!temp_io_buffer_end){
     return -ENOMEM;
   }

   numIn = simple_strtoul(temp_io_buffer, &temp_io_buffer_end, 10);

   if (((numIn >= 0) && (numIn <= 3)) && count == 2){
     direction = numIn;
   }

   return count;
 }

 static irq_handler_t gpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs){
   // Checking if BTN0 has been pressed;
   if(irq == irq_B0) {
     direction = 0;
   }
   // Checking if BTN1 has been pressed;
   if(irq == irq_B1) {
     direction = 1;
   }
   // Checking if BTN2 has been pressed;
   if(irq == irq_B2) {
     direction = 2;
   }
   // Checking if BTN3 has been pressed;
   if(irq == irq_B3) {
     direction = 3;
   }

   return (irq_handler_t) IRQ_HANDLED;
 }

 module_init(gpio_init);
 module_exit(gpio_exit);
