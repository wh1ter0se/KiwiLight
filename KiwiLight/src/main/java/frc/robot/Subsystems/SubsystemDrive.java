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

import com.revrobotics.CANSparkMax;
import com.revrobotics.CANSparkMax.IdleMode;
import com.revrobotics.CANSparkMaxLowLevel.MotorType;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.command.Subsystem;
import frc.robot.Constants;
import frc.robot.Commands.ManualCommandDrive;
import frc.robot.Util.Xbox;

/**
 * The vroom-vroom control room
 */
public class SubsystemDrive extends Subsystem {
  
  private static CANSparkMax leftMaster;
  private static CANSparkMax leftSlave;
  private static CANSparkMax rightMaster;
  private static CANSparkMax rightSlave;

  @Override
  public void initDefaultCommand() {
    setDefaultCommand(new ManualCommandDrive());
  }

  public SubsystemDrive() {
    leftMaster  = new CANSparkMax(Constants.LEFT_MASTER_ID, MotorType.kBrushless);
      leftSlave   = new CANSparkMax(Constants.LEFT_SLAVE_ID, MotorType.kBrushless);
    rightMaster = new CANSparkMax(Constants.RIGHT_MASTER_ID, MotorType.kBrushless);
      rightSlave  = new CANSparkMax(Constants.RIGHT_SLAVE_ID, MotorType.kBrushless);
    
    setInverts();
    setBraking(true);
  }

  /**
   * Sets the inverts of each motor controller
   */
  private void setInverts() {
    leftMaster.setInverted(Constants.LEFT_DRIVE_INVERT);
      leftSlave.setInverted(Constants.LEFT_DRIVE_INVERT);
    rightMaster.setInverted(Constants.RIGHT_DRIVE_INVERT);
      rightSlave.setInverted(Constants.RIGHT_DRIVE_INVERT);
  }

  /**
   * Sets each motor to braking or coasting mode
   * @param braking true if braking mode, false if coasting mode
   */
  public void setBraking(Boolean braking) {
    leftMaster.setIdleMode(braking ? IdleMode.kBrake : IdleMode.kCoast);
      leftSlave.setIdleMode(braking ? IdleMode.kBrake : IdleMode.kCoast);
    rightMaster.setIdleMode(braking ? IdleMode.kBrake : IdleMode.kCoast);
      rightSlave.setIdleMode(braking ? IdleMode.kBrake : IdleMode.kCoast);
  }

  /**
   * Rocket League-style control for a tank drive bot
   * Left and right triggers accelerate linearly and left stick rotates
   * @param joy the joystick to be used
   * @param ramp the ramprate to apply to the motors
   * @param inhibitor a scalar applied to the final motor output to limit speed
   */
  public void driveRocketLeague(Joystick joy, double ramp, double inhibitor, double turnInhibitor) {
    double adder = Xbox.RT(joy) - Xbox.LT(joy);
    double left = adder + (Xbox.LEFT_X(joy) * turnInhibitor);
    double right = adder - (Xbox.LEFT_X(joy) * turnInhibitor);

    left = (left > 1.0 ? 1.0 : (left < -1.0 ? -1.0 : left));
    right = (right > 1.0 ? 1.0 : (right < -1.0 ? -1.0 : right));

    left *= inhibitor;
    right *= inhibitor;

    leftMaster.set(left);
      leftSlave.follow(leftMaster);
    rightMaster.set(right);
      rightSlave.follow(rightMaster);
  }

  public void driveDirect(double leftOutput, double rightOutput) {
    leftMaster.set(leftOutput);
      leftSlave.follow(leftMaster);
    rightMaster.set(rightOutput);
      rightSlave.follow(rightMaster);
  }

  /**
   * Sets all motor controller values to zero
   */
  public void stopMotors() {
    leftMaster.set(0);
      leftSlave.follow(leftMaster);
    rightMaster.set(0);
      rightSlave.follow(rightMaster);
  }

}
