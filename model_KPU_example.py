'''
@author: Antonio11111111
@date: Mar 15th, 2025
description:
 The program is based on the code automatically based on CanMV's model training website.
 It mainly functions as a visual recognization and serial communication to the slave(STM32)
 Thanks for your support!

 这里的模型需要用户自己去训练，训练网址：https://developer.canaan-creative.com/en/products
 模型这里不提供，可以在上述网站进行训练

'''

import sensor, image, time, lcd, gc, cmath
from hiwonder import hw_uart
from maix import KPU

lcd.init()                          # Init lcd display
lcd.clear(lcd.RED)                  # Clear lcd screen.

# sensor.reset(dual_buff=True)      # improve fps
sensor.reset()                      # Reset and initialize the sensor.
sensor.set_pixformat(sensor.RGB565) # Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_framesize(sensor.QVGA)   # Set frame size to QVGA (320x240)
sensor.set_vflip(True)
#sensor.set_hmirror(True)
sensor.skip_frames(time = 1000)     # Wait for settings take effect.
clock = time.clock()                # Create a clock object to track the FPS.

str_buf = []
print("ready load model")

# building items
serial = hw_uart()
kpu = KPU()

kpu.load_kmodel('/sd/cls.kmodel')
#kpu.load_kmodel(0x300000, MODEL_SIZE)

labels = ['L', 'R', 'S']  # the label here should be the same order with the txt file generated

img256256 = image.Image(size=(256,256))

while(True):
    gc.collect()

    clock.tick()                    # Update the FPS clock.
    img = sensor.snapshot()         # Take a picture and return the image.

    img256240 = img.copy((32, 0, 256, 240))
    a = img256256.draw_image(img256240,0,8)
    del img256240

    img128128 = img256256.resize(128,128)
    img128128.pix_to_ai()

    result = kpu.run_with_output(img128128, getlist=True)
    del img128128
    #print(result)

    fps = clock.fps()

    result = kpu.softmax(result)
    #print(result)

    _max_val = max(result)
    _max_idx = result.index(_max_val)

    print(labels[_max_idx], _max_val)

    img.draw_string(32, 0, labels[_max_idx] + " %.3f"%_max_val,0xFFFF,2.0)
    str_buf.append(int(labels[_max_idx]))  # 这里写上你想要发送的信息


    img.draw_rectangle(32,0,256,240)
    a = img.draw_string(0, 240 - 32, "%2.1ffps" %(fps),color=(0,60,255),scale=2.0)
    serial.send_bytearray(str_buf)


    lcd.display(img)
