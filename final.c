//Phil Stenger
//Ryan Hooley
#include <stdio.h>
#include "gfx3.h"
#include <math.h>
#include <stdlib.h>
#define PI 3.1415926535897
void drawship(double, double, double, int);
void flyship(void);
void asteroids(double [][5]);
void move_asteroids(double [][5], double [][5], int, int, int, int, double *, double *, double *, int []);
void initialize(double [][5]);
void death(double [][5], double [][5], double *, double *, int []);
void shoot(double [][5], double, double, double);
void move_shots(double [][5]);
void destroy(double [][5], double [][5], int score[]);
void asteroids_level2(double [][5], int);
void asteroids_level3(double [][5], int);
int main()
{
 int height, width;
 height=700;
 width=700;
 srand(time(NULL));
 printf("Instructions\nPress W to fly forward\nPress A or D to turn\nPress O to shoot\nTry to dodge asteroids and destroy them for points!\nLarge asteroids are worth 1 point\nMedium asteroids are worth 2 points\nSmall asteroids are worth 3 points\n"); 
 gfx_open(width, height, "Asteroids");
 flyship();
 return 0;
}

void drawship(double x, double y, double theta, int go)
{
 gfx_flush();
 int i;
 gfx_line(x, y, x+20*cos(theta+4*PI/3), y-20*sin(theta+4*PI/3));  //draws long 2 lines that make up ship
 gfx_line(x, y, x+20*cos(theta+5*PI/3), y-20*sin(theta+5*PI/3));
 gfx_line( x+16*cos(theta+4*PI/3), y-16*sin(theta+4*PI/3), x-16*cos(theta+5*10*PI/3), y-16*sin(theta+5*PI/3));  //draws back of ship
 if(go>0){	//go will be greater than 1 if ship is being propulsed forward
	gfx_color(255, 69, 0);
	for(i=0; i<5; i++){
		gfx_line( x+(17+i)*cos(theta+4*PI/3), y-(17+i)*sin(theta+4*PI/3), x-(17+i)*cos(theta+5*10*PI/3), y-(17+i)*sin(theta+5*PI/3));
	}
 }
 gfx_color(255, 255, 255);
 gfx_flush();
}

void flyship(void)
{
 double x, y;
 double theta;
 char c;
 int i, go, x1ship, x2ship, y1ship, y2ship;	
 int score[2];	//contains int for score and int for lives
 score[0]=0;
 score[1]=3;
 char str[100];
 go=0;		//will tell drawship function if forward button is held down so flames can display
 int iterate=0;	//counts iterations of loop to determine when to move asteroids
 x=350;
 y=350;	       //initial conditions
 theta=0;
 		//the 2d array roids will contain 32 elements, one for each potential asteroid
 		//and each element will have four numbers for the asteroids' properties
 double astr[20][5];	
 double shots[20][5]; //2d array for the shots out of the ship and its properties
 initialize(astr);
 initialize(shots);
 drawship(x, y, theta, go);
 double speed=0;
 while(1){
         if (score[1]==0) {
                //this will run when lives are 0 and display the score
                        gfx_clear();
                        sprintf(str, "Score: %i   Press q to exit game", score[0], score[1]);
                gfx_text(250, 350, str);
                        c=gfx_wait();
                        if(c=='q'){
                                break;
                        }
                }

	if(gfx_event_waiting()) {
	
		c=gfx_wait();
                if (c=='q') {
          	//this will run when players quits
			gfx_clear();
			sprintf(str, "Score: %i Press q to exit game. Press any other key to continue.", score[0], score[1]);
       		gfx_text(250, 350, str);
			c=gfx_wait();				
			if(c=='q'){		
				break;
                	}
		}

		if(c=='d'){
      			theta=theta-PI/20;
			  //this will rotate the ship
		} 
		if(c=='a'){
			theta=theta+PI/20;
		}
		if(c=='w'){
			speed=speed+1;			//moves ship forward
			if (speed>5){
				speed=5;
			}
			go=1;			//will tell drawship function that ship is moving
		
		}
		if(c=='o') {
			shoot(shots, theta, x, y);
                }

		if(x>700){
		        x=1;
		} else if(x<0){
        		x=699;
 		} else if(y>700){      //this keeps the ship from flying off the screen
        		y=1;
		} else if(y<0){
        		y=699;
 		}
	}	
	gfx_clear();
	usleep(10000);
	x1ship= x+20*cos(theta+4*PI/3);		//these variables create a sort of box around the ship, which is later used to check the ship's coordinates for collisions
	y1ship= y;
	x2ship= x+20*cos(theta+5*PI/3);
	y2ship= y-20*sin(theta);
	move_shots(shots);
	destroy(astr, shots, score);	//checks whether an asteroid should be destroyed
	move_asteroids(astr, shots, x1ship, y1ship, x2ship, y2ship, &x, &y, &theta, score);
	x=x+speed*cos(theta+PI/2);	//iterates coordinates of ship based on changes to theta and speed
        y=y-speed*sin(theta+PI/2);
	drawship(x, y, theta, go);
	if(go>=1){
		go=go-1;	//keeps go at 1 so it stops when stop pressing forward button
	}
	if(speed>.025){
                speed=speed-.025;
	
        }
	iterate++; 
 	sprintf(str, "Score: %d Lives: %d", score[0], score[1]);
        gfx_text(20, 680, str);

	}
}

