/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

/*---------------------------------------------------------------*/
/*          .d8888b.   .d8888b.   .d8888b.  888888888            */
/*         d88P  Y88b d88P  Y88b d88P  Y88b 888                  */
/*              .d88P 888        888    888 888                  */
/*             8888"  888d888b.  Y88b. d888 8888888b.            */
/*              "Y8b. 888P "Y88b  "Y888P888      "Y88b           */
/*         888    888 888    888        888        888           */
/*         Y88b  d88P Y88b  d88P Y88b  d88P Y88b  d88P           */
/*          "Y8888P"   "Y8888P"   "Y8888P"   "Y8888P"            */
/*                                                               */
/*  This code was written by FRC3695 - Foximus Prime and stored  */
/*   at github.com/wh1ter0se/KiwiLight. KiwiLight is published   */
/*     under a GPL-3.0 license, permitting modification and      */
/*      distribution under the condition that the source is      */
/* disclosed and distribution is accompanied by the same license */
/*---------------------------------------------------------------*/
package frc.robot.util;

import edu.wpi.first.wpilibj.Joystick;

/** 
 * All of the stuff you need to implement X360 controllers 
 */
public class Xbox {
	
	public static final double DEADZONE = 0.1;
	
	public static final int
			A = 1,
			B = 2,
			X = 3,
			Y = 4,
			LB = 5,
			RB = 6,
			BACK = 7,
			START = 8,
			LSTICK = 9,
			RSTICK = 10;

	private static double deadzone(double rawAxis) { // deadzone value is in constants
		boolean positive = rawAxis > 0.0;
		rawAxis *= (positive ? 1.0 : -1.0); //flip if needed
		rawAxis -= DEADZONE; //clip dead zone
		if(rawAxis < 0.0) rawAxis = 0.0; //trim if less than 0
		rawAxis /= (1.0 - DEADZONE); //scale back to 1.0
		rawAxis *= (positive ? 1.0 : -1.0); //flip back
		return rawAxis;
	}
	
	public static double LEFT_X(Joystick joy) {return deadzone(joy.getRawAxis(0));}
	public static double LEFT_Y(Joystick joy) {return deadzone(joy.getRawAxis(1));}
	public static double RIGHT_X(Joystick joy) {return deadzone(joy.getRawAxis(4));}
	public static double RIGHT_Y(Joystick joy) {return deadzone(joy.getRawAxis(5));}
	public static double LT(Joystick joy) {return joy.getRawAxis(2);}
	public static double RT(Joystick joy) {return joy.getRawAxis(3);}
}