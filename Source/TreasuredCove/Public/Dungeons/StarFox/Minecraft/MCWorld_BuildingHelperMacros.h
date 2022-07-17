// Fill out your copyright notice in the Description page of Project Settings.

#define SMALLHOUSE_BOTTOMLAYER_X(x) for(int i = -2; i < 3; i++) { \
	HouseSmall.Add(FIntVector(x, -i, 0), COBBLE);		\
}

#define SMALLHOUSE_FIRSTLAYER \
	HouseSmall.Add(FIntVector(-2, -2, 1), OAKLOG);			\
	HouseSmall.Add(FIntVector(-2, -1, 1),OAKPLANKS);		\
	HouseSmall.Add(FIntVector(-2, 0, 1), OAKPLANKS);		\
	HouseSmall.Add(FIntVector(-2, 1, 1), OAKPLANKS);		\
	HouseSmall.Add(FIntVector(-2, 2, 1), OAKLOG);			\
\
	HouseSmall.Add(FIntVector(-1, -2, 1), OAKPLANKS);		\
	HouseSmall.Add(FIntVector(-1, -1, 1), CRAFTINGTABLE);	\
	HouseSmall.Add(FIntVector(-1, 0, 1), AIR);				\
	HouseSmall.Add(FIntVector(-1, 1, 1), OAKSTAIRS);		\
	HouseSmall.Add(FIntVector(-1, 2, 1), OAKPLANKS);		\
\
	HouseSmall.Add(FIntVector(0, -2, 1), OAKPLANKS);		\
	HouseSmall.Add(FIntVector(0, -1, 1), AIR);				\
	HouseSmall.Add(FIntVector(0, 0, 1), AIR);				\
	HouseSmall.Add(FIntVector(0, 1, 1), OAKFENCE);			\
	HouseSmall.Add(FIntVector(0, 2, 1), OAKPLANKS);			\
\
	HouseSmall.Add(FIntVector(1, -2, 1), OAKPLANKS);		\
	HouseSmall.Add(FIntVector(1, -1, 1), AIR);				\
	HouseSmall.Add(FIntVector(1, 0, 1), AIR);				\
	HouseSmall.Add(FIntVector(1, 1, 1), AIR);				\
	HouseSmall.Add(FIntVector(1, 2, 1), OAKPLANKS);			\
\
	HouseSmall.Add(FIntVector(2, -2, 1), OAKLOG);			\
	HouseSmall.Add(FIntVector(2, -1, 1), OAKPLANKS);		\
	HouseSmall.Add(FIntVector(2, 0, 1), OAKDOOR);			\
	HouseSmall.Add(FIntVector(2, 1, 1), OAKPLANKS);			\
	HouseSmall.Add(FIntVector(2, 2, 1), OAKLOG);				

#define SMALLHOUSE_SECONDLAYER_X(x) \
	switch(x)													\
	{ \
		case -2:												\
			HouseSmall.Add(FIntVector(x, -2, 2), OAKLOG);		\
			HouseSmall.Add(FIntVector(x, -1, 2), OAKPLANKS);	\
			HouseSmall.Add(FIntVector(x, 0, 2), OAKPLANKS);		\
			HouseSmall.Add(FIntVector(x, 1, 2), OAKPLANKS);		\
			HouseSmall.Add(FIntVector(x, 2, 2), OAKLOG);		\
			break;												\
		case -1:												\
			HouseSmall.Add(FIntVector(x, -2, 2), OAKPLANKS);	\
			HouseSmall.Add(FIntVector(x, -1, 2), FURNACE);		\
			HouseSmall.Add(FIntVector(x, 0, 2), AIR);			\
			HouseSmall.Add(FIntVector(x, 1, 2), AIR);			\
			HouseSmall.Add(FIntVector(x, 2, 2), OAKPLANKS);		\
			break;												\
		case 0:													\
			HouseSmall.Add(FIntVector(x, -2, 2), AIR);			\
			HouseSmall.Add(FIntVector(x, -1, 2), AIR);			\
			HouseSmall.Add(FIntVector(x, 0, 2), AIR);			\
			HouseSmall.Add(FIntVector(x, 1, 2), STONESLAB);		\
			HouseSmall.Add(FIntVector(x, 2, 2), AIR);			\
		break;													\
		case 2:													\
			HouseSmall.Add(FIntVector(x, -2, 2), OAKLOG);		\
			HouseSmall.Add(FIntVector(x, -1, 2), OAKPLANKS);	\
			HouseSmall.Add(FIntVector(x, 0, 2), OAKPLANKS);		\
			HouseSmall.Add(FIntVector(x, 1, 2), OAKPLANKS);		\
			HouseSmall.Add(FIntVector(x, 2, 2), OAKLOG);		\
			break;												\
		default:												\
			HouseSmall.Add(FIntVector(x, -2, 2), OAKPLANKS);	\
			HouseSmall.Add(FIntVector(x, -1, 2), AIR);			\
			HouseSmall.Add(FIntVector(x, 0, 2), AIR);			\
			HouseSmall.Add(FIntVector(x, 1, 2), AIR);			\
			HouseSmall.Add(FIntVector(x, 2, 2), OAKPLANKS);		\
			break;												\
	}

