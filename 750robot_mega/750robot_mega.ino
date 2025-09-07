// Sağ motor pinleri
#define EN_R 2
#define RPWM_R 3
#define LPWM_R 4

// Sol motor pinleri
#define RPWM_L 5
#define LPWM_L 6
#define EN_L 7

#define sol_goz A8
#define sag_goz A9

void setup() {
  pinMode(sol_goz, INPUT);
  pinMode(sag_goz, INPUT);
  
  // Sağ motor pin tanımları
  pinMode(EN_R, OUTPUT);
  pinMode(RPWM_R, OUTPUT);
  pinMode(LPWM_R, OUTPUT);

  // Sol motor pin tanımları
  pinMode(EN_L, OUTPUT);
  pinMode(RPWM_L, OUTPUT);
  pinMode(LPWM_L, OUTPUT);

  // Motor sürücülerini aktif et
  digitalWrite(EN_R, HIGH);
  digitalWrite(EN_L, HIGH);

  Serial.begin(9600);
  basla();
}

void loop() {
  //rastgele();
}

void rastgele()
{
  if(analogRead(sol_goz) < 5)
  {
    sag(200);
    delay(100);
  }
  else if(analogRead(sag_goz) < 5)
  {
    sag(200);
    delay(200);
  }
  else
  {
    ileri(150);
  }
}

void ileri(byte hiz)
{
  analogWrite(RPWM_R, 0);
  analogWrite(LPWM_R, hiz);
  analogWrite(RPWM_L, hiz);
  analogWrite(LPWM_L, 0);
}

void geri(byte hiz)
{
  analogWrite(RPWM_R, hiz);
  analogWrite(LPWM_R, 0);
  analogWrite(RPWM_L, 0);
  analogWrite(LPWM_L, hiz);
}

void sol(byte hiz)
{
  analogWrite(RPWM_R, 0);
  analogWrite(LPWM_R, hiz);
  analogWrite(RPWM_L, 0);
  analogWrite(LPWM_L, hiz);
}

void sag(byte hiz)
{
  analogWrite(RPWM_R, hiz);
  analogWrite(LPWM_R, 0);
  analogWrite(RPWM_L, hiz);
  analogWrite(LPWM_L, 0);
}

void dur(byte guc)
{
  analogWrite(RPWM_R, guc);
  analogWrite(LPWM_R, guc);
  analogWrite(RPWM_L, guc);
  analogWrite(LPWM_L, guc);
}

void basla()
{
  // İLERİ GİT (her iki motor ileri yönde döner)
  ileri(255); // ileri(byte hız)

  delay(1000);  // 1 saniye ileri git

  // DUR
  dur(100); //dur(byte guc) -> guc ne kadar yüksekse o kadar sert fren yapar.

  //durduğunu anla
  delay(1000);
}
