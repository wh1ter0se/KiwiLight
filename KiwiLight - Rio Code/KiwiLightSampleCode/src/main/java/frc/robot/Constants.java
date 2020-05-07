/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package frc.robot;

/**
 * The Constants class provides a convenient place for teams to hold robot-wide numerical or boolean
 * constants.  This class should not be used for any other purpose.  All constants should be
 * declared globally (i.e. public static).  Do not put anything functional in this class.
 *
 * <p>It is advised to statically import this class (or one of its inner classes) wherever the
 * constants are needed, to reduce verbosity.
 */
public final class Constants {
    /**
     * Motor IDs
     */
    public static final int
        DRIVE_RIGHT_MASTER_ID = 1,
        DRIVE_RIGHT_SLAVE_ID  = 2,
        DRIVE_LEFT_MASTER_ID  = 3,
        DRIVE_LEFT_SLAVE_ID   = 4;

    /**
     * KiwiLight port
     */
    public static final int 
        KIWILIGHT_PORT = 3695;

    /**
     * Motor inverts
     */
    public static final boolean
        DRIVE_RIGHT_MASTER_INVERT = false,
        DRIVE_RIGHT_SLAVE_INVERT  = false,
        DRIVE_LEFT_MASTER_INVERT  = true,
        DRIVE_LEFT_SLAVE_INVERT   = true;
}
