#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "lcd.h"
#include <stdint.h>
#include "hd44780.h"
#include "defines.h"

#define F_CPU 14745600UL

/*
 *
 */

FILE lcd_str = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);

void playnote_right(uint32_t);
void playnote_wrong(uint32_t);
void col_in();
void row_in();
void level_up();

int *gen_nums;
int lives = 3;
int level = 1;
int size = 4;
int ti = 10000;
int ptr = 0;
bool game_over = true;

// the interrupt will trigger when a life is lost and the buzzer is sounded
ISR(PCINT2_vect)
{
    // PORTC ^= (1<<PC1);
    _delay_ms(2000);
    lives--;
    playnote_wrong(500);
    _delay_ms(1000);
    free(gen_nums);
    gen_nums = malloc(size * sizeof(int));
    if (lives == 0)
    {
        hd44780_outcmd(HD44780_CLR);
        hd44780_outcmd(HD44780_HOME);
        fprintf(&lcd_str, "GAME OVER \x1b\xc0");
        fprintf(&lcd_str, "Lives:%d Score:%d", lives, level - 1);
        game_over = true;
        playnote_wrong(500);
    }
    else
    {
        fprintf(&lcd_str, "\x1b\x02");
        for (int i = 0; i < size; i++)
        {
            *(gen_nums + i) = rand() % 10;
            fprintf(&lcd_str, "%d", *(gen_nums + i));
        }

        if (level >= 10)
        {
            _delay_ms(500);
        }
        else
        {
            for (int i = 0; i < 11 - level; i++)
            {
                _delay_ms(500);
            }
        }

        //_delay_ms(2000 - level_up_time(100,level));
        fprintf(&lcd_str, "\x1b\x01");
    }
    ptr = 0;
    // if we want the wrong buzzer to buzz twice, turn this on
    // playnote_wrong(500);
}
int main(int argc, char **argv)
{
    srand(time(NULL));
    lcd_init();

    col_in();

    // enable interrupt
    PCICR |= (1 << PCIE2);
    PCMSK2 |= (1 << PCINT19);
    DDRD |= (1 << PD3);

    DDRC |= (1 << PC1);

    // gen_nums = malloc(size * sizeof(int));

    sei();
    while (1)
    {

        // main game loop
        while (!game_over)
        {

            // generate the random number
            fprintf(&lcd_str, "\x1b\x02");
            if ((level - 1) >= 5)
            {
                size = 5;
            }
            for (int i = 0; i < size; i++)
            {
                *(gen_nums + i) = rand() % 10;
                fprintf(&lcd_str, "%d", *(gen_nums + i));
            }
            // uint32_t del = (100) * (uint32_t)level;
            //  ******** we need to make this get shorter with a function
            if (level >= 10)
            {
                _delay_ms(500);
            }
            else
            {
                for (int i = 0; i < 11 - level; i++)
                {
                    _delay_ms(500);
                }
            }
            //_delay_ms(2000 - level_up_time(100,level));

            fprintf(&lcd_str, "\x1b\x01");
            // players turn to react.
            while (ptr < size && !game_over)
            {
                fprintf(&lcd_str, "\x1b\xc0");
                fprintf(&lcd_str, "Lives:%d Score:%d", lives, level - 1);

                // columns here are in order and so are the rows
                if (!(PINC & ((1 << PINC2))))
                {
                    row_in();
                    if (!(PINC & ((1 << PINC3))))
                    {
                        // check to see if the number is correct
                        *(gen_nums + ptr) == 1 ? ptr++ : (PORTD ^= (1 << PD3));
                        _delay_ms(500);
                    }
                    else if (!(PINB & ((1 << PINB1))))
                    {
                        *(gen_nums + ptr) == 4 ? ptr++ : (PORTD ^= (1 << PD3));
                        _delay_ms(500);
                    }
                    else if (!(PINB & ((1 << PINB2))))
                    {
                        *(gen_nums + ptr) == 7 ? ptr++ : (PORTD ^= (1 << PD3));
                        _delay_ms(500);
                    }
                    else if (!(PINB & ((1 << PINB4))))
                    {
                        game_over = true;
                        free(gen_nums);
                        col_in();
                        break;
                    }
                    col_in();
                }
                else if (!(PINC & ((1 << PINC4))))
                {
                    row_in();
                    if (!(PINC & ((1 << PINC3))))
                    {
                        *(gen_nums + ptr) == 2 ? ptr++ : (PORTD ^= (1 << PD3));
                        _delay_ms(500);
                    }
                    else if (!(PINB & ((1 << PINB1))))
                    {
                        *(gen_nums + ptr) == 5 ? ptr++ : (PORTD ^= (1 << PD3));
                        _delay_ms(500);
                    }
                    else if (!(PINB & ((1 << PINB2))))
                    {
                        *(gen_nums + ptr) == 8 ? ptr++ : (PORTD ^= (1 << PD3));
                        _delay_ms(500);
                    }
                    else if (!(PINB & ((1 << PINB4))))
                    {
                        *(gen_nums + ptr) == 0 ? ptr++ : (PORTD ^= (1 << PD3));
                        _delay_ms(500);
                    }
                    col_in();
                }
                else if (!(PINB & ((1 << PINB3))))
                {
                    row_in();
                    if (!(PINC & ((1 << PINC3))))
                    {
                        *(gen_nums + ptr) == 3 ? ptr++ : (PORTD ^= (1 << PD3));
                        _delay_ms(500);
                    }
                    else if (!(PINB & ((1 << PINB1))))
                    {
                        *(gen_nums + ptr) == 6 ? ptr++ : (PORTD ^= (1 << PD3));
                        _delay_ms(500);
                    }
                    else if (!(PINB & ((1 << PINB2))))
                    {
                        *(gen_nums + ptr) == 9 ? ptr++ : (PORTD ^= (1 << PD3));
                        _delay_ms(500);
                    }
                    else if (!(PINB & ((1 << PINB4))))
                    {
                    }
                    col_in();
                }
            }
            if (ptr < size)
            {
                game_over = true;
            }
            else
            {
                level_up();
                playnote_right(500);
            }
        }

        // check to see if the * key has been pressed. if it has start the game

        if (!(PINC & ((1 << PINC2))))
        {
            row_in();
            if (!(PINB & ((1 << PINB4))))
            {
                game_over = false;
                level = 1;
                size = 4;
                lives = 3;
                ptr = 0;
                gen_nums = malloc(size * sizeof(int));
            }
        }
    }

    return (EXIT_SUCCESS);
}

