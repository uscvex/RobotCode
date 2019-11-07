#define LOW 0
#define HIGH 1
#define OUTPUT sensorDigitalOut

int maxInUse = 2;    //change this variable to set how many MAX7219's you'll use

int e = 0;           // just a variable

// define max7219 registers
unsigned char max7219_reg_noop        = 0x00;
unsigned char max7219_reg_digit0      = 0x01;
unsigned char max7219_reg_digit1      = 0x02;
unsigned char max7219_reg_digit2      = 0x03;
unsigned char max7219_reg_digit3      = 0x04;
unsigned char max7219_reg_digit4      = 0x05;
unsigned char max7219_reg_digit5      = 0x06;
unsigned char max7219_reg_digit6      = 0x07;
unsigned char max7219_reg_digit7      = 0x08;
unsigned char max7219_reg_decodeMode  = 0x09;
unsigned char max7219_reg_intensity   = 0x0a;
unsigned char max7219_reg_scanLimit   = 0x0b;
unsigned char max7219_reg_shutdown    = 0x0c;
unsigned char max7219_reg_displayTest = 0x0f;

void digitalWrite(int s,int val) {
	SensorValue[s]=val;
}

/*void pinMode(int s,TSensorTypes t) {
	SensorType[s]=t;
}*/

void putByte(unsigned char data) {
    SensorValue[clock]=0; SensorValue[dataIn]=data&128; SensorValue[clock]=1;
    SensorValue[clock]=0; SensorValue[dataIn]=data&64; SensorValue[clock]=1;
    SensorValue[clock]=0; SensorValue[dataIn]=data&32; SensorValue[clock]=1;
    SensorValue[clock]=0; SensorValue[dataIn]=data&16; SensorValue[clock]=1;
    SensorValue[clock]=0; SensorValue[dataIn]=data&8; SensorValue[clock]=1;
    SensorValue[clock]=0; SensorValue[dataIn]=data&4; SensorValue[clock]=1;
    SensorValue[clock]=0; SensorValue[dataIn]=data&2; SensorValue[clock]=1;
    SensorValue[clock]=0; SensorValue[dataIn]=data&1; SensorValue[clock]=1;
}

/*void putByte2(unsigned char data) {
  unsigned char i = 8;
  unsigned char mask;
  while(i > 0) {
    mask = 0x01 << (i - 1);      // get bitmask
    SensorValue[clock]=0;
    if (data & mask){            // choose bit
	    SensorValue[dataIn]=1;
    }else{
	    SensorValue[dataIn]=0;
    }
    SensorValue[clock]=1;
    --i;                         // move to lesser bit
  }
}
*/

/*void maxSingle( unsigned char reg, unsigned char col) {
//maxSingle is the "easy"  function to use for a single max7219

  digitalWrite(load, LOW);       // begin
  putByte(reg);                  // specify register
  putByte(col);//((data & 0x01) * 256) + data >> 1); // put data
  digitalWrite(load, LOW);       // and load da stuff
  digitalWrite(load,HIGH);
}*/

void maxDouble( unsigned char reg, unsigned char col, unsigned char reg2, unsigned char col2) {
  SensorValue[load]=LOW;
  putByte(reg);                  // specify register
  putByte(col);//((data & 0x01) * 256) + data >> 1); // put data
  putByte(reg2);                  // specify register
  putByte(col2);//((data & 0x01) * 256) + data >> 1); // put data
//  SensorValue[load]=LOW;
  SensorValue[load]=HIGH;
}

void maxAll (unsigned char reg, unsigned char col) {    // initialize  all  MAX7219's in the system
  int c = 0;
  digitalWrite(load, LOW);  // begin
  for ( c =1; c<= maxInUse; c++) {
  putByte(reg);  // specify register
  putByte(col);//((data & 0x01) * 256) + data >> 1); // put data
    }
//  digitalWrite(load, LOW);
  digitalWrite(load,HIGH);
}

/*void maxOne(unsigned char maxNr, unsigned char reg, unsigned char col) {
//maxOne is for addressing different MAX7219's,
//while having a couple of them cascaded

  int c = 0;
  digitalWrite(load, LOW);  // begin

  for ( c = maxInUse; c > maxNr; c--) {
    putByte(0);    // means no operation
    putByte(0);    // means no operation
  }

  putByte(reg);  // specify register
  putByte(col);//((data & 0x01) * 256) + data >> 1); // put data

  for ( c =maxNr-1; c >= 1; c--) {
    putByte(0);    // means no operation
    putByte(0);    // means no operation
  }

//  digitalWrite(load, LOW); // and load da stuff
  digitalWrite(load,HIGH);
}*/