void asteroids(double astr[][5])
{
 int i;
 for(i=0; i<=19; i=i+4){
	if(astr[i][3]==0){
		astr[0+i][0]=rand()%300 || 400+rand()%300;	//x coordinate
		astr[0+i][1]=rand()%550;	//y coordinate
		astr[0+i][2]=((double)rand()/(double)RAND_MAX)*(2*PI);	//direction
 		astr[0+i][3]=40;			//size
		astr[0+i][4]=1;			//speed
	}
 }
}

void move_asteroids(double astr[][5], double shots[][5], int x1ship, int y1ship, int x2ship, int y2ship, double *x, double *y, double *theta, int score[])
{
 int i;
 double j;
 asteroids(astr);
 for(i=0; i<20; i++){
	if(astr[i][3]!=0){
		astr[i][0]=astr[i][0]+astr[i][4]*cos(astr[i][2]); 	//iterates asteroid coordinates
		astr[i][1]=astr[i][1]-astr[i][4]*sin(astr[i][2]);
		for(j=0; j<2*PI; j=j+PI/150){
			//the line below checks whether ship has collided with an asteroid
			if((astr[i][0]+astr[i][3]*cos(j))<=x1ship && (astr[i][1]-astr[i][3]*sin(j))<=y1ship && (astr[i][0]+astr[i][3]*cos(j+PI))>=x2ship && (astr[i][1]-astr[i][3]*sin(j+PI))>=y2ship){
			
				*theta=0;	//resets theta 
				
				death(astr, shots, x, y, score);
			} else if((astr[i][0]+astr[i][3]*cos(j))>=x1ship && (astr[i][1]-astr[i][3]*sin(j))>=y1ship && (astr[i][0]+astr[i][3]*cos(j+PI))<=x2ship && (astr[i][1]-astr[i][3]*sin(j+PI))<=y2ship){
			
                                *theta=0;       //resets theta 

                                death(astr, shots, x, y, score);
			}
		}
		if(astr[i][0]>=740){
                        astr[i][0]=1;
                } else if(astr[i][0]<=-40){
                        astr[i][0]=699;
                } else if(astr[i][1]>=740){      //this keeps the ship from flying off the screen
                        astr[i][1]=1;
                } else if(astr[i][1]<=-40){
                        astr[i][1]=699;
                }
		gfx_color(0, 170, 255);
		gfx_circle(astr[i][0], astr[i][1], astr[i][3]);
	 	gfx_color(255, 255, 255);
	}
 }
}

void initialize(double arrays[][5])
{
 int i, j;
 for(i=0; i<20; i++){
	for(j=0; j<5; j++){
		arrays[i][j]=0;
		
	}
 }
}

