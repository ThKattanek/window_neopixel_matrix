//////////////////////////////////////////////////
//                                              //
// GameOfLife Effect                            //
// von Tobias Kattanek                          //
//                                              //
// #file: game_of_life.cpp                      //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Sen. + Jun. Kattanek  //
//                                              //
// Letzte Änderung am 15.12.2022                //
//                                              //
//                                              //
//////////////////////////////////////////////////

#include "./game_of_life.h"

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#define DEAD_CELL 0x00000000
#define ALIVE_CELL 0xffffffff

GameOfLife::GameOfLife(int xw, int yw, uint32_t *buffer)
{
	this->xw = xw;
	this->yw = yw;
	this->buffer = buffer;
    led_count = xw * yw;

	bufferWasRandomized = false;

    privateBuffer = new uint32_t[led_count];

	srand (time(NULL));
}

GameOfLife::~GameOfLife()
{
    delete[] privateBuffer;
}

void GameOfLife::Init()
{
	//bufferWasRandomized = false;
	RandomizeBuffer();
}

void GameOfLife::RandomizeBuffer()
{
    for(int i=0; i<led_count; i++)
	{
        privateBuffer[i] = DEAD_CELL;
	}

	for(int i=0; i<150; i++)
	{
		privateBuffer[rand() % led_count] = ALIVE_CELL;
	}

    bufferWasRandomized = true;
}

void GameOfLife::Render()
{
	//bufferWasRandomized ? CalcNextGeneration(yw, xw) : RandomizeBuffer();

	CalcNextGeneration(yw, xw);

	for(int i=0; i<led_count; i++)
		buffer[i] = privateBuffer[i];
}

void GameOfLife::CalcNextGeneration(int row, int column)
{
	uint32_t new_playfield[(row+2) * (column+2)];
    for(int i=0; i<((row+2) * (column+2)); i++ ) new_playfield[i] = DEAD_CELL;

    for(int y=1; y<row+1; y++)
        for(int x=1; x<column+1; x++)
        {
            int neighbor = GetCountNeighbor(privateBuffer,column+2,x,y);

            if(GetCellLife(privateBuffer,column+2,x,y))
            {
                /// Zelle lebt
                if(neighbor < 2) SetCellLife(new_playfield,column+2,x,y,DEAD_CELL);
                if((neighbor == 2) || (neighbor == 3)) SetCellLife(new_playfield,column+2,x,y,ALIVE_CELL);
                if(neighbor > 3) SetCellLife(new_playfield,column+2,x,y,DEAD_CELL);
            }
            else
            {
                /// Zelle ist tod
                if(neighbor == 3) SetCellLife(new_playfield,column+2,x,y,ALIVE_CELL);
                else SetCellLife(new_playfield,column+2,x,y,DEAD_CELL);
            }
        }

	for(int i=0; i<((row) * (column)); i++)
    {
        privateBuffer[i] = new_playfield[i];
    }

}

int GameOfLife::GetCountNeighbor(uint32_t *playfield, int pitch, int x, int y)
{
    int pos = y * pitch + x;
    int count = 0;

    if(playfield[pos - 1] == ALIVE_CELL) count++;
    if(playfield[pos + 1] == ALIVE_CELL) count++;
    if(playfield[pos + pitch] == ALIVE_CELL) count++;
    if(playfield[pos + pitch - 1] == ALIVE_CELL) count++;
    if(playfield[pos + pitch + 1] == ALIVE_CELL) count++;
    if(playfield[pos - pitch] == ALIVE_CELL) count++;
    if(playfield[pos - pitch - 1] == ALIVE_CELL) count++;
    if(playfield[pos - pitch + 1] == ALIVE_CELL) count++;

    return count;
}

void GameOfLife::SetCellLife(uint32_t *playfield, int pitch, int x, int y, uint32_t life)
{
    int pos = y * pitch + x;
    playfield[pos] = life;
}

bool GameOfLife::GetCellLife(uint32_t *playfield, int pitch, int x, int y)
{
    int pos = y * pitch + x;
    return playfield[pos];
}
