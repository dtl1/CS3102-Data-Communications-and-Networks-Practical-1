/*
  Probe program for sending UDP probes to slurpe-3

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "UdpSocket.h"

#define MY_PORT   ((uint16_t) 21929) // from 'id -u'
#define PACKET_SIZE      ((int) 16)
#define NUM_PROBES ((int) 100)
#define SLEEPTIME ((uint32_t) 1) // 1 second

//error functions
#define ERROR(_s) fprintf(stderr, "%s\n", _s)
void perror(const char *s);

int
nonBlocking(int fd);

int
main(int argc, char *argv[]){
  UdpSocket_t *local, *remote;

  if (argc != 2) {
    ERROR("usage: slurpe-probe <hostname>");
    exit(0);
  }

  if ((local = setupUdpSocket_t((char *) 0, MY_PORT)) == (UdpSocket_t *) 0) {
    ERROR("local problem");
    exit(0);
  }

  if ((remote = setupUdpSocket_t(argv[1], MY_PORT)) == (UdpSocket_t *) 0) {
    ERROR("remote hostname/port problem");
    exit(0);
  }

  if (openUdp(local) < 0) {
    ERROR("openUdp() problem");
    exit(0);
  }

  //set non-blocking i/o
  if (nonBlocking(local->sd) < 0) {
    ERROR("nonBlocking(local->sd) problem");
    exit(0);
  }

  //print starting up information
  printf("** slurpe-probe %s : src/dst port %d, size %d\n", argv[1], MY_PORT, PACKET_SIZE);
  printf("** sending %d packets, 1 every %d second(s)\n", NUM_PROBES, SLEEPTIME);
  printf("** clock resolution: 1 ns, start time: ");
  printTimeUnits();
  printf("\n\n");

  //print column headers
  printf("packet arrival time\tpacket number\tpacket size (bytes)\n");

  int packetNum = 0;
  int lostPackets = 0;

  while(packetNum <= NUM_PROBES) {
    
    //create and initialse struct
    UdpBuffer_t* packet = malloc(PACKET_SIZE);
    uint8_t* packetBytes, number;
    number = 123;
    packetBytes = &number;
    packet->n = 16;
    packet->bytes = packetBytes;

    //send UDP packet
    int ret = sendUdp(local,remote,packet);
    //printf("%d bytes sent\n", ret);  --used earlier for testing/debugging


    //receive UDP packet
    int rec = recvUdp(local,remote, packet);

    //check for error return (-1) to see if packet was sent back or not, if not then print packet dropped
    if(rec == -1 && packetNum > 0){
        printf("\t\t----- packet dropped -----\n");
        lostPackets++;
    } else if (packetNum > 0){
         //print the time the packet is received along with its number and size
         printTime();
         printf("\t\t%d\t\t%d\n", packetNum, rec);
    }

   

    free(packet);
    packetNum++;

    (void) sleep(SLEEPTIME); // avoid hogging CPU in while() loop
  }

  closeUdp(local);
  closeUdp(remote);

  //print the final time and the packet loss statistics
  printf("end time:");
  printTimeUnits();
  float packetLoss = lostPackets / (float)NUM_PROBES;
  printf(", packets sent: %d, packets lost: %d, packet loss: %.2f\n", NUM_PROBES, lostPackets, packetLoss);

  return 0;
}


//sets i/o to be non-blocking
int
nonBlocking(int fd)
{
  int r, flags = O_NONBLOCK;

  if ((r = fcntl(fd, F_SETFL, flags)) < 0) {
    perror("setAsyncFd(): fcntl() problem");
    exit(0);
  }

  return r;
}