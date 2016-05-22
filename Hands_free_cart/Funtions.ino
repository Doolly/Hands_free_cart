void LostTimer(void) { //Mega PWM 9,10 금지
  finder_var++;
}
void InitMoter(void) {
  digitalWrite(L_MOTER_B, LOW);
  digitalWrite(L_MOTER_F, LOW);
  digitalWrite(R_MOTER_B, LOW);
  digitalWrite(R_MOTER_F, LOW);
}

void LeftMoterCtrl(int leftspeed) {
  int PWM_L = map(leftspeed, -400, 400, -255, 255 );
  if (PWM_L >= 0) {
    analogWrite(L_MOTER_F, PWM_L);
    analogWrite(L_MOTER_B, 0);
  }
  else {
    analogWrite(L_MOTER_F, 0);
    analogWrite(L_MOTER_B, -PWM_L);
  }
}

void RightMoterCtrl(int rightspeed) {
  int PWM_R = map(rightspeed, -400, 400, -255, 255 );
  if (PWM_R >= 0) {
    analogWrite(R_MOTER_F, PWM_R);
    analogWrite(R_MOTER_B, 0);
  }
  else {
    analogWrite(R_MOTER_F, 0);
    analogWrite(R_MOTER_B, -PWM_R);
  }
}

int prev_X, prev_Y, prev_Signature;
int Look(int object_cnt) {
  int tracked_obj = 0;
  long max_size = 0;

  for (int i = 0; i < object_cnt; i++)
  {
    if ((prev_Signature == 0) || (pixy.blocks[i].signature == prev_Signature))
    {
      long new_size = pixy.blocks[i].height * pixy.blocks[i].width;
      if (new_size > max_size)
      {
        tracked_obj = i;
        max_size = new_size;
      }
    }
  }
  int pan_error = X_CENTER - pixy.blocks[tracked_obj].x;
  int tilt_error = pixy.blocks[tracked_obj].y - Y_CENTER;

  panLoop.update(pan_error);
  tiltLoop.update(tilt_error);
  pixy.setServos(panLoop.m_pos, tiltLoop.m_pos);

  prev_X = pixy.blocks[tracked_obj].x;
  prev_Y = pixy.blocks[tracked_obj].y;
  prev_Signature = pixy.blocks[tracked_obj].signature;

  return tracked_obj;
}


unsigned long obj_size = 400;
void Follow(int target) {
  int follow_error = PIXY_RCS_CENTER_POS - panLoop.m_pos;

  obj_size += pixy.blocks[target].width * pixy.blocks[target].height;  //Low pass filter 혹시 모를 사이즈 노이즈
  obj_size -= obj_size >> 3;
  Serial.print("obj_size = " + String(obj_size) + "\n"); //1000대 굳
  int base_speed =constrain( -8*sqrt(obj_size) + 440, -200, 400);

  int speed_differential = constrain(follow_error/2.2, -200, 200); // follow_error가 500대 값

  int leftspeed = constrain(base_speed + speed_differential, -400, 400);
  int rightspeed = constrain(base_speed - speed_differential, -400, 400);
  Serial.print("base_speed = " + String(base_speed) + "\n");
  Serial.print("speed_differential = " + String(speed_differential) + "\n");

  LeftMoterCtrl(leftspeed);
  RightMoterCtrl(rightspeed);
}

int32_t toggle = 100L;
void Find() {
  panLoop.m_pos += toggle;

  if ((panLoop.m_pos >= PIXY_RCS_MAX_POS) || (panLoop.m_pos <= PIXY_RCS_MIN_POS)) {
    tiltLoop.m_pos = random(PIXY_RCS_MAX_POS * 0.1, PIXY_RCS_MAX_POS * 0.5);
    toggle = -toggle;
    delay(5);
  }
  if (toggle < 0) {
    LeftMoterCtrl(100);
    RightMoterCtrl(-100);
    delay(200);
  }
  else {
    LeftMoterCtrl(-150);
    RightMoterCtrl(150);
    delay(300);
  }
  pixy.setServos(panLoop.m_pos, tiltLoop.m_pos);
}

void Change_Value_in_Serial() { //new line
  if (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      String part1;
      String part2;

      part1 = command.substring(0, command.indexOf(" "));
      part2 = command.substring(command.indexOf(" ") + 1);

      //      if (part1.equalsIgnoreCase("A_variable"))
      //        A_variable = part2.toInt();
      //
      //      else if (part1.equalsIgnoreCase("B_variable"))
      //        B_variable = part2.toInt();

      command = "";
    }

    else
      command += c;
  }
}

