#include "MAX7456.h"

int led=13;
int green=30;
int yellow=31;

MAX7456 *mx = new MAX7456();

#define MAX7456_DATAOUT 51//MOSI, PB2
#define MAX7456_DATAIN  50//MISO, PB3
#define MAX7456_SCK  52//sck, PB1
#define MAX7456SELECT 9//pin 9 (one of the motor pwm, used for octo only)

void SPI_MasterInit() {
//  pinMode(MAX7456SELECT, OUTPUT); digitalWrite(MAX7456SELECT, HIGH); delay(10);
//  pinMode(MAX7456_DATAOUT, OUTPUT);
//  pinMode(MAX7456_DATAIN, INPUT);
//  pinMode(MAX7456_SCK, OUTPUT);
  pinMode(SS,OUTPUT); // to avoid that we can be switched back into Slave mode. 
  // This pin is not used, but may not be INPUT and then LOW. See datasheet pp 197ff
  
  // SPcontrolRegister = 
  // SPIE = SPI interrupt enable
  // SPE  = SPI enable
  // DORD = Data order (1=LSB first)
  // MSTR = microcontroller is Master(1)/Slave(0)
  // CPOL = Clock polarity (1 means, SCK is high when idle). For the MAX7456, SCK is low when idle, so this must be 0.
  // CPHA = clock phase (complex, see p 197 ff in datasheet). For write operations, this must be 0. For read, it must be 1.
  // SPR1, SPR0 - speed divider:
  // SPR1  SPR0  divider  @16Mhz
  //  0      0      4      4 MHz
  //  0      1     16      1 MHz
  //  1      0     64      250 kHz
  //  1      1    256      62.5 kHz
  
  SPCR = (0<<SPIE)|(1<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(0<<SPR1)|(1<<SPR0);
  // SPIF - SPI interrupt flag
  // WCOL - write collision flag
  // SPI2X - double speed SPI flag
  SPSR = (0<<SPIF)|(0<<WCOL)|(0<<SPI2X);
  
}

void setup() {
  Serial.begin(115200);
  Serial.println("overlay code started\n");
  // put your setup code here, to run once:
    // initialize the digital pin as an output.
  SPI_MasterInit();
  pinMode(led, OUTPUT);     
  pinMode(green,OUTPUT); pinMode(yellow,OUTPUT);
  digitalWrite(led,HIGH); delay(200);digitalWrite(led,LOW); 
  digitalWrite(green,HIGH);
  delay(200); 
  digitalWrite(green,LOW);
  digitalWrite(led,HIGH); 
  digitalWrite(yellow,HIGH);
  delay(200); 
  digitalWrite(yellow,LOW);
  digitalWrite(led,LOW);
  Serial.println("Started.");
  //pinMode(MAX7456_DATAOUT,OUTPUT);
  //pinMode(MAX7456_SCK,OUTPUT);
  //pinMode(MAX7456SELECT,OUTPUT);
  mx->begin();
  mx->offset(0,15); // hori=-32...31, vert=-15..16. void MAX7456::offset(int horizontal, int vertical)
  mx->home();
  for (int c=0; c<255;c++) {
    mx->write_0(c & 0xFF);
  }    
  char charA[128];
  mx->read_character(0x02, charA); // first param: which char to read
  for (int i=0; i < 32; i++) {
    Serial.print("char "); Serial.print(i); Serial.print(" is "); Serial.println((int)charA[i]);
  }
}

void loop() {
  byte c;
  Serial.println("Loop!\n");
  // put your main code here, to run repeatedly: 
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  //digitalWrite(MAX7456SELECT,HIGH);
  //digitalWrite(MAX7456_SCK,HIGH);
  //digitalWrite(MAX7456_DATAOUT,HIGH);
  delay(250);               // wait for a second
  
  SPCR = (0<<SPIE)|(1<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(0<<SPR1)|(0<<SPR0);
  delay(1);
  digitalWrite(MAX7456SELECT,LOW); 
  MAX7456_spi_transfer(0x80); // read status register
  SPCR = (0<<SPIE)|(1<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(1<<CPHA)|(0<<SPR1)|(1<<SPR0);
  c = MAX7456_spi_transfer(0x00); // dont care what I send, this is reading
  SPCR = (0<<SPIE)|(1<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(0<<SPR1)|(0<<SPR0);
  digitalWrite(MAX7456SELECT,HIGH);
  if (c==0) { digitalWrite(yellow,HIGH); } else { digitalWrite(yellow,LOW); }
  Serial.print("0x80=");Serial.println(c);
  
  digitalWrite(MAX7456SELECT,LOW); 
  c = MAX7456_spi_read(0xA0);
  digitalWrite(MAX7456SELECT,HIGH);
  Serial.print("stat=");Serial.println(c);
  
  digitalWrite(led,LOW);
  //digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  //digitalWrite(MAX7456SELECT,LOW);
  //digitalWrite(MAX7456_SCK,LOW);
  //digitalWrite(MAX7456_DATAOUT,LOW);

  delay(250);               // wait for a second
  
  //mx->write_to_screen("Hallo",1,1);

}
