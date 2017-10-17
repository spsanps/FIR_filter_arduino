import wave  # in-built library to read wav files

import pyaudio  # library to play audio stream
import serial  # library for communicating through Serial (used to connect to Arduino)
from pydub import AudioSegment  # library to edit/convert/down-sample ... audio

WAV_FILE = "humble.wav"  # file we want to play through the filter (must be wav)
ARDUINO_PORT = "COM10"  # Serial port to which Arduino is connected
CHUNK_SIZE = 512  # how many bytes to read at once.

"""At high CHUNK sizes the Arduino Filter will not function properly.
At very low CHUNK sizes processing will be too slow"""

print "Pre-processing..."
# read the file and pre-process it
sound = AudioSegment.from_mp3(WAV_FILE)  # read file
sound = sound.set_channels(1)  # convert to Mono channel

sound = sound.set_frame_rate(15000)  # Set frame-rate; (sample rate)
# At high rates Arduino won't be able to process in real time

sound = sound.set_sample_width(1)  # Convert to 8-bit Audio

sound.export("preprocessed.wav", format="wav")  # save the edited audio
print "Done..."

ard = serial.Serial(ARDUINO_PORT, 250000)  # connect to Arduino with required baud-rate
print "Waiting for Arduino..."
ard.readline()  # wait for Arduino to send "Ready!"
print "Ready!"

wf = wave.open("preprocessed.wav", 'rb')  # open edited audio file

# instantiate PyAudio
p = pyaudio.PyAudio()

# open audio stream for playing music
stream = p.open(format=p.get_format_from_width(wf.getsampwidth()),
                channels=1,
                rate=wf.getframerate(),
                output=True)

print "Number of Samples: ", wf.getnframes()  # print number of frames

# read data, send to ard (Arduino), receive data back and send to stream (audio stream) to play
data = wf.readframes(CHUNK_SIZE)
while len(data) > 0:  # till entire file is read
    ard.write(data)
    ret = ard.read(len(data))
    stream.write(ret)
    data = wf.readframes(CHUNK_SIZE)

# stop pyaduio and Audio stream
stream.stop_stream()
stream.close()
p.terminate()

# close wav file
wf.close()

# stop serial communication with Arduino
ard.close()
