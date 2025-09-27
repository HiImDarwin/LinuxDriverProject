#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h> 
#include <linux/interrupt.h>

#define IO_LED 21
#define IO_BUTTON 20

// gpiochip571
#define IO_OFFSET 571

// 用來存 GPIO descriptor 的指標
static struct gpio_desc *led, *button;

// 用來存 button 對應的 IRQ 編號
static int irq_num;

static irqreturn_t button_isr(int irq, void *dev_id) {
  // 讀取按鍵目前狀態
  int val = gpiod_get_value(button);
  printk("gpioctrl - Button interrupt, value = %d\n", val);

  // 按下時點亮 LED，放開時熄滅 LED
  gpiod_set_value(led, val);

  // 表示中斷已處理
  return IRQ_HANDLED;
}

static int __init my_init(void) {
  int status;

  // 取得 LED 的 GPIO descriptor
  led = gpio_to_desc(IO_OFFSET + IO_LED);
  if (!led) {
    printk("gpioctrl - Error getting pin 21\n");
    return -ENODEV;
  }

  
  button = gpio_to_desc(IO_OFFSET + IO_BUTTON);
  if (!button) {
    printk("gpioctrl - Error getting pin 20\n");
    return -ENODEV;
  }

  // 設定 LED 為輸出，初始值 0 (熄滅)
  status = gpiod_direction_output(led,0);
  if (status) {
    printk("gpioctrl - Error setting pin 21 to output\n");
    return status;
  }

  // 取得 Button 的 GPIO descriptor
  status = gpiod_direction_input(button);
  if (status) {
    printk("gpioctrl - Error setting pin 20 to output\n");
    return status;
  }

  // 將 Button GPIO 對應成 IRQ
  irq_num = gpiod_to_irq(button);
  if (irq_num < 0) {
    printk("gpioctrl - Fail to map gpio to irq\n");
    return irq_num;
  }

  // 註冊中斷，觸發方式為上升沿與下降沿
  status = request_irq(irq_num, button_isr, 
                       IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
                       "gpio_button", NULL);
  
  if (status) {
    printk("gpioctrl - Cannot request IRQ\n");
    return status;
  }

  return 0;
}


static void __exit my_exit(void) {
  // 釋放中斷
  free_irq(irq_num, NULL);
  // 熄滅 LED
  gpiod_set_value(led, 0);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Darwin");
MODULE_DESCRIPTION("A simple for using GPIO without device tree");