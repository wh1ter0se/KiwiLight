/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package frc.robot.subsystems;

import com.revrobotics.CANSparkMax;
import com.revrobotics.CANSparkMax.IdleMode;
import com.revrobotics.CANSparkMaxLowLevel.MotorType;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import frc.robot.Constants;
import frc.robot.util.Util;
import frc.robot.util.Xbox;

public class SubsystemDrive extends SubsystemBase {
  private CANSparkMax
    rightMaster,
    rightSlave,
    leftMaster,
    leftSlave;

  /**
   * Creates a new SubsystemDrive.
   */
  public SubsystemDrive() {
    rightMaster = new CANSparkMax(Constants.DRIVE_RIGHT_MASTER_ID, MotorType.kBrushless);
    rightSlave  = new CANSparkMax(Constants.DRIVE_RIGHT_SLAVE_ID, MotorType.kBrushless);
    leftMaster  = new CANSparkMax(Constants.DRIVE_LEFT_MASTER_ID, MotorType.kBrushless);
    leftSlave   = new CANSparkMax(Constants.DRIVE_LEFT_SLAVE_ID, MotorType.kBrushless);

    setInverts();
    setBraking();
  }

  @Override
  public void periodic() {
    // This method will be called once per scheduler run
  }

  /**
   * Drives the drivetrain motors using the passed controller
   * @param controller The controller to drive with
   */
  public void DriveTankByController(Joystick controller) {
    setInverts();

    double throttle = Xbox.RT(controller) - Xbox.LT(controller); 
    double steering = Xbox.LEFT_X(controller);

    double driveRight = throttle + steering;
    double driveLeft = throttle - steering; 

    driveRight = (driveRight < -1 ? -1 : (driveRight > 1 ? 1 : driveRight));
    driveLeft = (driveLeft < -1 ? -1 : (driveLeft > 1 ? 1 : driveLeft));

    double inhibitor = Util.getAndSetDouble("Drive Inhibitor", 1);
    driveRight *= inhibitor;
    driveLeft *= inhibitor;

    leftMaster.set(driveLeft);
    leftSlave.set(driveLeft);
    rightMaster.set(driveRight);
    rightSlave.set(driveRight);
  }

  public void driveDirect(double left, double right) {
    rightMaster.set(right);
    rightSlave.set(right);
    leftMaster.set(left);
    leftSlave.set(left);
  }

  private void setInverts() {
    rightMaster.setInverted(Constants.DRIVE_RIGHT_MASTER_INVERT);
    rightSlave.setInverted(Constants.DRIVE_RIGHT_SLAVE_INVERT);
    leftMaster.setInverted(Constants.DRIVE_LEFT_MASTER_INVERT);
    leftSlave.setInverted(Constants.DRIVE_LEFT_SLAVE_INVERT);
  }

  private void setBraking() {
    rightMaster.setIdleMode(IdleMode.kBrake);
    rightSlave.setIdleMode(IdleMode.kBrake);
    leftMaster.setIdleMode(IdleMode.kBrake);
    leftSlave.setIdleMode(IdleMode.kBrake);
  }
}
