#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

/*
 * Defensive UnStuffData, which prevents poorly
 * conditioned data at *ptr from over-running
 * the available buffer at *dst.
 */

void UnStuffData(const unsigned char *ptr,
                 unsigned long length, unsigned char *dst)
{
  const unsigned char *end = ptr + length;
  while (ptr < end)
  {
    int i, code = *ptr++;
    for (i=1; ptr<end && i < code; i++)
      *dst++ = *ptr++;
    if (code < 0xFF)
      *dst++ = 0;
  }
}

void setup_tty(int fd) {
  struct termios tty;
  struct termios tty_old;

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
}

void process(char *parsed, int len) {
  int i;

  printf("Got:");
  for (i=0; i<len; ++i) {
    printf(" %02x", parsed[i] & 0xFF);
  }
  printf("\n");
}

int main(int argc, char **argv) {
  int fd, rv, i, start, end, head, buflen, len;
  char buffer[1024];
  char parsed[1024];
  char temp[1024];

  fd = open( "/dev/ttyACM0", O_RDWR| O_NOCTTY );
  if (fd < 0) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  setup_tty(fd);

  head = 0;
  buflen = 0;
  while (1) {
    rv = read(fd, buffer + start, sizeof(buffer)/sizeof(*buffer) - head);
    if (rv < 0) {
      perror("read");
      exit(EXIT_FAILURE);
    }

    if (rv == 0) {
      break;
    }

    buflen += rv;

    while (1) {

      // Find the first 0x00 byte that starts the next message
      start = -1;
      for (i=0; i<buflen; ++i) {
        if (buffer[(head+i)%buflen] == 0) {
          start = i;
          break;
        }
      }

      // We didn't find 0x00, so let's skip to the next one
      if (start == -1) {
        break;
      }

      // Move head to point to 0x00
      head += start;
      head %= sizeof(buffer)/sizeof(*buffer);

      // Find end of message, the next 0x00
      end = -1;
      for (i=1; i<buflen; ++i) {
        if (buffer[(head+i)%buflen] == 0) {
          end = i;
          break;
        }
      }

      // If we didn't find it, fuck it, restart
      if (end == -1) {
        break;
      }

      // Copy range [start (now 0), end) to temp
      for (i=0; i<end; ++i) {
        temp[i] = buffer[(head+i+1)%buflen];
      }
      len = i;

      // Remove COBS overhead
      UnStuffData(temp, len - 1, parsed);

      // Process
      process(parsed, len - 2);

      // Move head to the next 0x00 after the one we processed
      head += end;
      head %= sizeof(buffer)/sizeof(*buffer);
    }
  }

  exit(EXIT_SUCCESS);
}
