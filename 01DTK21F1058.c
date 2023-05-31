#include <xc.h>
#include <pic18f4550.h>
#pragma config FOSC = INTOSCIO_EC
#pragma config FCMEN = OFF
#pragma config BORV = 3
#pragma config WDT = OFF
#pragma config CPB = OFF
#pragma config CPD = OFF
#pragma config IESO = OFF
#pragma config PWRT = OFF
#pragma config MCLRE = ON
#pragma config PBADEN = OFF
#pragma config LVP = OFF
#pragma config XINST = OFF
#define _XTAL_FREQ 20000000L
#define lcd PORTD           //----Assiging PORTD as lcd
#define rs PORTDbits.RD0    //----rs pin for LCD control
#define en PORTDbits.RD1    //----en pin for LCD control


//------Proto-type Decleration------//
void lcd_cmd(unsigned char x);      //----function to send command to lcd
void lcd_cmd_hf(unsigned char x);   //----function to send 4bit command to lcd
void lcd_dwr(unsigned char x);      //----function to send data to lcd
void lcd_msg(unsigned char *c);     //----function to send String of data to lcd
void lcd_init();                    //----lcd initialization
void lcd_lat();                     //----function to latch lcd data
void delay(unsigned int ms);  
void __interrupt() ISR(void);
void safelyDocked(void);
void nearDock(void);
void farDock(void);
void lcd_Clear();


void main()
{
    unsigned int Ch_result;
    TRISAbits.TRISA1=0;     // signal servo motor
    TRISAbits.TRISA4=1;     // latch gate
    TRISC = 0;              // input traffic light
    TRISE = 1;
    TRISAbits.TRISA0=1;     // voltage variable
    TRISBbits.TRISB2=1;     // interrupt button
    TRISBbits.TRISB7=0;     // buzzer
    TRISE = 1;
    ADCON1 = 0b00000000;
    ADCON2 = 0b10010010;
    
    TRISBbits.TRISB0=1;
    TRISBbits.TRISB4=1;
    
    INTCONbits.GIE=1;
    INTCON3bits.INT2IE=1;
    INTCON3bits.INT2IF=0;
    INTCON2bits.INTEDG0=0;
    INTCON3bits.INT2IP = 1;
    T0CON= 0b11101000;
    TMR0L=245;
    int i;
    LATA = 0;
    LATB = 0;
    LATC = 0;
    LATD = 0;
    LATE = 1;

    while(1)
    {
        ADCON0 = 0b00000001;
        
        ADCON0bits.GO = 1;
        
        while(ADCON0bits.GO==1);
        
        Ch_result = ADRES;
        
        if(ADRES>920)   //Safely docked
        {
            safelyDocked();
            rs=0;
            __delay_ms(10);
        }
        if(ADRES>512&&ADRES<920)    //near
        {
            nearDock();
            rs=0;
            __delay_us(10);
        }
        if(ADRES<511)   //far
        {
            farDock();
            rs=0;
            __delay_us(10);
        }   
    }
}

void __interrupt() ISR(void)
{
    if(INTCON3bits.INT2IF==1)
    {
        LATA = 0;
        LATB = 0;
        LATC = 0;
        LATD = 0;
        LATE = 0;
        lcd_cmd(0x01);
        while(1)
        {
        int i;
        LATBbits.LATB7=1;
        LATC=0b10000000;
            lcd_cmd_hf(0x30);       //---Sending Sequence Command
            lcd_cmd_hf(0x20);
            __delay_us(1);
            DDRD = 0x00;            //---PORTD is Output
            lcd_init();             //---LCD Initialization
            lcd_msg("DANGER");      //---Send String
            lcd_cmd(0xC0);          //---Command For Next Line in LCD
            lcd_msg("DANGER");
           
        
          for(i=0;i<50;i++)
          {
            LATAbits.LATA1=1;
            __delay_us(1500);
            LATAbits.LATA1=0;
            __delay_us(18500);
          }
        }
        INTCON3bits.INT2IF=0;
        lcd_cmd(0x01);
        }
    }
   

