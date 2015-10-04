# -*- coding: utf-8 -*-
#!/usr/bin/python


def proportional(error,kp):
    return (error*kp)

def integral(error,sampleTime,integral_,ki):
    return (integral_ + error*sampleTime*ki)

def derivative(error,errorPrevious,sampleTime,kd):
    return (((error-errorPrevious)/sampleTime)*kd)

def pid(error,errorPrevious,sampleTime,integral_,kp,ki,kd):
    P = proportional(error,kp)
    I = integral(error,sampleTime,integral_,ki)
    D = derivative(error,errorPrevious,sampleTime,kd)
    return (P+I+D)

def pi(error,sampleTime,integral_,kp,ki):
    P = proportional(error,kp)
    I = integral(error,sampleTime,integral_,ki)
    return (P+I)

def pd(error,errorPrevious,sampleTime,kp,kd):
    P = proportional(error,kp)
    D = derivative(error,errorPrevious,sampleTime,kd)
    return(P+D)

def pi_d(error,errorPrevious,sampleTime,integral_,kp,ki,kd):
    PI = pi(error,sampleTime,integral_,kp,ki)
    D = derivative(error,errorPrevious,sampleTime,kd)
    return (PI-D)

def pi_pd(error,errorPrevious,sampleTime,integral_,kp,ki,kd):
    PI = pi(error,sampleTime,integral_,kp,ki)
    PD = pd(error,errorPrevious,sampleTime,kp,kd)
    return (PI-PD)

def anti_windup(controlerOutPrevious,saturationLimit,kwd):
    if (controlerOutPrevious > saturationLimit):
        vs = saturationLimit
    else:
        vs = controlerOutPrevious
    es = vs - controlerOutPrevious
    return (es*kwd)
        
