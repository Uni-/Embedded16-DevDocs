#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <sys/stat.h>
#include <fcntl.h>

#include <string.h>

#define GPIO_CS_PORT "24"

int gpio_value_fd = 0;

void init()
{
  int gpio_export_fd = open("/sys/class/gpio/export", O_WRONLY);

  write(gpio_export_fd, GPIO_CS_PORT, strlen(GPIO_CS_PORT));

  close(gpio_export_fd);

  int gpio_dir = open("/sys/class/gpio/gpio" GPIO_CS_PORT "/direction", O_WRONLY);
  write(gpio_dir, "out", 3);
  close(gpio_dir);

  gpio_value_fd = open("/sys/class/gpio/gpio" GPIO_CS_PORT "/value", O_WRONLY);

  if(gpio_value_fd < 0)
    {
      printf("ERR!\n");
    }
}

int main(int argc, char **argv)
{
  int fd = open("/dev/spidev0.0", O_RDWR);
  char buf;
  char ret[128];

  init();

  write(gpio_value_fd, "0", 1);
  usleep(1000);

  write(fd, "abcd", 4);

  usleep(1000*10); // Wait 10msec.

  buf = 0x00;
  while(0x00 == buf)
    {
      usleep(1000);
      read(fd, &buf, 1);
    }

  ret[0] = buf;
  usleep(10);
  read(fd, ret + 1, 1);
  usleep(10);
  read(fd, ret + 2, 1);
  usleep(10);
  read(fd, ret + 3, 1);

  printf("Return is: 0x%X 0x%X 0x%X 0x%x\n",
         ret[0], ret[1], ret[2], ret[3]);

  write(gpio_value_fd, "1", 1);
  return 0;
}
