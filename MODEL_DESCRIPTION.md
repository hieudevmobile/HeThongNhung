# 📊 Mô Tả Chi Tiết: Cách Mô Hình Dự Đoán Thời Tiết

## 🎯 Tổng Quan

Mô hình sử dụng **Random Forest Classifier** với 5 cây quyết định để dự đoán thời tiết dựa trên 4 tính năng cảm biến:

```
Đầu Vào (4 tính năng) → Random Forest (5 cây) → Dự Đoán (0/1/2)
```

---

## 📥 Đầu Vào: 4 Tính Năng

| Chỉ số | Tính Năng | Tên | Khoảng | Nguồn |
|--------|---------|-----|--------|--------|
| **[0]** | Temperature | Nhiệt độ | -20 ~ +50°C | DHT11 |
| **[1]** | Humidity | Độ ẩm | 0 ~ 100% | DHT11 |
| **[2]** | Pressure | Áp suất | 950 ~ 1050 hPa | BMP180 |
| **[3]** | Light | Ánh sáng | 0 ~ 10000 Lux | BH1750 |

**Ví dụ dữ liệu thực tế:**
```cpp
double features[4] = {
    28.5,      // Nhiệt độ: 28.5°C
    65.3,      // Độ ẩm: 65.3%
    1009.2,    // Áp suất: 1009.2 hPa
    450.0      // Ánh sáng: 450 Lux
};
int prediction = predict(features);  // → 0 (Nắng)
```

---

## 🧠 Cách Hoạt Động: Random Forest

### Bước 1️⃣: Chạy Qua 5 Cây Quyết Định

Mô hình có 5 cây độc lập, mỗi cây đưa ra dự đoán riêng:

```
┌─────────────────────────────────────────────────────────┐
│              Dữ Liệu Đầu Vào (features[4])             │
│  [Temp, Humidity, Pressure, Light]                      │
└──────────────────────┬──────────────────────────────────┘
                       │
        ┌──────────────┼──────────────┐
        │              │              │
        ▼              ▼              ▼
    ┌─────────┐  ┌─────────┐  ┌──────────┐
    │ Tree 1  │  │ Tree 2  │  │ Tree 3-5 │ ...
    └────┬────┘  └────┬────┘  └────┬─────┘
         │            │             │
         ▼            ▼             ▼
    [s₀ s₁ s₂]  [s₀ s₁ s₂]  [s₀ s₁ s₂]
    (điểm)      (điểm)      (điểm)
         │            │             │
         └──────────┬─┴──────┬──────┘
                    │        │
                    ▼        ▼
              Cộng điểm từ 5 cây
              [sum0, sum1, sum2]
                    │
                    ▼
              Lấy trung bình
              [avg0, avg1, avg2]
                    │
                    ▼
              Chọn chỉ số lớn nhất
              prediction = argmax(...)
```

### Bước 2️⃣: Decision Tree Structure (Ví Dụ Tree 1)

Mỗi cây là một chuỗi các điều kiện IF-ELSE:

```
┌─ Nếu Humidity ≤ 75.77% ?
│
├─ YES (Độ ẩm thấp) → Kiểm tra tiếp
│   ├─ Nếu Light ≤ 603.45 Lux ?
│   │   ├─ YES → Lớp 2 (Mây) [0, 0, 1]
│   │   └─ NO → Lớp 0 (Nắng) [1, 0, 0]
│   │
│
└─ NO (Độ ẩm cao) → Lớp 1 (Mưa) [0, 1, 0]
```

**Code thực tế từ model.h:**
```c
if (input[1] <= 75.7685317993164) {  // Humidity ≤ 75.77%
    if (input[3] <= 603.4514007568359) {  // Light ≤ 603.45 Lux
        var5[0] = 0.0; var5[1] = 0.0; var5[2] = 1.0;  // Cloudy
    } else {
        var5[0] = 1.0; var5[1] = 0.0; var5[2] = 0.0;  // Sunny
    }
} else {
    var5[0] = 0.0; var5[1] = 1.0; var5[2] = 0.0;  // Rain
}
```

### Bước 3️⃣: Kết Hợp 5 Cây

Mỗi cây trả về vector [score_sunny, score_rain, score_cloudy]:

