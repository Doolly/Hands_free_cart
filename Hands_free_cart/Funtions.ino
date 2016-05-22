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
  int PWM_L = map(leftspeed, -400, 400, -128, 127 );
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
  int PWM_R = map(rightspeed, -400, 400, -128, 127 );
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


int obj_size = 400;
void Follow(int target) {
  int follow_error = PIXY_RCS_CENTER_POS - panLoop.m_pos;

  obj_size += pixy.blocks[target].width * pixy.blocks[target].height;  //Low pass filter 혹시 모를 사이즈 노이즈
  obj_size -= obj_size >> 3;
  Serial.print("obj_size = " + String(obj_size) + "\n");

  int a = map(distance_error, -900, 500, -450, 300);

  int base_speed = constrain(a, -150, 300);

  int speed_differential = (follow_error + (follow_error * base_speed)) >> 8; // follow_error가 500대 값

  int leftspeed = constrain(base_speed + speed_differential, -400, 400);
  int rightspeed = constrain(base_speed - speed_differential, -400, 400);

  LeftMoterCtrl(leftspeed);
  RightMoterCtrl(rightspeed);
}

int32_t toggle = 100L;
void Find() {
  panLoop.m_pos += toggle;

  if ((panLoop.m_pos >= PIXY_RCS_MAX_POS) || (panLoop.m_pos <= PIXY_RCS_MIN_POS)) {
    tiltLoop.m_pos = random(PIXY_RCS_MIN_POS, PIXY_RCS_MAX_POS * 0.8);
    toggle = -toggle;
    delay(5);
  }
  if (toggle < 0) {
    LeftMoterCtrl(200);
    RightMoterCtrl(-200);
    delay(200);
  }
  else {
    LeftMoterCtrl(-300);
    RightMoterCtrl(300);
    delay(300);
  }
  pixy.setServos(panLoop.m_pos, tiltLoop.m_pos);
}

void GetDistance (void) {
  int Raw_D = analogRead(DIS_SENSOR);
  int filtered_distance = Raw_D;
  filtered_distance += Raw_D;
  filtered_distance -= filtered_distance >> 3;
  distance = map (Raw_D, 500, 15, 200, 1500);
  distance_error = distance - distance_throtle;
  Serial.print("filtered_distance = " + String(filtered_distance) + "\n");
  Serial.print("distance = " + String(distance) + "\n");
  Serial.print("distance_error = " + String(distance_error) + "\n");
  //    if (Raw_D > 508 || Raw_D < 102) {
  //      distance = 0;
  //    }
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

