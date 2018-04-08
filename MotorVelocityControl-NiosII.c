
//Includes
#include "altera_avalon_timer_regs.h"
#include "altera_avalon_pio_regs.h"
#include <io.h>
#include "altera_avalon_pwm_regs.h"
#include "altera_avalon_pwm_routines.h"
#include "system.h"
#include <stdio.h>
#include "sys/alt_alarm.h"
#include "sys/alt_timestamp.h"
#include "alt_types.h"
#include <unistd.h>
#include <math.h>
#include "sys/alt_irq.h"
#include "chu_avalon_gpio.h"
#include "chu_avalon_vga.h"
#include <stdlib.h>


//Function Protypes
void print_error(unsigned int address, int return_code);
void check_return_code(unsigned int address, int return_code);

int initRPM=0;
char IRPM[4],SRPM[4];
int set_rpm=3100; // MAX 3350
int srpm;
int i=100;
float xpl =40.0;
float ypl, step;
int prx=0,pry=300;

void plot_RPM(alt_u32 vga_base,int rpm,int duty){
  const float XMAX=5.0;     // max range of x-axis
  const float YMAX=5.0;     // max range of y-axis
  int j;
  step = XMAX / (float)(DISP_GRF_X_MAX);
  xpl = xpl + step;
  ypl = 0.001 * rpm;
    if(xpl>600){
     int x=40,y=80;
     vga_clr_screen(VRAM_BASE,0xff);

	   for(x=0; x<40; x++)
	     for(y=0; y<480; y++)
	       vga_wr_pix(VRAM_BASE, x, y, 0x3B);
	   for(x=600; x<640; x++)
	     for(y=0; y<480; y++)
	       vga_wr_pix(VRAM_BASE, x, y, 0x3B);
	   for(x=0; x<640; x++)
	     for(y=0; y<80; y++)
	       vga_wr_pix(VRAM_BASE, x, y, 0x3B);
	   for(x=0; x<640; x++)
	     for(y=400; y<480; y++)
	       vga_wr_pix(VRAM_BASE, x, y, 0x3B);
	   x=40,y=80;


while(y<=400){
	 vga_plot_line(VRAM_BASE,x,y,DISP_GRF_X_MAX-40,y,0x1c);
	 y+=10;
}

	 x=40,y=80;

while(x<=600){
	 vga_plot_line(VRAM_BASE,x,y,x,DISP_GRF_Y_MAX-80,0x1c);
	 x+=10;
}

	 x=40,y=45;

vga_plot_line(VRAM_BASE,x,y,DISP_GRF_X_MAX-40,y,0x44);
vga_plot_line(VRAM_BASE,x,45,x,DISP_GRF_Y_MAX-40,0x44);
vga_plot_line(VRAM_BASE,DISP_GRF_X_MAX-40,45,DISP_GRF_X_MAX-40,DISP_GRF_Y_MAX-40,0x44);
vga_plot_line(VRAM_BASE,x,DISP_GRF_Y_MAX-40,DISP_GRF_X_MAX-40,DISP_GRF_Y_MAX-40,0x44);

	 sprintf(SRPM,"%i",set_rpm);

vga_wr_bit_str(VRAM_BASE,280,55,"SET RPM:",0x44,1);
vga_wr_bit_str(VRAM_BASE,345,55,SRPM,0x03,1);
vga_wr_bit_str(VRAM_BASE,140,410,"Time elapsed:",0x44,1);
vga_wr_bit_str(VRAM_BASE,360,410,"Duty Cycle :",0x44,1);
vga_wr_bit_str(VRAM_BASE,45,410,"Status:",0x22,1);
vga_wr_bit_str(VRAM_BASE,100,55,"Initial RPM:",0x44,1);

	 sprintf(IRPM,"%i",initRPM);

vga_wr_bit_str(VRAM_BASE,200,55,IRPM,0x03,1);
vga_wr_bit_str(VRAM_BASE,20,20,"FPGA Controls Systems",0x00,1);
vga_wr_bit_str(VRAM_BASE,400,55,"RPM:",0x44,1);

    xpl=40.0;
    }

if (ypl < YMAX){    // plot if only y is in range
    j = DISP_GRF_Y_MAX-(ypl/YMAX)*DISP_GRF_Y_MAX;
    vga_wr_pix(vga_base, xpl++, j, 0x00);
    }
}


int numRevsMotor=0;
volatile int edge_capture;

