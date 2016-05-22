/*------ Headers to Include ------*/
#include <SPI.h>
#include <Pixy.h>
#include <MsTimer2.h>
#include <SoftwareSerial.h>
/*------ Pin Define ------*/
#define L_MOTER_F 5
#define L_MOTER_B 6
#define R_MOTER_F 9
#define R_MOTER_B 10
#define DIS_SENSOR A1
/*------ Value Define ------*/
#define X_CENTER ((PIXY_MAX_X-PIXY_MIN_X)/2)
#define Y_CENTER ((PIXY_MAX_Y-PIXY_MIN_Y)/2)

//#define RCS_MIN_POS 0L            // RCS -> RC-servo values
//#define RCS_MAX_POS 1000L
//#define RCS_CENTER_POS ((RCS_MAX_POS-RCS_MIN_POS)/2)
/*------ Objects ------*/
Pixy pixy;
SoftwareSerial BTSerial(2, 3); //RX, TX
/*------ Functions ------*/
void LostTimer(void);
void LeftMoterCtrl(int leftspeed);
void RightMoterCtrl(int rightspeed);
int Look(int object_cnt);
void Follow(int target);
void Find();
void GetDistance (void);
void Change_Value_in_Serial();
/*------ Global Variables ------*/
const int distance_throtle = 70;
int distance;
int distance_error;
int Raw_D;
int PWM_L;
int PWM_R;
int mode;
int target;
int object_cnt;
int finder_var;
String command; //debuging용

void setup() {
  pinMode(L_MOTER_B, OUTPUT);
  pinMode(R_MOTER_F, OUTPUT);
  pinMode(L_MOTER_B, OUTPUT);
  pinMode(R_MOTER_F, OUTPUT);
  pinMode(DIS_SENSOR, INPUT);
  pixy.init();
  InitMoter();
  MsTimer2::set(100, LostTimer);
  MsTimer2::start();
  Serial.begin(9600);
  Serial.println("Lets go");
  BTSerial.begin(9600);
}

void loop() {
  //  while (mode == 0) { //자동모드
  //
  //    object_cnt = pixy.getBlocks();
  //
  //    if (object_cnt != 0 ) {
  //      target = Look(object_cnt);
  //      Follow(target);
  //      finder_var = 0;
  //    }
  //    if (distance < 20) { //장애물 감지 정지
  //      LeftMoterCtrl(0);
  //      RightMoterCtrl(0);
  //    }
  //    else if (object_cnt = 0 && finder_var > 10) { //일정시간 이상 타겟이 없다면
  //      Find();
  //    }
  //  }
  //
  //  while (mode == 1) { //수동모드
  //
  //  }

  pixy.setServos(500L, 160L);
  GetDistance ();
  delay(100);
}
