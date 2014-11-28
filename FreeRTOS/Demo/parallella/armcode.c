#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <e-hal.h>

void print_mbox(e_epiphany_t *dev, e_mem_t *emem, char *msg);

#define _BufOffset (0x01000000)

unsigned M[5];
unsigned IVT[10];
e_mem_t      emem;

int calc_remote(int n) {
    do {
        e_read(&emem, 0, 0, (off_t) (0x0000), (void *) &(M[0]), sizeof(M));
    } while(M[1]!=0); // waits until M[1]==0
    
    M[1]= n;
    e_write(&emem, 0, 0, (off_t) (0x0000), (void *) &(M[0]), sizeof(M));
    
    
    do {
        e_read(&emem, 0, 0, (off_t) (0x0000), (void *) &(M[0]), sizeof(M));
    } while(M[1]!=0); // waits until M[1]==0
    
    
    e_read(&emem, 0, 0, (off_t) (0x0000), (void *) &(M[0]), sizeof(M));
    
    return M[2];
    
}

int main(int argc, char *argv[])
{
    unsigned row, col, coreid;
    unsigned di, ci, go, go_all;
    e_platform_t platform;
    e_epiphany_t dev;

    // Initialize progress state in mailbox
    M[0]  = 0xdeadbeef;
    M[1]  = M[2] = M[3] = M[4] = M[0];

    // initialize system, read platform params from
    // default HDF. Then, reset the platform.
    e_init(NULL);
    e_reset_system();

    // Open the first and second cores for master and slave programs, resp.
    e_open(&dev, 0, 0, 1, 2);
    
    // Allocate the ext. mem. mailbox
    e_alloc(&emem, _BufOffset, sizeof(M));

    // Load programs on cores.
    e_load("main.srec", &dev, 0, 0, E_FALSE);
    e_load("worker.srec", &dev, 0, 1, E_FALSE);

    // clear mailbox.
    e_write(&emem, 0, 0, (off_t) (0x0000), (void *) &(M[0]), sizeof(M));

    // Print mbox status.
    
    // start all cores
    e_start_group(&dev);
    
    usleep(100e3);
    
    int i;
    for(i=1;i<24;i++) {
        int remote = calc_remote(i);
        printf("core: %i -> %i\n", i, remote);
        
    }
    // Finalize
    e_close(&dev);
    e_free(&emem);
    e_finalize();

    return 0;
}
