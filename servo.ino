#include <Servo.h> 
Servo myservo;  
Servo myservo2;  
#define DELAY_2CMD 500
#define INIT_IX 36
#define INIT_IY 90
#define SERVO_0 8
#define SERVO_2 9
#define LASER 10
#define MINX 50
#define MINY 100
#define OFFSET

unsigned char cI90[90*2] =
{
63,48,58,49,55,51,46,52,41,53,34,53,29,53,22,53,16,53,6,53,
63,45,58,46,55,48,46,49,41,50,34,50,29,50,22,50,16,50,6,50,
63,42,58,43,55,45,46,46,41,47,34,47,29,47,22,47,16,47,6,47,
63,39,58,40,55,42,46,43,41,44,34,44,29,44,22,44,15,44,5,44,
63,35,58,36,55,37,46,38,41,39,34,39,29,39,22,39,15,39,5,39,
63,31,58,32,55,33,46,34,41,35,34,35,29,35,22,35,15,35,5,35,
63,26,58,27,55,28,46,29,41,29,34,29,28,30,21,30,14,30,4,30,
63,21,58,22,55,23,46,24,41,24,34,24,28,25,21,25,14,25,4,25,
63,15,58,16,55,17,46,18,41,18,34,18,28,19,21,19,14,19,4,19,
};

#ifdef OFFSET
char fI90[90*2] =
{
0,-1,0,0,-2,0,1,0,-1,0,0,1,-2,1,-3,1,-3,1,0,-1,
0,-1,0,0,-2,0,1,0,-1,1,0,1,-2,1,-3,1,-3,1,0,0,
0,-1,0,0,-2,-1,1,0,-1,0,0,1,-2,1,-3,1,-3,1,0,-1,
0,-2,0,0,-2,-1,1,-1,-1,0,0,0,-2,0,-3,0,-3,0,0,-1,
0,-1,0,0,-2,0,1,0,-1,0,0,1,-2,1,-3,1,-3,1,0,-1,
0,-2,0,-1,-2,-1,1,0,-1,0,-1,1,-2,1,-3,1,-3,0,0,0,
0,-1,0,-1,-2,0,1,-1,-1,0,-1,1,-2,0,-3,0,-3,0,1,0,
0,-1,0,-1,-2,-1,1,0,-1,0,-1,1,-2,0,-3,0,-3,0,1,0,
0,0,0,0,-2,0,1,0,-1,0,-1,1,-2,0,-3,0,-3,0,1,0,
};
#endif

int curX = 90, 
    curY = 90;
void servoxy( int p = 90, int p2 = 90, int d = 15 )
{
    myservo.write(p);               
    myservo2.write(p2);               
    delay(d); 
    curX = p;
    curY = p2;                      
}

int curP = 0;
int pump( int on = 1, int del = DELAY_2CMD )
{
  int old = curP;
  curP = on;
  digitalWrite(LASER, on);   // turn the LED on (HIGH is the voltage level)
  if ( del > 0 )
  delay(del);              // wait for a second    
}

void showlaser( void );
void setup() 
{ 
  myservo.attach(SERVO_0);  
  myservo2.attach(SERVO_2);  
  servoxy(); // init to 90&90

  // initialize digital pin 10 as an output.
  pinMode(LASER, OUTPUT);
    
  // Open serial communications and wait for port to open:
  Serial.begin(9600); //115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("ServXY.");

  // show laser.
  showlaser();
}

int s2i( char* s, int* r )
{
  if ( s == NULL ||
       r == NULL ) return 0;
  int l = 0;
  *r=0;
  while ( s[l] >= '0' && s[l] <= '9' )
  {
    *r = (*r)*10 + ( s[l] - '0' );
    l++;
  }
  if ( s[l] == '-' )
  {
    *r = -(*r);
    l++;
  }
  return l;
}

int getxy( char *cmd, int *x, int *y, bool f=0 )
{
  int l, l2;
  if ( cmd == NULL ||
       x == NULL ) return 0;
  l = s2i( cmd, x );
//  Serial.println( l );
//  Serial.println( cmd );
//  Serial.println( *x );
  if ( y == NULL && f == 1 && cmd[l] == 0 )
    return l; 
  if ( l >= 1 && cmd[l] == ',' )
  {
    l2 = s2i( cmd+l+1, y );
//    Serial.println( l2 );
//    Serial.println( cmd+l+1 );
//    Serial.println( *y );
    if ( l2 >= 1 )
    {
      if ( f == 1 || (
         ( *x >= 0 && *x <= 180 &&
           *y >= 0 && *y <= 180 ) ) )
        return l + l2 + 1;
  } }
  return 0;
}