void death(double astr[][5], double shots[][5], double *x, double *y, int score[])
{
 int i;
 
 gfx_clear();	//redraws ship and pauses to allow user time to recover
 gfx_flush();
 drawship(350, 350, 0, 0);
 usleep(1000000);
 //gfx_flush();
 *x=350;
 *y=350;
 score[1]=score[1]-1;	//subtracts a life 
 initialize(astr);	//resets astroids 
 initialize(shots); //resets angle of shots

}

void shoot(double shots[][5], double theta, double x, double y)
{
// this will create a shot to destroy asteriods in the direction of the ship
 int i, j;
 for (i=0; i<20; i++) {
        if (shots[i][3]==0) {
                 shots[i][0]=x; //X coordinate
                 shots[i][1]=y; //Y coordinate
                 shots[i][2]=theta; //direction
                 shots[i][3]=2;	//radius
                 shots[i][4]=1;
		 break;
                }
        }
}

void move_shots(double shots[][5])
{
 int i, j;
 for(i=0; i<20; i++){
        if(shots[i][3]!=0){
                shots[i][0]=shots[i][0]+6*cos(shots[i][2]+PI/2);  //shots fired at angle ship is at
                shots[i][1]=shots[i][1]-6*sin(shots[i][2]+PI/2);
                if(shots[i][0]>=700){
                        shots[i][3]=0;
                } else if(shots[i][0]<=0){
                        shots[i][3]=0;
                } else if(shots[i][1]>=700){     //this kills shots when they leave the screen
                        shots[i][3]=0;
                } else if(shots[i][1]<=0){
                        shots[i][3]=0;
                }
                gfx_color(0,255,0);
                gfx_circle(shots[i][0], shots[i][1], shots[i][3]);
        	gfx_color(255,255, 255);
	}
 }
 gfx_flush();
}

void destroy(double astr[][5], double shots[][5], int score[])
{
// This will break down the asteriods when they are hit with a shot
 int i, j, k, l;
 for (i=0; i<20; i++) {
        for (j=0; j<20; j++) {
		if(shots[j][4]!=0){   //keeps from running following statements with nonfired shots
			//line below checks to see if shot overlaps with asteroid
			if(astr[i][0]+astr[i][3]>=shots[j][0]-shots[j][3] && astr[i][0]-astr[i][3] <= shots[j][0]+shots[j][3] && astr[i][1]+astr[i][3]>=shots[j][1]-shots[i][3] && astr[i][1]- astr[i][3] <= shots[j][1]+shots[j][3]) {
        	                 
				if (astr[i][3]==40) {
                                	shots[j][4]=0;		//shot disappears 
					score[0]=score[0]+1;	//score increases
					asteroids_level2(astr, i);
                        	} else if (astr[i][3]==20) {
                                	score[0]=score[0]+2;	//score increases
					shots[j][4]=0;		//shot disappears
					asteroids_level3(astr, i);
                        	} else if (astr[i][3]<=10) {
                                	shots[i][4]=0;		//shot disappears
					score[0]=score[0]+3;
					astr[i][3]=0;		//asteroid radius set to zero so that it can go through asteroid()
                                	astr[i][4]=1;
				
				}
				gfx_flush();
			}
                }
        }
 }
}

void asteroids_level2(double astr[][5], int p)
{
 // splits a 40 r asteriod into 2 20 r
 astr[p][2]=((double)rand()/(double)RAND_MAX)*(2*PI);  //direction
 astr[p][3]=20;			//size
 astr[p][4]=1.5;                        //speed changed
 astr[p+1][2]=((double)rand()/(double)RAND_MAX)*(2*PI);  //direction
 astr[p+1][3]=20;
 astr[p+1][0]=astr[p][0];
 astr[p+1][1]=astr[p][1];
 astr[p+1][4]=2;
}
void asteroids_level3(double astr[][5], int p)
{
 // splits a 20 r asteriod into 2 10 r
 astr[p][2]=((double)rand()/(double)RAND_MAX)*(2*PI);  //direction
 astr[p][3]=10; 			//size
 astr[p][4]=2;                       //speed
 astr[p+2][2]=((double)rand()/(double)RAND_MAX)*(2*PI);  //direction
 astr[p+2][3]=10;
 astr[p+2][0]=astr[p][0];
 astr[p+2][1]=astr[p][1];
 astr[p+2][4]=3;
}

