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

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

const int rcvhwm = 0;

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage:\n\n    %s <mode> <zeromq-pull-url>", argv[0]);
        fprintf(stderr, "\n\n    example: $ %s connect tcp://localhost:4001\n", argv[0]);
        fprintf(stderr, "\n\n    example: $ %s bind tcp://*:4001\n", argv[0]);
        fflush(stderr);
        return 1;
    }

    const char* mode = argv[1];
    const char* zurl = argv[2];
    unsigned long int min = ULONG_MAX;

    printf("mode: %s\n", mode);
    printf("zurl: %s\n", zurl);

    void *context = zmq_ctx_new ();
    void *subscriber = zmq_socket (context, ZMQ_PULL);
    zmq_setsockopt (subscriber, ZMQ_RCVHWM, &rcvhwm, sizeof(rcvhwm));
    if (strncmp("connect",mode,128) == 0)
    {
        zmq_connect (subscriber, zurl);
        printf("Connecting to %s\n", zurl);
    }
    else
    {
        zmq_bind (subscriber, zurl);
        printf("Listening on %s\n", zurl);
    }

    while (1) {
        char data[1024];
        int size = zmq_recv(subscriber, data, 1023, 0);
        if (size == -1)
        {
            break;
        }
        if (size > 1023)
        {
            size = 1023;
        }
        data[size] = 0;
        
        unsigned long int as_num = strtoul(data, NULL, 10);
        if (as_num < min)
        {
            min = as_num;
        }

        printf ("%s (%lu)\n", data, min);
    }

    // ZeroMQ context was terminated (for some unknown reason)
    //
    zmq_close (subscriber);
    zmq_ctx_destroy (context);

    return 0;
}