void matrixSetup () {

//  pinMode(dataIn, OUTPUT);
//  pinMode(clock,  OUTPUT);
//  pinMode(load,   OUTPUT);

//  digitalWrite(13, HIGH);

//initiation of the max 7219
  maxAll(max7219_reg_scanLimit, 0x07);
  maxAll(max7219_reg_decodeMode, 0x00);  // using an led matrix (not digits)
  maxAll(max7219_reg_shutdown, 0x01);    // not in shutdown mode
  maxAll(max7219_reg_displayTest, 0x00); // no display test
   for (e=1; e<=8; e++) {    // empty registers, turn all LEDs off
    maxAll(e,0);
  }
  maxAll(max7219_reg_intensity, 0x0f & 0x0f);    // the first 0x0f is the value you can set
                                                  // range: 0x00 to 0x0f
}

void matrixReset ()
{
  maxDouble(max7219_reg_decodeMode,0,max7219_reg_decodeMode,0);  // using an led matrix (not digits)
  maxDouble(max7219_reg_shutdown,1,max7219_reg_shutdown,1);    // not in shutdown mode
}


/*unsigned char nums[8*16]={
  0B00111110,0B00000000,0B01100010,0B00100010,0B00001111,0B01001111,0B00111110,0B01111111,0B00110110,0B00000110,0B01001001,0B00000000,0B00000000,0B00000000,0B00000000,0B00000000,
  0B01100001,0B00000000,0B01010001,0B01000001,0B00001000,0B01001001,0B01001001,0B00000001,0B01001001,0B01001001,0B00101010,0B00000000,0B00000000,0B00000000,0B00000000,0B00000000,
  0B01010001,0B01000010,0B01001001,0B01000001,0B00001000,0B01001001,0B01001001,0B00000010,0B01001001,0B01001001,0B00010100,0B00000000,0B00000000,0B00000000,0B00000000,0B00000000,
  0B01001001,0B01111111,0B01001001,0B01001001,0B01111111,0B01001001,0B01001001,0B00000100,0B01001001,0B01001001,0B01111111,0B00000000,0B00000000,0B00000000,0B00000000,0B00000000,
  0B01000101,0B01000000,0B01001001,0B01001001,0B00001000,0B01001001,0B01001001,0B00001000,0B01001001,0B01001001,0B00011100,0B00000000,0B00000000,0B00000000,0B00000000,0B00000000,
  0B01000011,0B00000000,0B01001001,0B01001001,0B00001000,0B01001001,0B01001001,0B00001000,0B01001001,0B01001001,0B00101010,0B00000000,0B00000000,0B00000000,0B00000000,0B00000000,
  0B00111110,0B00000000,0B01001110,0B00111110,0B00001000,0B00110010,0B00110000,0B00001000,0B00110110,0B00111110,0B01001001,0B00000000,0B00000000,0B00000000,0B00000000,0B00000000,
  0B00000000,0B00000000,0B00000000,0B00000000,0B00000000,0B00000000,0B00000000,0B00000000,0B00000000,0B00000000,0B00000000,0B00000000,0B00000000,0B00000000,0B00000000,0B00000000,
};*/

unsigned char screen1[8]={0,0,0,0,0,0,0,0};
unsigned char screen2[8]={0,0,0,0,0,0,0,0};
unsigned char oscreen1[8]={0,0,0,0,0,0,0,0};
unsigned char oscreen2[8]={0,0,0,0,0,0,0,0};

unsigned char blank[8]={0,0,0,0,0,0,0,0};

unsigned char eye[8]={
  0B00111100,
  0B01111110,
  0B11111111,
  0B11111111,
  0B11111111,
  0B11111111,
  0B01111110,
  0B00111100
};

unsigned char pupil[8] = {
  0B0110,
  0B1111,
  0B1111,
  0B0110,

  0B0110,
  0B1001,
  0B1001,
  0B0110,
};

unsigned char shades[8]={
  0B00111111,
  0B01000001,
  0B10001101,
  0B10010001,
  0B10000001,
  0B10000001,
  0B01000001,
  0B00111111,
};

unsigned char logoA[8]={
  0B11111011,
  0B11011011,
  0B11011111,
  0B00000000,
  0B11111011,
  0B11011011,
  0B11011111,
  0B00000000,
};

unsigned char logoB[8]={
  0B00011100,
  0B00011010,
  0B11111111,
  0B00000000,
  0B11100111,
  0B00011000,
  0B00011000,
  0B11100111,
};

#define pupilsize 4

int briteL=15;
int briteR=15;
int brite=15;
unsigned char oldBriteL=0;
unsigned char oldBriteR=0;

int pupilx=0;
int pupily=0;

int pupildx=0;
int pupildy=0;

int e_nexttime=0;
int e_command=0;
int e_oldcommand=0;
int e_status=0;

unsigned char *logo1;
unsigned char *logo2;
unsigned char *eyegfx;

