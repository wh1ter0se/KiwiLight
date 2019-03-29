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
package frc.robot.Commands;

import edu.wpi.first.wpilibj.command.Command;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import frc.robot.Constants;
import frc.robot.OI;
import frc.robot.Robot;
import frc.robot.Util.MiniPID;
import frc.robot.Util.Util;
import frc.robot.Util.Xbox;

public class CyborgCommandAlign extends Command {

  private static MiniPID alignment;

  private static double topSpeed;
  private static double loopOutput;

  public CyborgCommandAlign() {
    requires(Robot.SUB_DRIVE);
    SmartDashboard.putBoolean("Aligning", false);
  }

  // Called just before this Command runs the first time
  @Override
  protected void initialize() {
    alignment = new MiniPID(Util.getAndSetDouble("Docking kP", Constants.BACKUP_DOCKING_kP),
                            Util.getAndSetDouble("Docking kI", Constants.BACKUP_DOCKING_kI),
                            Util.getAndSetDouble("Docking kD", Constants.BACKUP_DOCKING_kD));
    topSpeed = Util.getAndSetDouble("Max Alignment Speed", Constants.BACKUP_MAX_SPEED);
      alignment.setOutputLimits(-1 * topSpeed, topSpeed); // tells the PID loop the maximum and minimum values
      alignment.setSetpoint(0); // tells the PID loop to try to regulate the angle to 0 degrees (the center)

    SmartDashboard.putBoolean("Aligning", true);
  }

  // Called repeatedly when this Command is scheduled to run
  @Override
  protected void execute() {
    loopOutput = alignment.getOutput(Robot.SUB_RECEIVER.getLastKnownData()[3]); // gets the PID loop's output
      if (Robot.SUB_RECEIVER.targetLost()) { loopOutput = 0; } // tosses that value if the target is out of the FOV
    
    if (Robot.SUB_RECEIVER.targetSpotted()) {
      Robot.oi.setDriverRumble(1); // rumble if robot is aligning itself
      double throttle = Xbox.RT(OI.DRIVER) - Xbox.LT(OI.DRIVER); // get the driver's forward/reverse throttle
      throttle *= Util.getAndSetDouble("RL Inhibitor", 1); // apply the rocket league inhibitor
      Robot.SUB_DRIVE.driveDirect(throttle - loopOutput, throttle + loopOutput); // combine throttle with PID output and send values
    } else {
      Robot.oi.setDriverRumble(0); // stop rumbling if the target is not in sight
      Robot.SUB_DRIVE.driveRocketLeague(OI.DRIVER,
                                        Util.getAndSetDouble("RL Ramp", 0), 
                                        Util.getAndSetDouble("RL Inhibitor", 1),
                                        Util.getAndSetDouble("RL Turn Inhibitor", .75)); // give controls back if not spotted
    }
  }

  // Make this return true when this Command no longer needs to run execute()
  @Override
  protected boolean isFinished() {
    return false;
  }

  // Called once after isFinished returns true
  @Override
  protected void end() {
    Robot.oi.setDriverRumble(0);
    Robot.SUB_DRIVE.stopMotors();
    SmartDashboard.putBoolean("Aligning", false);
  }

  // Called when another command which requires one or more of the same
  // subsystems is scheduled to run
  @Override
  protected void interrupted() {
    Robot.oi.setDriverRumble(0);
    Robot.SUB_DRIVE.stopMotors();
    SmartDashboard.putBoolean("Aligning", false);
  }
}
