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
package frc.robot.Subsystems;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.util.stream.IntStream;

import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.command.Subsystem;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import frc.robot.Constants;
import frc.robot.Robot;
import frc.robot.Util.Util;

/**
 * The receiver code that runs on the Rio to listen for UDP data
 */
public class SubsystemReceiver extends Subsystem {

  private static String latestSegment;

  private static DatagramSocket serverSocket;
  private static byte[]         receiveData;

  private static long latestTime;

  @Override
  public void initDefaultCommand() {
  }

  public SubsystemReceiver() {
    latestSegment = "-1,-1,-1,-1";
    latestTime    = System.currentTimeMillis();

    SmartDashboard.putString("RPi Data", latestSegment);

    // EXPECTED FORMAT OF INPUT STRING:
    // :X,Y,H,D,A;
      // X = X-coordinate
      // Y = Y-coordinate
      // D = Distance from target
      // A = Angle from center (positive = CW)

    Thread listener = new Thread(() -> {
      while(!Thread.interrupted()) {
        try {
          DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length); //create a new packet for the receiving data 
          serverSocket.receive(receivePacket); //receive the packet from the Socket
          String segment = new String(receivePacket.getData()).replaceAll("\\s+",""); //remove whitespace and place data in 'segment'
          latestSegment = segment.substring(segment.indexOf(":") + 1, segment.indexOf(";")); // store segment without borders
          latestTime = System.currentTimeMillis(); // add timestamp for stored segment
          SmartDashboard.putString("RPi Data", segment.substring(segment.indexOf(":") + 1, segment.indexOf(";"))); // put string on dashboard without borders
        } catch (IOException e) { //thrown when the socket cannot receive the packet
          DriverStation.reportError("IO EXCEPTION", true);
        }
      }
    });
    
    listener.start();

  }

  /**
   * Retrieves the last known pixel coordinates of the target
   * @return [0] = X-coordinate (in pixels from left)
   *         [1] = Y-coordinate (in pixels from bottom)
   *         [2] = Distance (in inches)
   *         [3] = Angle from center (in degrees; positive = CW)
   *         {-1,-1,-1, 180} for no known location
   */
  public double[] getLastKnownData() {
      double[] data = new double[]{-1,-1,-1,-1};
      int[] indices = new int[]{2,5,8};
      try {
        indices = IntStream.range(0, latestSegment.length() - 1)
                  .filter(i -> latestSegment.charAt(i) == ',')
                  .toArray();
        data[0] = Integer.parseInt(latestSegment.substring(0, latestSegment.indexOf(",", indices[0])));
        data[1] = Integer.parseInt(latestSegment.substring(latestSegment.indexOf(",", indices[0]) + 1, latestSegment.indexOf(",", indices[1])));
        data[2] = Integer.parseInt(latestSegment.substring(latestSegment.indexOf(",", indices[1]) + 1, latestSegment.indexOf(",", indices[2])));
        data[3] = Integer.parseInt(latestSegment.substring(latestSegment.indexOf(",", indices[2]) + 1));
      } catch (NumberFormatException e) {
        DriverStation.reportError("NUMBER FORMAT EXCEPTION", true); 
        DriverStation.reportError("latestSegment = " + latestSegment, false);
      } catch (StringIndexOutOfBoundsException e) {
        DriverStation.reportError("STRING INDEX OUT OF BOUNDS EXCEPTION", true);
        DriverStation.reportError("latestSegment = " + latestSegment, false);
      } catch (ArrayIndexOutOfBoundsException e) {
        DriverStation.reportError("ARRAY INDEX OUT OF BOUNDS EXCEPTION", true);
        DriverStation.reportError("latestSegment = " + latestSegment, false);
      }
      return data;
  }

  /**
   * Returns the miliseconds since the pi sent the LastKnownLocation
   * @return ms since last received UDP packet
   */
  public double getSecondsSinceUpdate() {
    return Util.roundTo((double) ((System.currentTimeMillis() - latestTime) / 1000), 2);
  }

  /**
   * Checks if the JeVois is sending coordinates
   * @return true if receiving a distance, false if distance is -1
   */
  public Boolean targetSpotted() {
    return Robot.SUB_RECEIVER.getLastKnownData()[2] != -1;
  }

  /**
   * Checks if the current coordinate is outside of target lock IF within target lock range.
   * This prevents the loop from getting confused if it spots another target in its periphery.
   * Without this function, KiwiLight is usually unable to do the shuttle without getting confused.
   * @return true if within target lock range and target is outside of the locked field of view, false otherwise
   */
  public Boolean targetLost() {
    return Math.abs(Robot.SUB_RECEIVER.getLastKnownData()[3]) > Constants.TARGET_LOCK_FOV && Robot.SUB_RECEIVER.getLastKnownData()[2] < Constants.TARGET_LOCK_RANGE;
  }
}
