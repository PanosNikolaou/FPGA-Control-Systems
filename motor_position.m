function dcmotor

clear all;

ser = serial('COM1');   
set(ser, 'Terminator','CR/LF'); 
set(ser, 'BaudRate', 115200);
set(ser, 'DataBits', 8);
set(ser, 'Parity', 'none');
set(ser, 'Timeout', 10);
set(ser, 'StopBits', 1);

fopen(ser); % open the serial port connection

ser.ReadAsyncMode = 'continuous';
readasync(ser);

fprintf('DC motor FPGA controller is on-line.\n');

x1=0;x2=0;x3=0;d=0;
v=0;vi=0;

final=5000;
xset=[(pi/3)*ones(1,final) -(pi/3)*ones(1,final)];
k=1:100:final;
j=0;
x3plot=zeros(1,final);
xsetplot=zeros(1,final);

for i=1:length(xset)
  	j=j+1;  
  	x1=0.9668*x1-0.0131*x2+0.0098*v-d; 
  	x2=0.0010*x1+x2;
  	x3=0.0010*x2+x3;
  	x3plot(i)=x3;
  	xsetplot(i)=xset(i);
  	fprintf(ser,num2str(xset(i)));
  	fprintf(ser,num2str(x3));
  	v=str2double(fscanf(ser,'%s'));
end

figure(1);plot(1:i,xsetplot,'r',1:i,x3plot,'b');

fclose(ser);
clear ser;