int getp( char *cmd, int *p )
{
  return getxy( cmd, p, NULL, 1 );
}

void cmdP( int x, int y, int dbg = 0 )
{
  if ( dbg ) {
//        Serial.print( "l" );
//        Serial.print( l );
//        Serial.print( ",x" );
        Serial.print( "(" );
        Serial.print( x );
        Serial.print( "," );
//        Serial.print( "y" );
        Serial.print( y );
        Serial.println( ")" ); }
        // do servoxy
        servoxy( x, y );
}

void cmdI( int x )
{
  int y;
        if ( x == 0 )
        { x = 90; y = 90; }
        else
        if ( x <= 90 )
        { 
          x --;
          y = MINY + cI90[x*2+1]
          #ifdef OFFSET
            + fI90[x*2+1];
          #endif
              ;
          x = MINX + cI90[x*2]
          #ifdef OFFSET
            + fI90[x*2];
          #endif
              ;
        }
//        Serial.println( x );
//        Serial.println( y );
        if ( x >0 && x <= 180 &&
             y >0 && y <= 180 )
          cmdP( x, y );
}

void showlaser( void )
{
  pump(); 
  cmdI( 0 );  delay(DELAY_2CMD); 
  cmdI( 1 );  delay(DELAY_2CMD);
  cmdI( 10 );  delay(DELAY_2CMD);
  cmdI( 81 );  delay(DELAY_2CMD);
  cmdI( 90 );  delay(DELAY_2CMD);
  cmdI( 45 );  delay(DELAY_2CMD);
//  pump( 0 );  delay(DELAY_2CMD);
}

void testline( int y )
{
  if ( y>=1 && y<=9 )
  {
    int x;
    for ( x=0; x<=9; x++ )
    {
      pump( 0 ); // switch off.
      servoxy( INIT_IX, INIT_IY, DELAY_2CMD );
      pump(); // switch on.
      cmdI( (9-y)*10 + x + 1 ); delay(DELAY_2CMD*3);
    }
  }
}


void TESTrow( int x )
{
  if ( x>=0 && x<=9 )
  {
    int y;
    for ( y=1; y<=9; y++ )
    {
      pump( 0 ); // switch off.
      servoxy( INIT_IX, INIT_IY, DELAY_2CMD );
      pump(); // switch on.
      cmdI( (9-y)*10 + x + 1 ); delay(DELAY_2CMD*3);
    }
  }
}

void cmdi( int i, int del = 1 )
{
      pump( 0 ); // switch off.
      servoxy( INIT_IX, INIT_IY, DELAY_2CMD );
      pump(); // switch on.
      cmdI( i ); delay(DELAY_2CMD*del);
}

void TESTall( void )
{
  int i;
  for ( i=1; i<=90; i++ )
  {
//      pump( 0 ); // switch off.
//      servoxy( INIT_IX, INIT_IY, DELAY_2CMD );
//      pump(); // switch on.
//      cmdI( i ); delay(DELAY_2CMD*3);
        cmdi( i, 3 );
  }
}

void Laser_Shell( char n, char y, char x, int del = 3 ) 
{
    int c = n - '0';
    int fx = x - '0';
    int fy = y - '0';
    // point number.
    cmdi( (10-c)*10, del );
    // point position.
    cmdi( (8-fy)*10 + (10-fx), del );
    // Metal3_90mov( n - '1', fx, fy );
    // delay( 1000 );    
}

void Go_bat(  int del = 3 );

