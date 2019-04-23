
void runAuton() {                         //delay(n) pauses robot to prevent inertial interference from previous move
    armAutonSeek = 0;                     //Seek 0. This should essentially do nothing since it's the beginning of auton
    
    driveForward(-377,8000);              //Roll back. Passes ball to BallBot intake
    delay(1200);
    driveForward(870,8000);               //Forward to corner
    delay(1000);
    
    //200deg turn = 90 deg irl
    turnMotorsAt(-300*colorMultiplier);   //Make 135 degree turn so claw faces platforms
    delay(1000);
    driveForward(-100,5000);              //Back in a bit further
    turnMotorsAt(120*colorMultiplier);    //Turn 45 deg, so claw faces opponent's side.
    delay(1000);
    driveForward(-350,8000);              //Slams backwards into wall to self-align
    delay(1000);
    driveForward(1000,9900);              //Engage w/ cap
    driveMotorsAt(0);
    
    delay(1000);
    
    armAutonSeek = POLE_HEIGHT_DEGS + 100; //Raise arm to just above pole
    delay(1200);
    turnMotorsAt(120*colorMultiplier);     //Turn towards the pole
    flipCap();                   //Turn the cap over 180 deg
    delay(500);
    armAutonSeek = POLE_HEIGHT_DEGS -200   //Lower cap onto pole
    delay(500);
    
    driveForward(-304,8000);           //Back out of cap
    delay(500);
    armAutonSeek = 0;                      //Put arm back down
    delay(500);
    
    turnMotorsAt(-120);               //Turn ccw so that claw faces other cap
    delay(500);
    driveForward(450,12000);           //Forward into cap
    delay(500);
    
    turnMotorsAt(170);               //Turn cw to knock ball out from under cap
    delay(500);
    
    driveForward(-1100,12000);           //Drive back into wall, slamming into it to re-align
    delay(1000);
    driveForward(350,12000);           //Drive out towards opposing side
    
    delay(1000);
    turnMotorsAt(230);               //Turn cw so claw faces back wall
    
    delay(500);
    
    driveForward(-800,12000);           //Drive backwards to that robot is to the left of the platform
    delay(1000);
    
    armAutonSeek = 377;               //Arm up so that it doesn't jam under the platform
    turnMotorsAt(-200);               //Slam into wall to align
    delay(1000);
    
    driveForward(400,3000);           //Slowly drive up to platform, gently touching it to align again
    driveMotorsAt(0);               //Stop completely
    
    delay(1000);
    
    driveForward(1200,12000);           //Run motor for a long time to get onto platform
    driveMotorsAt(0);
}