void row_in()
{
    DDRC |= (0x14);
    DDRB |= (1 << PB3);

    PORTC &= ~(0x14);
    PORTB &= ~(1 << PB3);

    DDRC &= ~(1 << PC3);
    DDRB &= ~(0x16);

    PORTC |= (1 << PC3);
    PORTB |= (0x16);
    _delay_ms(200);
}

void col_in()
{
    DDRC &= ~(0x14);
    DDRB &= ~(1 << PB3);

    PORTC |= (0x14);
    PORTB |= (1 << PB3);

    DDRC |= (1 << PC3);
    DDRB |= (0x16);

    PORTC &= ~(1 << PC3);
    PORTB &= ~(0x16);
    _delay_ms(200);
}

void level_up()
{
    level++;
    ptr = 0;
    free(gen_nums);
    gen_nums = malloc(size * sizeof(int));
}

void playnote_wrong(uint32_t duration)
{
    // Physics variables
    int i;
    for (i = 0; i < duration; i++)
    {
        _delay_ms(5);
        PORTC |= (1 << PC1);
        _delay_ms(5);
        PORTC &= ~(1 << PC1);
    }
}

void playnote_right(uint32_t duration)
{
    // Physics variables
    int i;
    for (i = 0; i < duration; i++)
    {
        _delay_ms(3);
        PORTC |= (1 << PC1);
        _delay_ms(3);
        PORTC &= ~(1 << PC1);
    }
}
