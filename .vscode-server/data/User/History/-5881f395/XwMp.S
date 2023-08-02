	.text

	/*
         * save the old thread's registers,
         * restore the new thread's registers.
         */

	.globl thread_switch
thread_switch:
	/* YOUR CODE HERE */
 12         sd ra, 0(a0)
 13         sd sp, 8(a0)
 14         sd s0, 16(a0)
 15         sd s1, 24(a0)
 16         sd s2, 32(a0)
 17         sd s3, 40(a0)
 18         sd s4, 48(a0)
 19         sd s5, 56(a0)
 20         sd s6, 64(a0)
 21         sd s7, 72(a0)
 22         sd s8, 80(a0)
 23         sd s9, 88(a0)
 24         sd s10, 96(a0)
 25         sd s11, 104(a0)
 26 
 27         ld ra, 0(a1)
 28         ld sp, 8(a1)
 29         ld s0, 16(a1)
 30         ld s1, 24(a1)
 31         ld s2, 32(a1)
 32         ld s3, 40(a1)
 33         ld s4, 48(a1)
 34         ld s5, 56(a1)
 35         ld s6, 64(a1)
 36         ld s7, 72(a1)
 37         ld s8, 80(a1)
 38         ld s9, 88(a1)
 39         ld s10, 96(a1)
 40         ld s11, 104(a1)



	ret    /* return to ra */