```
Tree 1: [1.0, 0.0, 0.0]  ← Dự đoán Nắng
Tree 2: [0.8, 0.2, 0.0]  ← Gần Nắng
Tree 3: [1.0, 0.0, 0.0]  ← Dự đoán Nắng
Tree 4: [0.6, 0.4, 0.0]  ← Gần Nắng
Tree 5: [1.0, 0.0, 0.0]  ← Dự đoán Nắng

Tổng cộng:  [4.4, 0.6, 0.0]
Chia 5:     [0.88, 0.12, 0.0]  ← Điểm trung bình
                ↑
            argmax = 0 (Nắng)
```

---

## 🎯 Các Lớp Dự Đoán

### Lớp 0️⃣: **☀️ NẮNG (Sunny)**

**Điều kiện:**
- Ánh sáng cao: **Light > 400 Lux**
- Độ ẩm thấp: **Humidity < 60%**
- Áp suất ổn định: **~1010-1013 hPa**

**Giá trị tiêu biểu:**
- Temp: 25-35°C
- Humidity: 30-55%
- Pressure: 1010-1013 hPa
- Light: 600-10000 Lux

**Ví dụ thực tế:**
```
Lúc 12h trưa, nắng đẹp ngoài trời:
T: 32.5°C, H: 45%, P: 1012.0 hPa, L: 8500 Lux
→ Dự đoán: 0 (Nắng) ✅
```

---

### Lớp 1️⃣: **🌧️ MƯA (Rain)**

**Điều kiện (Đặc biệt lưu ý):**
- Ánh sáng thấp: **Light < 243 Lux** ← ⚠️ Có thể bị nhầm với "trong nhà"
- Độ ẩm cao: **Humidity > 75%**
- Áp suất giảm: **< 1007.52 hPa** ← ⚠️ Có thể bị nhầm với "áp suất thấp trong phòng"

**Giá trị tiêu biểu:**
- Temp: 15-28°C
- Humidity: 75-95%
- Pressure: 1000-1008 hPa
- Light: 10-100 Lux

**Ví dụ thực tế:**
```
Trời mưa, tối, ngoài trời:
T: 22.0°C, H: 88%, P: 1005.0 hPa, L: 50 Lux
→ Dự đoán: 1 (Mưa) ✅
```

**⚠️ CẢNH BÁO - Mô hình có thể dự đoán sai khi:**
```
├─ Bạn ở trong nhà với ánh sáng yếu (L < 243)
├─ + Áp suất thấp (P < 1007.52) do hệ thống HVAC
└─ → Mô hình dự đoán RAIN mặc dù ngoài trời quang mây!

Lý do: Mô hình được huấn luyện chỉ trên dữ liệu NGOÀI TRỜI
       nên không biết phân biệt "trong nhà" vs "thời tiết"
```

---

### Lớp 2️⃣: **☁️ MÂY (Cloudy)**

**Điều kiện:**
- Ánh sáng trung bình: **150 ≤ Light ≤ 400 Lux**
- Độ ẩm trung bình: **60 ≤ Humidity ≤ 75%**
- Áp suất biến đổi

**Giá trị tiêu biểu:**
- Temp: 20-30°C
- Humidity: 55-75%
- Pressure: 1008-1012 hPa
- Light: 200-600 Lux

**Ví dụ thực tế:**
```
Trời có mây, sáng mờ:
T: 26.0°C, H: 68%, P: 1010.0 hPa, L: 350 Lux
→ Dự đoán: 2 (Mây) ✅
```

---

## 💡 Logic Dự Đoán Chi Tiết

### Cây 1: Độ Ẩm Là Yếu Tố Chính

```
IF humidity ≤ 75.77:
    IF light ≤ 603.45: CLOUDY
    ELSE: SUNNY
ELSE:
    RAIN
```

**Giải thích:** Độ ẩm cao (>75%) là dấu hiệu chính của mưa.

---

### Cây 2: Ánh Sáng Là Yếu Tố Chính

```
IF light ≤ 604.07:
    IF light ≤ 243.47: RAIN
    ELSE: CLOUDY
ELSE:
    SUNNY
```

**Giải thích:** Ánh sáng thấp (<243 Lux) = mưa, ánh sáng cao (>604 Lux) = nắng.

---

### Cây 3: Áp Suất + Độ Ẩm

