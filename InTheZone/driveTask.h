//
//  drive Task.c
//
//
//  Created by Sam Burton on 21/02/2018.
//

#define USER 0
#define DISTANCE -1
#define RIGHTWHITELINETRIGGER 1000
#define LEFTWHITELINETRIGGER 1000
#define NONE -1
#define TURN 0
#define RWHITE -69420
#define RBLACK -42069
#define LWHITE -420
#define LBLACK -69
#define	CSCALE1 			-1							//Clockwise scale adjustments to counteract rotation errors
#define	ASCALE1 			-1							//Anti-clockwise scale adjustments to counteract rotation errors
#define	CSCALE2				-1						//Clockwise scale adjustments to counteract rotation errors
#define	ASCALE2 			-1							//Anti-clockwise scale adjustments to counteract rotation errors

float ticksPerTile = 14000;

bool nextCommand = true;

float breakRate = 3;        // how hard to break
float breakTime = 0.2;      // how long to break
float minForward = 0.2;     // minimum speed ratio for slow down to destination
float turnRate = 2;
int driveStartTime = 0;
float driveTime = USER;
int driveSpeed = 0;
int driveTimeOut = 0;
bool breakDrive = false;
int faceDirection = 0;
float distanceToDrive = 0;
float currentDirection = 0;                // this will be updated by gyroFix task to represent direction robot is facing (0-360)
int turnAccepted = 20;                  // motor value for which turns will terminate

task newDrive(void* arg);

void initAll()
{
    // cut this into initAll()
    //resetGyro();
    //StartTask(readGyro);
    StartTask(newDrive);
}

// Call this functon in autonomous
// speed : power for the motors, -1 is backwards ... pass 0 for turn, direction is where to face, duration is time out
// breaking is whether or not to break
// direction : direction to drive in
// duration : time if > 0
// distance : distance to drive
void driveNoTimeout(int speed, bool breaking, int direction, int duration)                  // works for time/whitelines, no time out
{
    nextCommand = false;
    driveTime = duration;
    breakDrive = breaking;
    driveSpeed = speed;
    faceDirection = direction;
    driveStartTime = chTimeNow();
    driveTimeOut = -1;

    waitForCommand();
}


void driveTimeout(int speed, bool breaking, int direction, int duration, float timeOut)                  // works for time/whitelines with timeout
{
    nextCommand = false;
    if (duration>0)
      driveTime = duration;
      else
      driveTime = duration;
    breakDrive = breaking;
    driveSpeed = speed;
    vex_printf("Speed: %d\n", speed);
    faceDirection = direction;
    driveStartTime = chTimeNow();

    if (timeOut > 0)
    {
        driveTimeOut = timeOut * 1000;
    }
    else
    {
        driveTimeOut = -1;

    }
    waitForCommand();
}

// distance is relative, but since we never reset encoder values, we can easily add an 'ABSPOS' as an option for driveTime
// for ABSPOS, we know where we started, we know where we are, and we know where we want to be
// encoder values haven't changed,
// so we take the average encoder value (distance we are from the start), and subtract from where we want to be, and set that as distanceToDrive
// if distanceToDrive < 0, speed = speed * -1

void driveDistance(int speed, bool breaking, int direction, float duration, float distance, float timeOut)    // works for distance (encoder/sonar) with time out
{
    nextCommand = false;
    driveTime = duration;
    breakDrive = breaking;
    driveSpeed = speed;
    faceDirection = direction;

    if (duration == DISTANCE)
    {
        int leftDriveDist = -vexSensorValueGet(S_DRIVE_ENC_LEFT);
        int rightDriveDist = vexSensorValueGet(S_DRIVE_ENC_RIGHT);     // one of these should be negative, such that forwards is positive
        int averageDist = (leftDriveDist + rightDriveDist)/2;

                            // where we are + how far we want to move, moving a negative distance if we want to go backwards
        distanceToDrive = averageDist + (distance*TICKS_PER_TILE*speed/abs(speed));     // distance should be added if going forwards, subtracted if backwards
        vex_printf("Average distance: %d, distance to drive: %f\n", averageDist, distanceToDrive);
    }

    driveStartTime = chTimeNow();

    if (timeOut > 0)
    {
        driveTimeOut = timeOut * 1000;
    }
    else
    {
        driveTimeOut = -1;

    }
    waitForCommand();
}


bool getRightWhiteLine()
{
    bool yup = (vexSensorValueGet(S_RIGHT_WHITE) <= RIGHTWHITELINETRIGGER);
    return yup;
}

bool getLeftWhiteLine()
{
    bool yup = (vexSensorValueGet(S_LEFT_WHITE) <= LEFTWHITELINETRIGGER);
    return yup;
}

void waitForCommand()
{
    while (!nextCommand)
    {
        wait1Msec(10);
    }
}


