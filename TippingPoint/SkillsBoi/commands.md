# Auton Commands  
  
*27 Commands auto-generated from "src/auton.cpp"*  

---

## TURN:  
Robot will turn to face the specified angle  
*BLOCKING, has timeout*  
### Usage:
  > TURN, drive_turn_target, command_time_out,  
### Arguments: (2)
  * **drive_turn_target**:	Angle to turn to (degrees)
  * **command_time_out**:	Timout for command (seconds)  
  
## FACE:  
Robot will turn to face the specified point  
*BLOCKING, has timeout*  
### Usage:
  > FACE, drive_target_x, drive_target_y, command_time_out,  
### Arguments: (3)
  * **drive_target_x**:	X target (field coord in inches)
  * **drive_target_y**:	Y target (field coord in inches)
  * **command_time_out**:	Timout for command (seconds)  
  
## DRIVE:  
Robot will drive in a direction, at a speed, for a time  
*BLOCKING, has timeout*  
### Usage:
  > DRIVE, drive_speed_target, drive_turn_target, command_time_out,  
### Arguments: (3)
  * **drive_speed_target**:	Speed to drive (-127, 127)
  * **drive_turn_target**:	Direction to face (degrees)
  * **command_time_out**:	Timout for command (seconds)  
  
## DRIVEDIST:  
Robot will drive in a direction, at a speed, for a distance  
*BLOCKING, has timeout*  
### Usage:
  > DRIVEDIST, drive_speed_target, drive_turn_target, drive_distance_target, command_time_out,  
### Arguments: (4)
  * **drive_speed_target**:	Speed to drive (-127, 127)
  * **drive_turn_target**:	Direction to face (degrees)
  * **drive_distance_target**:	Distance to drive (inches)
  * **command_time_out**:	Timout for command (seconds)  
  
## END:  
End of routine  
*BLOCKING, NO TIMEOUT*  
### Usage:
  > END,   
  
## PAUSE:  
Do nothhing for a time  
*BLOCKING, has timeout*  
### Usage:
  > PAUSE, command_time_out,  
### Arguments: (1)
  * **command_time_out**:	Timout for command (seconds)  
  
## WAIT:  
Do nothing until condition is met, eg. WAIT, LIFTABOVE, 1000, 5,  
*BLOCKING, has timeout*  
### Usage:
  > WAIT, wait_condition, wait_parameter, command_time_out,  
### Arguments: (3)
  * **wait_condition**:	What to wait for (keyword)
  * **wait_parameter**:	Value to wait until (number)
  * **command_time_out**:	Timout for command (seconds)  
  
## DEPLOY:  
Deploys the robot  
*non-blocking, NO TIMEOUT*  
### Usage:
  > DEPLOY,   
  
## CHILLYEET:  
Deactivates yeet release piston  
*non-blocking, NO TIMEOUT*  
### Usage:
  > CHILLYEET,   
  
## DRIVETO:  
Robot will drive to target position on field  
*BLOCKING, has timeout*  
### Usage:
  > DRIVETO, drive_speed_target, drive_target_x, drive_target_y, command_time_out,  
### Arguments: (4)
  * **drive_speed_target**:	How fast to drive (-127, 127)
  * **drive_target_x**:	X target (field coord in inches)
  * **drive_target_y**:	Y target (field coord in inches)
  * **command_time_out**:	Timout for command (seconds)  
  
## SETPOS:  
Sets the robot's position on the field  
*non-blocking, NO TIMEOUT*  
### Usage:
  > SETPOS, x_pos, y_pos,  
### Arguments: (2)
  * **x_pos**:	X coord (field coord in inches)
  * **y_pos**:	Y coord (field coord in inches)  
  
## SETDIR:  
Sets the robot's direction on the field  
*non-blocking, NO TIMEOUT*  
### Usage:
  > SETDIR, dir,  
### Arguments: (1)
  * **dir**:	Direction (degrees)  
  
## YEET:  
Hekkin yeeeeet  
*BLOCKING, has timeout*  
### Usage:
  > YEET, yeet_distance, command_time_out,  
