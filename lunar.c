#include <gb.h>
#include "gfx.h"
#include "levels.h"

#define FLAME1 4
#define FLAME2 6
#define FLAME3 10
#define BLANK  8

unsigned char tile[] = { 1 };
unsigned char star1[]= { 2 };
unsigned char star2[]= { 3 };
unsigned char tilebuf;

long fuel, time, score, level;
unsigned int x, y, c, idx, idx2, key;
unsigned int screen_x, t1, t2;
long screen_y, ty;
int play_x, frame;
long play_y, tmp;
long frac_x, frac_y;
int xspeed, yspeed;
int temp, flag, gameover;
char buf[127];
unsigned char *ad;

void drawship();
void explode();
void do_win();

void main()
{
printf(" ");  /* this cheats and makes the library initialize and */
              /* load a character set, turn on the screen, etc    */

SPRITES_8x16;

set_bkg_data(0, 8, tiles);
set_sprite_data(0, 14, sprites);

fuel=899;
score=0;
level=1;

restart:

fuel+=100;
if (fuel>999) fuel=999;
time=0;
frame=0;

set_sprite_tile(0, 0);          /* ship 1/2 */
set_sprite_tile(1, 2);          /* ship 2/2 */
set_sprite_tile(2, FLAME1);     /* flame 1  */
set_sprite_tile(3, FLAME1);     /* flame 2  */
set_sprite_tile(4, FLAME1);     /* flame 3  */

play_x=80;
play_y=20;

drawship();

move_win(158, 0);
SHOW_SPRITES;
SHOW_WIN;

for (x=0; x<32; x++)
  for (y=0; y<32; y++)
  {
	  set_bkg_tiles(x, y, 1, 1, tile);
	  set_win_tiles(x, y, 1, 1, tile);
  }

for (idx=0; idx<50; idx++)
  set_bkg_tiles(rand()%32, rand()%32, 1, 1, star1);

for (idx=0; idx<20; idx++)
  set_bkg_tiles(rand()%32, rand()%32, 1, 1, star2);

screen_x=0;
screen_y=0;
frac_x=0;
frac_y=0;
xspeed=0;
yspeed=0;

switch ((level-1)%4)
{
case 0: ad=ground0; break;
case 1: ad=ground1; break;
case 2: ad=ground2; break;
case 3: ad=ground3; break;
}
temp=*ad;
set_bkg_tiles(0, 32-temp, 32, temp, ad+1);

while (1)
{

  frame++;
  do_win();

  if (c)
  {
    c=0;
    temp=FLAME1;
  }
  else
  {
    c=1;
    temp=FLAME2;
  }

  flag=0;
  key=joypad();

  if (fuel<0) fuel=0;

  if ((key & J_RIGHT)&&(fuel))
  {
     xspeed+=2;
     set_sprite_tile(2, temp);
     set_sprite_tile(3, BLANK);
     set_sprite_tile(4, BLANK);
     flag=1;
	 fuel--;
  }

  if ((key & J_LEFT)&&(fuel))
  {
     xspeed-=2;
     set_sprite_tile(2, BLANK);
     set_sprite_tile(3, BLANK);
     set_sprite_tile(4, temp);
     flag=1;
	 fuel--;
  }

  if (((key & J_A) || (key & J_B)) && (fuel))
  {
     if (key & J_A)
     {
		yspeed-=2;
		fuel-=2;
	 }
     if (key & J_B)
     {
		yspeed-=2;
		fuel-=2;
	 }

     if ((key & J_A) && (key & J_B) && (temp == FLAME1))
     {
       set_sprite_tile(2, FLAME3);
       set_sprite_tile(3, FLAME3);
       set_sprite_tile(4, FLAME3);
     }
     else
     {
       set_sprite_tile(2, temp);
       set_sprite_tile(3, temp);
       set_sprite_tile(4, temp);
     }

     if (xspeed>0) xspeed--;
     else if (xspeed<0) xspeed++;

     flag=1;
  }
  else
  {
     yspeed+=2;
  }

  if (flag==0)
  {
     set_sprite_tile(2, BLANK);
     set_sprite_tile(3, BLANK);
     set_sprite_tile(4, BLANK);
  }

  if (xspeed > 0x70) xspeed=0x70;
  if (xspeed < -0x70) xspeed=-0x70;

  if (yspeed > 0x63) yspeed=0x63;
  if (yspeed < -0x60) yspeed=-0x60;

  frac_x+=xspeed;
  if (frac_x > 0x10)
  {
    screen_x+=frac_x>>4;
    frac_x&=0xf;
  }
  if (frac_x < -0x10)
  {
    screen_x+=frac_x>>4;
    frac_x&=0xf;
  }

  frac_y+=yspeed;
  if (frac_y > 0x10)
  {
    play_y+=frac_y>>4;
    frac_y&=0xf;
  }
  if (frac_y < -0x10)
  {
    play_y+=frac_y>>4;
    frac_y&=0xf;
  }

  if (play_y<70)
    screen_y=0;
  else
    screen_y=play_y-70;

  if (screen_y > 111)
    screen_y=111;

  if (play_y<16)
  {
    play_y=16;
    yspeed=0;
  }
  if (play_y>247)
  {
    play_y=247;
    yspeed=0;
    xspeed=0;
  }

  /* check ground */
  x=(screen_x+play_x)>>3;
  y=play_y>>3;

  get_bkg_tiles(x, y, 1, 1, &tilebuf);

  if (tilebuf>3)
  {
    do_win();

    if ((tilebuf!=5)||(yspeed>25))
    {
      explode();
      strcpy(buf, "Missed Landing");
      if (tilebuf==5)
        strcpy(buf,"Landed too hard!");
      set_win_tiles(2,0,strlen(buf),1, (unsigned char*)buf);
      strcpy(buf, "Game Over");
      set_win_tiles(2,1,strlen(buf),1, (unsigned char*)buf);
      gameover=1;
    }
    else
    {
      move_bkg(screen_x, screen_y);
      drawship();
      set_sprite_tile(2, BLANK);
      set_sprite_tile(3, BLANK);
      set_sprite_tile(4, BLANK);
      strcpy(buf, "Successful!");
      set_win_tiles(2,0,strlen(buf),1,(unsigned char*)buf);
      strcpy(buf,  "Craft Damaged 0 pts");
      if (yspeed<20)
      {
        strcpy(buf,"HARD landing 5 pts");
        score+=5;
      }
      if (yspeed<10)
      {
        strcpy(buf,"Soft Landing 10 pts");
        score+=5;
      }
      if (yspeed<5)
      {
        strcpy(buf,"!!PERFECT 50 pts!!");
        score+=40;
      }
      set_win_tiles(2,1,strlen(buf),1,(unsigned char*)buf);
      gameover=0;
    }

    play_y=(play_y>>3)<<3;
    xspeed=0;
    yspeed=0;

    if ((gameover==0)&&((level-1)%4==3))
    {
      strcpy(buf,"All Waves! 100pts");
      set_win_tiles(2,2,strlen(buf),1,(unsigned char*)buf);
      score+=100;
    }

    strcpy(buf, "Score:0000");
    tmp=score;
    buf[6]=tmp/1000+48;
    tmp%=1000;
    buf[7]=tmp/100+48;
    tmp%=100;
    buf[8]=tmp/10+48;
    tmp%=10;
    buf[9]=tmp+48;
    set_win_tiles(2,3,strlen(buf),1,(unsigned char*)buf);

    strcpy(buf, "Press Button");
    set_win_tiles(2,4,strlen(buf),1,(unsigned char*)buf);

    while (joypad() & (J_A | J_B));

    for (tmp=158*5; tmp>7*5; tmp--)
    {
      move_win(tmp/5,0);
    }

    move_win(7,0);

    while (joypad() & (J_A | J_B)); 
    while (!(joypad() & (J_A | J_B)));
    if (gameover==1)
    {
      fuel=899;
      score=0;
      level=1;
    }
    else
    {
      level++;
    }
    goto restart;

  }

  move_bkg(screen_x, screen_y);
  drawship();

  delay(50);

}

}

