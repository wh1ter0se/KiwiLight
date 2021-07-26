/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

/**
 * _____/\\\\\\\\\\_____________/\\\\\______/\\\\\\\\\______/\\\\\\\\\\\\\\\_         
 *  ___/\\\///////\\\________/\\\\////_____/\\\///////\\\___\/\\\///////////__        
 *   __\///______/\\\______/\\\///_________/\\\______\//\\\__\/\\\_____________       
 *    _________/\\\//_____/\\\\\\\\\\\_____\//\\\_____/\\\\\__\/\\\\\\\\\\\\_____     
 *     ________\////\\\___/\\\\///////\\\____\///\\\\\\\\/\\\__\////////////\\\___    
 *      ___________\//\\\_\/\\\______\//\\\_____\////////\/\\\_____________\//\\\__   
 *       __/\\\______/\\\__\//\\\______/\\\____/\\________/\\\___/\\\________\/\\\__  
 *        _\///\\\\\\\\\/____\///\\\\\\\\\/____\//\\\\\\\\\\\/___\//\\\\\\\\\\\\\/___ 
 *         ___\/////////________\/////////_______\///////////______\/////////////_____
 * 
 * KiwiLight SubsystemReceiver - Version 1.4
 * Written By: Colton Kreischer and Brach Knutson of FRC team 3695 - Foximus Prime
 */

package frc.robot.subsystems;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import frc.robot.util.Util;

/**
 * The thing that listens to the Pi.
 */
public class SubsystemReceiver extends SubsystemBase {
  private String         latestSegment;
  private double[]       latestData;
  private DatagramSocket serverSocket;
  private byte[]         receiveData;
  private long           latestTime;

  /**
   * Creates a new SubsystemReceiver.
   * @param port The port to listen for data on. This should match the port that the configuration uses.
   */
  public SubsystemReceiver(int port) {
    latestSegment = "-1,-1,-1,-1,-1,-1,180,180";
    latestData = new double[] {-1, -1, -1, -1, -1, -1, 180, 180};
    latestTime    = System.currentTimeMillis();

    SmartDashboard.putString("RPi Data", latestSegment);
    SmartDashboard.putBoolean("Spotted", false);
    SmartDashboard.putBoolean("Updated", false);

    try {
      serverSocket = new DatagramSocket(port);
      receiveData  = new byte[1024];
    } catch (SocketException e) { //thrown when a socket cannot be created
      DriverStation.reportError("SOCKET EXCEPTION", true);
    }

    // EXPECTED FORMAT OF INPUT STRING:
    // :X,Y,W,H,D,HA,VA;
      // ID = ID of focused target
      // X = X-coordinate
      // Y = Y-coordinate
      // W = Width of target in image space.
      // H = Height of target in image space
      // D = Distance from target
      // HA = Horizontal Angle from center (positive = CW)
      // VA = Vertical Angle from center (positive = up)

    Thread listener = new Thread(() -> {
      while(!Thread.interrupted()) {
        try {
          DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length); //create a new packet for the receiving data 
          serverSocket.receive(receivePacket); //receive the packet from the Socket
          String segment = new String(receivePacket.getData()).replaceAll("\\s+",""); //remove whitespace and place data in 'segment'
          latestSegment = segment.substring(segment.indexOf(":") + 1, segment.indexOf(";")); // store segment without borders
          latestTime = System.currentTimeMillis(); // add timestamp for stored segment
          String formattedString = segment.substring(segment.indexOf(":") + 1, segment.indexOf(";"));
          SmartDashboard.putString("RPi Data", formattedString); // put string on dashboard without borders
          latestData = analyzeData(formattedString);

        } catch (IOException e) { //thrown when the socket cannot receive the packet
          DriverStation.reportError("IO EXCEPTION", true);
        }
      }
    });
    
