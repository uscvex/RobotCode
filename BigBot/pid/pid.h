typedef struct {
	float pGain;
	float iGain;
	float dGain;
	unsigned long errorsum;
	int target;
	int (*valFunc)();
	int16_t motorIndex;
	int16_t motorMinValue;
	int lastDError;
	int errorTolerace
	int noChangeCutoff;
	int noChangeCount
} PIDControl;