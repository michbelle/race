/*
codice per la realizzazione del differenziale per un auto
*/

//include quello che serve
#include "ros/ros.h"
#include "std_msgs/Float32MultiArray.h"
#include "sensor_msgs/Joy.h"
#include <sstream>
#include <math.h>

//dati della macchina
float a=100;//distanza centri ruote anteriori
float b=100;//distanza centri ruote posteriori
float L=100;//distanza tra ruote anteriori e posteriori
float r=0.5;//raggio ruota
#define PI 3.14159265

/*test input*/
float w;
float v;
float gam;
float R, vlf,vrf,vlr,vrr, al, be, va, vb;


/*
 * sistema ricezione comandi
 */
void joy_input(const sensor_msgs::Joy::ConstPtr& joy)
{
        v=joy->axes[5];
        v=-(v-1)/4;
        gam=joy->axes[0];
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "differenziale");
    ros::NodeHandle n;
    ros::Publisher velocity_pub = n.advertise<std_msgs::Float32MultiArray>("velo", 1000);
    ros::Subscriber sub = n.subscribe("joy", 10, joy_input);
    ros::Rate loop_rate(500);

    while (ros::ok())
    {

        ros::Subscriber subjoy = n.subscribe("joy", 1000, &joy_input);
        /*w=subjoy.axes[0];
        v=subjoy->axes[5];
        v=-(v-1)/4;*/

        std_msgs::Float32MultiArray vel;
	vel.data.resize(4);
        vel.layout.dim.resize(4);

        /*formule*/

        /*
        //posizione imu a caso riutilizzando le stesse formule
        v=v_imu*R/(sqrt(R^2+y_imu^2))
        */

	if (gam < 0.01 && gam >-0.01)
	{
            //se gamma troppo basso allora infinito --> errore
            //faccio un if ed elimino casi sotto tot valore in base a sensibilità
            vrr=v;
            vlr=v;
            vlf=v;
            vrf=v;
	}
	else
	{
            if(gam>0)
            {
                R=L/tan(w);
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
                be=atan(L/(R+a/2));//angolo di sterzata ruota sinistra

                va=vlf*cos(al);
                vb=vrf*cos(be);
            }
            else
            {
                w=-w; //faccio inversa
                R=L/tan(w);
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

                be=atan(L/(R-a/2));//angolo di sterzata ruota sinistra
                al=atan(L/(R+a/2));//angolo di sterzata ruota sinistra

                va=vlf*cos(al);
                vb=vrf*cos(be);

		}
	}

        //se velocità oltre 1 impossibile per il sistema
        if (va>1 || vb>1 || vlf>1 || vrf>1)
        {

        }



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
        vel.data[0]=w;//vlf;//va
        vel.data[1]=vrf;//;vb
        vel.data[2]=vlr;
        vel.data[3]=vrr;

        velocity_pub.publish(vel);
        ros::spinOnce();
        //loop_rate.sleep();

        }
    return 0;

}
