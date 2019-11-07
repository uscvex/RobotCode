task sounds()
{
	while (true)
	{
	//JUST TAUNTING
			if (vexRT[Btn7RXmtr2]==1)
			{
				playSoundFile("FEAR_SMELL.wav");
				while(bSoundActive){wait1Msec(1);}
			}
			if (vexRT[Btn7DXmtr2]==1)
			{
				playSoundFile("OPPORTUNITY.wav");
				while(bSoundActive){wait1Msec(1);}
			}
			if (vexRT[Btn7LXmtr2]==1)
			{
				//playSoundFile("VICTORY_IMPOSSIBLE.wav");
				while(bSoundActive){wait1Msec(1);}
			}
			/*if (vexRT[Btn7UXmtr2]==1)
			{
				playSoundFile("FAIL_5X.wav");
				while(bSoundActive){wait1Msec(1);}
			}*/
			if (vexRT[Btn8LXmtr2]==1)
			{
				playSoundFile("FAIL_TEST.wav");
				while(bSoundActive){wait1Msec(1);}
			}
			if (vexRT[Btn8DXmtr2]==1)
			{
				playSoundFile("REPLACEMENT.wav");
				while(bSoundActive){wait1Msec(1);}
			}
			if (vexRT[Btn8RXmtr2]==1)//
			{
				playSoundFile("SMALL_TROPHY.wav");
				while(bSoundActive){wait1Msec(1);}
			}
			if (vexRT[Btn7LXmtr2]==1)
			{
				playSoundFile("WINNING.wav");
				while(bSoundActive){wait1Msec(1);}
			}
				if (vexRT[Btn8UXmtr2]==1)
			{
				playSoundFile("LIFT_ACTIVATED.wav");
				while(bSoundActive){wait1Msec(1);}
			}

		wait1Msec(20);
	}
}