// single task to do all of the drive
// should work in user and auton
// should also work just for turning
// use functions above to set the flags for the speed, duration, etc...

task newDrive(void *arg) {
    vexTaskRegister("newDrive");

    int ld = 0;
    int rd = 0;

    int forward = 0;
    float turn  =  0;

    ld = VALLEY(forward + (int)turn, 25, 127);
    rd = VALLEY(forward - (int)turn, 25, 127);

    while (true)
    {
        if(vexControllerGet(J_TEST_AUTON)){
          driveTime = USER;
          nextCommand = true;
        }

        currentDirection = 0;//vexGyroGet();
        // user inputs... will be overridden in autonomous mode
        forward = VALLEY(vexControllerGet(J_DRIVE), 20, 127);
        turn  =  (-0.4)*VALLEY(vexControllerGet(J_TURN), 20, 127);
        turn = turn + abs(forward)*turn;
        if(turn > 0){
          turn = turn > 127 ? 127 : turn;                                     //faster we drive, more we turn
        }
        if(turn < 0){
          turn = turn < -127 ? -127 : turn;
        }

        if (driveTime != USER)                  //auton takes over
        {
            int doneDriving = false;
            forward = driveSpeed;
            turn = 0;// (faceDirection-(currentDirection))/turnRate;      // should lerp to face correct direction, should keep facing that direction, even if the bot is moving

            // put better code for turning here / keeping straight
            // can use either gyro or encoders to change 'turn' to keep the bot driving straight
            //
            //
            //
            //
            //



            // conditions for terminating movement

            //regular time
            if (driveTime > 0 && (int)(chTimeNow())-driveStartTime > driveTime*1000)
            {
                doneDriving = true;
            }


            // distance
            if (driveTime == DISTANCE)
            {

                // put slew/slow down code here

                // so will start slowing down when we get within one tile of destination

                if (forward > 127) forward = 127;
                if (forward < -127) forward = -127;

                int leftDriveDist = -vexSensorValueGet(S_DRIVE_ENC_LEFT);
                int rightDriveDist = vexSensorValueGet(S_DRIVE_ENC_RIGHT);     // one of these should be negative, such that forwards is positive
                int averageDist = (leftDriveDist + rightDriveDist)/2;
                forward = forward * abs(averageDist - distanceToDrive)/(1*ticksPerTile) + (forward*minForward);

                if (driveSpeed > 0)
                {
                    if (averageDist > distanceToDrive)          // forwards
                    {
                        doneDriving = true;
                    }
                }
                else
                {
                    if (averageDist < distanceToDrive)          // backwards
                    {
                        doneDriving = true;
                    }
                }
            }


            // white line sensors
            if (driveTime == RWHITE)
            {
                doneDriving = getRightWhiteLine();
            }
            if (driveTime == LWHITE)
            {
                doneDriving = getLeftWhiteLine();
            }
            if (driveTime == RBLACK)
            {
                doneDriving = !getRightWhiteLine();
            }
            if (driveTime == LBLACK)
            {
                doneDriving = !getLeftWhiteLine();
            }


            // time out
            if (driveTime != USER)
            {
                if ( (int)(chTimeNow())-driveStartTime > driveTimeOut && driveTimeOut > 0 )
                {
                    doneDriving = true;
                }
            }


            // turn
            if (driveSpeed == 0)                    // must be turning
            {
                if (abs(turn) < turnAccepted)       // if the robot is so close to the correct angle that it has stoped trying to rotate, then stop the turn
                {
                    doneDriving = true;
                }
            }


            // now actually stop the drive
            if (doneDriving)
            {
                if (breakDrive)       // if we want to break...
                {
                    driveSpeed = -forward/breakRate;        // drive back at some multiple of current speed
                    driveTime = breakTime;           // for a specific time
                    breakDrive = false;                     // don't break nextime
                    driveStartTime = chTimeNow();           // record time to know when to stop
                }
                else
                {
                    driveSpeed = 0;                         // stop driving
                    driveTime = USER;
                    nextCommand = true;                     // ready for next command
                    forward = 0;
                    turn = 0;
                }
            }
        }

        if (driveTime == USER)
        {
            ld = VALLEY(forward + (int)turn, 25, 127);
            rd = VALLEY(forward - (int)turn, 25, 127);
        }
        else
        {
            ld = forward + (int)turn;       // lets us run very slowly for precise turns, etc.
            rd = forward - (int)turn;
        }

        SetMotor(M_DRIVE_LEFT_F, ld);      // finally set drive motor values once in the entire code!
        SetMotor(M_DRIVE_LEFT_B, ld);
        SetMotor(M_DRIVE_RIGHT_F, rd);
        SetMotor(M_DRIVE_RIGHT_B, rd);

        wait1Msec(10);
    }
}
