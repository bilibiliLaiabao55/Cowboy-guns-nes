#include <stdlib.h>

#include "LIB/neslib.h"
#include "LIB/nesdoug.h"
#include "LIB/zaplib.h"
#include "BG/sands.h"
#include "BG/title.h"
#include "boxes.h"
#include "sprites.c"

#pragma bss-name(push, "ZEROPAGE")
unsigned char pad2_zapper;
unsigned char zapper_ready; 
unsigned char hit_detected;
unsigned char temp1;
unsigned char temp2;
unsigned char state;
unsigned char scroll_x;
unsigned char enemy_x;
unsigned char enemy_y;
unsigned char enemy_type;
unsigned char enemy_frame;
unsigned char health=10;
unsigned char enemy_up_speed=10;
unsigned char timer=0;
unsigned char timer0=0;
unsigned char shoot=5;
unsigned char floating_down=0;
unsigned char score0=0;
unsigned char score1=0;
#pragma bss-name(push, "BSS")

const unsigned char pal[]={
    0x0f,0x27,0x17,0x07,
    0x0f,0x0F,0x37,0x10,
    0x0f,0x30,0x30,0x30,
    0x0f,0x0f,0x0f,0x0f,

    0x0f,0x30,0x0f,0x0f,
    0x0f,0x0f,0x27,0x37,
    0x0f,0x0f,0x0f,0x0f,
    0x0f,0x0f,0x0f,0x0f
};
void draw_title_box(){
    oam_meta_spr(110,58,boxes_list[0]);
}
void draw_enemy_box(){
    oam_meta_spr(enemy_x,enemy_y,boxes_list[1]);
}
void draw_enemy(){
	oam_meta_spr(enemy_x,enemy_y,enemy_list[enemy_frame]);
}
void main (void) {
	
	ppu_off(); // screen off
	
	bank_bg(1);
    pal_all(pal);
	vram_adr(NAMETABLE_A);
	vram_unrle(title);
	ppu_wait_nmi(); // wait
	
	set_vram_buffer(); // points ppu update to vram_buffer, do this at least once
	music_play(1);

	ppu_on_all(); // turn on screen
	while (1){
		// infinite loop
		ppu_wait_nmi(); // wait till beginning of the frame
		if(timer0>0)
		timer0--;
		oam_clear();
		
		zapper_ready = pad2_zapper^1; // XOR last frame, make sure not held down still
		
		// is trigger pulled?
		pad2_zapper = zap_shoot(1); // controller slot 2
		if(state==1){
            scroll_x+=5;
			draw_enemy();
			if(enemy_frame==11){
				enemy_frame=0;
			}else
				enemy_frame++;
			if(enemy_type==0){
				enemy_x++;
				enemy_y=190;
			}
			if(enemy_type==1){
				enemy_x+=3;
				enemy_y=190;
			}
			
			if(enemy_type==2){
				enemy_x++;
				if(floating_down==0){
					enemy_y-=3;
					if(enemy_y<100)
						floating_down=1;
				}else{
					enemy_y+=3;
					if(enemy_y>190)
						floating_down=0;
				}
			}
		}
		if(state==2){
			if(timer==60){
				scroll_x=0;
				ppu_off();
				vram_adr(0x210C);
				vram_put(0x15);
				vram_put(0x08);
				vram_put(0x04);
				vram_put(0x07);
				vram_put(0x1C);
				ppu_on_all();
			}
			timer--;
			if(timer==0){
				ppu_off();
				vram_adr(NAMETABLE_A);
				vram_unrle(sands);
				ppu_on_all();
				enemy_x=0;
				enemy_y=190;
				state=1;
				health=10;
				enemy_type=rand() % 3 + 0;
				if(enemy_type==0){
					pal_col(0x3F14,0x0f);
					pal_col(0x3F15,0x0f);
					pal_col(0x3F16,0x27);
					pal_col(0x3F17,0x37);
				}
				if(enemy_type==1){
					pal_col(0x3F14,0x22);
					pal_col(0x3F15,0x01);
					pal_col(0x3F16,0x21);
					pal_col(0x3F17,0x31);
				}
				if(enemy_type==2){
					floating_down=0;
					pal_col(0x3F14,0x09);
					pal_col(0x3F15,0x19);
					pal_col(0x3F16,0x29);
					pal_col(0x3F17,0x39);
				}
			}
		}
        set_scroll_x(scroll_x);
		
		if((pad2_zapper)&&(zapper_ready)){
			if(shoot==0){
				if(timer0==0){
					shoot=5;
				}
			}else{
				shoot--;
				if(shoot==0){
					timer0=30;
				}
			}
			// trigger pulled, play bang sound
			if((state!=2)&&(shoot>0)){
				sfx_play(0,0);
				pal_col(0x3F00,0x0F);
				pal_col(0x3F10,0x0F);
				// bg off, project white boxes
				oam_clear();
				if(state==0)
					draw_title_box();
				if(state==1){
					draw_enemy_box();
				}
				ppu_mask(0x16); // BG off, won't happen till NEXT frame
				
				ppu_wait_nmi(); // wait till the top of the next frame
				// this frame will display no BG and a white box
				
				oam_clear(); // clear the NEXT frame
				if(state==1){
					draw_enemy();
				}
			}
			ppu_mask(0x1e); // bg on, won't happen till NEXT frame

			if(state==1){
                pal_col(0x3F00,0x22);
                pal_col(0x3F10,0x22);
            }
			hit_detected = zap_read(1); // look for light in zapper, port 2
			
			if((hit_detected)&&(shoot>0)){
				if(state==1){
					health--;
					if((health==0)){
						state=2;
						timer=60;
						score0++;
						if(score0==10){
							score0=0;
							score1++;
						}
						ppu_off();
						vram_adr(0x20A0);
						vram_put(0x16);
						vram_put(0x06);
						vram_put(0x12);
						vram_put(0x15);
						vram_put(0x08);
						vram_put(0x00);
						temp1=score1+0x1E;
						temp2=score0+0x1E;
						vram_put(temp1);
						vram_put(temp2);
						ppu_on_all();
					}
				}
                if(state==0){
					music_play(0);
                    state=2;
					timer=60;
                    ppu_off();
                    vram_adr(NAMETABLE_A);
                    vram_unrle(sands);
                    pal_col(0x3F00,0x22);
                    pal_col(0x3F10,0x22);
                    ppu_on_all();
                }
			}
			// if hit failed, it should have already ran into the next nmi
			}
		}
}
	

