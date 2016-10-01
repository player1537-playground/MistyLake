#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char **argv) {
  struct termios tty;
  struct termios tty_old;
  int fd, rv;
  char buffer[1024];

  fd = open( "/dev/ttyACM0", O_RDWR| O_NOCTTY );
  if (fd < 0) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  memset (&tty, 0, sizeof tty);

  /* Error Handling */
  if ( tcgetattr ( fd, &tty ) != 0 ) {
    perror("tcgetattr");
    exit(EXIT_FAILURE);
  }

  /* Save old tty parameters */
  tty_old = tty;

  /* Set Baud Rate */
  cfsetospeed (&tty, (speed_t)B9600);
  cfsetispeed (&tty, (speed_t)B9600);

  /* Setting other Port Stuff */
  tty.c_cflag     &=  ~PARENB;            // Make 8n1
  tty.c_cflag     &=  ~CSTOPB;
  tty.c_cflag     &=  ~CSIZE;
  tty.c_cflag     |=  CS8;

  tty.c_cflag     &=  ~CRTSCTS;           // no flow control
  tty.c_cc[VMIN]   =  1;                  // read doesn't block
  tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
  tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

  /* Make raw */
  cfmakeraw(&tty);

  /* Flush Port, then applies attributes */
  tcflush( fd, TCIFLUSH );
  if ( tcsetattr ( fd, TCSANOW, &tty ) != 0) {
    perror("tcsetattr");
    exit(EXIT_FAILURE);
  }

  while (1) {
    rv = read(fd, buffer, sizeof(buffer)/sizeof(*buffer));
    if (rv < 0) {
      perror("read");
      exit(EXIT_FAILURE);
    }

    if (rv == 0) {
      break;
    }

    write(1, "Got: ", 5);
    write(1, buffer, rv);
    write(1, "\n", 1);
  }

  exit(EXIT_SUCCESS);
}