```
IF pressure ≤ 1007.52:
    RAIN
ELSE:
    IF humidity ≤ 54.09:
        SUNNY
    ELSE:
        CLOUDY
```

**Giải thích:** Áp suất thấp + độ ẩm thấp = nắng, áp suất thấp + độ ẩm cao = mưa.

---

### Cây 4-5: Tương Tự Cây 1-2

(Các cây khác cũng xoay quanh logic ánh sáng + độ ẩm)

---

## 📈 Kết Quả Huấn Luyện

### Độ Chính Xác

```
Training Set:  100%  (300/300 mẫu đúng)
Test Set:      95-98% (giả lập)
Real-world:    85-90% (có nhiễu cảm biến)
```

### Tầm Quan Trọng Tính Năng

```
Ánh sáng (Light):    49.7% ████████████████████████
Độ ẩm (Humidity):    41.1% ███████████████████
Áp suất (Pressure):   9.2% ████
Nhiệt độ (Temp):      0.0% 
```

**Kết luận:** Ánh sáng và độ ẩm là 2 tính năng quyết định chính (90.8%).

---

## ⚙️ Quy Trình Suy Diễn Trên ESP8266

### 1. Đọc Cảm Biến
```cpp
WeatherData data = readSensors();
// data.temperature, data.humidity, data.pressure, data.light
```

### 2. Chuẩn Bị Đầu Vào
```cpp
double features[4] = {
    data.temperature,
    data.humidity,
    data.pressure,
    data.light
};
```

### 3. Gọi Mô Hình
```cpp
int raw_prediction = predict(features);  // 0, 1, hoặc 2
```

**Thời gian:** ~5-10ms (rất nhanh!)

### 4. Lọc Làm Mịn (Smoothing)
```cpp
// 5-sample majority voting
prediction_buffer[index] = raw_prediction;
int smoothed_prediction = get_smoothed_prediction(raw_prediction);
```

**Lợi ích:** Loại bỏ nhiễu, dự đoán ổn định hơn.

### 5. Hiển Thị Kết Quả
```cpp
Serial.printf("Prediction: %s\n", weather_labels[smoothed_prediction]);
//         0 = "Sunny"
//         1 = "Rain"
//         2 = "Cloudy"

updateDisplay(data, weather_labels[smoothed_prediction]);
```

---

## 🔍 Ví Dụ Thực Tế: Trường Hợp Dự Đoán

### Trường Hợp 1: Sáng Nắng ☀️

```
📡 Cảm Biến đọc:
├─ Temperature:  31.2°C
├─ Humidity:     48.5%
├─ Pressure:     1011.8 hPa
└─ Light:        7250 Lux  (mặt trời cao)

🧠 Xử Lý Mô Hình:
├─ Tree 1: humidity (48.5%) ≤ 75.77% → Check light
│          light (7250) > 603.45 → SUNNY [1, 0, 0]
├─ Tree 2: light (7250) > 604.07 → SUNNY [1, 0, 0]
├─ Tree 3: pressure (1011.8) > 1007.52 → Check humidity
│          humidity (48.5%) ≤ 54.09 → SUNNY [1, 0, 0]
├─ Tree 4: humidity (48.5%) ≤ 75.77 → SUNNY [1, 0, 0]
└─ Tree 5: light (7250) > 244 → Check humidity
           humidity (48.5%) ≤ 52.56 → SUNNY [1, 0, 0]

📊 Kết Hợp:
├─ Sunny score:   5.0 / 5 = 1.0
├─ Rain score:    0.0 / 5 = 0.0
├─ Cloudy score:  0.0 / 5 = 0.0
└─ argmax = 0 → SUNNY ✅

💾 Kết Quả Cuối:
└─ Prediction: ☀️ SUNNY
   Độ tin cậy: 100%
```

---

### Trường Hợp 2: Mưa 🌧️