#define SMALLHOUSE_THIRDLAYER_X(x) \
	switch(x)												\
	{ \
		case -2:													\
			HouseSmall.Add(FIntVector(x, -2, 3), OAKLOG);			\
			HouseSmall.Add(FIntVector(x, -1, 3), OAKPLANKS);		\
			HouseSmall.Add(FIntVector(x, 0, 3), OAKPLANKS);			\
			HouseSmall.Add(FIntVector(x, 1, 3), OAKPLANKS);			\
			HouseSmall.Add(FIntVector(x, 2, 3), OAKLOG);			\
			break;													\
		case 2:														\
			HouseSmall.Add(FIntVector(x, -2, 3), OAKLOG);			\
			HouseSmall.Add(FIntVector(x, -1, 3), OAKPLANKS);		\
			HouseSmall.Add(FIntVector(x, 0, 3), OAKPLANKS);			\
			HouseSmall.Add(FIntVector(x, 1, 3), OAKPLANKS);			\
			HouseSmall.Add(FIntVector(x, 2, 3), OAKLOG);			\
			break;													\
		default:													\
			HouseSmall.Add(FIntVector(x, -2, 3), OAKPLANKS);		\
			HouseSmall.Add(FIntVector(x, -1, 3), AIR);				\
			HouseSmall.Add(FIntVector(x, 0, 3), AIR);				\
			HouseSmall.Add(FIntVector(x, 1, 3), AIR);				\
			HouseSmall.Add(FIntVector(x, 2, 3), OAKPLANKS);			\
			break;													\
	}

#define SMALLHOUSE_FOURTHLAYER_X(x) \
	switch(x)												\
	{ \
		case -2:												\
			HouseSmall.Add(FIntVector(x, -2, 4), OAKLOG);		\
			HouseSmall.Add(FIntVector(x, -1, 4), OAKLOG);		\
			HouseSmall.Add(FIntVector(x, 0, 4), OAKLOG);		\
			HouseSmall.Add(FIntVector(x, 1, 4), OAKLOG);		\
			HouseSmall.Add(FIntVector(x, 2, 4), OAKLOG);		\
			break;												\
		case 2:													\
			HouseSmall.Add(FIntVector(x, -2, 4), OAKLOG);		\
			HouseSmall.Add(FIntVector(x, -1, 4), OAKLOG);		\
			HouseSmall.Add(FIntVector(x, 0, 4), OAKLOG);		\
			HouseSmall.Add(FIntVector(x, 1, 4), OAKLOG);		\
			HouseSmall.Add(FIntVector(x, 2, 4), OAKLOG);		\
			break;												\
		default:												\
			HouseSmall.Add(FIntVector(x, -2, 4), OAKLOG);		\
			HouseSmall.Add(FIntVector(x, -1, 4), AIR);			\
			HouseSmall.Add(FIntVector(x, 0, 4), AIR);			\
			HouseSmall.Add(FIntVector(x, 1, 4), AIR);			\
			HouseSmall.Add(FIntVector(x, 2, 4), OAKLOG);		\
			break;												\
	}

#define SMALLHOUSE_FIFTHLAYER_X(x) \
	HouseSmall.Add(FIntVector(x, -2, 5), AIR);		\
	switch(x)													\
	{ \
		case -2:												\
			HouseSmall.Add(FIntVector(x, -1, 5), AIR);			\
			HouseSmall.Add(FIntVector(x, 0, 5), AIR);			\
			HouseSmall.Add(FIntVector(x, 1, 5), AIR);			\
			break;												\
		case 2:													\
			HouseSmall.Add(FIntVector(x, -1, 5), AIR);			\
			HouseSmall.Add(FIntVector(x, 0, 5), AIR);			\
			HouseSmall.Add(FIntVector(x, 1, 5), AIR);			\
			break;												\
		default:												\
			HouseSmall.Add(FIntVector(x, -1, 5), OAKLOG);		\
			HouseSmall.Add(FIntVector(x, 0, 5), OAKLOG);		\
			HouseSmall.Add(FIntVector(x, 1, 5), OAKLOG);		\
			break;												\
	} \
	HouseSmall.Add(FIntVector(x, 2, 5), AIR);	

