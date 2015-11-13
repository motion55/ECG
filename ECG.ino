
#define TIMER_FREQ  500
#define TIMER_CLK (F_CPU/64)
#define TIMER_VAL ((TIMER_CLK/TIMER_FREQ)-1)

#define ledPin    13
#define analogPin A0

int ledState = LOW;

uint8_t Head, Tail;

volatile int AnalogValue;
volatile boolean AnalogReady; 

ISR(TIMER1_COMPA_vect)
{
  if (bit_is_clear(ADCSRA, ADSC))
  {
    //read ADC conversion results
    uint8_t low  = ADCL;
    uint8_t high = ADCH;
    
    AnalogValue = (high<<8)+low;
    AnalogReady = true;
    
    //start the next conversion
    _SFR_BYTE(ADCSRA) |= _BV(ADSC);
    
    Head++;  
    if (Head==0)
    {
      if (ledState==LOW)
      {
        ledState = HIGH;
        digitalWrite(ledPin,HIGH);
      }
      else
      {
        ledState = LOW;
        digitalWrite(ledPin,LOW);
      }
    }
  }
}

void InitADCTimer()
{
  //Perform dummy read to initialize 
  //the analog multiplexer.  
  AnalogValue = analogRead(analogPin);
  
  uint8_t oldSREG = SREG; //backup status reg
  cli();  //disable interrupts
  
  TCCR1A = 0;
  TCCR1B = (1<<WGM12)+(1<<CS11)+(1<<CS10);  //F_CPU/64 prescale & CTC mode4
  
  OCR1AH = TIMER_VAL/256; //500Hz timer rate.
  OCR1AL = TIMER_VAL%256;
  
  _SFR_BYTE(TIMSK1) |= _BV(OCIE1A); //enable interrupt on compare A match.
  
  SREG = oldSREG; //restore status reg

  AnalogReady = false; 
}

void setup() 
{
  // put your setup code here, to run once:
  pinMode(ledPin,OUTPUT);
  Serial.begin(57600);
  
  InitADCTimer();
}

unsigned long int oldmillis = 0;

void loop() 
{
  // put your main code here, to run repeatedly:
  if (AnalogReady)
  {
    AnalogReady = false;
    Serial.println(AnalogValue);
  }
}