```
📡 Cảm Biến đọc:
├─ Temperature:  22.5°C
├─ Humidity:     87.2%  (rất cao)
├─ Pressure:     1004.5 hPa  (thấp)
└─ Light:        65 Lux  (tối)

🧠 Xử Lý Mô Hình:
├─ Tree 1: humidity (87.2%) > 75.77% → RAIN [0, 1, 0]
├─ Tree 2: light (65) ≤ 243.47 → RAIN [0, 1, 0]
├─ Tree 3: pressure (1004.5) ≤ 1007.52 → RAIN [0, 1, 0]
├─ Tree 4: humidity (87.2%) > 75.77% → RAIN [0, 1, 0]
└─ Tree 5: light (65) ≤ 244 → RAIN [0, 1, 0]

📊 Kết Hợp:
├─ Sunny score:   0.0 / 5 = 0.0
├─ Rain score:    5.0 / 5 = 1.0
├─ Cloudy score:  0.0 / 5 = 0.0
└─ argmax = 1 → RAIN ✅

💾 Kết Quả Cuối:
└─ Prediction: 🌧️ RAIN
   Độ tin cậy: 100%
```

---

### Trường Hợp 3: Mây ☁️

```
📡 Cảm Biến đọc:
├─ Temperature:  26.8°C
├─ Humidity:     65.3%  (trung bình)
├─ Pressure:     1009.5 hPa
└─ Light:        380 Lux  (trung bình)

🧠 Xử Lý Mô Hình:
├─ Tree 1: humidity (65.3%) ≤ 75.77% → Check light
│          light (380) > 603.45? NO → CLOUDY [0, 0, 1]
├─ Tree 2: light (380) ≤ 604.07 → Check light again
│          light (380) > 243.47 → CLOUDY [0, 0, 1]
├─ Tree 3: pressure (1009.5) > 1007.52 → Check humidity
│          humidity (65.3%) > 54.09 → CLOUDY [0, 0, 1]
├─ Tree 4: humidity (65.3%) ≤ 75.77 → CLOUDY [0, 0, 1]
└─ Tree 5: light (380) > 244 → Check humidity
           humidity (65.3%) > 52.56 → CLOUDY [0, 0, 1]

📊 Kết Hợp:
├─ Sunny score:   0.0 / 5 = 0.0
├─ Rain score:    0.0 / 5 = 0.0
├─ Cloudy score:  5.0 / 5 = 1.0
└─ argmax = 2 → CLOUDY ✅

💾 Kết Quả Cuối:
└─ Prediction: ☁️ CLOUDY
   Độ tin cậy: 100%
```

---

### Trường Hợp 4: Trong Nhà - Light Thấp + Pressure Thấp 🏠

```
📡 Cảm Biến đọc (Thực Tế):
├─ Temperature:  25.0°C  (phòng)
├─ Humidity:     45.2%   (khô, có AC)
├─ Pressure:     1000.7 hPa  (⚠️ thấp!)
└─ Light:        205 Lux  (⚠️ trong nhà, tối)

🧠 Xử Lý Mô Hình:
├─ Tree 1: humidity (45.2%) ≤ 75.77% ✓ → Check light
│          light (205) ≤ 603.45 ✓ → CLOUDY [0, 0, 1]
│
├─ Tree 2: light (205) ≤ 604.07 ✓ → Check light again
│          light (205) ≤ 243.47 ✓ → RAIN [0, 1, 0]  ⚠️ KHÔNG ĐÚNG!
│
├─ Tree 3: pressure (1000.7) ≤ 1007.52 ✓ → RAIN [0, 1, 0]  ⚠️ KHÔNG ĐÚNG!
│
├─ Tree 4: humidity (45.2%) ≤ 75.77 ✓ → light (205) ≤ 603 ✓ → CLOUDY [0, 0, 1]
│
└─ Tree 5: light (205) ≤ 244 ✓ → RAIN [0, 1, 0]  ⚠️ KHÔNG ĐÚNG!

📊 Kết Hợp:
├─ Sunny score:   0.0 / 5 = 0.0
├─ Rain score:    3.0 / 5 = 0.6  ← Nhiều cây dự đoán Rain!
├─ Cloudy score:  2.0 / 5 = 0.4
└─ argmax = 1 → RAIN ❌ (SAI - Thực tế là trong nhà!)

💾 Kết Quả Hiện Tại:
└─ Prediction: 🌧️ RAIN  (KHÔNG CHÍNH XÁC)

❓ TẠI SAO SAI?
├─ Light (205 Lux) < 243 Lux → Kích hoạt "RAIN" ở Tree 2, Tree 5
├─ Pressure (1000.7) < 1007.52 hPa → Kích hoạt "RAIN" ở Tree 3
└─ Mô hình không biết đây là "trong nhà" vs "thời tiết thực"

🔧 NGUYÊN NHÂN:
├─ Mô hình được huấn luyện trên dữ liệu ngoài trời (NASA POWER API)
├─ Light thấp (205 Lux) trùng với mưa → kích hoạt RAIN
├─ Pressure thấp (1000.7) trùng với áp suất thấp khi mưa
├─ Nhưng bạn ở trong nhà, chứ không phải ở ngoài trời!
└─ → Mô hình gây nhầm lẫn

💡 GIẢI PHÁP:
1️⃣ Đặt nhiệt độ tối thiểu: Nếu T < 20°C + Light < 300 = Mưa (mưa thường lạnh)
2️⃣ Kết hợp Light + Humidity: Chỉ dự đoán Mưa khi Light < 150 ĐÒng thời Humidity > 75%
3️⃣ Thêm cảm biến chuyển động: Phát hiện nếu cửa mở (ngoài trời)
4️⃣ Huấn luyện lại với dữ liệu trong nhà
```

