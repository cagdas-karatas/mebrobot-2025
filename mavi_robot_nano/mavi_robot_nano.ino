#include <Servo.h>

Servo tokat;  // Servo nesnesi oluştur
Servo ceza_tokat;

int out = 2, s0 = 3, s1 = 4, s2 = 5, s3 = 6, top_sensor = 12;

float kirmizi = 0, mavi = 0;
int kirmizi_veriler[4] = { 0, 0, 0, 0 };
int mavi_veriler[4] = { 0, 0, 0, 0 };
boolean rastgele = true;

// TOPLARI TOKATLAYAN VE CEZAYI TOKATLAYAN SERVOLARIN NORMAL DURUMLARI
// ÖZELLİKLE tokat_default TOKATLAMA KODLARINDA DA KULLANILIYOR, BURADAN DEĞİŞTİRİLMESİ ÖNEMLİ
byte tokat_default = 84, ceza_tokat_default = 70;

void setup() {

  tokat.attach(10);
  ceza_tokat.attach(11);
  tokat.write(tokat_default);
  ceza_tokat.write(ceza_tokat_default);

  pinMode(out, INPUT);
  pinMode(top_sensor, INPUT);
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);

  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);

  Serial.begin(9600);
}

void loop() {
  
  /* //Bİ SAĞA Bİ SOLA ATAN KOD
    if(digitalRead(top_sensor)==1)
    {
    if(rastgele)
    {dogru_al();
    }
    else
    {
    rakip_al();
    }
    rastgele=!rastgele;
    }*/

  /* //Bİ SAĞ Bİ SOL TOKATLAMA SERVO TESTİ
    tokat.write(0);
    delay(100);
    tokat.write(90);
    delay(100);
    tokat.write(180);
    delay(100);*/

  //NORMAL KOD
  if (digitalRead(top_sensor) == 1)
  {
    int sonuc = olcum(); //OLCUM YAKLAŞIK 240 MİLİSANİYEDE TAMAMLANIYOR
    if (sonuc > 235 && digitalRead(top_sensor) == 1) //TOKATLAYACAKSAK, OLCUM SIRASINDA TOPUN AĞIZDAN ÇIKMADIĞINI TEYİT ETMELİYİZ
    {
      Serial.print("KIRMIZI: ");
      Serial.println(sonuc);
      dogru_al();
    }
    else if (sonuc < 85 && digitalRead(top_sensor) == 1) //TOKATLAYACAKSAK, OLCUM SIRASINDA TOPUN AĞIZDAN ÇIKMADIĞINI TEYİT ETMELİYİZ
    {
      Serial.print("MAVİ: ");
      Serial.println(sonuc);
      rakip_al();
    }
    else if (sonuc > 150 && sonuc < 200 && digitalRead(top_sensor) == 1) //TOKATLAYACAKSAK, OLCUM SIRASINDA TOPUN AĞIZDAN ÇIKMADIĞINI TEYİT ETMELİYİZ
    {
      Serial.println("CEZA: ");
      Serial.println(sonuc);
      ceza_al();
    }
  }
  else
  {
    //Serial.println("BOŞŞ");
  }


}

void dogru_al() {
  tokat.write(180);
  delay(150);
  tokat.write(tokat_default);
}

void rakip_al() {
  tokat.write(0);
  delay(100);
  tokat.write(tokat_default);
}

void ceza_al() {
  ceza_tokat.write(150);
  delay(120);
  tokat.write(180);
  delay(150);
  tokat.write(tokat_default);
  delay(100);
  ceza_tokat.write(100);
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

void renk_sensor_test(int delayValue) {
  olc();
  Serial.print("KIRMIZI: ");
  Serial.print(kirmizi);
  Serial.print(" MAVİ: ");
  Serial.println(mavi);
  delay(delayValue);
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
