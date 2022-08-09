// to be included

//#define SHOW_OVER_LIMIT_ONLY 1900    // show only if max is more. (remember: fprintf will slow down...)

void initMeasure(const char * title, struct runtimeStat * stat, int snapCount, int snapReset, int slotMax)
{
	stat->state = STAT_STATE_INIT;
	stat->title = title;
    stat->snapCount = snapCount;
    stat->snapReset = snapReset;
#ifdef STAT_SLOTS
    stat->slotStep = slotMax / STAT_SLOTS;
    stat->slotMax = stat->slotStep * STAT_SLOTS;
#endif
    statDataReset(stat);
}

void enterMeasure(struct runtimeStat * stat)
{
	if (stat->state != STAT_STATE_INIT && stat->state != STAT_STATE_IDLE) {
		LL_DEBUG("stat: wrong state %d\n", stat->state);
	}
    gettimeofday(&stat->lastEntry, NULL);
    stat->state = STAT_STATE_ENTERED;
}

void exitMeasure(struct runtimeStat * stat)
{
	if (stat->state != STAT_STATE_ENTERED) {
		LL_DEBUG("stat: wrong state %d\n", stat->state);
	}
	gettimeofday(&stat->lastExit, NULL);
	
	long dura = (stat->lastExit.tv_sec - stat->lastEntry.tv_sec) * 1000000L
	          + (stat->lastExit.tv_usec - stat->lastEntry.tv_usec);
	
	stat->duraSum += dura;
	++stat->count;
	
	if (dura > stat->maxDura ) stat->maxDura = dura;
	if (dura < stat->minDura || stat->minDura == 0) stat->minDura = dura;
	
	// slot count
#ifdef STAT_SLOTS
	if ( dura > stat->slotMax) {
		++stat->slots[STAT_SLOTS];
	} else {
		int snr = dura / stat->slotStep;
		++stat->slots[snr];
	}
#endif

	if (stat->snapCount > 0 && stat->count >= stat->snapCount 
#ifdef SHOW_OVER_LIMIT_ONLY
	&& stat->maxDura > SHOW_OVER_LIMIT_ONLY
#endif
	) {
		displayStat(stat, 1);
	}
	
	stat->state = STAT_STATE_IDLE;
}

void endMeasure(struct runtimeStat * stat)
{
	displayStat(stat, 0);
}

void addSideCount(struct runtimeStat * stat, int value)
{
	stat->sideCount += value;
}

void displayStat( struct runtimeStat * stat, int fromSnap)
{
	long avg = (long)((float)stat->duraSum / stat->count);
 	LL_DEBUG_PLAIN("%s: min %ld uSec, max %ld uSec, avg %ld uSec, count %d, sidecount %d\n", stat->title, stat->minDura, stat->maxDura, avg, stat->count, stat->sideCount);
	
#ifdef STAT_SLOTS	
    double pctSum = 0;
	for ( int i=0; i < STAT_SLOTS; ++i)
	{
		if ( stat->slots[i] > 0) {
			double pct = 100*(double)stat->slots[i] / stat->count;
			pctSum += pct;
			LL_DEBUG_PLAIN("%4d..%4d: %6d  (%5.2f %) (%5.2f %)\n", i*stat->slotStep, ( i+1)*stat->slotStep-1, stat->slots[i], pct, pctSum);
		}
	}
	if ( stat->slots[STAT_SLOTS] > 0) {
	    LL_DEBUG_PLAIN("over  %4d: %6d  (%5.2f %)\n", stat->slotMax, stat->slots[STAT_SLOTS], 100*(double)stat->slots[STAT_SLOTS] / stat->count);
	}
#endif

	if (fromSnap && stat-> snapReset) {
		statDataReset(stat);
	}
}

void statDataReset(struct runtimeStat * stat)
{
    stat->count = 0;
    stat->duraSum = 0;
    stat->minDura = 0;
    stat->maxDura = 0;
    stat->sideCount = 0;
#ifdef STAT_SLOTS
    memset(stat->slots, 0, sizeof(stat->slots));
#endif
}
