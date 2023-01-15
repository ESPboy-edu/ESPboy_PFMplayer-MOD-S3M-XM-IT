
#define  CLK_FREQ   160*1000000
#define TIMER_TICKER	500

/* Note that 52/1 = 52, 52/16 = 3.25, and 52/256 = 0.203125
 * So, in the case when the prescaler is set to divide by 16,
 * if we are given a clock period in microsecons (call it P)
 * We can calculate the load value for the timer as
 * load = P*325/100
 * This calculation will overflow a 32 bit unsigned when
 *  P*325 = 2^32, i.e. when P = 13,215,284, i.e.
 *  when a period of over 13 seconds is requested.
 */
//#define  CLK_FREQ       (52*1000000)
//#define TIMER_TICKER	325

struct timer {
	volatile unsigned int	load;
	volatile unsigned int	count;
	volatile unsigned int	ctrl;
	volatile unsigned int	intack;
};

#define TIMER_BASE (struct timer *) 0x60000600;
#define TIMER_INUM 9

/* bits in the timer control register */
#define	TC_ENABLE	0x80
#define	TC_AUTO_LOAD	0x40
#define	TC_DIV_1	0x00
#define	TC_DIV_16	0x04
#define	TC_DIV_256	0x08
#define TC_LEVEL	0x01
#define TC_EDGE		0x00

/* It looks like we have 2 bits assigned to a prescaler.
 *  probably 0x0c -- setting both bits to one sill gives
 *  a divide by 256, so my experiments have found no hidden values.
 *
 * note that 80,000,000 / 16 = 5,000,000
 *      also 52,000,000 / 16 = 3,250,000
 */

struct dport {
	volatile unsigned int	_unk1;
	volatile unsigned int	edge;
};

#define DPORT_BASE (struct dport *) 0x3ff00000;

/* --------------------------------- */

/* Initializing this to zero here does not work.
 * My code would have to include some code to clear
 * the bss area.  Setting a non-zero initial value
 * works just fine.  No big surprise here.
 */
//int count = 1;

/*void
timer_isr ( char *msg )
{
	//if ( count < 100 )
	//    ets_printf ( "Tick %d: %s\n", count++, msg );
	Serial.print("lol");
}*/

/* Just something to test passing a pointer to the isr attach routine */
//char *bird = "duck";

/* Call with rate in microseconds */
void
setupTimer ( int rate, int_handler_t handler )
{
	struct timer *tp = TIMER_BASE;
	struct dport *dp = DPORT_BASE;

	tp->ctrl = TC_ENABLE | TC_AUTO_LOAD | TC_DIV_16 | TC_EDGE;
	ets_isr_attach ( TIMER_INUM, (int_handler_t)handler, 0 );

	dp->edge |= 0x02;
	ets_isr_unmask ( 1 << TIMER_INUM );

	tp->load = (rate * TIMER_TICKER) / 100;

#ifdef notdef
	/* Here is the odd logic used in the SDK examples.
	 * IT avoids overflow with a 32 bit unsigned int.
	 * The magic number 858 is based on a 80 Mhz clock.
	 */
	if ( rate <= 858 )
	    tp->load = (rate * (CLK_FREQ>>4)) / 1000000;
	else
	    tp->load = (rate>>2) * ((CLK_FREQ>>4)/250000) + (rate&0x3) * ((CLK_FREQ>>4)/1000000);
#endif
}

/* 1 second In microseconds */
/*
#define TIMER_RATE	1000000

void call_user_start ( void ){
    timer_setup ( TIMER_RATE );
}
*/
