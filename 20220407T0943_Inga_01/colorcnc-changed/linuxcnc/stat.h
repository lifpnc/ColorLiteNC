

#define STAT_STATE_INIT 0
#define STAT_STATE_ENTERED 1
#define STAT_STATE_IDLE 2

#define STAT_SLOTS 100

struct runtimeStat {
	
	struct timeval lastEntry;
	struct timeval lastExit;
	long minDura;
	long maxDura;
	long duraSum;
	
	int state;
	int count;
	int sideCount;
	int snapCount;
	int snapReset;
	const char * title;

#ifdef STAT_SLOTS	
	int slotStep;
	int slotMax;
	int slots[STAT_SLOTS+1];
#endif
};


void initMeasure(const char * title, struct runtimeStat * stat, int snapCount, int snapReset, int slotMaxValue);

void enterMeasure(struct runtimeStat * stat);

void exitMeasure(struct runtimeStat * stat);

void endMeasure(struct runtimeStat * stat);

void addSideCount(struct runtimeStat * stat, int value);

// internals
void displayStat( struct runtimeStat * stat, int fromSnap);
void statDataReset(struct runtimeStat * stat);
