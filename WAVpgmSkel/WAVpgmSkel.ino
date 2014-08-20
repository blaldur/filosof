// this program/sketch allows one to play specific files on an SD Card.
//
// stripped down LadyAda Wave Shield Code
//  adapted from:  http://www.ladyada.net/media/wavshield/daphc.pde
// Lucky and Dory
//
// we have moved the SD card functions and setup to the end of the program - leave them be please.
//  code you should modify is in the loop function

// don't change any of these 'includes'   //
#include <FatReader.h>          
#include <SdReader.h>         
#include <avr/pgmspace.h>     
#include "WaveUtil.h"          
#include "WaveHC.h"            

// don't change any of the following block of code  //
SdReader card;     // This object holds the information for the card
FatVolume vol;     // This holds the information for the partition on the card
FatReader root;    // This holds the information for the filesystem on the card
FatReader f;       // This holds the information for the file we're play
WaveHC wave;       // This is the only wave (audio) object, since we will only play one at a time

// your variables go here
int sensorValue = 0;
int MappedVal = 0;
int NewVal = 0;
int SensorInPin = 0;  // analog sensor mapped to pin A0

void setup() 
{
    Serial.begin(9600);                 // set up serial port
    Serial.println("  setup  ");    
    putstring_nl("Wave player program");

    putstring("Free RAM: ");       // This can help with debugging, running out of RAM is bad
    Serial.println(freeRam());      // if this is under 150 bytes it may spell trouble!

    // Set the output pins for the DAC control. This pins are defined in the library
    pinMode(2, OUTPUT);                // lady ada uses pins 2,3,4,5, 10,11,12,13  don't change
    pinMode(3, OUTPUT);                // lady ada uses pins 2,3,4,5, 10,11,12,13 don't change
    pinMode(4, OUTPUT);                // lady ada uses pins 2,3,4,5, 10,11,12,13 don't change
    pinMode(5, OUTPUT);                // lady ada uses pins 2,3,4,5, 10,11,12,13 don't change
    cardcheck();   // run function to check SD Card - leave this alone  //
}

void loop() 
{
    Serial.println(" beginning of  loop ");

// the code section below can be used (uncommented) to read an analog input pin and then map to one of the 5 WAV files
//    sensorValue = analogRead(SensorInPin);        // read analog sensor (in)
//    MappedVal = map(sensorValue, 0 , 550, 0, 4);    // map - in our case Dory had 5 wav files, so we need a number from 0-4
//    NewVal = MappedVal;

// the following code creates a random number from 0-4 and then plays the given WAV file
    int rmd = random(0,5);  // to make the program play random sound files
    NewVal = rmd;

// Now use the number 0-4 to select the WAV file to play. 
    Serial.print(" Value used in selecting program to play = ");
    Serial.println(NewVal, DEC);

    // if you don't know what the switch/case statement does - look it up!
    switch (NewVal)         // look up in file:///Applications/Arduino.app/Contents/Resources/Java/reference/SwitchCase.html
    {
    case 0:
        playcomplete("0.wav");    //  here you must fill in your file names (wav files on the SD Card)
        break;
    case 1:
        playcomplete("1.WAV");
        break;
    case 2:
        playcomplete("2.WAV");
        break;
    case 3:
        playcomplete("3.WAV");
        break;
    case 4:
        playcomplete("4.WAV");
    }
    delay(200);    // slows program down (feel free to delete)
}

void playcomplete(char *name)     // Plays a full file from beginning to end with no pause.
{
    Serial.println("  playcomplete  ");
    // call our helper to find and play this name
    playfile(name);
    while (wave.isplaying) 
    {
        // do nothing while its playing
    }
    // now it's done playing
}

void playfile(char *name) 
{
    Serial.println("  playfile  ");
    if (wave.isplaying)     // see if the wave object is currently doing something if already playing something, so stop it!
    {
        wave.stop(); // stop it
    }
    if (!f.open(root, name))      // look in the root directory and open the file
    {
        putstring("Couldn't open file ");
        Serial.print(name);
        return;
    }
    if (!wave.create(f))          // OK read the file and turn it into a wave object
    {
        putstring_nl("Not a valid WAV");
        return;
    }
    wave.play();         // ok time to play! start playback
}

////////// don't change things below this line unless you do it carefully ////////////
////////// don't change things below this line unless you do it carefully ////////////
////////// don't change things below this line unless you do it carefully ////////////
////////// don't change things below this line unless you do it carefully ////////////
////////// don't change things below this line unless you do it carefully ////////////
////////// don't change things below this line unless you do it carefully ////////////

int freeRam(void)    // this handy function will return the number of bytes currently free in RAM, great for debugging!
{
    Serial.println("  freeRAM  ");
    extern int  __bss_end;
    extern int  *__brkval;
    int free_memory;
    if((int)__brkval == 0) 
    {
        free_memory = ((int)&free_memory) - ((int)&__bss_end);
    }
    else 
    {
        free_memory = ((int)&free_memory) - ((int)__brkval);
    }
    return free_memory;
}

void sdErrorCheck(void)
{
    Serial.println("  dErrorCheck ");
    if (!card.errorCode()) return;
    putstring("\n\rSD I/O error: ");
    Serial.print(card.errorCode(), HEX);
    putstring(", ");
    Serial.println(card.errorData(), HEX);
    while(1);
}

void cardcheck()
{
    Serial.println("cardcheck");
    //  if (!card.init(true)) [ //play with 4 MHz spi if 8MHz isn't working for you
    if (!card.init())          //play with 8 MHz spi (default faster!)
    {
        putstring_nl("Card init. failed!");  // Something went wrong, lets print out why
        sdErrorCheck();
        while(1);                            // then 'halt' - do nothing!
    }
    // enable optimize read - some cards may timeout. Disable if you're having problems
    card.partialBlockRead(true);

    // Now we will look for a FAT partition!
    uint8_t part;
    for (part = 0; part < 5; part++)      // we have up to 5 slots to look in
    {
        if (vol.init(card, part))
            break;                             // we found one, lets bail
    }
    if (part == 5)                        // if we ended up not finding one  :(
    {
        putstring_nl("No valid FAT partition!");
        sdErrorCheck();      // Something went wrong, lets print out why
        while(1);                            // then 'halt' - do nothing!
    }
    putstring("Using partition ");        // Let's tell the user about what we found
    Serial.print(part, DEC);
    putstring(", type is FAT");
    Serial.println(vol.fatType(),DEC);     // FAT16 or FAT32?
    if (!root.openRoot(vol))             // Try to open the root directory
    {
        putstring_nl("Can't open root dir!"); // Something went wrong,
        while(1);                             // then 'halt' - do nothing!
    }
    putstring_nl("Ready!");          // Whew! We got past the tough parts.
}

