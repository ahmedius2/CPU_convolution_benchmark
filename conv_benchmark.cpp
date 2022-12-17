#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>

#include <algorithm>
#include <charconv>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale.h>
#include <random>
#include <sstream>
#include <stdio.h>
#include <vector>


struct timeval tv; 

double timestamp()
{
        double t;
        gettimeofday(&tv, NULL);
        t = tv.tv_sec + (tv.tv_usec/1000000.0);
        return t;
}

// A naive convolution without strides and dilation
// Input : C_in x H_in x W_in
// Kernel: C_out x C_in x kH x kW
// Output: C_out x H_out x W_out
void conv(float *input, int C_in, int H_in, int W_in,
	float *kernels, int C_out, int kH, int kW,
	float *output, int H_out, int W_out)
{
	// num_filters == num_out_channels
	// each filter has size of kw X kh X in_channels
	for(auto filter=0; filter < C_out; ++filter)
		for(auto channel=0; channel < C_in; ++channel)
			for (auto out_h=0; out_h < H_in; ++out_h)
				for (auto out_w=0; out_w < W_in; ++out_w)
					for (auto k_h=0; k_h < kH; ++k_h)
						for (auto k_w=0; k_w < kW; ++k_w)
							output[filter*C_out + out_h*H_out + out_w] += 
								kernels[filter*C_out + channel*C_in + k_h*kH + k_w] * 
								input[channel*C_in + (out_h+k_h)*H_in + (out_w+k_w)];
}




int main(int argc, char *argv[]) 
{
	float *input, *kernels, *output;
	auto channels=64, H_in=256, W_in=256, kernel_size=3;
    int opt;

	/*
	 * get command line options 
	 */
	//while ((opt = getopt(argc, argv, "m:a:n:t:c:i:p:o:f:l:xh")) != -1) {
	while ((opt = getopt(argc, argv, "c:h:w:k:")) != -1) {
		switch (opt) {
		case 'c':
			channels = strtol(optarg, NULL, 0);
			break;
		case 'h':
			H_in = strtol(optarg, NULL, 0);
			break;
		case 'w':
			W_in = strtol(optarg, NULL, 0);
			break;
		case 'k':
			kernel_size = strtol(optarg, NULL, 0);
			break;
		}
	}

	auto H_out = H_in - kernel_size +1;
	auto  W_out = W_in - kernel_size +1;
	//printf("dimention = %d algo = %d\n", dimension, algo);

	input   = (float*)malloc(channels*H_in*W_in* sizeof(float));
	kernels = (float*)malloc(channels*channels*kernel_size*kernel_size* sizeof(float));
	output  = (float*)calloc(channels*H_out*W_out, sizeof(float));

	srand(292);

	// tensor initialization
	for(auto i = 0; i < channels*H_in*W_in; i++)
			input[i] = rand() / 17.3;
	for(auto  i = 0; i < channels*channels*kernel_size*kernel_size; i++)
			kernels[i] = rand() / 17.3;
	for(auto i = 0; i < channels*H_out*W_out; i++)
			output[i] = rand() / 17.3;

  	double start_ts, end_ts; /* time */
	// do convolution
	start_ts = timestamp();
	conv(input, channels, H_in, W_in,
            kernels, channels, kernel_size, kernel_size,
            output, H_out, W_out);
	end_ts = timestamp();
	printf(" %18.2f msecs \n", (end_ts-start_ts)*1000);
  
	free(input);
	free(kernels);
	free(output);


	return EXIT_SUCCESS;
}
