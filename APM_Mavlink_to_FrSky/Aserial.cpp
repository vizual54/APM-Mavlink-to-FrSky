/* ============================================================
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================*/

// Based loosely on:
// Atmel AVR304: Half Duplex Interrupt Driven Software UART

// Author: Mike Blandford

//#include "oxs_config.h"
#include <Arduino.h>
#include "Aserial.h"
/*
#define FORCE_INDIRECT(ptr) __asm__ __volatile__ ("" : "=e" (ptr) : "0" (ptr))

struct t_sportData *FirstData = 0 ;
struct t_sportData *ThisData = 0 ;

#define DEBUG 1

#define BR_57600     //!< Desired baudrate

//This section chooses the correct timer values for the chosen baudrate.
// Assumes a 16MHz clock
#define TICKS2COUNT         278  // Ticks between two bits.
#define TICKS2WAITONE       278  // Wait one bit period.
#define TICKS2WAITONE_HALF  416	 // Wait one and a half bit period.

#define INTERRUPT_EXEC_CYCL   90       // Cycles to execute interrupt routines from interrupt.
#define INTERRUPT_EARLY_BIAS  32       // Cycles to allow of other interrupts.
// INTERRUPT_EARLY_BIAS is to bias the sample point a bit early in case
// the Timer 0 interrupt (5.5uS) delays the start bit detection

//Some IO, timer and interrupt specific defines.

#define ENABLE_TIMER_INTERRUPT( )       ( TIMSK1 |= ( 1<< OCIE1A ) )
#define DISABLE_TIMER_INTERRUPT( )      ( TIMSK1 &= ~( 1<< OCIE1A ) )
#define CLEAR_TIMER_INTERRUPT( )        ( TIFR1 = (1 << OCF1A) )
  
#define TX_PIN           4                //!< Transmit data pin
#define RX_PIN           4                //!< Receive data pin
#define TCCR             TCCR1A             //!< Timer/Counter Control Register
#define TCCR_P           TCCR1B             //!< Timer/Counter Control (Prescaler) Register
#define OCR              OCR1A              //!< Output Compare Register
#define EXT_IFR          EIFR               //!< External Interrupt Flag Register
#define EXT_ICR          EICRA              //!< External Interrupt Control Register
//  #define TIMER_COMP_VECT  TIMER1_COMPA_vect  //!< Timer Compare Interrupt Vector


#define TRXDDR  DDRD
#define TRXPORT PORTD
#define TRXPIN  PIND

#define SET_TX_PIN( )    ( TRXPORT |= ( 1 << TX_PIN ) )
#define CLEAR_TX_PIN( )  ( TRXPORT &= ~( 1 << TX_PIN ) )
#define GET_RX_PIN( )    ( TRXPIN & ( 1 << RX_PIN ) )


 #ifndef SENSOR_ID
  #define SENSOR_ID		0x1B
 #endif

// UART's state.
#define   IDLE               0        // Idle state, both transmit and receive possible.
#define   TRANSMIT           1        // Transmitting byte.
#define   TRANSMIT_STOP_BIT  2        // Transmitting stop bit.
#define   RECEIVE            3        // Receiving byte.
#define		TxPENDING          4
#define		WAITING            5

static volatile uint8_t state ;     //!< Holds the state of the UART.
static volatile unsigned char SwUartTXData;     //!< Data to be transmitted.
static volatile unsigned char SwUartTXBitCount; //!< TX bit counter.
static volatile uint8_t SwUartRXData;     //!< Storage for received bits.
static volatile unsigned char SwUartRXBitCount; //!< RX bit counter.

uint8_t LastRx ;
uint8_t TxCount ;
uint8_t TxData[7] ;
uint16_t Crc ;
uint8_t DataSent ;

void setNewData( struct t_sportData *pdata, uint16_t id, uint32_t value )
{
	pdata->dataLock = 1 ;
	pdata->data[0] = 0x10 ;
	pdata->data[1] = id ;
	pdata->data[2] = id >> 8 ;
	pdata->data[3] = value ;
	pdata->data[4] = value >> 8 ;
	pdata->data[5] = value >> 16 ;
	pdata->data[6] = value >> 24 ;
	pdata->dataLock = 0 ;
}
*/
 /*! \brief  External interrupt service routine.
 *
 *  The falling edge in the beginning of the start
 *  bit will trig this interrupt. The state will
 *  be changed to RECEIVE, and the timer interrupt
 *  will be set to trig one and a half bit period
 *  from the falling edge. At that instant the
 *  code should sample the first data bit.
 *
 *  \note  initSoftwareUart( void ) must be called in advance.
 */
