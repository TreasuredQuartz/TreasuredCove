// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Dungeons/Zombie/Zombie_SettingsConfig.h"

uint32 UZombie_SettingsConfig::GetTotalEnemiesToSpawn(uint32 WaveCount, uint8 PlayerCount) const
{
	/*		P1	P2	P3	P4
		R1: 6	7	9	10
		R2: 8	9	10	12
		R3: 13	15	18	21
		R4: 18	21	25	29
		R5: 24	27	32	37
		R6: 27	31	38	45
		R7: 28	32	40	49
		R8: 28	33	43	52
		R9: 29	34	45	56
		R10:33	42	60	78

			1	2	4	5
			2	2	1	2
			5	6	8	9
			5	6	7	8
			6	6	7	8
			3	4	6	8
			1	1	2	4
			0	1	3	3
			1	1	2	4
			4	8	15	18 */


	if (WaveCount < 10)
	{
		switch (PlayerCount)
		{
		case 1:
			return (uint32)((3.01818 * WaveCount + 4.8));
		case 2:
			return (uint32)(FMath::RoundFromZero(3.73939 * WaveCount + 4.53333));
		case 3:
			return (uint32)(FMath::RoundFromZero(5.33333 * WaveCount + 2.66667));
		case 4:
			return (uint32)(FMath::RoundFromZero(6.92727 * WaveCount + 0.8));
		}
	}

	/*  1 player: 0, 000058* R ^ 3 + 0, 074032 * R ^ 2 + 0, 718119 * R + 14, 738699
		2 players: 0, 000054 * R ^ 3 + 0, 169717 * R ^ 2 + 0, 541627 * R + 15, 917041
		3 players : 0, 000169* R ^ 3 + 0, 238079 * R ^ 2 + 1, 307276 * R + 21, 291046
		4 players: 0, 000225 * R ^ 3 + 0, 314314 * R ^ 2 + 1, 835712 * R + 27, 596132 */

	switch (PlayerCount)
	{
	case 1:
		return (uint32)(FMath::RoundFromZero(0.000058 * (WaveCount ^ 3) + 0.074032 * (WaveCount ^ 2) + 0.718119 * WaveCount + 14.738699));
	case 2:
		return (uint32)(FMath::RoundFromZero(0.000054 * (WaveCount ^ 3) + 0.169717 * (WaveCount ^ 2) + 0.541627 * WaveCount + 15.917041));
	case 3:
		return (uint32)(FMath::RoundFromZero(0.000169 * (WaveCount ^ 3) + 0.238079 * (WaveCount ^ 2) + 1.307276 * WaveCount + 21.291046));
	case 4:
		return (uint32)(FMath::RoundFromZero(0.000225 * (WaveCount ^ 3) + 0.314314 * (WaveCount ^ 2) + 1.835712 * WaveCount + 27.596132));
	}

	return 0;
};