    listener.start();
  }

  @Override
  public void periodic() {
    SmartDashboard.putBoolean("KiwiLight Target Spotted", targetSpotted());
    SmartDashboard.putBoolean("KiwiLight Data Updated", getSecondsSinceUpdate() < 0.5);
  }

  /**
   * Returns true if the subsystem has received a packet in the last half second,
   * false otherwise.
   * @return true if the system is ready for a match, false otherwise.
   */
  public boolean updated() {
    return getSecondsSinceUpdate() < 0.5;
  }

  /**
   * Retrieves the last known pixel coordinates of the target
   * @return [0] = ID of target
   *         [1] = X-coordinate (in pixels from left)
   *         [2] = Y-coordinate (in pixels from bottom) 
   *         [3] = Width (in pixels) 
   *         [4] = Height (in pixels) 
   *         [5] = Distance 
   *         [6] = Horizontal Angle from Center (in degrees; positive = CW)
   *         [7] = Vertical Angle from Center (in degrees; positive = up)
   *         {-1, -1, -1 ,-1 ,-1 ,-1 ,180, 180} for no known location
   */
  public double[] getLatestData() {
    return latestData;
  }

  /**
   * Returns the spotted target's ID number, or -1 if no target is spotted.
   * @return Target ID.
   */
  public int getSpottedTargetID() {
    return (int) latestData[0];
  }

  /**
   * Returns the spotted target's X position.
   * @return Target X coordinate in pixels from left of image.
   */
  public int getTargetPositionX() {
    return (int) latestData[1];
  }

  /**
   * Returns the spotted target's Y position.
   * @return Target Y coordinate in pixels from top of image.
   */
  public int getTargetPositionY() {
    return (int) latestData[2];
  }

  /**
   * Returns the width of the seen target in pixels, or -1 if no target is seen.
   * @return Target width.
   */
  public double getTargetWidthPixels() {
    return latestData[3];
  }

  /**
   * Returns the height of the seen target in pixels, or -1 if no target is seen.
   * @return Target height.
   */
  public double getTargetHeightPixels() {
    return latestData[4];
  }

  /**
   * Returns the distance of the camera to the target, or -1 if no target is seen.
   * @return Target distance.
   */
  public double getDistanceToTarget() {
    return latestData[5];
  }

  /**
   * Returns the horizontal angle (degrees) to the target, or 180 if no target is seen.
   * @return Horizontal angle to target.
   */
  public double getHorizontalAngleToTarget() {
    return latestData[6];
  }

  /**
   * Returns the vertical angle (degrees) to the target, or 180 if no target is seen.
   * @return Vertical angle to target.
   */
  public double getVerticalAngleToTarget() {
    return latestData[7];
  }

  /**
   * Returns true if a target is seen, false otherwise.
   * @return Whether or not the target is spotted.
   */
  public boolean targetSpotted() {
    return latestData[2] > -1;
  }

  /**
   * Returns the miliseconds since the pi sent the LastKnownLocation
   * @return ms since last received UDP packet
   */
  public double getSecondsSinceUpdate() {
    return Util.roundTo((double) ((System.currentTimeMillis() - latestTime) / 1000), 5);
  }

  /**
   * Retrieves the last known pixel coordinates of the target
   * @return Array of {@code double}s with the same formatting as that which is returned by {@link #getLatestData()}.
   */
  private double[] analyzeData(String input) {
    double[] newData = {-1, -1, -1, -1, -1, -1, 180, 180};
    String[] stringData = input.split(",");

    if(stringData.length != newData.length) {
      DriverStation.reportWarning("INPUT STRING IMPROPERLY FORMATTED!", true);
      return newData;
    }

    try {
      for(int i=0; i<stringData.length; i++) {
        newData[i] = Double.parseDouble(stringData[i]);
      }
    } catch(Exception ex) {
      DriverStation.reportWarning("PARSING DATA ERROR: " + ex.getMessage(), true);
    }

    return newData;
  }
}