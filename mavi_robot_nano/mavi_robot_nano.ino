#include <Servo.h>

Servo tokat;  // Servo nesnesi oluştur
Servo ceza_tokat;

// HABERLEŞME
#define sayac_sinyal A0
#define ceza_sinyal A1
#define kilit_bildirim A2
#define ceza_tokatla_bildirim A3
#define otur_sinyal A4

#define KIRMIZI 1
#define MAVI 0

boolean baslangic_ceza_aldik_mi = false;

byte out = 2, s0 = 3, s1 = 4, s2 = 5, s3 = 6, top_sensor = 12, bolge_switch = 8, bolge = 0;

float kirmizi = 0, mavi = 0;
int kirmizi_veriler[4] = { 0, 0, 0, 0 };
int mavi_veriler[4] = { 0, 0, 0, 0 };

int kirmizi_alt_limit = 270, mavi_ust_limit = 75, ceza_alt_limit = 150, ceza_ust_limit = 240;

// TOPLARI TOKATLAYAN VE CEZAYI TOKATLAYAN SERVOLARIN NORMAL DURUMLARI
// ÖZELLİKLE tokat_default TOKATLAMA KODLARINDA DA KULLANILIYOR, BURADAN DEĞİŞTİRİLMESİ ÖNEMLİ
byte tokat_default = 90, ceza_tokat_default = 70;
byte sayac = 0;
byte sayac_ust_limit = 3;

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

  //HABERLEŞME PİNLERİ
  pinMode(sayac_sinyal, OUTPUT);
  pinMode(ceza_sinyal, OUTPUT);
  pinMode(otur_sinyal, OUTPUT);
  pinMode(kilit_bildirim, INPUT);
  pinMode(ceza_tokatla_bildirim, INPUT);
  pinMode(bolge_switch, INPUT);

  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);

  bolge = digitalRead(bolge_switch);

  Serial.begin(9600);
}

void loop() {
  baslangic_kod();
  //otur();
  if (baslangic_ceza_aldik_mi == true)
  {
    ceza_birak();
  }
  unsigned long firstMillis = millis();
  while( (millis() - firstMillis) < 90000)
  {
    normal_kod();
  }
  bizim_topu_birak();
  while(1)
  {
    tek_tek_topla();
  }

}

void normal_kod()
{
  //NORMAL KOD
  if (digitalRead(top_sensor) == 1)
  {
    int sonuc = olcum(); //OLCUM YAKLAŞIK 240 MİLİSANİYEDE TAMAMLANIYOR
    if (sonuc > kirmizi_alt_limit && digitalRead(top_sensor) == 1) //TOKATLAYACAKSAK, OLCUM SIRASINDA TOPUN AĞIZDAN ÇIKMADIĞINI TEYİT ETMELİYİZ
    {
      //Serial.print("KIRMIZI: ");
      //Serial.println(sonuc);

      if (bolge == KIRMIZI)
      {
        dogru_al();
        sayac++;
      }
      else
      {
        rakip_al();
      }

      if (sayac == sayac_ust_limit)
      {
        bizim_topu_birak();
        sayac = 0;
      }

    }
    else if (sonuc < mavi_ust_limit && digitalRead(top_sensor) == 1) //TOKATLAYACAKSAK, OLCUM SIRASINDA TOPUN AĞIZDAN ÇIKMADIĞINI TEYİT ETMELİYİZ
    {
      //Serial.print("MAVİ: ");
      //Serial.println(sonuc);

      if (bolge == MAVI)
      {
        dogru_al();
        sayac++;
      }
      else
      {
        rakip_al();
      }

      if (sayac == sayac_ust_limit)
      {
        bizim_topu_birak();
        sayac = 0;
      }

    }
    else if (sonuc > ceza_alt_limit && sonuc < ceza_ust_limit && digitalRead(top_sensor) == 1) //TOKATLAYACAKSAK, OLCUM SIRASINDA TOPUN AĞIZDAN ÇIKMADIĞINI TEYİT ETMELİYİZ
    {
      //Serial.print("CEZA: ");
      //Serial.println(sonuc);

      ceza_al();
      ceza_birak();
    }
    else
    {
      //Serial.print("KARARSIZ: ");
      //Serial.println(sonuc);
    }
  }
  else
  {
    //Serial.println("BOŞŞ");
  }
}

