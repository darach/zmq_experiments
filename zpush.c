/******************************************************************************
 * The MIT License (MIT)
 * 
 * Copyright (c) 2014 Darach Ennis
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zmq.h>

int main (int argc, char **argv)
{
    if (argc != 5)
    {
        fprintf(stderr, "Usage:\n\n    %s <mode> <zeromq-push-url> <delay-us> <count>", argv[0]);
        fprintf(stderr, "\n\n    example: $ %s bind tcp://*:4001 10000\n", argv[0]);
        fprintf(stderr, "\n\n    example: $ %s connect tcp://localhost:4001 10000\n", argv[0]);
        fflush(stderr);
        return 1;
    }

    const char *mode = argv[1];
    const char *zurl = argv[2];
    const int delay_us = atoi(argv[3]);
    unsigned long int count = strtoul(argv[4], NULL, 10);

    printf("mode: %s\n", mode);
    printf("zurl: %s\n", zurl);
    printf("delay_us: %i\n", delay_us);
    printf("count: %lu\n", count);

    //  Prepare our context and publisher
    void *context = zmq_ctx_new ();
    void *publisher = zmq_socket (context, ZMQ_PUSH);
    if (strncmp("bind",mode,128) == 0)
    {
        zmq_bind (publisher, zurl);
        printf("Listening on %s\n", zurl);
    }
    else
    {
        zmq_connect (publisher, zurl);
        printf("Connecting to %s\n", zurl);
    }

    unsigned long int current = 0;
    unsigned long int tx = 0;
    do {
        // int size = zmq_send(publisher, channel_name, strlen(channel_name), ZMQ_SNDMORE);
        char buf[256];
        sprintf(buf,"%lu bottles of beer on the wall",current++);
        int size = zmq_send(publisher, buf, strlen(buf), 0);
        usleep(delay_us);
        tx += size;
    } while(--count > 0);

    printf("Total bytes sent: %lu", tx);

    zmq_close (publisher);
    zmq_ctx_destroy (context);
    return 0;
}
