#include "pmf_player.h"
#include "music.h"
#include "lib/ESPboyInit.h"
#include "lib/ESPboyInit.cpp"

#define SAMPLE_RATE 40000
#define REFRESH_FPS 80
#define MILLIS_FPS 1000/REFRESH_FPS

ESPboyInit myESPboy;
TFT_eSprite sprBuffer = TFT_eSprite(&myESPboy.tft);

static const uint32_t colors[] = {TFT_RED, TFT_ORANGE, TFT_YELLOW, TFT_GREEN, TFT_CYAN, TFT_BLUE, TFT_VIOLET};
static uint8_t hights[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

// globals
static pmf_player s_player;
static uint8_t s_effect_channel=0;
static uint8_t cntChannels;
static uint8_t barWidth;


// row_callback_test
void row_callback_test(void *custom_data_, uint8_t channel_idx_, uint8_t &note_idx_, uint8_t &inst_idx_, uint8_t &volume_, uint8_t &effect_, uint8_t &effect_data_){
  if(channel_idx_==s_effect_channel){
    static unsigned s_counter=1;
    if(--s_counter==0){
      note_idx_=0+5*12; // C-5 (note+octave*12, note: 0=C, 1=C#, 2=D, 3=D#, 4=E, 5=F, 6=F#, 7=G, 8=G#, 9=A, 10=A#, 11=B)
      inst_idx_=2;      // sample 2
      volume_=63;       // volume 63 (max)
      s_counter=8;      // hit note every 8th row
    }
  }
}


void visualisation(){
 uint8_t clr;
   sprBuffer.fillScreen(TFT_BLACK);
   for (uint8_t i=0; i<cntChannels; i++){
     if (s_player.channel_info(i).note_hit) hights[i] = 64;
     if (hights[i] > 0) hights[i]--;    
     sprBuffer.fillRect(barWidth*i, 64-hights[i], barWidth, 64, colors[clr]);
     clr++;
     if (clr >= sizeof(colors)/sizeof(uint32_t)) clr=0;
   }
}


// setup
void setup(){
  myESPboy.begin("MOD/S3M/XM/IT player"); //Init ESPboy
  sprBuffer.createSprite(128, 64);
  
  s_player.load(s_pmf_file);
  
/*
  // Uncomment this code block to demo code-controlled effect. The code adds 13th channel to Aryx and plays drum beat every 8th row on the channel
  s_effect_channel=s_player.num_playback_channels();
  s_player.enable_playback_channels(s_player.num_playback_channels()+1); // add one extra audio channel for audio effects
  s_player.set_row_callback(&row_callback_test); // setup row callback for the effect
*/
  s_player.start(SAMPLE_RATE);

  cntChannels = s_player.num_playback_channels();
  barWidth = 128 / cntChannels;
}



void loop(){
 static uint32_t counter;
  if(millis() > counter + MILLIS_FPS){
    counter = millis();
    visualisation();
    sprBuffer.pushSprite(0, 32);
  }
  s_player.update();
}
