//////////////////////////////////////////////////
//                                              //
// GameOfLife Effect                            //
// von Tobias Kattanek                          //
//                                              //
// #file: game_of_life.h                        //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Sen. + Jun. Kattanek  //
//                                              //
// Letzte Änderung am 15.12.2022                //
//                                              //
//                                              //
//////////////////////////////////////////////////

#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H


#include <cstdint>
class GameOfLife
{
public:
    GameOfLife(int xw, int yw, uint32_t *buffer);
    ~GameOfLife();

	void Init();
	void Render();

private:
	int xw, yw, led_count;
	uint32_t *buffer;
    uint32_t *privateBuffer;
    bool bufferWasRandomized = false;

    void RandomizeBuffer();

    void CalcNextGeneration(int row, int column);
    int GetCountNeighbor(uint32_t *playfield, int pitch, int x, int y);
    void SetCellLife(uint32_t *playfield, int pitch, int x, int y, uint32_t life);
    bool GetCellLife(uint32_t *playfield, int pitch, int x, int y);
};

#endif // GAME_OF_LIFE_H