void command( char* cmd )
{
  int l, x, y;
//  Serial.println( cmd );
  switch( cmd[0] )
  {
    case 'P': // servo angle.
      l = getxy( cmd+1, &x, &y );
      if ( l >= 1+1+1 )
      {
loc_P:/*
//        Serial.print( "l" );
//        Serial.print( l );
//        Serial.print( ",x" );
        Serial.print( "(" );
        Serial.print( x );
        Serial.print( "," );
//        Serial.print( "y" );
        Serial.print( y );
        Serial.println( ")" );
        // do servoxy
        servoxy( x, y );*/
        cmdP( x, y );
      }
      break;
    case 'p': // chess board 90 position.
      l = getxy( cmd+1, &y, &x );
//        Serial.print( "l" );
//        Serial.print( l );
//        Serial.print( ",x" );
//        Serial.print( x );
//        Serial.print( ",y" );
//        Serial.println( y );
      if ( l >= 1+1+1 &&
           y >= 1 && y <= 9 &&
           x >= 0 && x <= 9 )
      {
        pump( 0 ); // switch off.
        servoxy( INIT_IX, INIT_IY, DELAY_2CMD );
        pump(); // switch on.     
        cmdI( 90 - ( (y-1)*10 + x ) );
      }
      break;
    case 'F': // servo angle offset.
      l = getxy( cmd+1, &x, &y, 1 );
      if ( l >= 1+1+1 )
      {
        x += curX;
        y += curY;
        if ( x >=0 && x <= 180 &&
             y >=0 && y <= 180 )
          goto loc_P;
      }
      break;
    case 'L': // servo angle + offset.
      l = getp( cmd+1, &x );
//      Serial.print( "l" );
//      Serial.print( l );
//      Serial.print( ",x" );
//      Serial.print( x );
      if ( l >= 1 )
      {
        x = curX + x;
        y = curY;
        if ( x >=0 && x <= 180 &&
             y >=0 && y <= 180 )
          goto loc_P;
      }
      break;
    case 'R': // servo angle - offset.
      l = getp( cmd+1, &x );
      if ( l >= 1 )
      {
        x = curX - x;
        y = curY;
        if ( x >=0 && x <= 180 &&
             y >=0 && y <= 180 )
          goto loc_P;
      }
      break;
    case 'U': // servo2 angle + offset.
      l = getp( cmd+1, &y );
      if ( l >= 1 )
      {
        x = curX;
        y = curY + y;
        if ( x >=0 && x <= 180 &&
             y >=0 && y <= 180 )
          goto loc_P;
      }
      break;
    case 'D': // servo2 angle - offset.
      l = getp( cmd+1, &y );
      if ( l >= 1 )
      {
        x = curX;
        y = curY - y;
        if ( x >=0 && x <= 180 &&
             y >=0 && y <= 180 )
          goto loc_P;
      }
      break;
    case 'I': // index array 90.
loc_I:
      l = getp( cmd+1, &x );
//      Serial.println( l );
//      Serial.println( x );
      if ( l >= 1 && x <= 90 )
      {/*
        if ( x == 0 )
        { x = 90; y = 90; }
        else
        { 
          x --;
          y = I90[x*2+1];
          x = I90[x*2];
        }
//        Serial.println( x );
//        Serial.println( y );
        if ( x >0 && x <= 180 &&
             y >0 && y <= 180 )
          goto loc_P;*/
          cmdI( x );
      }
      break;
    case 'i':
      pump( 0 ); // switch off.
      servoxy( INIT_IX, INIT_IY, DELAY_2CMD );
      pump(); // switch on.
      goto loc_I;
    case 't':
      l = getp( cmd+1, &x );
//      Serial.println( l );
//      Serial.println( x );
      if ( l == 1 )
        testline( x );
      break;
    case 'T':
      l = getp( cmd+1, &x );
      if ( l == 1 )
        TESTrow( 9-x );
      break;
    case 'f': // chess board 90 position offset.
      TESTall();
      break;
    case 'g': // random sudoku 4x4.
      Go_bat( 2 );
      cmdi( 45 );
      break;
    case 'G': // sudoku 6x6
      break;
  }
}

int iBufferPtr = 0;
char cmdBuffer[80];
void writeBuffer( char c )
{
  cmdBuffer[iBufferPtr] = c;
  iBufferPtr ++;
  cmdBuffer[iBufferPtr] = 0;

  switch( cmdBuffer[iBufferPtr-1] )
  {
    case '\r':
    case '\n':
      cmdBuffer[iBufferPtr-1] = 0;
//      Serial.println( cmdBuffer );
      // compare 'm' string.
      if ( cmdBuffer[0] == 'm' )
        strcpy( cmdBuffer, cmdBuffer + 1 );
      // do command.
      command( cmdBuffer );
      // clear command.
      iBufferPtr = 0; 
      cmdBuffer[0] = 0;
  }     
}

void loop() 
{ 
  char inByte;
  if (Serial.available()) {
    inByte = Serial.read();
    // recording the command.
    writeBuffer( inByte );
  }    
} 

//=======================//
//----- random 4x4 ------//
//-----------------------//

char looparray_bat4[3*4*4+1] =
"\
101102103104\
211212213214\
321322323324\
431432433434\
";

void writearray4x4( char n, int i )
{
  looparray_bat4[3*i] = n;
}

char readarray4x4( int i )
{
  return looparray_bat4[3*i];
}

char Random4( char* a4 )
{
  int len = strlen( a4 );
  return a4[random( len )];  
}

