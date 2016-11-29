void setupClock(void) {
   setup_oscillator(OSC_8MHZ|OSC_INTRC);
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_32|RTCC_8_bit);		//4,0 ms overflow
   setup_timer_2(T2_DIV_BY_16,255,10);		//2,0 ms overflow, 20,4 ms interrupt
}   