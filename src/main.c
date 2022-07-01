/*

    SYSVIEW16: a star system layout
    Copyright (C) 2022 Robert Eaglestone

    This file is part of SYSVIEW16.

    SYSVIEW16 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SYSVIEW16 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SYSVIEW16.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <cx16.h>

#include "sprite.h"

#define  VERSION                       "0.00"

#define  CHARACTER_MAP_ADDRESS_R38    0x0f800
#define  CHARACTER_MAP_ADDRESS_R39    0x0f000
#define  LOAD_SECONDARY_ADDRESS_R38   2
#define  LOAD_SECONDARY_ADDRESS_R39   3
#define  CHARACTER_MAP_ADDRESS        CHARACTER_MAP_ADDRESS_R39
#define  LOAD_SECONDARY_ADDRESS       LOAD_SECONDARY_ADDRESS_R39


#define  WORLD_SPRITE_ADDRESS(i)      (0x4000 + i * 0x400)
#define  GG_SPRITE_ADDRESS            0x7000
#define  BIGWORLD_SPRITE_ADDRESS      0x8000

#define  WORLD_TYPE_ASTEROID          0
#define  WORLD_TYPE_TERRAN            1
#define  WORLD_TYPE_ICEWORLD          2
#define  WORLD_TYPE_PINK_OCEAN        3
#define  WORLD_TYPE_ROCKY_YELLOW_SEAS 4
#define  WORLD_TYPE_GREEN             5
#define  WORLD_TYPE_PURPLE_CYAN       6
#define  WORLD_TYPE_DUSTY_ORANGE_SEAS 7
#define  WORLD_TYPE_TOXIC_SEAS        8
#define  WORLD_TYPE_OCEAN             9
#define  WORLD_TYPE_THIN_EXOTIC       10
#define  WORLD_TYPE_GG                11
#define  WORLD_TYPE_BIGWORLD          12

#define  WORLD_TYPE_EMPTY_ORBIT       -1
#define  ROCKBALL_WORLDS              2


SpriteDefinition shipicon, worldicon, jamison;
SpriteDefinition world[10];
char worldSpec[25];

char bigWorldShow(int sprNum, int x, int y)
{
   world[sprNum].block = BIGWORLD_SPRITE_ADDRESS;
   world[sprNum].mode  = SPRITE_MODE_8BPP;
   world[sprNum].layer = SPRITE_LAYER_BACKGROUND;
   world[sprNum].dimensions = SPRITE_64_BY_64;
   world[sprNum].x     = SPRITE_X_SCALE(x-32);
   world[sprNum].y     = SPRITE_Y_SCALE(y-16);
   sprite_define(sprNum, &world[sprNum]);
   return 'B';
}

char ggShow(int sprNum, int x, int y)
{
   world[sprNum].block = GG_SPRITE_ADDRESS;
   world[sprNum].mode  = SPRITE_MODE_8BPP;
   world[sprNum].layer = SPRITE_LAYER_BACKGROUND;
   world[sprNum].dimensions = SPRITE_64_BY_64;
   world[sprNum].x     = SPRITE_X_SCALE(x-32);
   world[sprNum].y     = SPRITE_Y_SCALE(y-16);
   sprite_define(sprNum, &world[sprNum]);
   return 'g';
}

char worldShow(int sprNum, int worldType, int x, int y)
{
   if (worldType == WORLD_TYPE_GG)
     return ggShow(sprNum, x, y);

   if (worldType == WORLD_TYPE_BIGWORLD)
     return bigWorldShow(sprNum, x, y);
   
   world[sprNum].block = WORLD_SPRITE_ADDRESS(worldType);
   world[sprNum].mode  = SPRITE_MODE_8BPP;
   world[sprNum].layer = SPRITE_LAYER_BACKGROUND;
   world[sprNum].dimensions = SPRITE_32_BY_32;
   world[sprNum].x     = SPRITE_X_SCALE(x);
   world[sprNum].y     = SPRITE_Y_SCALE(y);
   sprite_define(sprNum, &world[sprNum]);
   return 'w';
}

// char* buildWorldSpec(int belts, int ggs, int worlds)
// {
//    int total = belts + ggs + worlds;

//    memset(worldSpec, 32, 25); // space (empty, right?)
//    worldSpec[ total ] = 0;    // edge of system
   
//    return worldSpec;
// }

void worldsShow(char* spec)
{
   int i;
   int worldType;
   int x = 70;
   for(i=0; i<strlen(spec); ++i)
   {
      switch(spec[i])
      {         
         case '?': worldType = rand() % 13;                    break;
         case 'a': worldType = WORLD_TYPE_ASTEROID;            break;
         case 'b': worldType = WORLD_TYPE_BIGWORLD;            break; // BigWorld
         case 'g': worldType = WORLD_TYPE_GG;                  break;
         case 'h': worldType = WORLD_TYPE_TERRAN;              break; // Hospitable
         case 'i': worldType = WORLD_TYPE_DUSTY_ORANGE_SEAS;   break; // Inferno World
         case 'r': worldType = WORLD_TYPE_GREEN;   break; // RadWorld
         case 't': worldType = WORLD_TYPE_ICEWORLD;            break;
         case 'w': worldType = rand() % 9 + ROCKBALL_WORLDS;   break;
         default: worldType = WORLD_TYPE_EMPTY_ORBIT;          break;
      }
      if (worldType >= WORLD_TYPE_GG) x += 64;
      else x += 32;

      x += 4; // padding

      worldShow( i, worldType, x, 240 );
   }
}

void jamisonShow()
{
   jamison.block =  0x5000;
   jamison.mode  =  SPRITE_MODE_8BPP;
   jamison.layer =  SPRITE_LAYER_1;
   jamison.dimensions = SPRITE_64_BY_64;
   jamison.x     = SPRITE_X_SCALE(440);
   jamison.y     = SPRITE_Y_SCALE(70);

   sprite_define(3, &jamison);
}

void jamisonHide()
{
   jamison.layer = SPRITE_DISABLED;
   sprite_define(3, &jamison);
}

void loadCharacterSet(char* filename)
{
   cbm_k_setnam(filename);
   cbm_k_setlfs(0,8,0);
   cbm_k_load(LOAD_SECONDARY_ADDRESS, CHARACTER_MAP_ADDRESS);
}

//
//   print splash screen
//
void splash()
{
   cbm_k_bsout(CH_BLACK);
   cbm_k_bsout(0x01);  // #define TO_BACKGROUND	0x01
   clrscr();

   // load PET font
   cbm_k_bsout(0x8E); // revert to primary case
   loadCharacterSet("petfont.bin");

   //
   //  Print the Title Banner
   //
//   loadFileToBank("bt-title.bin",    MISC_BANK, 0xa100); // title splash
   textcolor(COLOR_CYAN);
   gotoxy(15,4);
   cputs("systemview/16");
//   setBank(MISC_BANK); 
//   for(i=0; i<14; ++i)
//      cputsxy(15, i+4, ((char*) 0xa100 + i*52));
   cputsxy(49,16,VERSION);

   vera_sprites_enable(1); // cx16.h

   // show Jamison
   //sprite_loadToVERA("bi-jamison.bin", 0x5000); 
//   jamisonShow();

   cputsxy(20,40, "press a key to begin");
   cgetc();
   clrscr();
   _randomize();
}

void run()
{
      // show worlds
   sprite_loadToVERA("bi-rocks32.bin",     0x4000);
   sprite_loadToVERA("bi-worlds32.bin", 0x4400);
   sprite_loadToVERA("bi-gg64.bin",     0x7000);
   sprite_loadToVERA("bi-bigw64.bin",   0x8000);

   for(;;)
   {
      worldsShow("wwwwg");
      cgetc();
   }
}

void main() 
{
   splash();
   run();
}

