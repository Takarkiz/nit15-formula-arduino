#include <MsTimer2.h>

/*ヘッダーファイルの追加(ミリ秒単位でしてするタイマー)
loop関数を実行中にも周期的に割り込みで実行することができる
*/

int rev_cnt; //rpmに関する電流
int rev; //rpm自体の値
int rev_state,sp_val,sp_cnt_sp_state,spd;  //それぞれのプロパティが何を示すのかを記述する

//それぞれの値を転送用にするために簡略化した変数
int Check;
int voltInt;
int rpmInt;
int waterInt;
int passSequence = 0;
int shiftCheck = 0;

/*passSequenceを求める
passSequence = 
*/

//初回起動時に実行
void setup(){
    Serial.begin(9600);
    MsTimer2::set(250,flash);   //250msごとにflashを実行
    MsTimer2::start();

  //led用のピンモードを設定  
  for(int i = 2;i < 11;i++){
    pinMode(i,OUTPUT);
    digitalWrite(i,LOW);
  }
/*
analogReadに関しては
A2...rmp..1
A3...water...3 
A6...shift...1
A0...volt...3
*/
}
void loop(){

    palseSet();
    
    //水温
    double water = analogRead(A3);
    double temp = water/1024*5;
    //水温を導き出す式
    water = 1.5369*pow(temp,4)-18.468*pow(temp,3)+77.948*pow(temp,2)-160.27*temp+186.45+4.7;
    //整数型に変換
    waterInt = (int)water;
    
    
    //電圧計
    double volt = analogRead(A0);
    //double volt = 13.3;
    volt = volt*5/1024*3;
    voltInt = ((int)(volt * 10));
    //delay(100);
    
    
    //シフトポジション
    double shift = analogRead(A6);
    shift = shift/1024*5;
    
    if (shift >= 0.10 && shift <= 0.50){
        shiftCheck = 1; //1
    }else if(shift >= 2.00 && shift <= 2.35){
        shiftCheck = 2; //2
    }else if (shift >= 2.40 && shift <= 2.70){
        shiftCheck = 3; //3
    }else if (shift >= 3.80 && shift <= 4.20){
        shiftCheck = 4; //4
    }else if (shift >= 4.70 && shift <= 5.00){
        shiftCheck = 5; //5
    }else if (shift >= 1.20 && shift <= 1.50){
      //ニュートラル
        shiftCheck = 6;
    }else{
        shiftCheck = 7; //なんでもない
    }

    double oilPresure = analogRead(A1);
    oilPresure = (oilPresure - 0.5) / 25; //MPaに変換
    int oil = int(oilPresure);

    //それぞれの値を時間を分けて送出させる
    //rpm&shift
    passSequence = 200 + oil;
    Serial.write(passSequence);
    delay(50);

    //waterTemp
    Serial.write(waterInt);
    delay(50);

    //volt
    Serial.write(voltInt);
    delay(50);
}

void flash(){
    
    //電流から回転数を導く式
    rev = rev_cnt / 2 * 4 * 60;
    
    //小回転は誤差として無視する
    if (rev < 250){
        rev = 0;
    }
    
    //rpmの値からシフトするレベルの値を割りあてる
    if (rev > 10500){
        lightupLED(10);
    }else if (rev > 10000){
        lightupLED(9);
    }else if (rev > 9500){
        lightupLED(8);
    }else if (rev > 9000){
        lightupLED(7);
    }else if (rev > 8000){
        lightupLED(6);
    }else if (rev > 7000){
        lightupLED(5);
    }else if (rev > 6000){
        lightupLED(4);
    }else if (rev > 5000){
        lightupLED(3);
    }else if (rev > 4000){
        lightupLED(2);
    }else {
        
    }

}

void lightupLED(int num){

  int j = 2;
  for(j;j <= num;j++){
    digitalWrite(j,HIGH);
  }
  if (j != 11){
    for(j;j <= 1; j++){
      digitalWrite(j,LOW);
    }
  }
}

void palseSet(){
      //パルスの設定
    double rev_val = analogRead(A2);    //回転数の電流の値を読み込んでいる
    
    //回転パルス部
    if (rev_val > 1000 && rev_state == 0){
        rev_state = 1;
        rev_cnt++;
    }
    if (rev_val < 1 && rev_state == 1){
        rev_state = 0;
        rev_cnt++;
    }

}