void drawship()
{
int y;

y=play_y-screen_y;
move_sprite(0, play_x, y);
move_sprite(1, play_x+8, y);
move_sprite(2, play_x, y+17);
move_sprite(3, play_x+6, y+16);
move_sprite(4, play_x+12, y+17);
}

void explode()
{

for (idx=0; idx<5; idx++)
{
  set_sprite_tile(idx, BLANK);
}

t1=play_x+8;
t2=(play_y-screen_y)+12;

for (c=0; c<40; c++)
{
  move_sprite(c, t1, t2);
  set_sprite_tile(c, 13);
}

for (idx=1; idx<20; idx++)
{
  for (c=0; c<20; c++)
  {
    x=(rand()%3)-1;
    ty=(rand()%3)-1;

    x=(x*(rand()%idx))+t1;
    ty=(ty*(rand()%idx))+t2;

    move_sprite(c, x, ty);
  }
}

for (idx=0; idx<40; idx++)
  set_sprite_tile(idx, BLANK);

}

void do_win()
{
  strcpy(buf,"Fuel:000 Speed:000");

  tmp=fuel;
  buf[5]=tmp/100;
  tmp=tmp%100;
  buf[6]=tmp/10;
  tmp=tmp%10;
  buf[7]=tmp+48;
  buf[6]+=48;
  buf[5]+=48;
  if ((buf[5]=='0')&&((frame/10)%2))
  {
    buf[0]='*';
    buf[1]='*';
    buf[2]='*';
    buf[3]='*';
  }
  tmp=yspeed;
  if (yspeed<0)
  {
    tmp=-yspeed;
  }
  buf[15]=(yspeed>0 ? '+' : '-');
  buf[16]=tmp/10+48;
  tmp=tmp%10;
  buf[17]=tmp+48;

  set_win_tiles(0,0,1,strlen(buf), (unsigned char*)buf);
}
