/******************************************************************************\
 * Copyright (c) 2010, Tyndall National Institute
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the Tyndall National Institute nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

/***************************************************************************//**
 * Functions to make using external interrupts easier. Currently only for
 * atmega. Note that INT0 is connected to the radio on 2420revA and INT4 is used
 * for other boards.
 *
 * @file externInt.h
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef EXTERNINT_H
#define EXTERNINT_H


/** Interrupt on low level. Value for @c externInt_init(). **/
#define EXTERN_INT_LOW          0

/** Interrupt on level change. Value for @c externInt_init(). **/
#define EXTERN_INT_CHANGE       1

/** Interrupt on falling edge. Value for @c externInt_init(). **/
#define EXTERN_INT_FALLING      2

/** Interrupt on rising edge. Value for @c externInt_init(). **/
#define EXTERN_INT_RISING       3


/**
 * Sets up and enables interrupt.
 *
 * @param interrupt which interrupt (0-7) pin to initialise.
 * @param mode what mode to use.
 **/
void externInt_init(uint8_t interrupt, uint8_t mode);


/**
 * Enables interrupt.
 *
 * @param interrupt which interrupt pin to enable.
 **/
void externInt_enable(uint8_t interrupt);


/**
 * Disables interrupt.
 *
 * @param interrupt which interrupt pin to disable.
 **/
void externInt_disable(uint8_t interrupt);


/**
 * Clears interrupt flag.
 *
 * @param interrupt which flag to clear.
 **/
void externInt_clear(uint8_t interrupt);


/**
 * Called when interrupt is triggered.
 *
 * @param interrupt which interrupt pin was triggered.
 **/
void externInt_callback(uint8_t interrupt);


#endif