void baslangic_kod()
{
  while (1)
  {
    //NORMAL KOD
    if (digitalRead(top_sensor) == 1)
    {
      int sonuc = olcum(); //OLCUM YAKLAŞIK 240 MİLİSANİYEDE TAMAMLANIYOR
      if (sonuc > kirmizi_alt_limit && digitalRead(top_sensor) == 1) //TOKATLAYACAKSAK, OLCUM SIRASINDA TOPUN AĞIZDAN ÇIKMADIĞINI TEYİT ETMELİYİZ
      {
        //Serial.print("KIRMIZI: ");
        //Serial.println(sonuc);

        if (bolge == KIRMIZI)
        {
          dogru_al();
          baslangic_birak();
          break;
        }
        else
        {
          rakip_al();
        }

      }
      else if (sonuc < mavi_üst_limit && digitalRead(top_sensor) == 1) //TOKATLAYACAKSAK, OLCUM SIRASINDA TOPUN AĞIZDAN ÇIKMADIĞINI TEYİT ETMELİYİZ
      {
        //Serial.print("MAVİ: ");
        //Serial.println(sonuc);

        if (bolge == MAVI)
        {
          dogru_al();
          baslangic_birak();
          break;
        }
        else
        {
          rakip_al();
        }

      }
      else if (sonuc > ceza_alt_limit && sonuc < ceza_ust_limit && digitalRead(top_sensor) == 1) //TOKATLAYACAKSAK, OLCUM SIRASINDA TOPUN AĞIZDAN ÇIKMADIĞINI TEYİT ETMELİYİZ
      {
        //Serial.print("CEZA: ");
        //Serial.println(sonuc);

        ceza_al();
        baslangic_ceza_aldik_mi = true;
      }
      else
      {
        //Serial.print("KARARSIZ: ");
        //Serial.println(sonuc);
      }
    }
    else
    {
      //Serial.println("BOŞŞ");
    }
  }
}

void tek_tek_topla()
{
  //TEK TEK BIRAKAN KOD
  if (digitalRead(top_sensor) == 1)
  {
    int sonuc = olcum(); //OLCUM YAKLAŞIK 240 MİLİSANİYEDE TAMAMLANIYOR
    if (sonuc > kirmizi_alt_limit && digitalRead(top_sensor) == 1) //TOKATLAYACAKSAK, OLCUM SIRASINDA TOPUN AĞIZDAN ÇIKMADIĞINI TEYİT ETMELİYİZ
    {
      //Serial.print("KIRMIZI: ");
      //Serial.println(sonuc);

      if (bolge == KIRMIZI)
      {
        dogru_al();
        bizim_topu_birak();
      }
      else
      {
        rakip_al();
      }

    }
    else if (sonuc < mavi_üst_limit && digitalRead(top_sensor) == 1) //TOKATLAYACAKSAK, OLCUM SIRASINDA TOPUN AĞIZDAN ÇIKMADIĞINI TEYİT ETMELİYİZ
    {
      //Serial.print("MAVİ: ");
      //Serial.println(sonuc);

      if (bolge == MAVI)
      {
        dogru_al();
        bizim_topu_birak();
      }
      else
      {
        rakip_al();
      }

    }
    else if (sonuc > ceza_alt_limit && sonuc < ceza_ust_limit && digitalRead(top_sensor) == 1) //TOKATLAYACAKSAK, OLCUM SIRASINDA TOPUN AĞIZDAN ÇIKMADIĞINI TEYİT ETMELİYİZ
    {
      //Serial.print("CEZA: ");
      //Serial.println(sonuc);

      ceza_al();
      ceza_birak();
    }
    else
    {
      //Serial.print("KARARSIZ: ");
      //Serial.println(sonuc);
    }
  }
  else
  {
    //Serial.println("BOŞŞ");
  }
}

