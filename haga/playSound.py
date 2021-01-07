import numpy as np
import pyaudio
import re
# サンプリングレートを定義
SAMPLE_RATE = 44000

Num2Hz = [
    0,          # 0
    261.626,    # 1
    277.183,    # 2
    293.665,    # 3
    311.127,    # 4
    329.628,    # 5
    349.228,    # 6
    369.994,    # 7
    391.995,    # 8
    415.305,    # 9
    440.000,    # 10
    466.164,    # 11
    493.883,    # 12
    523.251,    # 13   （ド）
    554.365,    # 14
    587.330,    # 15
    622.254,    # 16
    659.255,    # 17
    698.456,    # 18
    739.989,    # 19
    783.991,    # 20
    830.609,    # 21
    880.000,    # 22
    932.328,    # 23
    987.767     # 24
]

# 指定ストリームで、指定周波数のサイン波を、指定秒数再生する関数
def play(s: pyaudio.Stream, freq1: float, freq2: float, freq3: float, duration: float):
    # 指定周波数のサイン波を指定秒数分生成
    samples = np.sin(np.arange(int(duration * SAMPLE_RATE)) * freq1 * np.pi * 2 / SAMPLE_RATE)
    samples += np.sin(np.arange(int(duration * SAMPLE_RATE)) * freq2 * np.pi * 2 / SAMPLE_RATE)
    samples += np.sin(np.arange(int(duration * SAMPLE_RATE)) * freq3 * np.pi * 2 / SAMPLE_RATE)
    # ストリームに渡して再生
    s.write(samples.astype(np.float32).tostring())

# PyAudio開始
p = pyaudio.PyAudio()
# ストリームを開く
stream = p.open(format=pyaudio.paFloat32,
                channels=1,
                rate=SAMPLE_RATE,
                frames_per_buffer=1024,
                output=True)

print('type filename "default" or custom')
filename = input()
if filename == 'default':
    filename = '../_data/sounds.txt'
with open(filename) as f:
    lines = f.readlines()
    for line in lines:
        play(
            stream,
            Num2Hz[int(re.findall('(\d+) (\d+) (\d+)', line)[0][0])],
            Num2Hz[int(re.findall('(\d+) (\d+) (\d+)', line)[0][1])],
            Num2Hz[int(re.findall('(\d+) (\d+) (\d+)', line)[0][2])],
            0.5)

# ストリームを閉じる
stream.close()
# PyAudio終了
p.terminate()