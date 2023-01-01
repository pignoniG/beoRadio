import time, vlc
import serial

import serial.tools.list_ports

# creating a vlc instance
vlc_instance = vlc.Instance()

# creating a media player
player = vlc_instance.media_player_new()

radioList = ("http://icestreaming.rai.it/1.mp3",
             "http://icestreaming.rai.it/2.mp3",
             "http://icestreaming.rai.it/3.mp3",
             "http://icecast.unitedradio.it/Virgin.mp3",
             "http://icecast.unitedradio.it/RMC.mp3",
             "https://radio24-lh.akamaihd.net/i/radio24_1@99307/master.m3u8",
             "https://comodino.org:8000/rcf.ogg")

# method to play video
def radio(source):
    player.stop()

    # creating a media
    media = vlc_instance.media_new(source)
     
    # setting media to the player
    player.set_media(media)
     
    # play the video
    player.play()
     

lastRadio = 2    
# call the video method

def ceckPlayer():
    if(not player.is_playing()):
        print ("tring to play")
        radio(radioList[lastRadio])
        time.sleep(4)
                    
while True:
    try:
        ports = list(serial.tools.list_ports.comports())
        for p in ports:
            
            print (p)
            if "FT232R USB UART" in p[1]:
                print ("Arduino found")
                ser = serial.Serial(p[0], 9600, timeout=1)
                ser.flush()
            elif "ttyUSB0" in p[1]:
                print ("Moteino Found")
                ser = serial.Serial(p[0], 9600, timeout=1)
                ser.flush()

            
            while True:
                
                if ser.in_waiting > 0:
                    line = ser.readline().decode('utf-8').rstrip()
                    print(line)

                    try:
                        value = int(line)
                        if value<= len(radioList) and value-1!=lastRadio:
                            player.stop()
                            lastRadio = value-1
                            print(radioList[value-1])
                
                    except: 
                        print('not an int')

                ceckPlayer()
    except: 
        print('no serial')
        ceckPlayer()
        time.sleep(3)