void safelyDocked(void)
{
    int i;
    LATC=0b00100000;

    do
    {
    __delay_ms(10);
        if(PORTAbits.RA3 == 1)
            {
            for(i=0;i<50;i++)    //buka
        {
            LATAbits.LATA1=1;
            __delay_us(800);
            LATAbits.LATA1=0;
            __delay_us(19200);
        }
            }
    
        if(PORTAbits.RA3 == 1)
           {
            for(i=0;i<50;i++)   //tutup
        {
            LATAbits.LATA1=1;
            __delay_us(1500);
            LATAbits.LATA1=0;
            __delay_us(18500);
        }
           }
    }
    while(i<50);
        
         if(TMR0L==255)
        {
            while(TMR0L==255)
            {
            LATC=0b10000000;
            lcd_cmd_hf(0x30);   //---Sending Sequence Command
            lcd_cmd_hf(0x20);
            __delay_us(1);
            DDRD = 0x00;                //---PORTD is Output
            lcd_init();                 //---LCD Initialization
            lcd_msg("KAPAL KARAM PIER");    //---Send String
            lcd_cmd(0xC0);              //---Command For Next Line in LCD
            lcd_msg("Please Wait");

            for(i=0;i<50;i++) //tutup
        {
            LATAbits.LATA1=1;
            __delay_us(1500);
            LATAbits.LATA1=0;
            __delay_us(18500);
        }
           
        __delay_us(1);
            }
        }
        else
        {
        lcd_cmd_hf(0x30);   //---Sending Sequence Command
        lcd_cmd_hf(0x20);
        __delay_ms(10);
        DDRD = 0x00;                //---PORTD is Output
        lcd_init();                 //---LCD Initialization
        lcd_msg("KAPAL KARAM PIER");    //---Send String
        lcd_cmd(0xC0);              //---Command For Next Line in LCD
        lcd_msg("Passenger Count");
        }
    
    if(INTCONbits.TMR0IF==1)
    {
        INTCONbits.TMR0IF=0;
        TMR0L=245;
    }
}

void nearDock(void)
{
    int i;
            LATC=0b01000000;
            __delay_ms(10);
            rs=0;
            __delay_us(10);
            lcd_cmd_hf(0x30);   //---Sending Sequence Command
            lcd_cmd_hf(0x20);
            __delay_us(10);
            DDRD = 0x00;                //---PORTD is Output
            lcd_init();                 //---LCD Initialization
            lcd_msg("KAPAL KARAM PIER");    //---Send String
            lcd_cmd(0xC0);              //---Command For Next Line in LCD
            lcd_msg("Boat Arriving");
            for(i=0;i<50;i++)
        {
            LATAbits.LATA1=1;
            __delay_us(1500);
            LATAbits.LATA1=0;
            __delay_us(18500);
        }
            
            
        __delay_us(10);
}

void farDock(void)
{
    int i;
    LATC=0b10000000;
            __delay_ms(10);
            rs=0;
            __delay_us(10);
            lcd_cmd_hf(0x30);   //---Sending Sequence Command
            lcd_cmd_hf(0x20);
            __delay_us(10);
            lcd_cmd(0x01);
            DDRD = 0x00;                //---PORTD is Output
            lcd_init();                 //---LCD Initialization
            lcd_msg("KAPAL KARAM PIER");    //---Send String
            lcd_cmd(0xC0);              //---Command For Next Line in LCD
            lcd_msg("No Boat");
            for(i=0;i<50;i++)
        {
            LATAbits.LATA1=1;
            __delay_us(1500);
            LATAbits.LATA1=0;
            __delay_us(18500);
        }
        __delay_us(10);
}




void lcd_lat()
{
    //---Latching function high to low
    en = 1;    //----Enable Pin is high
    delay(1);  //----1ms delay
    en = 0;    //----Enable Pin is Low
}

void lcd_cmd(unsigned char x)
{
    rs = 0;            //----Register Selected is Command register
    lcd &= 0x0F;       //----Masking Higher 4-bit of PORTD
    lcd |= (x & 0xF0); //----Masking Lower 4-bit of Command
    lcd_lat();         //----Latching it to lcd

    lcd &= 0x0F;       //----Masking Higher 4-bit of PORTD
    lcd |= ((x & 0x0F)<<4); //----Masking Higher 4-bit of Command
    lcd_lat();         //----Latching it to lcd
}

void lcd_cmd_hf(unsigned char x)
{
    rs = 0;            //----Register Selected is Command register
    lcd &= 0x0F;       //----Masking Higher 4-bit of PORTD
    lcd |= (x & 0xF0); //----Masking Lower 4-bit of Command
    lcd_lat();         //----Latching it to lcd
}

void lcd_dwr(unsigned char x)
{
    rs = 1;              //----Register Select Pin is set to Data Register
    lcd &= 0x0F;         //----Masking Higher 4-bit of PORTD
    lcd |= ((x & 0xF0)); //----Masking Lower 4-bit of Data
    lcd_lat();           //----Latching it to lcd

    lcd &= 0x0F;         //----Masking Higher 4-bit of PORTD
    lcd |= ((x & 0x0F)<<4); //----Masking Higher 4-bit of Data
    lcd_lat();           //----Latching it to lcd
}

void lcd_msg(unsigned char *c)
{
    while(*c != 0)
    {
        lcd_dwr(*c++);
    }
}

void delay(unsigned int ms)
{
    int i,j;
    for(i=0;i<=ms;i++)
        for(j=0;j<=1;j++);
}

void lcd_init()
{
    lcd_cmd_hf(0x30);   //---Sending Sequence Command
    lcd_cmd_hf(0x20);   //---Sending Sequence Command
    lcd_cmd(0x28);      //---Command to Select 4-bit LCD
    lcd_cmd(0x0E);      //---Cursor Blinking
    lcd_cmd(0x01);      //---Clear LCD Display
    lcd_cmd(0x06);      //---Auto-Increment LCD
    lcd_cmd(0x80);      //---Location address
}