#include <Arduino.h>
#include "ManageStone.h"
#include "Constants.h"

ManageStone::ManageStone(Robot const* robot):
  my_PILLAR_DISTANCE(Robot::instance()->PILLAR_DISTANCE), my_TEAM(Robot::instance()->TEAM)
{}

//stone 2, 3, 4,  same direction 
//stone 1 diff direction
//only translate all the way after picking up 1st stone, 
//no translating after that, only moving up from 6->9 after stone 2, 9->12 after stone 3
//dropInStorage right after picking up the stone except for 1st stone 

void ManageStone::collectStone(){
  Robot::instance()->stoneNumber++;
  moveArmToPillar();
  Robot::instance()->clawServo.write(180); //deploy claw to get stone 
  pwm_start(ARM_MOTOR_UP, CLOCK_FQ, MAX_SPEED, MAX_SPEED, 0);
  delay(2000);
  dropInStorage(); 
  if(Robot::instance()->direction){
    pwm_start(ARM_MOTOR_LEFT, CLOCK_FQ, MAX_SPEED, MAX_SPEED, 0);
    while(true){
      if(multi(0,1,0)==HIGH){
        pwm_start(ARM_MOTOR_LEFT, CLOCK_FQ, MAX_SPEED, 0, 0);
        return;
      }
    }
  }
}

void ManageStone::dropInStorage(){
  switch(Robot::instance()->stoneNumber){
    case 1: 
      Robot::instance()->armServo.write(30); //middle left or right 
      //check if middle can be accessed by either side 
  }
  Robot::instance()->clawServo.write(0); //open claw
  if(my_TEAM){
    Robot::instance()->armServo.write(180); //default side 
  }
  else{
    Robot::instance()->armServo.write(0);
  }
  return;
}

void ManageStone::moveArmToPillar(){
  if(Robot::instance()->direction){
      pwm_start(ARM_MOTOR_RIGHT, CLOCK_FQ, MAX_SPEED, MAX_SPEED, 0);
  }
  else{
      pwm_start(ARM_MOTOR_LEFT, CLOCK_FQ, MAX_SPEED, MAX_SPEED, 0);
  }
  while (true){
    if(readSonar()<=my_PILLAR_DISTANCE || multi(0,0,1) == HIGH){ //multi(0,0,1) = limit switch
      pwm_start(ARM_MOTOR_LEFT, CLOCK_FQ, MAX_SPEED, 0, 0);
      pwm_start(ARM_MOTOR_RIGHT, CLOCK_FQ, MAX_SPEED, 0, 0);   
      return; 
    }
  }
}

void ManageStone::turnClaw(){
  if(Robot::instance()->direction){
    Robot::instance()->L_GauntletServo.write(180);
    Robot::instance()->R_GauntletServo.write(180);
    Robot::instance()->armServo.write(180); //180 degrees corresponds to right side
    Robot::instance()->L_GauntletServo.write(0);
    Robot::instance()->R_GauntletServo.write(0);
  }
  else{
    Robot::instance()->L_GauntletServo.write(180);
    Robot::instance()->R_GauntletServo.write(180);
    Robot::instance()->armServo.write(0);
    Robot::instance()->L_GauntletServo.write(0);
    Robot::instance()->R_GauntletServo.write(0);
  }
}

void ManageStone::raiseClaw(){ //backup function using sonar 
  if(Robot::instance()->collisionNumber != 0){
    pwm_start(ARM_MOTOR_UP, CLOCK_FQ, MAX_SPEED, MAX_SPEED, 0);
    while(true){
      if(readSonar()>=my_PILLAR_DISTANCE+3){ //check that sonar doesnt become negative
        pwm_start(ARM_MOTOR_UP, CLOCK_FQ, MAX_SPEED, 0, 0);
        return;
      }
    }
  }
}

int ManageStone::readSonar(){ //inches
  digitalWrite(ARM_SONAR_TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(ARM_SONAR_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(ARM_SONAR_TRIGGER, LOW);
  return pulseIn(ARM_SONAR_ECHO, HIGH)*(0.034/2);
}

bool ManageStone::multi(bool C, bool B, bool A) {
  digitalWrite(MULTIPLEX_A, A);
  digitalWrite(MULTIPLEX_B, B);
  digitalWrite(MULTIPLEX_C, C); 
  return digitalRead(MULTIPLEX_OUT);
}

