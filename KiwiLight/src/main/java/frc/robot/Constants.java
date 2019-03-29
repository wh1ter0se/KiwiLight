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
package frc.robot;

/**
 * A place to store magic numbers
 */
public class Constants {

    /**
     * Align Values
     */
    public static final double 
        TARGET_LOCK_FOV   = 10,
        TARGET_LOCK_RANGE = 35;

    /**
     * Backup Docking PID values
     */
    public static final double
        BACKUP_DOCKING_kP = .25, // start tuning by using kP and 'Max Alignment Speed' only
        BACKUP_DOCKING_kI = 0,
        BACKUP_DOCKING_kD = .125, // a touch of kD helps to smooth out the loop if kP alone oscilates too much
        BACKUP_MAX_SPEED  = .25;

    /**
     * Inverts
     */
    public static final Boolean
        LEFT_DRIVE_INVERT  = false,
        RIGHT_DRIVE_INVERT = true;
        
    /**
     * Spark MAX ID's
     */
    public static final int
        LEFT_MASTER_ID  = 0,
        LEFT_SLAVE_ID   = 1,
        RIGHT_MASTER_ID = 2,
        RIGHT_SLAVE_ID  = 3;
   
}