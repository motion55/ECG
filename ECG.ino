
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define TIMER_CLK (F_CPU/64)
#define TIMER_VAL ((TIMER_CLK/500)-1)

#define ledPin    13
#define analogPin A0

int ledState = LOW;

uint8_t Head, Tail;

ISR(TIMER1_COMPA_vect)
{
  if (bit_is_clear(ADCSRA, ADSC))
  {
    uint8_t low  = ADCL;
    uint8_t high = ADCH;
    
    // start the next conversion
    sbi(ADCSRA, ADSC);
  }
  
  Tail++;
//  if (Tail==0)
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

void InitADCTimer()
{
  //Perform dummy read to initialize 
  //the analog multiplexer.  
  analogRead(analogPin);
  
  uint8_t oldSREG = SREG;
  cli();
  
  sbi(TCCR1B, CS10);
  sbi(TCCR1B, CS11);  //F_CPU/64 prescale
  cbi(TCCR1B, CS12);
  
  cbi(TCCR1A, WGM10);
  cbi(TCCR1A, WGM11);
  sbi(TCCR1A, WGM12); //CTC Mode
  
  OCR1AH = TIMER_VAL/256;
  OCR1AL = TIMER_VAL%256;
  
  sbi(TIMSK1, OCIE1A);
  
  SREG = oldSREG;
}

void setup() 
{
  // put your setup code here, to run once:
  pinMode(ledPin,OUTPUT);
  
  InitADCTimer();
}

unsigned long int oldmillis = 0;

void loop() 
{
  // put your main code here, to run repeatedly:
  unsigned long int newmillis = millis();
  if (oldmillis!=newmillis)
  {
    oldmillis = newmillis;
  }
}
