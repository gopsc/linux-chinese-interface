# -*- coding: utf-8 -*-

from __future__ import division
import time

import Adafruit_PCA9685

pwm = Adafruit_PCA9685.PCA9685(busnum=1)
servo_min = 150
servo_max = 600

def set_servo_pulse(channel, pulse):
    pulse_length = 1000000
    pulse_length //=60
    print('{0}us per period'.format(pulse_length))
    pulse_length //= 4096
    print('{0}us per bit'.format(pulse_length))
    pulse *= 1000
    pulse //= pulse_length
    pwm.set_pwm(channel, 0, pulse)

def set_servo_angle(channel, angle):
    angle = 4096 * ((angle*11)+500)/20000
    pwm.set_pwm(channel, 0, int(angle))

pwm.set_pwm_freq(50)

class Leg:
    def __init__(self, id, start_angle):
        self.id = id
        self.sa = start_angle
        self.cur = self.sa
    def move(self, angle):
        if self.sa < 90: self.cur += angle
        elif self.sa >= 90: self.cur -= angle
        set_servo_angle(self.id, self.cur)

legs = (Leg(3,200), Leg(7,15), Leg(11,10), Leg(15,200))

for i in legs:
    i.move(0)