// This is the pin change interrupt for port D
// This assumes it is the only pin change interrupt
// on this port
/*
ISR(PCINT2_vect)
{
	if ( TRXPIN & ( 1 << RX_PIN ) )			// Pin is high = start bit (inverted)
	{
		PCICR &= ~(1<<PCIE2) ;						// pin change interrupt disabled
		state = RECEIVE ;                 // Change state
  	
  	DISABLE_TIMER_INTERRUPT() ;       // Disable timer to change its registers.
  	
  	OCR1A = TCNT1 + TICKS2WAITONE_HALF - INTERRUPT_EXEC_CYCL - INTERRUPT_EARLY_BIAS ; // Count one and a half period into the future.

#if DEBUG
	PORTC |= 1 ;
#endif

  	SwUartRXBitCount = 0 ;            // Clear received bit counter.
  	CLEAR_TIMER_INTERRUPT() ;         // Clear interrupt bits
  	ENABLE_TIMER_INTERRUPT() ;        // Enable timer1 interrupt on again
	}
}
*/
/*! \brief  Timer0 interrupt service routine.
 *
 *  Timer0 will ensure that bits are written and
 *  read at the correct instants in time.
 *  The state variable will ensure context
 *  switching between transmit and recieve.
 *  If state should be something else, the
 *  variable is set to IDLE. IDLE is regarded
 *  as a safe state/mode.
 *
 */