#define SMALLHOUSE_BOTTOMLAYER			\
	SMALLHOUSE_BOTTOMLAYER_X(-2)		\
	SMALLHOUSE_BOTTOMLAYER_X(-1)		\
	SMALLHOUSE_BOTTOMLAYER_X(0)			\
	SMALLHOUSE_BOTTOMLAYER_X(1)			\
	SMALLHOUSE_BOTTOMLAYER_X(2)

#define SMALLHOUSE_SECONDLAYER			\
	SMALLHOUSE_SECONDLAYER_X(-2)		\
	SMALLHOUSE_SECONDLAYER_X(-1)		\
	SMALLHOUSE_SECONDLAYER_X(0)			\
	SMALLHOUSE_SECONDLAYER_X(1)			\
	SMALLHOUSE_SECONDLAYER_X(2)

#define SMALLHOUSE_THIRDLAYER			\
	SMALLHOUSE_THIRDLAYER_X(-2)			\
	SMALLHOUSE_THIRDLAYER_X(-1)			\
	SMALLHOUSE_THIRDLAYER_X(0)			\
	SMALLHOUSE_THIRDLAYER_X(1)			\
	SMALLHOUSE_THIRDLAYER_X(2)

#define SMALLHOUSE_FOURTHLAYER			\
	SMALLHOUSE_FOURTHLAYER_X(-2)		\
	SMALLHOUSE_FOURTHLAYER_X(-1)		\
	SMALLHOUSE_FOURTHLAYER_X(0)			\
	SMALLHOUSE_FOURTHLAYER_X(1)			\
	SMALLHOUSE_FOURTHLAYER_X(2)

#define SMALLHOUSE_FIFTHLAYER			\
	SMALLHOUSE_FIFTHLAYER_X(-2)			\
	SMALLHOUSE_FIFTHLAYER_X(-1)			\
	SMALLHOUSE_FIFTHLAYER_X(0)			\
	SMALLHOUSE_FIFTHLAYER_X(1)			\
	SMALLHOUSE_FIFTHLAYER_X(2)

#define SMALLHOUSE_FULL					\
	SMALLHOUSE_BOTTOMLAYER				\
	SMALLHOUSE_FIRSTLAYER				\
	SMALLHOUSE_SECONDLAYER				\
	SMALLHOUSE_THIRDLAYER				\
	SMALLHOUSE_FOURTHLAYER				\
	SMALLHOUSE_FIFTHLAYER				

#define LARGEHOUSE_FULL \
2

#define BLACKSMITH_BOTTOMLAYER \
for (int x = -4; x < 6; x++) {												\
	for (int i = -3; i < 4; i++)											\
	{																		\
		Blacksmith.Add(FIntVector(x, i, 0), COBBLE);						\
	}																		\
}

#define BLACKSMITH_FIRSTLAYER \
	Blacksmith.Add(FIntVector(-4, -3, 1), COBBLE);		\
	Blacksmith.Add(FIntVector(-4, -2, 1), COBBLE);		\
	Blacksmith.Add(FIntVector(-4, -1, 1), COBBLE);		\
	Blacksmith.Add(FIntVector(-4, 0, 1), AIR);			\
	Blacksmith.Add(FIntVector(-4, 1, 1), AIR);			\
	Blacksmith.Add(FIntVector(-4, 2, 1), AIR);			\
	Blacksmith.Add(FIntVector(-4, 3, 1), FMCWorld_ChunkField(OAKFENCEPOST, QUARTERBLOCK));	\
\
	Blacksmith.Add(FIntVector(-3, -3, 1), COBBLE);		\
	Blacksmith.Add(FIntVector(-3, -2, 1), LAVA);			\
	Blacksmith.Add(FIntVector(-3, -1, 1), COBBLE);		\
	Blacksmith.Add(FIntVector(-3, 0, 1), AIR);			\
	Blacksmith.Add(FIntVector(-3, 1, 1), AIR);			\
	Blacksmith.Add(FIntVector(-3, 2, 1), SMOOTHSTONE);	\
	Blacksmith.Add(FIntVector(-3, 3, 1), AIR);			\
\
	Blacksmith.Add(FIntVector(-2, -3, 1), COBBLE);		\
	Blacksmith.Add(FIntVector(-2, -2, 1), LAVA);			\
	Blacksmith.Add(FIntVector(-2, -1, 1), COBBLE);		\
	Blacksmith.Add(FIntVector(-2, 0, 1), AIR);			\
	Blacksmith.Add(FIntVector(-2, 1, 1), AIR);			\
	Blacksmith.Add(FIntVector(-2, 2, 1), AIR);			\
	Blacksmith.Add(FIntVector(-2, 3, 1), AIR);			\
