/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package frc.robot.commands;

import edu.wpi.first.wpilibj.controller.PIDController;
import edu.wpi.first.wpilibj2.command.CommandBase;
import frc.robot.subsystems.SubsystemDrive;
import frc.robot.subsystems.SubsystemReceiver;
import frc.robot.util.Util;

public class CyborgCommandAlign extends CommandBase {
  private SubsystemDrive drivetrain;
  private SubsystemReceiver kiwilight;
  private PIDController pidcontroller;

  /**
   * Creates a new CyborgCommandAlign.
   */
  public CyborgCommandAlign(SubsystemDrive drivetrain, SubsystemReceiver kiwilight) {
    this.drivetrain = drivetrain;
    this.kiwilight = kiwilight;
    addRequirements(drivetrain);
  }

  // Called when the command is initially scheduled.
  @Override
  public void initialize() {
    double kP = Util.getAndSetDouble("Align kP", 0);
    double kI = Util.getAndSetDouble("Align kI", 0);
    double kD = Util.getAndSetDouble("Align kD", 0);

    pidcontroller = new PIDController(kP, kI, kD);
  }

  // Called every time the scheduler runs while the command is scheduled.
  @Override
  public void execute() {
    if(kiwilight.targetSpotted()) {
      double horizontalAngle = kiwilight.getHorizontalAngleToTarget();
      double output = pidcontroller.calculate(horizontalAngle);
      drivetrain.driveDirect(output, output * -1);
    } else {
      drivetrain.driveDirect(0, 0);
    }
  }

  // Called once the command ends or is interrupted.
  @Override
  public void end(boolean interrupted) {
    drivetrain.driveDirect(0, 0);
  }

  // Returns true when the command should end.
  @Override
  public boolean isFinished() {
    return false;
  }
}
