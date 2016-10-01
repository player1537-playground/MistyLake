#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>

#define DIE(msg)                                \
  do {                                          \
    perror(msg);                                \
    exit(EXIT_FAILURE);                         \
  } while (0)

#define TRY(command)                            \
  do {                                          \
    if (command) {                              \
      DIE(#command);                            \
    }                                           \
  } while (0)

int main(int argc, char **argv) {
  int fd, i, x, dx;
  struct uinput_user_dev uidev;
  struct input_event ev;

  TRY((fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK)) < 0);

  TRY(ioctl(fd, UI_SET_EVBIT, EV_ABS) < 0);

  TRY(ioctl(fd, UI_SET_ABSBIT, ABS_X) < 0);

  memset(&uidev, 0, sizeof(uidev));
  snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput_test");
  uidev.id.bustype = BUS_USB;
  uidev.id.vendor = 0x0079;
  uidev.id.product = 0x0006;
  uidev.id.version = 1;
  uidev.absmin[ABS_X] = 0;
  uidev.absmax[ABS_X] = (1 << 16) - 1;

  TRY(write(fd, &uidev, sizeof(uidev)) == -1);
  TRY(ioctl(fd, UI_DEV_CREATE) < 0);

  x = uidev.absmin[ABS_X];
  dx = 123;
  for (i=0; i<4000; ++i) {
    x += dx;
    if (x + dx >= uidev.absmax[ABS_X] || x + dx <= uidev.absmin[ABS_X]) {
      dx *= -1;
    }

    memset(&ev, 0, sizeof(ev));
    ev.type = EV_ABS;
    ev.code = ABS_X;
    ev.value = x;
    TRY(write(fd, &ev, sizeof(ev)) == -1);

    memset(&ev, 0, sizeof(ev));
    ev.type = EV_SYN;
    ev.code = 0;
    ev.value = 0;
    TRY(write(fd, &ev, sizeof(ev)) == -1);

    usleep(10*1000);
  }

  TRY(ioctl(fd, UI_DEV_DESTROY) < 0);
  TRY(close(fd) != 0);

  exit(EXIT_SUCCESS);
}
