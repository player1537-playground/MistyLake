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

  // Open shit. If it's not /dev/uinput, it's /dev/input/uinput.
  TRY((fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK)) < 0);

  // UI_SET_EVBIT is in /usr/include/linux/uinput.h
  // EV_ABS is in /usr/include/linux/input-event-codes.h

  // You do a UI_SET_EVBIT to EV_ABS (or EV_KEY, EV_REL, etc)
  TRY(ioctl(fd, UI_SET_EVBIT, EV_ABS) < 0);

  // Then for each EV_* you also do UI_SET_*BIT to specify what parts of the *
  // system you want to use. For instance, with EV_ABS we want to use axis X, so
  // we need a UI_SET_ABSBIT with ABS_X.
  TRY(ioctl(fd, UI_SET_ABSBIT, ABS_X) < 0);

  memset(&uidev, 0, sizeof(uidev));
  snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput_test");
  uidev.id.bustype = BUS_USB;
  uidev.id.vendor = 0x0079;
  uidev.id.product = 0x0006;
  uidev.id.version = 1;

  // For each axis, we need to specicy the min/max. This is a signed 32 bit
  // integer, but the actual range is -2**15 and +2**15, I think. You should
  // just use the max/min from the hardware.
  uidev.absmin[ABS_X] = 0;
  uidev.absmax[ABS_X] = (1 << 16) - 1;

  TRY(write(fd, &uidev, sizeof(uidev)) == -1);
  TRY(ioctl(fd, UI_DEV_CREATE) < 0);

  // x and dx are testing code. Use the values from the arduino.
  x = uidev.absmin[ABS_X];
  dx = 123;
  for (i=0; i<4000; ++i) {
    x += dx;
    if (x + dx >= uidev.absmax[ABS_X] || x + dx <= uidev.absmin[ABS_X]) {
      dx *= -1;
    }

    // Send an axis. For axis data, type is EV_ABS, code is the axis (ABS_X),
    // and absmin <= value <= absmax.
    memset(&ev, 0, sizeof(ev));
    ev.type = EV_ABS;
    ev.code = ABS_X;
    ev.value = x;
    TRY(write(fd, &ev, sizeof(ev)) == -1);

    // Send more axes.

    // Send buttons. Check out BTN_X in
    // /usr/include/linux/input-event-codes.h. Don't forget to specify EV_KEY
    // and make sure to enable that button.


    // Send a SYN packet to synchronize.
    memset(&ev, 0, sizeof(ev));
    ev.type = EV_SYN;
    ev.code = 0;
    ev.value = 0;
    TRY(write(fd, &ev, sizeof(ev)) == -1);

    usleep(10*1000);
  }

  // Close shit.
  TRY(ioctl(fd, UI_DEV_DESTROY) < 0);
  TRY(close(fd) != 0);

  exit(EXIT_SUCCESS);
}
