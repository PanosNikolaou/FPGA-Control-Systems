DC motor control- Matlab integration
#include <stdio.h>
#include <unistd.h>

#pragma no_custom_fadds /* floating-point add */
#pragma no_custom_fsubs /* floating-point subtract */
#pragma no_custom_fmuls /* floating-point multiply */
#pragma no_custom_fdivs /* floating-point divide */

typedef float real;


int main ( void )
{
	char *buffer;
	real v=0,vi=0,vp=0,vd=0;
	real y=0,yset=0;
	real kp=20,ki=0.5,kd=10,ts=0.001;
	real error_old=0;
	real error;


while(1){

	scanf("%s",buffer);
	yset = atof(buffer);
	scanf("%s",buffer);
	y = atof(buffer);

	error = (yset-y);
	vp = kp*error;
	vi = vi+ts*ki*error;
	vd = (kd/ts)*(error-error_old);
	v = vp + vi + vd;
	error_old = error;

	sprintf(buffer,"%f",v);
	printf("%s\r\n",buffer);

}

return 0;
}