int blinking=0;
int mirror=0;
int logo=0;
int e_flashing=0;
int e_flashtype=0;

int e_spin=0;
int e_fun=0;
int e_funtimer=600;
float circle=0;

#define LOOKLEFT 			0
#define LOOKRIGHT 		1
#define LOOKUP 				2
#define LOOKDOWN		 	3
#define LOOKSTRAIGHT 	4
#define DIZZY				 	5
#define LOADING			 	6

#define SPIN_CW			0
#define SPIN_ACW		1
#define SPIN_CRAZY	2
#define LOGO				3
#define FLASH_EYES	4
#define SHADES			5
#define FLIP_EYES		6
#define FUN_END			7

#define NO_BLINK	1
#define NO_FUN		2

/*
  setup
*/
void eyeSetup()
{
//  randomSeed(analogRead(0));
  blinking=8;
  e_nexttime=60;
  maxAll(max7219_reg_intensity, 0x0f & 0x0f);    // the first 0x0f is the value you can set
}

/*
  loop
*/

task eyeLoop()
{
	matrixSetup();
	eyeSetup();
while(true)
{

//draw eye to "screen"
	if (e_spin==1) eyegfx=&blank; else eyegfx=&eye;
  for (int r=0; r<=7; r++)
  {
    screen1[r]=eyegfx[r];
  }
//draw pupil onto eye in right position
  int add=0;
  if (e_spin==1) add=4;
  int a=((pupilx+128)>>8)+(8-pupilsize)/2;
  int b=((pupily+128)>>8)+(8-pupilsize)/2;
  for (int r=0; r<pupilsize; r++)
  {
    int p=pupil[r+add];
    if (a<0) a=0;
    if (a>7) a=7;
    if (b<0) b=0;
    if (b>7) b=7;
    p=p<<b;
    screen1[a]=eyegfx[a]^p;
    a=a+1;
  }

	if (e_spin==1)
	{
  	a=4-(((pupilx+128)>>8)+(8-pupilsize)/2);
  	b=4-(((pupily+128)>>8)+(8-pupilsize)/2);
  	for (int r=0; r<pupilsize; r++)
  	{
    	int p=pupil[r+add];
    	if (a<0) a=0;
    	if (a>7) a=7;
    	if (b<0) b=0;
    	if (b>7) b=7;
    	p=p<<b;
  	  screen1[a]=screen1[a]^p;
    	a=a+1;
  	}
	}

//lerp pupil position
  pupilx-=(pupilx-pupildx)/4;
  pupily-=(pupily-pupildy)/4;

//processes commands
  if (e_oldcommand!=e_command)
  {
  	if (e_oldcommand==DIZZY) {e_spin=0;mirror=0;}
  	e_oldcommand=e_command;
  	e_status=0;
//    e_spin=0;               //clear old command variables
//    logo=0;
    switch (e_command)  //new command
    {
      case LOOKLEFT:			//look left
        pupildx=-2<<8;
        pupildy=0;
        break;
      case LOOKRIGHT:     //look right
        pupildx=2<<8;
        pupildy=0;
        break;
      case LOOKUP:        //look up
        pupildx=0;
        pupildy=-2<<8;
        break;
      case LOOKDOWN:      //look down
        pupildx=0;
        pupildy=2<<8;
        break;
      case LOOKSTRAIGHT:  //look straight
        pupildx=0;
        pupildy=1<<8;
        break;
	    case DIZZY:
  	    e_spin=2;         //spin crazy
    	  mirror=2;
		  	e_status=NO_BLINK+NO_FUN;
      	break;
	    case LOADING:
  	    e_spin=1;         //loading...
		  	e_status=NO_BLINK+NO_FUN;
      	break;
     }
	}

//process fun commands if needed
	if (e_status&NO_FUN)			//supress fun
	{
		if ((mirror!=2)&&(e_spin!=1))		//not DIZZY
		{
			e_spin=0;	//cancel spinning
			mirror=0;	//cancel mirroring
		}
		logo=0;		//cancel logo
		e_flashing=0;	//cancel flashing
		e_flashtype=0;
		e_nexttime=0;
		e_funtimer=10;
	}

	if (e_nexttime>0)
	{
		e_nexttime--;
		if (e_nexttime<=0)
		{
			e_spin=0;
			logo=0;
			e_oldcommand=-1;
		}
	}

	if (e_funtimer>0) e_funtimer--;
	else
	{
		e_funtimer=random(300)+600;
    switch (e_fun)
    {
    case SPIN_CW:
      e_spin=2;         //spin clockwise
      mirror=0;
      e_nexttime=random(60)+60;
      break;
    case SPIN_ACW:
      e_spin=-2;         //spin anticlockwise
      mirror=0;
      e_nexttime=random(60)+60;
      break;
    case SPIN_CRAZY:
      e_spin=2;         //spin crazy
      mirror=1;
      e_nexttime=random(60)+60;
      break;
    case LOGO:           //flash logo
      logo=1;
      logo1=&logoA;
      logo2=&logoB;
      blinking=15;    //blink to reveal logo
      e_nexttime=32*5;  //flash 5 times
      e_flashing=e_nexttime;
      e_flashtype=0;
      break;
    case FLASH_EYES:           //flash alternate eyes whatever they are doing
      e_flashing=32*8;
      e_nexttime=0;
      e_flashtype=1;    //alternate flashing
      break;
    case SHADES:           //wear shades
      logo=1;
      logo1=&shades;
      logo2=&shades;
      blinking=15;    //blink to reveal logo
      e_nexttime=32*8;  //flash 5 times
//          e_flashing=e_nexttime;
//          e_flashtype=0;
      break;
    case FLIP_EYES:
      mirror=1;
      e_nexttime=0;
      break;
    }
    e_fun++;
    if (e_fun>=FUN_END) e_fun=0;
  }

//spin eyeballs
  if (e_spin)
  {
    float t=cos(circle)*4*128;
    pupildx=t;
    t=sin(circle)*4*128;
    pupildy=t;
    circle+=0.1*e_spin;   //spin value dictates dirction
  }

//draw the 224X logo
  if (logo==2)
  {
    for (int r=0; r<=7; r++)
    {
      screen1[r]=logo1[r];    //one half
      screen2[r]=logo2[r];    //to each eye
    }
  }
  else
//if not drawing the logo copy or mirror left eye to right
  {
    if (mirror==0) {for (int r=0; r<=7; r++)  screen2[r]=screen1[r];}
    else {for (int r=0; r<=7; r++)  screen2[r]=screen1[7-r];}
  }

//blink the eyes
  if ((e_status&NO_BLINK)&&(blinking>=16)) blinking=17;

  if (blinking<16)
  {
//    pupildy=1<<8;       //centre pupil vertically

    switch (blinking)
    {
      case 15:
      case 0:
        a=0B01111110;
        break;
      case 14:
      case 1:
        a=0B00111100;
        break;
      case 2:
        if (logo==1) {logo=2;mirror=0;blinking=e_nexttime+4;} //switch on logo if needed during blink. Will blink again to remove it.
      case 13:
        a=0B00011000;
        break;
      case 12:
      	matrixSetup();
     	default:
        a=0B00000000;
        if (e_spin==0) mirror=0;      //cancel mirror on blink
        break;
    }
    for (int r=0; r<=7; r++)        //copy eyelids over eye
    {
      screen1[r]&=a;
      screen2[r]&=a;
    }
  }
  blinking--;
  if (blinking<0) blinking=random(200)+200;   //time before next blink

//flash eyes
  if (e_flashing>0)
  {
    b=(e_flashing+15)&0x1f;
    if (b>15) b=31-b;
    e_flashing--;
    if (e_flashing==0) e_flashtype=0;             //reset flash type
//    if (e_flashing==0) e_flashing=32*8;             //reset flash type
  }
  else b=15;

  briteL-=(briteL-b*32)/4;
  if (e_flashtype==0) briteR-=(briteR-b*32)/4;  //both same brightness
  else briteR-=(briteR-(15*32-b*32))/4;          //each eye alternate brightness

//set eye brightness
  unsigned char bL=0;
  unsigned char bR=0;
  bL=(briteL*brite)/512;
  bR=(briteR*brite)/512;

  if ((bL!=oldBriteL)||(bR!=oldBriteR))
  {
	  maxDouble(max7219_reg_intensity,bL,max7219_reg_intensity,bR);
	  oldBriteL=bL;
	  oldBriteR=bR;
	}

//	matrixSetup();

	  //copy screen to LED matrix
  maxDouble(max7219_reg_scanLimit,0x07,max7219_reg_scanLimit,0x07);
	maxDouble(max7219_reg_decodeMode,0,max7219_reg_decodeMode,0);  // using an led matrix (not digits)
  maxDouble(max7219_reg_shutdown,1,max7219_reg_shutdown,1);    // not in shutdown mode
  maxDouble(max7219_reg_displayTest,0,max7219_reg_displayTest,0);  		//no display test
  for (int r=0; r<=7; r++)
  {
//  	if ((oscreen1[r]!=screen1[r])||(oscreen2[r]!=screen2[r]))
  	{
		  maxDouble(r+1,0,r+1,0);
		  maxDouble(r+1,screen2[r],r+1,screen1[r]);
//		  oscreen1[r]=screen1[r];
//		  oscreen2[r]=screen2[r];
		}
  }
  wait1Msec(20);

//  SensorValue[Yellow]=e_funtimer&1;

}
}
