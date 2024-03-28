import wave
import numpy as np
import matplotlib.pyplot as plt
def print_wav_header(filename):
    with wave.open(filename, 'rb') as wav_file:
        n_channels = wav_file.getnchannels()
        sample_width = wav_file.getsampwidth()
        frame_rate = wav_file.getframerate()
        n_frames = wav_file.getnframes()
        comp_type = wav_file.getcomptype()
        comp_name = wav_file.getcompname()
        
        print(f"WAV File: {filename}")
        print(f"Number of Channels: {n_channels}")
        print(f"Sample Width (bytes): {sample_width}")
        print(f"Frame Rate (Sample Rate, Hz): {frame_rate}")
        print(f"Number of Frames: {n_frames}")
        print(f"Compression Type: {comp_type}")
        print(f"Compression Name: {comp_name}")
def read_wav_file(filename):
    with wave.open(filename, 'r') as wav_file:
        n_channels = wav_file.getnchannels()
        sample_width = wav_file.getsampwidth()
        framerate = wav_file.getframerate()
        n_frames = wav_file.getnframes()
        
        audio_data = wav_file.readframes(n_frames)
        
        if sample_width == 1:  # 8-bit audio
            data = np.frombuffer(audio_data, dtype=np.uint8) - 128
        elif sample_width == 2:  # 16-bit audio
            data = np.frombuffer(audio_data, dtype=np.int16)
        else:
            raise ValueError("Only 8-bit and 16-bit audio are supported")
        
        # here I am taking only the first channel for the streo file! 
        if n_channels == 2:
            data = data[::2]
        
        return framerate, data

def plot_waveform(framerate, data):
    time_axis = np.linspace(0, len(data) / framerate, num=len(data))
    
    plt.figure(figsize=(10, 4))
    plt.plot(time_axis, data)
    plt.title("Sine Wave from WAV File")
    plt.xlabel("Time (seconds)")
    plt.ylabel("Amplitude")
    plt.grid(True)
    plt.show()

filename = 'sine1.wav'
print_wav_header(filename)
framerate, data = read_wav_file(filename)
plot_waveform(framerate, data)
