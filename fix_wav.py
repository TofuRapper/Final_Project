import scipy.io.wavfile
import numpy as np

try:
    rate, data = scipy.io.wavfile.read('select.wav')
    print(f"Read successful. Rate: {rate}, Shape: {data.shape}, Dtype: {data.dtype}")
    
    # Convert to 16-bit PCM if it's not
    if data.dtype != np.int16:
        print(f"Converting {data.dtype} to int16...")
        
        # Normalize float to int16
        if data.dtype == np.float32 or data.dtype == np.float64:
            data = (data * 32767).astype(np.int16)
            
        # Convert int32 to int16 by shifting
        elif data.dtype == np.int32:
            # int32 is 32-bit, int16 is 16-bit. We need the most significant bits.
            # Right shift by 16 to keep the high bits.
            data = (data >> 16).astype(np.int16)
            
        # Convert uint8 to int16
        elif data.dtype == np.uint8:
            data = ((data - 128) * 256).astype(np.int16)
            
    scipy.io.wavfile.write('select_fixed.wav', rate, data)
    print("Wrote select_fixed.wav")
    
except Exception as e:
    print(f"Scipy failed: {e}")
