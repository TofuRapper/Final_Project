import wave
try:
    w = wave.open('select.wav', 'r')
    print(f"Valid WAV: channels={w.getnchannels()}, width={w.getsampwidth()}, rate={w.getframerate()}, comp={w.getcomptype()}")
    w.close()
except Exception as e:
    print(f"Error opening WAV: {e}")
