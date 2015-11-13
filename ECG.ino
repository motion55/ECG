
#define TIMER_FREQ  1000
#define TIMER_CLK (F_CPU/64)
#define TIMER_VAL ((TIMER_CLK/TIMER_FREQ)-1)

#define ledPin    13
#define analogPin A0

int ledState = LOW;

uint8_t Head, Tail;

volatile int AnalogValue,AnalogValue2;
volatile boolean AnalogReady,AnalogReady2; 

ISR(TIMER2_COMPA_vect)
{
  if (bit_is_clear(ADCSRA, ADSC))
  {
    //read ADC conversion results
    uint8_t low  = ADCL;
    uint8_t high = ADCH;
    
    if (AnalogReady2)
    {
      AnalogReady2 = false;
      AnalogValue = AnalogValue2 + (high<<8)+low;
      AnalogReady = true;
    }
    else
    {
      AnalogValue2 = (high<<8)+low;
      AnalogReady2 = true;
    }
    
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
  AnalogReady = false; 
  AnalogReady2 = false; 
  
  uint8_t oldSREG = SREG; //backup status reg
  noInterrupts();  //disable interrupts
  
  TCCR2A = (1<<WGM21); //CTC mode2
  TCCR2B = (1<<CS22);  //F_CPU/64 prescale
  
  OCR2A = TIMER_VAL; //1000Hz timer rate.
  
  _SFR_BYTE(TIMSK2) |= _BV(OCIE2A); //enable interrupt on compare A match.
  
  SREG = oldSREG; //restore status reg
}

void setup() 
{
  // put your setup code here, to run once:
  pinMode(ledPin,OUTPUT);
  Serial.begin(57600);
  
  InitADCTimer();
}

void loop() 
{
  // put your main code here, to run repeatedly:
  if (AnalogReady)
  {
    AnalogReady = false;
    uint8_t oldSREG = SREG; //backup status reg
    noInterrupts();  //disable interrupts
    int Value = AnalogValue;
    SREG = oldSREG; //restore status reg
    Serial.println(Value);
  }
}
