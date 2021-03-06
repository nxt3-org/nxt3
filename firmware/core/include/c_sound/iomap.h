//
// Programmer      
//
// Date init       14.12.2004
//
// Reviser         $Author:: Dkandlun                                        $
//
// Revision date   $Date:: 14-11-07 12:40                                    $
//
// Filename        $Workfile:: c_sound.iom                                   $
//
// Version         $Revision:: 1                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/c_soun $
//
// Platform        C
//

#ifndef   SOUND_IOMAP
#define   SOUND_IOMAP

#define   pMapSound ((IOMAPSOUND*)(pHeaders[ENTRY_SOUND]->pIOMap))  


/* HOW TO

Start a sound file            strcpy((char*)pMapSound->SoundFilename,"xxxxxxx.rso");
                              pMapSound->Volume   =  IOMapUi.Volume;
                              pMapSound->Mode     =  SOUND_ONCE;
                              pMapSound->Flags   |=  SOUND_UPDATE;


Start and loop a sound file   strcpy((char*)pMapSound->SoundFilename,"xxxxxxx.rso");
                              pMapSound->Volume   =  IOMapUi.Volume;
                              pMapSound->Mode     =  SOUND_LOOP;
                              pMapSound->Flags   |=  SOUND_UPDATE;


Start a tone                  pMapSound->Freq     =  440;
                              pMapSound->Duration =  1000;
                              pMapSound->Volume   =  IOMapUi.Volume;
                              pMapSound->Mode     =  SOUND_TONE;
                              pMapSound->Flags   |=  SOUND_UPDATE;


Start and loop a tone         pMapSound->Freq     =  440;
                              pMapSound->Duration =  1000;
                              pMapSound->Volume   =  IOMapUi.Volume;
                              pMapSound->Mode     =  SOUND_TONE | SOUND_LOOP;
                              pMapSound->Flags   |=  SOUND_UPDATE;



Test for sound finished       if (!(pMapSound->Flags & (SOUND_RUNNING | SOUND_UPDATE)))
                              {
                                // FINISHED
                              }
                              
                              
Abort sound or tone           pMapSound->State    =  SOUND_STOP;


**** Start always abort running sound or tone ****



*/

// Constants related to Flags
enum
{
  SOUND_UPDATE  = 0x01,                       // W  - Make changes take effect
  SOUND_RUNNING = 0x02                        // R  - Processing tone or file
};

// Constants related to State
enum
{
  SOUND_IDLE    = 0x00,                       // R  - Idle, ready for start sound (SOUND_UPDATE)
  SOUND_BUSY    = 0x02,                       // R  - Processing file of sound/melody data
  SOUND_FREQ    = 0x03,                       // R  - Processing play tone request
  SOUND_STOP    = 0x04                        // W  - Stop sound imedately and close hardware
};

// Constants related to Mode
enum
{
  SOUND_ONCE    = 0x00,                       // W  - Only play file once
  SOUND_LOOP    = 0x01,                       // W  - Play file until writing "SOUND_STOP" into "State" or new "update"
  SOUND_TONE    = 0x02                        // W  - Play tone specified in Freq for Duration ms
};

typedef   struct                              
{
  UWORD   Freq;                               // RW - Tone frequency [Hz]
  UWORD   Duration;                           // RW - Tone duration  [mS]
  UWORD   SampleRate;                         // RW - Sound file sample rate [2000..16000]
  UBYTE   SoundFilename[FILENAME_LENGTH + 1]; // RW - Sound/melody filename 
  UBYTE   Flags;                              // RW - Play flag  - descripted above
  UBYTE   State;                              // RW - Play state - descriped above
  UBYTE   Mode;                               // RW - Play mode  - descriped above
  UBYTE   Volume;                             // RW - Sound/melody volume [0..4] 0 = off
}IOMAPSOUND;


#endif // SOUND_IOMAP
