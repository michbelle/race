/*
codice per la realizzazione del differenziale per un auto
*/

//include quello che serve
#include "ros/ros.h"
#include "std_msgs/Float32MultiArray.h"
#include "sensor_msgs/Joy.h"
#include <sstream>
#include <math.h>
#include <stdlib.h>

//dati della macchina
float a=0.13;//distanza centri ruote anteriori
float b=0.13;//distanza centri ruote posteriori
float L=0.15;//distanza tra ruote anteriori e posteriori
float r=0.025;//raggio ruota
#define PI 3.14159265

/*test input*/
float w;
float v;
float gam2,gam1;
float R, vlf,vrf,vlr,vrr, al, be, va, vb;

sensor_msgs::Joy::ConstPtr cosas;

/*
 * sistema ricezione comandi
 */
void joy_input(const sensor_msgs::Joy::ConstPtr& joysub)
{
        cosas=joysub;
        v=joysub->axes[5];
        v=-(v-1)/4;
        gam2=joysub->axes[0];
        fprintf(stderr,"veloc %f\n",v);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "differenziale");
    ros::NodeHandle n;

    ros::Subscriber subjoy = n.subscribe("joy", 50, &joy_input);
    std_msgs::Float32MultiArray vel;
    vel.data.resize(4);
    vel.layout.dim.resize(4);

    ros::Publisher velocity_pub = n.advertise<std_msgs::Float32MultiArray>("velo", 100);
        
    ros::Rate loop_rate(50);

    while (ros::ok())
    {
        /*formule*/

        /*
        //posizione imu a caso riutilizzando le stesse formule
        v=v_imu*R/(sqrt(R^2+y_imu^2))
        */

	if (gam2 < 0.01 && gam2 >-0.01)
	{
            //se gamma troppo basso allora infinito --> errore
            //faccio un if ed elimino casi sotto tot valore in base a sensibilità
            vrr=v;
            vlr=v;
            vlf=v;
            vrf=v;
            va=v;
            vb=v;
	}
	else
	{
            if(gam2>0)
            {
                R=L/tan(gam2);
                w=v/R;
                vlf=w*sqrt(pow((R-a/2),2)+pow(L,2));
                vrf=w*sqrt(pow((R+a/2),2)+pow(L,2));
                vlr=w*(R-b/2);
                vrr=w*(R+b/2);


                //velocità per ruota
                //wr=[v_lf/r, v_rf/r, v_lr/r, v_rr/r];

                 /*
                test per me
                */

                al=atan(L/(R-a/2));//angolo di sterzata ruota sinistra
                be=atan(L/(R+a/2));//angolo di sterzata ruota destra

                va=vlf*cos(al);
                vb=vrf*cos(be);
            }
            else
            {
                gam1=abs(gam2); //faccio inversa
                R=L/tan(gam1);
                w=v/R;
                vrf=w*sqrt(pow((R-a/2),2)+pow(L,2));
                vlf=w*sqrt(pow((R+a/2),2)+pow(L,2));
                vrr=w*(R-b/2);
                vlr=w*(R+b/2);

                //velocità per ruota
                //wr=[v_lf/r, v_rf/r, v_lr/r, v_rr/r];

                /*
                test per me
                */

                be=atan(L/(R-a/2));//angolo di sterzata ruota destra
                al=atan(L/(R+a/2));//angolo di sterzata ruota sinistra

                va=vlf*cos(al);
                vb=vrf*cos(be);

		}
	}

        //se velocità oltre 1 impossibile per il sistema
        if(va>1){va=1;}
        if(vb>1){vb=1;}
        if(vlf>1){vlf=1;}
        if(vrf>1){vrf=1;}


        /*
        output velocità per ruota
        */
	
	//test con Float32MultiArray: compila ma dump
	vel.layout.dim[0].label  = "vlf";
	//vel.layout.dim[0].size   = 1;
	//vel.layout.dim[0].stride = 1;
	vel.layout.dim[1].label  = "vrf";
	//vel.layout.dim[1].size   = 1;
	//vel.layout.dim[1].stride = 1;
        vel.layout.dim[2].label  = "vlr";
	//vel.layout.dim[2].size   = 1;
	//vel.layout.dim[2].stride = 1;
        vel.layout.dim[3].label  = "vrr";
	//vel.layout.dim[3].size   = 1;
	//vel.layout.dim[3].stride = 1;
        vel.data[2]=va;//vlf//va
        vel.data[3]=vb;//vrf//vb
        vel.data[1]=vlr;
        vel.data[4]=vrr;

        if(cosas)
        {
                velocity_pub.publish(vel);
                }
        
        ros::spinOnce();
        loop_rate.sleep();

        }
        
    return 0;

}