void sadece_ayikla()
{
    //NORMAL KOD
    if (digitalRead(top_sensor) == 1)
    {
      int sonuc = olcum(); //OLCUM YAKLAŞIK 240 MİLİSANİYEDE TAMAMLANIYOR
      if (sonuc > kirmizi_alt_limit && digitalRead(top_sensor) == 1) //TOKATLAYACAKSAK, OLCUM SIRASINDA TOPUN AĞIZDAN ÇIKMADIĞINI TEYİT ETMELİYİZ
      {
        Serial.print("KIRMIZI: ");
        Serial.println(sonuc);

        if (bolge == KIRMIZI)
        {
          dogru_al();
        }
        else
        {
          rakip_al();
        }

      }
      else if (sonuc < mavi_üst_limit && digitalRead(top_sensor) == 1) //TOKATLAYACAKSAK, OLCUM SIRASINDA TOPUN AĞIZDAN ÇIKMADIĞINI TEYİT ETMELİYİZ
      {
        Serial.print("MAVİ: ");
        Serial.println(sonuc);

        if (bolge == MAVI)
        {
          dogru_al();
        }
        else
        {
          rakip_al();
        }

      }
      else if (sonuc > ceza_alt_limit && sonuc < ceza_ust_limit && digitalRead(top_sensor) == 1) //TOKATLAYACAKSAK, OLCUM SIRASINDA TOPUN AĞIZDAN ÇIKMADIĞINI TEYİT ETMELİYİZ
      {
        Serial.print("CEZA: ");
        Serial.println(sonuc);

        ceza_al();
        baslangic_ceza_aldik_mi = true;
      }
      else
      {
        Serial.print("KARARSIZ: ");
        Serial.println(sonuc);
      }
    }
    else
    {
      //Serial.println("BOŞŞ");
    }
}

void dogru_al() {
  tokat.write(180);
  delay(120);
  tokat.write(tokat_default);
}

void rakip_al() {
  tokat.write(0);
  delay(120);
  tokat.write(tokat_default);
}

void ceza_al() {
  ceza_tokat.write(ceza_tokat_default + 80); // CEZA KAPISINI AÇ
  delay(120);
  tokat.write(180);
  delay(150);
  tokat.write(tokat_default);
  delay(100);
  ceza_tokat.write(ceza_tokat_deafult + 30); // CEZA KAPISINI SIKIŞTIR
}

void bizim_topu_birak()
{
  digitalWrite(sayac_sinyal, HIGH);
  delay(2000);
  while (digitalRead(kilit_bildirim) == 1) {}
  digitalWrite(sayac_sinyal, LOW);
}

void baslangic_birak()
{
  digitalWrite(sayac_sinyal, HIGH);
  delay(2000);
  while (digitalRead(kilit_bildirim) == 1) 
  {
    sadece_ayikla();
  }
  digitalWrite(sayac_sinyal, LOW);
}

void ceza_birak()
{
  digitalWrite(ceza_sinyal, HIGH);
  delay(4000);
  while (digitalRead(kilit_bildirim) == 1)
  {
    if (digitalRead(ceza_tokatla_bildirim) == 1)
    {
      ceza_tokat.write(ceza_tokat_default + 40); // 10 DERECE GERİ SALINIM
      delay(50);
      ceza_tokat.write(ceza_tokat_default - 50); // CEZAYI DIŞARI AT
      delay(200);
      ceza_tokat.write(ceza_tokat_default);
      delay(400);
    }
  }
  digitalWrite(ceza_sinyal, LOW);
}

void otur()
{
  digitalWrite(otur_sinyal, HIGH);
  while (1);
}

float olcum() {

  //verileri güncelliyoruz
  for (int i = 0; i < 4; i++) {
    olc();
    kirmizi_veriler[i] = kirmizi;
    mavi_veriler[i] = mavi;
  }

  // 4 TANE VERİ ALIP İÇELERİNDEN TUTARLI HALE GETİRİYORUZ
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
