#include "stdio.h"
#include "stdlib.h"
#include "complex.h"

void fft(complex* target, size_t n)
{
    // Not basic question
    if(n > 1)
    {
        // Separating Array, prepare for following operations
        complex* target_tmp = (complex*)malloc((n/2) * sizeof(complex));
        if(target_tmp == NULL)
            usbport.printf("WARNING: MEMORY ALLOCATE ERROR.\n");
        
        for(size_t i = 0; i != (n/2); i++)
        {
            //shifted + down_sampled
            target_tmp[i] = target[2*i + 1];
            //down_sampled
            target[i] = target[2*i];
        }
        
        for(size_t i = 0; i != (n/2); i++)
        {
            target[n/2+i] = target_tmp[i];
        }
        
        free(target_tmp);
        
        // Recursive Logic, breaks into two sub-questions
        fft(target, n/2);
        fft((target + n/2), n/2);
        
        // Sub-questions solved, DFT synthesis
        for(size_t i = 0; i != n/2; i++)
        {
            complex e(target[i]);
            complex o(target[i+n/2]);
            complex w = exp(complex(0,-2.0*PI*i/n));
            
            // Symmetric properties of w applied in calculation.
            target[i]       = e + (w * o);
            target[i+n/2]   = e - (w * o);
        }
    }
}