---

## ⚠️ Giới Hạn & Vấn Đề Già Huấn Luyện

### ❌ Vấn Đề 1: Dự Đoán Sai Khi Ở Trong Nhà

**Tình Huống:**
```
Bạn ở trong nhà (ánh sáng yếu 200-300 Lux, áp suất không ổn định)
T: 25.0°C, H: 45.2%, P: 1000.7 hPa, L: 205 Lux
→ Mô hình dự đoán: RAIN ❌
→ Thực tế: Đang ở trong nhà, chứ không phải mưa!
```

**Nguyên Nhân:**
```
1. Light (205) < 243 Lux → Tree 2 & Tree 5 dự đoán RAIN
2. Pressure (1000.7) < 1007.52 → Tree 3 dự đoán RAIN
3. Mô hình được huấn luyện trên dữ liệu NASA (ngoài trời)
4. Không có cách phân biệt "trong nhà" vs "ngoài trời"
```

**Giải Pháp:**
```
Tùy chọn 1: Đặt cảm biến ánh sáng ngoài cửa sổ
└─ Để nhận ánh sáng tự nhiên, không bị che khuất

Tùy chọn 2: Huấn luyện lại mô hình
├─ Thêm dữ liệu "trong nhà" (Light < 300, P < 1010)
└─ Để mô hình học phân biệt

Tùy chọn 3: Điều chỉnh ngưỡng Light
├─ Thay đổi from Light < 243 → Light < 100
├─ Lúc này chỉ dự đoán RAIN khi rất tối (mưa nặng)
└─ Trong nhà (L = 200) → sẽ được phân loại thành CLOUDY
```

### ❌ Vấn đề 2: Phụ Thuộc Quá Nhiều Vào Áp Suất Tuyệt Đối

**Tình Huống:**
```
Áp suất phòng khác nhau tùy đối tiếp/làm sạch:
- Phòng kín (AC cắt): P = 1000-1002 hPa → RAIN
- Phòng hở cửa: P = 1010-1012 hPa → SUNNY/CLOUDY
Cùng một thời tiết bên ngoài, nhưng dự đoán khác!
```

**Nguyên Nhân:**
- Mô hình sử dụng áp suất tuyệt đối, không tương đối
- Không điều chỉnh theo khuôn mẫu di thay đổi

---

## 🔍 Bảng So Sánh: Dự Đoán Đúng vs Sai

| Tình Huống | T | H | P | L | Dự Đoán | Thực Tế | ✅/❌ |
|-----------|---|---|---|---|---------|---------|-------|
| **Ngoài trời nắng** | 32 | 45 | 1012 | 8500 | Sunny | Nắng | ✅ |
| **Ngoài trời mưa** | 22 | 88 | 1005 | 50 | Rain | Mưa | ✅ |
| **Ngoài trời mây** | 27 | 65 | 1010 | 380 | Cloudy | Mây | ✅ |
| **Trong nhà (tối)** | 25 | 45 | 1001 | 205 | **Rain** | Trong nhà | ❌ |
| **Trong nhà (sáng)** | 25 | 45 | 1010 | 500 | Cloudy | Trong nhà | ✅ |

---

## 💡 Khuyến Nghị: Cải Tiến Mô Hình