char* dela4( char* a4, char c )
{
  int len = strlen( a4 );
  Serial.print( a4 );
  Serial.print( "," );
  Serial.print( c );
  Serial.print( "," );
  for ( int i=0; i<len; i++ )
  {
    if ( a4[i] == c )
    {
      // found.
      strcpy( a4+i, a4+i+1 );
      break;
    }
  }
  Serial.println( a4 );
  return a4;
}

void Random4x4( void )
{
  char a4[5];
  // 0
  strcpy( a4, "1234" );
  writearray4x4( Random4( a4 ), 0 );
  // 1
  dela4( a4, readarray4x4( 0 ) );
  writearray4x4( Random4( a4 ), 1 );
  // 2
  dela4( a4, readarray4x4( 1 ) );
  writearray4x4( Random4( a4 ), 2 );
  // 3
  dela4( a4, readarray4x4( 2 ) );
  writearray4x4( a4[0], 3 );

  // get line.
  a4[0] = readarray4x4( 2 );
  a4[1] = readarray4x4( 3 );
  a4[2] = 0;
  // 4
  writearray4x4( Random4( a4 ), 4 );
  // 5
  dela4( a4, readarray4x4( 4 ) );
  writearray4x4( a4[0], 5 );
  // get line.
  a4[0] = readarray4x4( 0 );
  a4[1] = readarray4x4( 1 );
  a4[2] = 0;
  // 6
  writearray4x4( Random4( a4 ), 6 );
  // 7
  dela4( a4, readarray4x4( 6 ) );
  writearray4x4( a4[0], 7 );  

  // get line.
  a4[0] = readarray4x4( 1 );
  a4[1] = readarray4x4( 5 );
  a4[2] = 0;
  // 8
  writearray4x4( Random4( a4 ), 8 );
  // 12
  dela4( a4, readarray4x4( 8 ) );
  writearray4x4( a4[0], 12 );  
  // get line.
  a4[0] = readarray4x4( 0 );
  a4[1] = readarray4x4( 4 );
  a4[2] = 0;
  // 9
  writearray4x4( Random4( a4 ), 9 );
  // 13
  dela4( a4, readarray4x4( 9 ) );
  writearray4x4( a4[0], 13 );  

loc_a4_zero10:    
  // get line.
  a4[0] = readarray4x4( 3 );
  a4[1] = readarray4x4( 7 );
  a4[2] = 0;
  // 10
  dela4( a4, readarray4x4( 8 ) );
  dela4( a4, readarray4x4( 9 ) );
  // check zero.
  if ( strlen( a4 ) == 0 )
  { // change 9 & 13
    a4[0] = readarray4x4( 9 );
    writearray4x4( readarray4x4( 13 ), 9 );
    writearray4x4( a4[0], 13 );
    a4[1] = 0;    
  }
  writearray4x4( Random4( a4 ), 10 );
  // 14
  a4[0] = readarray4x4( 3 );
  a4[1] = readarray4x4( 7 );
  a4[2] = 0;
  dela4( a4, readarray4x4( 10 ) );
  writearray4x4( a4[0], 14 );

  // get line.
  a4[0] = readarray4x4( 2 );
  a4[1] = readarray4x4( 6 );
  a4[2] = 0;
  // 11
  dela4( a4, readarray4x4( 8 ) );
  dela4( a4, readarray4x4( 9 ) );  
  dela4( a4, readarray4x4( 10 ) );
  if ( strlen( a4 ) == 0 )
  { // change 9 & 13
    a4[0] = readarray4x4( 9 );
    writearray4x4( readarray4x4( 13 ), 9 );
    writearray4x4( a4[0], 13 );
    a4[1] = 0;    
    goto loc_a4_zero10;
  }
  writearray4x4( Random4( a4 ), 11 );
  // 15
  a4[0] = readarray4x4( 2 );
  a4[1] = readarray4x4( 6 );
  a4[2] = 0;
  dela4( a4, readarray4x4( 11 ) );
  writearray4x4( a4[0], 15 );  
}

void Go_bat( int del )
{
  // test loop failed, try another.
  // strcpy( BLE_inLine, "m3#25 F1 X0 Y9\n" );
  // BLE_loop();
  // Metal_init();
  // delay( 1000 );
  // testing success, rem it. at 2017/3/3
  // Metal_Shell_Bat3( '1','0','2' );
  int i;
  Random4x4();
  for ( i=0; i<4*4; i++ )
  {
    Laser_Shell( looparray_bat4[i*3+0],
                 looparray_bat4[i*3+1],
                 looparray_bat4[i*3+2], del );
  }
  // Metal_init();
  // delay( 1000 );
}
