/*
** Controllo dei motori
*/


//subscribe da /velo

//applica i valor


#include <stdio.h>
#include <signal.h>
#include <stdlib.h> // for atoi
#include <getopt.h>

//include per beaglebone ricordarsi di collegarsi alla libreria nel make

#include <rc/motor.h>
#include <rc/time.h>
#include <rc/button.h>

//include for ros communication

#include "ros/ros.h"
#include "std_msgs/Float32MultiArray.h"
#include <sstream>
#include <math.h>

float invel[]

//
void velo_input(const std_msgs::Float32MultiArray::ConstPtr& velo)
{
    invel=velo->data;
}

int running; static int running = 0;

// possible modes, user selected with command line arguments
typedef enum m_mode_t{
	DISABLED,
	NORMAL,
	BRAKE,
	FREE,
	SWEEP
} m_mode_t;

// interrupt handler to catch ctrl-c
void signal_handler(__attribute__ ((unused)) int dummy)
{
	running=0;
	return;
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "differenziale");
    ros::NodeHandle n;
    ros::Publisher velocity_pub = n.advertise<std_msgs::Float32MultiArray>("velo", 1000);
    //ros::Subscriber sub = n.subscribe("velo", 10, joy_input);
    ros::Rate loop_rate(500);

	while (ros::ok())
    {

		ros::Subscriber subvelo = n.subscribe("velo", 1000, &velo_input);
        
        //std_msgs::Float32MultiArray vel;
        
		vel.data.resize(4);
        vel.layout.dim.resize(4);

		//rc_motor_set(ch,duty);//set duty to motor(ch)
		rc_motor_set(1,invel[0]);
		rc_motor_set(2,invel[1]);
		rc_motor_set(3,invel[2]);
		rc_motor_set(4,invel[3]);
		m_mode = NORMAL//m_mode = FREE;m_mode = BRAKE;m_mode = SWEEP;//modalità

		rc_motor_free_spin_all(duty);//free spin
		rc_motor_free_spin(ch);
		//rc_motor_brake_all(); rc_motor_brake(ch); fermare i motori al canale
		//rc_motor_set_all(duty);rc_motor_set(ch,duty); //mettere il motore a quella dutycycle

		//rc_usleep(500000);

		double duty = 0.0;
		int ch = 1;
		int c, in;
		int all = 1;	// set to 0 if a motor (-m) argument is given
		m_mode_t m_mode = DISABLED;

		// parse arguments
		opterr = 0;

		/*
		// set signal handler so the loop can exit cleanly
		signal(SIGINT, signal_handler);
		running =1;
		
		// final cleanup
		printf("calling motor_cleanup\n");
		rc_motor_cleanup();
		return 0;
		*/

		ros::spinOnce();
        //loop_rate.sleep();
	}

	return 0;
}


static void __on_pause_press(void)
{
	printf("Pause Pressed\n");
	return;
}

static void __on_pause_release(void)
{
	printf("Pause Released\n");
	return;
}

static void __on_mode_press(void)
{
	printf("Mode Pressed\n");
	return;
}

static void __on_mode_release(void)
{
	printf("Mode Released\n");
	return;
}



int main()
{
	// initialize pause and mode buttons
	if(rc_button_init(RC_BTN_PIN_PAUSE, RC_BTN_POLARITY_NORM_HIGH,
						RC_BTN_DEBOUNCE_DEFAULT_US)){
		fprintf(stderr,"ERROR: failed to init buttons\n");
		return -1;
	}
	if(rc_button_init(RC_BTN_PIN_MODE, RC_BTN_POLARITY_NORM_HIGH,
						RC_BTN_DEBOUNCE_DEFAULT_US)){
		fprintf(stderr,"ERROR: failed to init buttons\n");
		return -1;
	}

	// set signal handler so the loop can exit cleanly
	signal(SIGINT, __signal_handler);
	running = 1;

	// Assign callback functions
	rc_button_set_callbacks(RC_BTN_PIN_PAUSE, __on_pause_press, __on_pause_release);
	rc_button_set_callbacks(RC_BTN_PIN_MODE, __on_mode_press, __on_mode_release);


	//toggle leds till the program state changes
	printf("Press buttons to see response\n");
	while(running)	rc_usleep(500000);

	// cleanup and exit
	rc_button_cleanup();
	return 0;
}