\
	Blacksmith.Add(FIntVector(-1, -3, 1), COBBLE);		\
	Blacksmith.Add(FIntVector(-1, -2, 1), COBBLE);		\
	Blacksmith.Add(FIntVector(-1, -1, 1), COBBLE);		\
	Blacksmith.Add(FIntVector(-1, 0, 1), COBBLE);			\
	Blacksmith.Add(FIntVector(-1, 1, 1), AIR);			\
	Blacksmith.Add(FIntVector(-1, 2, 1), AIR);			\
	Blacksmith.Add(FIntVector(-1, 3, 1), AIR);			\
\
	Blacksmith.Add(FIntVector(0, -3, 1), OAKPLANKS);		\
	Blacksmith.Add(FIntVector(0, -2, 1), CHEST);			\
	Blacksmith.Add(FIntVector(0, -1, 1), AIR);			\
	Blacksmith.Add(FIntVector(0, 0, 1), OAKPLANKS);		\
	Blacksmith.Add(FIntVector(0, 1, 1), AIR);				\
	Blacksmith.Add(FIntVector(0, 2, 1), AIR);				\
	Blacksmith.Add(FIntVector(0, 3, 1), FMCWorld_ChunkField(OAKFENCEPOST, QUARTERBLOCK));	\
\
	Blacksmith.Add(FIntVector(1, -3, 1), OAKPLANKS);		\
	Blacksmith.Add(FIntVector(1, -2, 1), AIR);			\
	Blacksmith.Add(FIntVector(1, -1, 1), AIR);			\
	Blacksmith.Add(FIntVector(1, 0, 1), OAKPLANKS);		\
	Blacksmith.Add(FIntVector(1, 1, 1), AIR);				\
	Blacksmith.Add(FIntVector(1, 2, 1), AIR);				\
	Blacksmith.Add(FIntVector(1, 3, 1), AIR);				\
\
	Blacksmith.Add(FIntVector(2, -3, 1), OAKPLANKS);		\
	Blacksmith.Add(FIntVector(2, -2, 1), AIR);			\
	Blacksmith.Add(FIntVector(2, -1, 1), AIR);			\
	Blacksmith.Add(FIntVector(2, 0, 1), AIR);				\
	Blacksmith.Add(FIntVector(2, 1, 1), OAKPLANKS);		\
	Blacksmith.Add(FIntVector(2, 2, 1), AIR);				\
	Blacksmith.Add(FIntVector(2, 3, 1), OAKLOG);			\
\
	Blacksmith.Add(FIntVector(3, -3, 1), OAKPLANKS);		\
	Blacksmith.Add(FIntVector(3, -2, 1), OAKSTAIRS);		\
	Blacksmith.Add(FIntVector(3, -1, 1), FMCWorld_ChunkField(OAKFENCEPOST));	\
	Blacksmith.Add(FIntVector(3, 0, 1), AIR);				\
	Blacksmith.Add(FIntVector(3, 1, 1), AIR);				\
	Blacksmith.Add(FIntVector(3, 2, 1), AIR);				\
	Blacksmith.Add(FIntVector(3, 3, 1), OAKPLANKS);		\
\
	Blacksmith.Add(FIntVector(4, -3, 1), OAKPLANKS);		\
	Blacksmith.Add(FIntVector(4, -2, 1), OAKPLANKS);		\
	Blacksmith.Add(FIntVector(4, -1, 1), OAKSTAIRS);		\
	Blacksmith.Add(FIntVector(4, 0, 1), AIR);				\
	Blacksmith.Add(FIntVector(4, 1, 1), AIR);				\
	Blacksmith.Add(FIntVector(4, 2, 1), AIR);				\
	Blacksmith.Add(FIntVector(4, 3, 1), OAKPLANKS);		\
\
	Blacksmith.Add(FIntVector(5, -3, 1), OAKLOG);			\
	Blacksmith.Add(FIntVector(5, -2, 1), OAKPLANKS);		\
	Blacksmith.Add(FIntVector(5, -1, 1), OAKPLANKS);		\
	Blacksmith.Add(FIntVector(5, 0, 1), OAKPLANKS);		\
	Blacksmith.Add(FIntVector(5, 1, 1), OAKPLANKS);		\
	Blacksmith.Add(FIntVector(5, 2, 1), OAKPLANKS);		\
	Blacksmith.Add(FIntVector(5, 3, 1), OAKLOG);

#define BLACKSMITH_FULL \
	BLACKSMITH_BOTTOMLAYER
