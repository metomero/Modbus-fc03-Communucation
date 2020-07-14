/*
 * File:   newmain.c
 * Author: Metehan ?lter
 *
 * Created on 2 January 2020 Thursday, 11:48
 */

#include <stdio.h>
#include <stdlib.h>
#include <pic18f45k22.h>
#include <xc.h>

#define _XTAL_FREQ 8000000
#define BAUD_RATE 9600
#pragma config FOSC = HSHP
#pragma config WDTEN = OFF
#define TX_EN PORTCbits.RC5
#define RA0 PORTAbits.RA0

/* Variable Definitions */
int Count = 0, Count2 = 0, timeoutCount = 0, RX_Count = 0, RX_Length = 0, errorCode = 0, TX_Length = 0, TX_Count = 0, tempTX_Index; 

unsigned char address = 3;
char RX_Buffer[8]; // Request Buffer
char TX_Buffer[13]; // Responce Buffer

bit RX_Flag = 0;
bit TX_Flag = 0;

/* Analog Write Function */
void SetAnalogValue(int chs){
    ADCON0bits.CHS = chs;
    GODONE = 1;
    while(GODONE);
}

void SetTheData(char * buf, int data_num, int tx_idx){
    int idx = tx_idx;
   
    switch(data_num){
        case 0:{
            TX_Buffer[idx++] = PORTB;
            TX_Buffer[idx++] = PORTD; 
        break;
        }
        default:{
            SetAnalogValue(data_num - 1);
            TX_Buffer[idx++] = ADRESH;
            TX_Buffer[idx++] = ADRESL;
            break;
        }
    }
}

/* Communication Functions Read & Write Data for UART */

void UART_Write(char data)
{
  while(!TXIF);
  TXREG = data;
}

char UART_Read()
{
  while(!RCIF);
  return RCREG;
}

void SetError(char * buf, int error){
    if(error == 1){
  	    tempTX_Index = 1;
        buf[tempTX_Index++] = 0x83;
        buf[tempTX_Index++] = 0x01;
        buf[tempTX_Index++] = 0xAC;
        buf[tempTX_Index++] = 0x63;
    }
    else if(error == 2){
        tempTX_Index = 1;      
        buf[tempTX_Index++] = 0x83;
        buf[tempTX_Index++] = 0x02;
        buf[tempTX_Index++] = 0xB0;
        buf[tempTX_Index++] = 0x53;
    }
}


/* Main Function */

int main(int argc, char** argv) {

	// ADC Configurations

    ANSELA = 0;
    TRISA = 0x00;
    PORTA = 0x00;

    ANSELB = 0;
    TRISB = 0x00;
    PORTB = 0x00;

    ANSELC = 0;
    TRISC = 0xDF;
    PORTC = 0;

    ANSELD = 0;
    TRISD = 0x00;
    PORTD = 0x00;
    
    ADCON2 = 0xA1;
    ADCON1 = 0x00;
    
    ADCON0 = 0x03;

    BAUDCONbits.BRG16 = 0;
    TXSTAbits.BRGH = 0;
    SPBRG = 12 ;   
    TXSTAbits.TXEN = 1;
    RCSTAbits.CREN = 1;
    TXSTAbits.SYNC = 0;
    RCSTAbits.SPEN = 1;

    RCIE = 1;
    RCIP = 1;

    T0CON = 0xC3; // 4 Milisecond
    TMR0IE = 1;
    PEIE = 1;
    GIE = 1;

    while (1){
         if (RX_Flag) {
              
			RX_Flag = 0;
            TX_Buffer[0] = 0x03;
             
            if(errorCode == 0){
                int i = 0;
                int recDataCount = (int)RX_Buffer[5];
                int rev_data = (int)RX_Buffer[3];
                tempTX_Index = 3;
                TX_Buffer[1] = RX_Buffer[1];
                TX_Buffer[2] = (int)(RX_Buffer[5] << 1);
                
                for(i = 0; i < recDataCount; i++){
                    SetTheData(TX_Buffer, i + rev_data, tempTX_Index);
                    tempTX_Index = tempTX_Index + 2;
                }        
                TX_Buffer[tempTX_Index++] = 0x49;
                TX_Buffer[tempTX_Index++] = 0xAD;
             }
             else if(errorCode == 1)
                 SetError(TX_Buffer, 1);
             else if(errorCode == 2)
                 SetError(TX_Buffer, 2);                
            TX_Length = tempTX_Index;
            TX_Flag = 1;      
         }      
        
    };

    return (EXIT_SUCCESS);
}

// Interrupt Timer0 Configurations

void high_priority interrupt high_priorty() {
    
    if (RC1IF) {
        RX_Buffer[RX_Count] = UART_Read();
        RX_Count++;
        timeoutCount = 0;
    }
    
    if (TMR0IF) {
        TMR0IF = 0;

        if (RCSTAbits.FERR || RCSTAbits.OERR) {
            RCSTAbits.SPEN = 0;
            TXSTAbits.TXEN = 1;
            RCSTAbits.CREN = 1;
            TXSTAbits.SYNC = 0;
            RCSTAbits.SPEN = 1;
        }
        
        if (RX_Count >= 8) {
            if (RX_Buffer[0] == address) {
                
                if(RX_Buffer[1] == 3){
                    RX_Flag = 1;
                    RX_Length = RX_Count;                    
                }else{
                    errorCode = 1;
                    RX_Flag = 1;
                }
            }
            else{ 
                errorCode= 2;
                RX_Flag = 1;
            }
            
            RX_Count = 0;
        }        

        if(TX_Flag && TXSTA1bits.TRMT){
            
            TX_Flag = 0;        
            int i = 0;
            TX_EN= 1;             
            for(i = 0; i < TX_Length; i++){
                UART_Write(TX_Buffer[i]);
            }
            TX_EN= 0;
            TX_Length = 0; 
            errorCode = 0;                   
        }   
    }
}