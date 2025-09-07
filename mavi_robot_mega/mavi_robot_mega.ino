#include<Servo.h>
Servo bizim;
Servo rakip;
Servo ceza;

// Sağ motor pinleri
#define EN_R 2
#define RPWM_R 3
#define LPWM_R 4

// Sol motor pinleri
#define RPWM_L 5
#define LPWM_L 6
#define EN_L 7

#define sol_goz 22
#define sag_goz 24

#define s0 52
#define s1 50
#define s2 46
#define s3 48
#define out 44

#define rgb 42

float kirmizi = 0, mavi = 0;
int kirmizi_veriler[4] = { 0, 0, 0, 0 };
int mavi_veriler[4] = { 0, 0, 0, 0 };

void setup() {
  // SERVO TANIMLARI
  bizim.attach(A7);
  rakip.attach(A8);
  ceza.attach(A9);
  bizim.write(180);
  rakip.write(0);
  ceza.write(0);

  // MZ80 TANIMLARI
  pinMode(sol_goz, INPUT);
  pinMode(sag_goz, INPUT);

  //RENK SENSÖRÜ TANIMLARI
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(out, INPUT);
  
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

  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);

  Serial.begin(9600);
  //basla();
}

void loop() {
  olcum();
  Serial.print("KIRMIZI: ");
  Serial.print(kirmizi);
  Serial.print(" MAVİ: ");
  Serial.println(mavi);
  delay(200);
}

void goz_okuma()
{
  Serial.print("Sol göz: ");
  Serial.println(digitalRead(sol_goz));
  Serial.print("Saü göz: ");
  Serial.println(digitalRead(sag_goz));
  Serial.print("\n\n\n");
  delay(300);
}

void rastgele()
{
  if(digitalRead(sol_goz) == 0)
  {
    sag(200);
    delay(100);
  }
  else if(digitalRead(sag_goz) == 0)
  {
    sag(200);
    delay(150);
  }
  else
  {
    ileri(80);
  }
}

void ileri(byte hiz)
{
  analogWrite(RPWM_R, hiz);
  analogWrite(LPWM_R, 0);
  analogWrite(RPWM_L, 0);
  analogWrite(LPWM_L, hiz);
}

void geri(byte hiz)
{
  analogWrite(RPWM_R, 0);
  analogWrite(LPWM_R, hiz);
  analogWrite(RPWM_L, hiz);
  analogWrite(LPWM_L, 0);
}

void sol(byte hiz)
{
  analogWrite(RPWM_R, hiz);
  analogWrite(LPWM_R, 0);
  analogWrite(RPWM_L, hiz);
  analogWrite(LPWM_L, 0);
}

void sag(byte hiz)
{
  analogWrite(RPWM_R, 0);
  analogWrite(LPWM_R, hiz);
  analogWrite(RPWM_L, 0);
  analogWrite(LPWM_L, hiz);
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


float olcum() {
  //verileri güncelliyoruz
  for (int i = 0; i < 4; i++) {
    olc();
    kirmizi_veriler[i] = kirmizi;
    mavi_veriler[i] = mavi;
  }

  // 6 TANE VERİ ALIP İÇELERİNDEN TUTARLI HALE GETİRİYORUZ
  // ÖRNEK : 64,65,68,97 ÖLÇMÜŞ OLALIM -> 67 GİBİ BİR SONUÇ DÖNDÜRÜYOR ANLIK DALGALANMALARDAN ETKİLENMEMİŞ OLUYORUZ
  kirmizi = stabilSonucuBul(kirmizi_veriler, 4);
  mavi = stabilSonucuBul(mavi_veriler, 4);
  
  float sonuc = ((float)mavi / (float)kirmizi) * 100;
  return sonuc;
}

void olc() {
  //RENK SENSÖRÜ KIRMIZI FİLTRE AYARI
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  delay(10);
  kirmizi = pulseIn(out, LOW);

  //RENK SENSÖRÜ MAVİ FİLTRE AYARI
  digitalWrite(s2, LOW);
  digitalWrite(s3, HIGH);
  delay(10);
  mavi = pulseIn(out, LOW);
}





// ******************************Ortalama hesaplayan fonksiyon*****************************
double ortalamaHesapla(const int arr[], int size) {
  double sum = 0;
  for (int i = 0; i < size; i++) {
    sum += arr[i];
  }
  return sum / size;
}

// Standart sapmayı hesaplayan fonksiyon
double standartSapmaHesapla(const int arr[], int size, double mean) {
  double sum = 0;
  for (int i = 0; i < size; i++) {
    sum += pow(arr[i] - mean, 2);
  }
  return sqrt(sum / size);
}

// Uç değerleri filtreleyerek en istikrarlı değeri bul
int stabilSonucuBul(const int arr[], int size) {
  // Dizinin ortalamasını hesapla
  double mean = ortalamaHesapla(arr, size);

  // Dizinin standart sapmasını hesapla
  double stdDev = standartSapmaHesapla(arr, size, mean);

  // Standart sapmanın 1.5 katından daha uzak olan değerleri göz ardı et
  const double threshold = 1.5 * stdDev;

  // Filtrelenmiş dizinin ortalamasını ve en yakın değeri bul
  double filteredSum = 0;
  int filteredCount = 0;
  int closestValue = arr[0];
  double minDifference = 1e6;  // çok büyük bir sayı kullanıyoruz

  for (int i = 0; i < size; i++) {
    if (fabs(arr[i] - mean) <= threshold) {
      filteredSum += arr[i];
      filteredCount++;

      // Ortalamaya en yakın değeri bl
      double diff = fabs(arr[i] - mean);
      if (diff < minDifference) {
        minDifference = diff;
        closestValue = arr[i];
      }
    }
  }

  return closestValue;
}