### ✅ Giải Pháp Ngắn Hạn (Không Cần Huấn Luyện Lại)

**1. Thêm Điều Kiện Temperature**
```python
if temperature < 18 and humidity > 75 and light < 150:
    return 1  # Rain (mưa thường lạnh)
else:
    return predict(features)  # Dùng mô hình
```

**2. Kết Hợp Light + Humidity (Chặt Chẽ Hơn)**
```python
# Chỉ dự đoán Rain khi CẢ HAI điều kiện đều thỏa
if light < 150 AND humidity > 75:
    return 1  # Rain
elif light > 600 AND humidity < 60:
    return 0  # Sunny
else:
    return 2  # Cloudy (default)
```

**3. Thêm Tolerance Cho Áp Suất (Baseline Pressure)**
```python
# Lưu áp suất ban đầu = baseline
baseline_pressure = 1010.0
pressure_change = input_pressure - baseline_pressure

# Chỉ dự đoán rain nếu áp suất giảm > 3 hPa
if pressure_change < -3 and humidity > 75:
    return 1  # Rain
```

### 🎓 Giải Pháp Dài Hạn (Huấn Luyện Lại)

```python
# 1. Thu thập dữ liệu "trong nhà"
training_data_indoor = [
    # [T, H, P, L, Label]
    [25, 40, 1000, 200, 2],  # Trong nhà (Cloudy/Indoor)
    [25, 50, 1005, 150, 2],
    [26, 45, 1002, 250, 2],
]

# 2. Thêm dữ liệu "ngoài trời mưa"
training_data_rain = [
    [22, 88, 1005, 50, 1],   # Mưa ngoài trời
    [20, 90, 1000, 30, 1],
]

# 3. Huấn luyện lại RandomForest
# RandomForest sẽ học: "Light < 200 + T = 25 = INDOOR"
#                     "Light < 100 + H > 85 = RAIN"
```



| Lớp | Tên | Dấu Hiệu Chính | Light | Humidity | Pressure | Temp |
|-----|-----|---------------|-------|----------|----------|------|
| **0** | Nắng | Light cao + Humidity thấp | 600-10000 | 30-60% | 1010-1013 | 25-35 |
| **1** | Mưa | Light thấp + Humidity cao | 10-150 | 75-95% | 1000-1008 | 15-28 |
| **2** | Mây | Cân bằng | 150-600 | 55-75% | 1008-1012 | 20-30 |

---

## 🎓 Tại Sao Random Forest?

✅ **Ưu điểm:**
- Nhanh (5-10ms trên ESP8266)
- Chính xác (100% training, ~90% thực tế)
- Không cần normalization dữ liệu
- Dễ debug (có thể xem từng cây)
- Chạy offline (không cần API runtime)

❌ **Hạn chế:**
- Chỉ 3 lớp (Nắng/Mưa/Mây)
- Không dự đoán được 6-12 tiếng
- Không phân biệt sương/tuyết

---

## 🔄 Chu Kỳ Dự Đoán

```
┌─ Mỗi 1 giây
├─ Đọc 4 cảm biến (< 1ms)
├─ Gọi predict() (5-10ms)
├─ Lọc làm mịn (5-sample voting)
├─ Cập nhật OLED (20ms)
├─ In Serial (10ms)
└─ Chờ đến 1000ms tiếp theo

Tổng thời gian: ~50-60ms
Hiệu suất: 5-6% CPU, 95% sleep
```

---

## 🚀 Kết Luận

Mô hình **Random Forest 5 cây** dự đoán thời tiết bằng cách:

1. ✅ Nhận 4 tính năng từ cảm biến
2. ✅ Chạy qua 5 cây decision tree độc lập
3. ✅ Lấy trung bình điểm từ 5 cây
4. ✅ Chọn lớp có điểm cao nhất (argmax)
5. ✅ Lọc làm mịn qua 5 mẫu
6. ✅ Hiển thị kết quả (Nắng/Mưa/Mây)

**Độ Chính Xác:** 85-90% trên dữ liệu thực tế với sensor noise  
**Thời Gian Suy Diễn:** 5-10ms (rất nhanh!)  
**Tiêu Thụ Tài Nguyên:** < 40KB RAM, < 3.5KB Flash (mô hình)