/*
ISR(TIMER1_COMPA_vect)
{
  switch (state)
	{
  // Transmit Byte.
	  case TRANSMIT :		// Output the TX buffer.
#if DEBUG
	PORTC |= 1 ;
#endif
    	if( SwUartTXBitCount < 8 )
			{
    	  if( SwUartTXData & 0x01 )
				{           // If the LSB of the TX buffer is 1:
    	    CLEAR_TX_PIN() ;                    // Send a logic 1 on the TX_PIN.
    	  }
    	  else
				{                                // Otherwise:
    	    SET_TX_PIN() ;                      // Send a logic 0 on the TX_PIN.
    	  }
    	  SwUartTXData = SwUartTXData >> 1 ;    // Bitshift the TX buffer and
    	  SwUartTXBitCount += 1 ;               // increment TX bit counter.
    	}
    	else		//Send stop bit.
			{
    	  CLEAR_TX_PIN();                         // Output a logic 1.
    	  state = TRANSMIT_STOP_BIT;
    	}
	  	OCR1A += TICKS2WAITONE ;  // Count one period into the future.
#if DEBUG
	PORTC &= ~1 ;
#endif
  	break ;

  // Go to idle after stop bit was sent.
  case TRANSMIT_STOP_BIT:
		if ( ++TxCount < 8 )		// Have we sent 8 bytes?
		{
			if ( TxCount < 7 )		// Data (or crc)?
			{
				SwUartTXData = TxData[TxCount] ;
			  Crc += SwUartTXData ; //0-1FF
			  Crc += Crc >> 8 ; //0-100
			  Crc &= 0x00ff ;
			}
			else
			{
				SwUartTXData = 0xFF-Crc ;
			}
      SET_TX_PIN() ;                // Send a logic 0 on the TX_PIN.
	  	OCR1A += TICKS2WAITONE ;      // Count one period into the future.
		  SwUartTXBitCount = 0 ;
		  state = TRANSMIT ;
		}
		else
		{
			state = WAITING ;
			OCR1A += ((uint16_t)3500*16) ;		// 3.5mS gap before listening
			TRXDDR &= ~( 1 << RX_PIN );   // PIN is input, tri-stated.
		  TRXPORT &= ~( 1 << RX_PIN );  // PIN is input, tri-stated.
			PCIFR = (1<<PCIF2) ;					// clear pending interrupt
			PCICR |= (1<<PCIE2) ;					// pin change interrupt enabled

			struct t_sportData *pdata = ThisData ;
			FORCE_INDIRECT( pdata ) ;

			pdata->serialSent = 1 ;
			DataSent = 1 ;
			pdata = pdata->next ;
			if ( pdata == 0 )		// Wrap at end
			{
				pdata = FirstData ;
			}
			ThisData = pdata ;
		}

  break ;

  //Receive Byte.
  case RECEIVE :
    OCR1A += TICKS2WAITONE ;                // Count one period after the falling edge is trigged.
    //Receiving, LSB first.
		{
			uint8_t data ;				// Use a temporary local storage
		 	data = SwUartRXBitCount ;
    	if( data < 8 )
			{
    	  SwUartRXBitCount = data + 1 ;
				data = SwUartRXData ;
				data >>= 1 ;		         // Shift due to receiving LSB first.
#if DEBUG
	PORTC &= ~1 ;
#endif
    	  if( GET_RX_PIN( ) == 0 )
				{
    	    data |= 0x80 ;          // If a logical 1 is read, let the data mirror this.
    	  }
#if DEBUG
	PORTC |= 1 ;
#endif
				SwUartRXData = data ;
    	}
    	else	//Done receiving
			{
#if DEBUG
	PORTC &= ~1 ;
#endif
				if ( LastRx == 0x7E )
				{
					if ( SwUartRXData == SENSOR_ID )
					{
						// This is us
						struct t_sportData *pdata = ThisData ;
						FORCE_INDIRECT( pdata ) ;
						if ( pdata )	// We have something to send
						{
							if ( pdata->dataLock == 0 )
							{
 								TxData[0] = pdata->data[0] ;
 								TxData[1] = pdata->data[1] ;
 								TxData[2] = pdata->data[2] ;
 								TxData[3] = pdata->data[3] ;
 								TxData[4] = pdata->data[4] ;
 								TxData[5] = pdata->data[5] ;
 								TxData[6] = pdata->data[6] ;
							}
							else
							{	// Discard frame to be sent
								TxData[0] = 0 ;
								TxData[1] = 0 ;
								TxData[2] = 0 ;
								TxData[3] = 0 ;
								TxData[4] = 0 ;
								TxData[5] = 0 ;
								TxData[6] = 0 ;
							}
							state = TxPENDING ;
							OCR1A += (400*16-TICKS2WAITONE) ;		// 400 uS gap before sending
						}
						else
						{
							// Wait for idle time
							LastRx = 0 ;
							state = WAITING ;
							OCR1A += ((uint16_t)3500*16) ;		// 3.5mS gap before listening
						}
					}
				}
				else
				{
		    	DISABLE_TIMER_INTERRUPT() ;		// Stop the timer interrupts.
	  		  state = IDLE ;                // Go back to idle.
					PCIFR = (1<<PCIF2) ;					// clear pending interrupt
					PCICR |= (1<<PCIE2) ;					// pin change interrupt enabled
				}
				LastRx = SwUartRXData ;
    	}
		}
  break ;
  
		case TxPENDING :
#if DEBUG
	PORTC |= 1 ;
#endif
			TRXDDR |= ( 1 << RX_PIN ) ;       // PIN is output
      SET_TX_PIN() ;                    // Send a logic 0 on the TX_PIN.
	  	OCR1A = TCNT1 + TICKS2WAITONE ;   // Count one period into the future.
		  SwUartTXBitCount = 0 ;
			Crc = SwUartTXData = TxData[0] ;
			TxCount = 0 ;
		  state = TRANSMIT ;
#if DEBUG
	PORTC &= ~1 ;
#endif
  	break ;

		case WAITING :
    	DISABLE_TIMER_INTERRUPT() ;		// Stop the timer interrupts.
	    state = IDLE ;                // Go back to idle.
			PCIFR = (1<<PCIF2) ;					// clear pending interrupt
			PCICR |= (1<<PCIE2) ;					// pin change interrupt enabled
		break ;

  // Unknown state.
	  default:        
  	  state = IDLE;                           // Error, should not occur. Going to a safe state.
  }
}
*/

/*! \brief  Function to initialize the UART.
 *
 *  This function will set up pins to transmit and
 *  receive on. Control of Timer0 and External interrupt 0.
 *
 *  \param  void
 *
 *  \retval void
 */
/*
void initSportUart( struct t_sportData *pdata )
{
	FirstData = ThisData = pdata ;
  //PORT
	TRXDDR &= ~( 1 << RX_PIN ) ;       // PIN is input, tri-stated.
  TRXPORT &= ~( 1 << RX_PIN ) ;      // PIN is input, tri-stated.

  // Timer1
	TIMSK1 &= ~( 1<< OCIE1A ) ;
  TCCR1A = 0x00 ;    //Init.
  TCCR1B = 0xC1 ;    // I/p noise cancel, rising edge, Clock/1

  //External interrupt

	PCMSK2 |= 0x10 ;			// IO4 (PD4) on Arduini mini
	PCIFR = (1<<PCIF2) ;	// clear pending interrupt
	PCICR |= (1<<PCIE2) ;	// pin change interrupt enabled

  //Internal State Variable
  state = IDLE ;

#if DEBUG
	DDRC = 0x03 ;		// PC0,1 as o/p debug
	PORTC = 0 ;
#endif
}
*/