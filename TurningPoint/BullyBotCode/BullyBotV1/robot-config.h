vex::brain Brain;
vex::motor LeftMotor1 = vex::motor(vex::PORT10, vex::gearSetting::ratio18_1, false);
vex::motor LeftMotor2 = vex::motor(vex::PORT18, vex::gearSetting::ratio18_1, false);
vex::motor LeftMotor3 = vex::motor(vex::PORT20, vex::gearSetting::ratio18_1, false);
vex::motor RightMotor1 = vex::motor(vex::PORT4,vex::gearSetting::ratio18_1, true);
vex::motor RightMotor2 = vex::motor(vex::PORT13,vex::gearSetting::ratio18_1, true);
vex::motor RightMotor3 = vex::motor(vex::PORT14,vex::gearSetting::ratio18_1, true);
vex::motor ApexMotor1 = vex::motor(vex::PORT1, vex::gearSetting::ratio18_1, true); //one true one false
vex::motor ApexMotor2 = vex::motor(vex::PORT2, vex::gearSetting::ratio18_1, false);
vex::controller Controller1 = vex::controller();
