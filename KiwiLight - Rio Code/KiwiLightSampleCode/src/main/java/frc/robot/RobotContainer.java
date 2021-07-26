/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package frc.robot;

import edu.wpi.first.wpilibj.GenericHID;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.XboxController;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.RunCommand;
import edu.wpi.first.wpilibj2.command.button.JoystickButton;
import frc.robot.commands.CyborgCommandAlign;
import frc.robot.subsystems.SubsystemDrive;
import frc.robot.subsystems.SubsystemReceiver;
import frc.robot.util.Xbox;

/**
 * This class is where the bulk of the robot should be declared.  Since Command-based is a
 * "declarative" paradigm, very little robot logic should actually be handled in the {@link Robot}
 * periodic methods (other than the scheduler calls).  Instead, the structure of the robot
 * (including subsystems, commands, and button mappings) should be declared here.
 */
public class RobotContainer {
  private final SubsystemDrive SUB_DRIVE        = new SubsystemDrive();
  private final SubsystemReceiver SUB_KIWILIGHT = new SubsystemReceiver(Constants.KIWILIGHT_PORT);

  private final Joystick DRIVER = new Joystick(0);

  /**
   * The container for the robot.  Contains subsystems, OI devices, and commands.
   */
  public RobotContainer() {
    // Configure the button bindings
    configureButtonBindings();
  }

  /**
   * Called every robot frame
   */
  public void periodic() {
    SmartDashboard.putNumber("Target ID", SUB_KIWILIGHT.getSpottedTargetID());
    SmartDashboard.putNumber("Target X", SUB_KIWILIGHT.getTargetPositionX());
    SmartDashboard.putNumber("Target Y", SUB_KIWILIGHT.getTargetPositionY());
    SmartDashboard.putNumber("Target Width", SUB_KIWILIGHT.getTargetWidthPixels());
    SmartDashboard.putNumber("Target Height", SUB_KIWILIGHT.getTargetHeightPixels());
    SmartDashboard.putNumber("Target Distance", SUB_KIWILIGHT.getDistanceToTarget());
    SmartDashboard.putNumber("Target Horizontal Angle", SUB_KIWILIGHT.getHorizontalAngleToTarget());
    SmartDashboard.putNumber("Target Vertical Angle", SUB_KIWILIGHT.getVerticalAngleToTarget());
  }

  /**
   * Use this method to define your button->command mappings.  Buttons can be created by
   * instantiating a {@link GenericHID} or one of its subclasses ({@link
   * edu.wpi.first.wpilibj.Joystick} or {@link XboxController}), and then passing it to a
   * {@link edu.wpi.first.wpilibj2.command.button.JoystickButton}.
   */
  private void configureButtonBindings() {
    SUB_DRIVE.setDefaultCommand(
      new RunCommand(() -> SUB_DRIVE.DriveTankByController(DRIVER), SUB_DRIVE)
    );

    JoystickButton toggleAlign = new JoystickButton(DRIVER, Xbox.A);
    toggleAlign.toggleWhenPressed(new CyborgCommandAlign(SUB_DRIVE, SUB_KIWILIGHT));
  }


  /**
   * Use this to pass the autonomous command to the main {@link Robot} class.
   *
   * @return the command to run in autonomous
   */
  public Command getAutonomousCommand() {
    // An ExampleCommand will run in autonomous
    return null;
  }
}