### Arguments: (2)
  * **yeet_distance**:	ARG;
  * **command_time_out**:	Timout for command (seconds)  
### Warnings: (1)  
* Argument yeet_distance is missing a description  
  
## RETRACTYEET:  
Retracts the yeet wheel such that drive wheels are on the ground  
*non-blocking, NO TIMEOUT*  
### Usage:
  > RETRACTYEET,   
  
## INTAKE:  
Turns intake on or off  
*non-blocking, NO TIMEOUT*  
### Usage:
  > INTAKE, intake,  
### Arguments: (1)
  * **intake**:	ARG;  
### Warnings: (1)  
* Argument intake is missing a description  
  
## WOBBLE:  
Turns wobble on or off  
*non-blocking, NO TIMEOUT*  
### Usage:
  > WOBBLE, intake_wobble,  
### Arguments: (1)
  * **intake_wobble**:	ARG;  
### Warnings: (1)  
* Argument intake_wobble is missing a description  
  
## FRONTARM:  
Move front arm to a positon  
*non-blocking, NO TIMEOUT*  
### Usage:
  > FRONTARM, front_lift_state,  
### Arguments: (1)
  * **front_lift_state**:	ARG;  
### Warnings: (1)  
* Argument front_lift_state is missing a description  
  
## BACKARM:  
Move back arm to a positon  
*non-blocking, NO TIMEOUT*  
### Usage:
  > BACKARM, back_lift_state,  
### Arguments: (1)
  * **back_lift_state**:	ARG;  
### Warnings: (1)  
* Argument back_lift_state is missing a description  
  
## TIPBASE:  
Turns back tip latch on or off  
*non-blocking, NO TIMEOUT*  
### Usage:
  > TIPBASE, tip_latch,  
### Arguments: (1)
  * **tip_latch**:	ARG;  
### Warnings: (1)  
* Argument tip_latch is missing a description  
  
## FRONTDROP:  
Turns front base latch on or off  
*non-blocking, NO TIMEOUT*  
### Usage:
  > FRONTDROP, front_latch_on,  
### Arguments: (1)
  * **front_latch_on**:	ARG;  
### Warnings: (1)  
* Argument front_latch_on is missing a description  
  
## BACKDROP:  
Turns back base latch on or off  
*non-blocking, NO TIMEOUT*  
### Usage:
  > BACKDROP, back_latch_on,  
### Arguments: (1)
  * **back_latch_on**:	ARG;  
### Warnings: (1)  
* Argument back_latch_on is missing a description  
  
## SIDEARM:  
Move side arm to a positon  
*non-blocking, NO TIMEOUT*  
### Usage:
  > SIDEARM, pos,  
### Arguments: (1)
  * **pos**:	ARG;  
### Warnings: (1)  
* Argument pos is missing a description  
  
## BRAKE:  
Hold drive in current position  
*BLOCKING, has timeout*  
### Usage:
  > BRAKE, command_time_out,  
### Arguments: (1)
  * **command_time_out**:	Timout for command (seconds)  
  
## ULTRABOOST:  
Give all power to drive  
*non-blocking, NO TIMEOUT*  
### Usage:
  > ULTRABOOST,   
  
## UNBOOST:  
Give all power to drive  
*non-blocking, NO TIMEOUT*  
### Usage:
  > UNBOOST,   
  
## AUTOPARK:  
Autopark the robot  
*BLOCKING, NO TIMEOUT*  
### Usage:
  > AUTOPARK, drive_turn_target,  
### Arguments: (1)
  * **drive_turn_target**:	ARG;  
### Warnings: (1)  
* Argument drive_turn_target is missing a description  
  
## SETTILT:  
Set the tilt of the imu to 0  
*non-blocking, NO TIMEOUT*  
### Usage:
  > SETTILT, imu_sensor.set_pitch(process_entry()),  
### Arguments: (1)
  * **imu_sensor.set_pitch(process_entry())**:	imu_sensor.set_pitch(ARG);  
### Warnings: (2)  
* Argument imu_sensor.set_pitch(process_entry()) is missing a description  
* Command needs to print name: cout << "SETTILT" << endl;  
  