void write_pixel(int x, int y, short colour) {
  volatile short *vga_addr=(volatile short*)(0x08000000 + (y<<10) + (x<<1));
*vga_addr=colour;
}

alt_u8 sseg_32conv_hex(int hex)
{
  /* active-low hex digit 7-seg patterns (0-9,a-f); MSB ignored */
  static const alt_u8 SSEG_HEX_TABLE[16] = {
    0x40, 0x79, 0x24, 0x30, 0x19, 0x92, 0x02, 0x78, 0x00, 0x10, //0-9
    0x88, 0x03, 0x46, 0x21, 0x06, 0x0E};                        //a-f
  alt_u8 ptn;

  if (hex < 16)
    ptn = SSEG_HEX_TABLE[hex];
  else
    ptn = 0xff;
  return (ptn);
}

int main(void){
	char *strduty;
	vga_clr_screen(VRAM_BASE,0xff);
	volatile int duty_cycle;
	int return_code = ALTERA_AVALON_PWM_OK;

	 //Initialize PWM and Check Return Code
	 return_code = altera_avalon_pwm_init(Z_PWM_0_BASE, 500000, 1);
	 check_return_code(Z_PWM_0_BASE, return_code);

	 //Enable PWM and Check Return Code
	 return_code = altera_avalon_pwm_enable(Z_PWM_0_BASE);
	 check_return_code(Z_PWM_0_BASE, return_code);

	//init duty_cycle with the value written to duty_cycle register during initialization
	 duty_cycle = IORD_ALTERA_AVALON_PWM_DUTY_CYCLE(Z_PWM_0_BASE);

	 altera_avalon_pwm_disable(Z_PWM_1_BASE);

	 IOWR_ALTERA_AVALON_PIO_DATA(HBRIDGE_ENABLE_BASE,1);

	 duty_cycle = 10; // ( 1 - 100000 )
	 return_code = altera_avalon_pwm_change_duty_cycle(Z_PWM_0_BASE, duty_cycle);
	 check_return_code(Z_PWM_0_BASE, return_code);

	 int first_val, second_val;
	 int intit_pulse_val,final_pulse_val;
	 int RPM;
	 srpm=set_rpm;

	 float Error_term=0, P_Term=0, I_Term=0, D_Term=0, D_State=0, I_Gain=5, P_Gain=15, D_Gain=2;

	 int x,y;


	 // --------- Σχεδιασμός ορθογωνίων ---------- //

	   for(x=0; x<40; x++)
	     for(y=0; y<480; y++)
	       vga_wr_pix(VRAM_BASE, x, y, 0x3B); //3b
	   for(x=600; x<640; x++)
	     for(y=0; y<480; y++)
	       vga_wr_pix(VRAM_BASE, x, y, 0x3B);
	   for(x=0; x<640; x++)
	     for(y=0; y<80; y++)
	       vga_wr_pix(VRAM_BASE, x, y, 0x3B);
	   for(x=0; x<640; x++)
	     for(y=400; y<480; y++)
	       vga_wr_pix(VRAM_BASE, x, y, 0x3B); //DA ,r 0x03

	 // ----------- Επικεφαλίδες ---------------//

vga_wr_bit_str(VRAM_BASE,20,20,"ZNUC II - CYCLONE II PID MOTOR CONTROL SYSTEM",0x00,1);
vga_wr_bit_str(VRAM_BASE,400,55,"RPM:",0x44,1);


	 // ----------- Σχεδιασμός πλεγματος ------//

	 x=40,y=80;
	 while(y<=400){
	 vga_plot_line(VRAM_BASE,x,y,DISP_GRF_X_MAX-40,y,0x1c);
	 y+=10;
	 }
	 x=40,y=80;
	 while(x<=600){
	 vga_plot_line(VRAM_BASE,x,y,x,DISP_GRF_Y_MAX-80,0x1c);
	 x+=10;
	 }


	 // ---------------  Περίγραμμα πλαισίου ----------------//

	 x=40,y=45;
	 vga_plot_line(VRAM_BASE,x,y,DISP_GRF_X_MAX-40,y,0x44);
	 vga_plot_line(VRAM_BASE,x,45,x,DISP_GRF_Y_MAX-40,0x44);
	 vga_plot_line(VRAM_BASE,DISP_GRF_X_MAX-40,45,DISP_GRF_X_MAX-40,DISP_GRF_Y_MAX-40,0x44);
	 vga_plot_line(VRAM_BASE,x,DISP_GRF_Y_MAX-40,DISP_GRF_X_MAX-40,DISP_GRF_Y_MAX-40,0x44);



	 // ----------------- Επικεφαλιδες ---------------------//

	 vga_wr_bit_str(VRAM_BASE,140,410,"Time elapsed:",0x44,1);
	 vga_wr_bit_str(VRAM_BASE,360,410,"Duty Cycle :",0x44,1);
	 vga_wr_bit_str(VRAM_BASE,45,410,"Status:",0x22,1);
     vga_wr_bit_str(VRAM_BASE,100,55,"Initial RPM:",0x44,1);

	 int previous_duty=0;





	 while(1){
		   second_val = 0;
		   sprintf(SRPM,"%i",set_rpm);
		   vga_wr_bit_str(VRAM_BASE,280,55,"SET RPM:",0x44,1);
		   vga_wr_bit_str(VRAM_BASE,345,55,SRPM,0x03,1);

		   // ------ RMP computation in 0.1 sec delay ---- //

		   first_val  = alt_nticks();
		   intit_pulse_val = IORD_32DIRECT(VELOCITY_BASE,0);
		   while((second_val-first_val)<100){ // 0.1 sec
		     second_val = alt_nticks();
		   }
		   final_pulse_val = IORD_32DIRECT(VELOCITY_BASE,0);
		   numRevsMotor = (final_pulse_val-intit_pulse_val);
		   RPM = abs((numRevsMotor*600)/1440);

		   // ------------------------------------------- //

		   plot_RPM(VRAM_BASE,RPM,RPM);

		   int xt, yt;

		   for(xt=460; xt<510; xt++)
		     for(yt=410; yt<425; yt++)
		       vga_wr_pix(VRAM_BASE, xt, yt, 0x3B);

		   for(xt=440; xt<475; xt++)
		     for(yt=55; yt<75; yt++)
		       vga_wr_pix(VRAM_BASE, xt, yt, 0x3B);

		   for(xt=265; xt<295; xt++)
		     for(yt=410; yt<425; yt++)
		       vga_wr_pix(VRAM_BASE, xt, yt, 0x3B);

		   if(srpm!=set_rpm)
		   {
		   for(xt=345; xt<380; xt++)
		     for(yt=55; yt<70; yt++)
		        vga_wr_pix(VRAM_BASE, xt, yt, 0x3B);
		   set_rpm=srpm;
		   }

		   if(initRPM==0){
				 sprintf(strduty,"%i",RPM);
			          vga_wr_bit_str(VRAM_BASE,200,55,strduty,0x03,1);
			     initRPM=RPM;
		   }

		   sprintf(strduty,"%i",RPM);
		   vga_wr_bit_str(VRAM_BASE,440,55,strduty,0x03,1);

		






               // --------------- PID Computation ----------- //

		   Error_term = (float)set_rpm - RPM;
		   P_Term = P_Gain * Error_term;
		   D_Term = D_Gain * (Error_term - D_State);
		   D_State = Error_term;
		   I_Term = I_Term + I_Gain * Error_term;
		   duty_cycle = duty_cycle - (P_Term + I_Term + D_Term);

		   // ------------------------------------------ //


return_code = altera_avalon_pwm_change_duty_cycle(Z_PWM_0_BASE,duty_cycle);
if(return_code==-1)  //duty cycle reg must be less than or equal to clock divider
		   {
			duty_cycle = previous_duty;
		      return_code = altera_avalon_pwm_change_duty_cycle(Z_PWM_0_BASE,duty_cycle);
		   }
		   check_return_code(Z_PWM_0_BASE, return_code);


		   sprintf(strduty,"%u32",alt_nticks()/alt_ticks_per_second());
		   vga_wr_bit_str(VRAM_BASE,265,410,strduty,0x03,1);
		   vga_wr_bit_str(VRAM_BASE,300,410,"sec",0x44,1);
		   previous_duty=duty_cycle;

		   if(alt_nticks()/alt_ticks_per_second()==20){
			   srpm=3300;
		   }

		   if(alt_nticks()/alt_ticks_per_second()==40){
			   srpm=2900;
		   }

		   if(alt_nticks()/alt_ticks_per_second()>=60){
			   srpm=3400;
		   }

		   if(alt_nticks()/alt_ticks_per_second()>=80){
			   srpm=3200;
		   }

	}
return 0;
}

