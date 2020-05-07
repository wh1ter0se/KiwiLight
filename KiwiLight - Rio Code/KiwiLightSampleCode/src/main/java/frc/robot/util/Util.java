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

import edu.wpi.first.wpilibj.Preferences;

/**
 * If this repo is colonial America, this class is Rhode Island
 */
public class Util {

	/**
	 * Trims int within a range
	 * @param input int before truncation
	 * @param lowLimit lowest allowable int
	 * @param highLimit highest allowable int
	 * @return truncated int
	 */
	public static int truncateInt(int input, int lowLimit, int highLimit) {
		if (input < lowLimit) { input = lowLimit; }
		if (input > highLimit) { input = highLimit; }
		return input;
	}

	/**
	 *	Kind of self explanatory, but with some spice
	 *	Use this mainly as a get method to retrieve values the user types into the dashboard
	 *		(the 'Set' part is only in case the value doesn't exist, backup is a default to use and set if it isn't there)
	 */
	static Preferences pref = Preferences.getInstance();
	public static double getAndSetDouble(String key, double backup) {
		if(!pref.containsKey(key)) pref.putDouble(key, backup);
		return pref.getDouble(key, backup);
	}

	/**
     * Really stupid but needed to round a double to n places
     * @param value  original value
     * @param places how many values after decimal point
     * @return       rounded value
     */
    public static double roundTo(double value, int places) {
        double val = value;
        val *= Math.pow(10, places);
        val = Math.round(val);
        val /= Math.pow(10, places);
        return val;
    }
}
