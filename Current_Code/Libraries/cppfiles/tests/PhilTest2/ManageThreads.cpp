
#if using_ITG3200	
void *Manage_ITG3200(void* Something) {
	ThreadPass & stuff = *(ThreadPass*) Something;
	ITG3200& itg = *(ITG3200*)stuff.SomeClass;
	int SampleRate = ITG_Sample_Period;
	int ReTryRate = Retry_Period;
	while (ContinueThreads) {
		WaitForBus(SampleRate, stuff.BusUsage, stuff.BusUseMask, ReTryRate);
		*stuff.i2c_bus_call_count += 1;
#if Debug
		cout << "ITG3200 In action!" << endl;
#endif
	//	TakeDataITG();


		TakeDataITG(&itg,*stuff.BusUsage ,stuff.BusUseMask );
		usleep(1000);
		// finnished with i2cbus, resets its bit to be 0. Can also be done with xor ^
	//	*stuff.BusUsage = (~(stuff.BusUseMask) &  *stuff.BusUsage);
	}
	ThreadFinnished |= stuff.BusUseMask;
	return NULL;
}
#endif


#if using_BMP180
void *Manage_BMP180(void* Something) {
	ThreadPass & stuff = *(ThreadPass*) Something;
		BMP_Presure_Temp& bmp = *(BMP_Presure_Temp*)stuff.SomeClass;

	int SampleRate = BMP_Sample_Period;
	int ReTryRate = Retry_Period;
	while (ContinueThreads) {
		WaitForBus(SampleRate, stuff.BusUsage, stuff.BusUseMask, ReTryRate);
		*stuff.i2c_bus_call_count += 1;
#if Debug
		cout << "BMP180 In action!" << endl;
#endif
		TakeDataBMP(&bmp,*stuff.BusUsage ,stuff.BusUseMask );
		usleep(1000);
		// finnished with i2cbus, resets its bit
	//	*stuff.BusUsage = (~(stuff.BusUseMask) &  *stuff.BusUsage);
	}
		ThreadFinnished |= stuff.BusUseMask;
	return NULL;
}
#endif

#if using_LIS331
void *Manage_LIS331(void* Something) {
	ThreadPass & stuff = *(ThreadPass*) Something;
			LIS331& lis331 = *(LIS331*)stuff.SomeClass;
	int SampleRate = LIS331_Sample_Period;
	int ReTryRate = Retry_Period;
	while (ContinueThreads) {
		WaitForBus(SampleRate, stuff.BusUsage, stuff.BusUseMask, ReTryRate);
		*stuff.i2c_bus_call_count += 1;
#if Debug
		cout << "LIS331 In action!" << endl;
#endif
	//	TakeDataLIS();
		TakeDataLIS(&lis331,*stuff.BusUsage ,stuff.BusUseMask );
		usleep(1000);
		// finnished with i2cbus, resets its bit
	//	*stuff.BusUsage = (~(stuff.BusUseMask) &  *stuff.BusUsage);
		//	cout << "New Mask: " << *stuff.BusUsage << endl;
	}
		ThreadFinnished |= stuff.BusUseMask;
	return NULL;
}
#endif

	
	#if using_LM73
void *Manage_LM73(void* Something) {

	ThreadPass & stuff = *(ThreadPass*) Something;
LM73& lm73 = *(LM73*)stuff.SomeClass;
	int SampleRate = LM73_Sample_Period;
	int ReTryRate = Retry_Period;
	while (ContinueThreads) {
		WaitForBus(SampleRate, stuff.BusUsage, stuff.BusUseMask, ReTryRate);
#if Debug
		cout << "LM73 In action!" << endl;
#endif
		*stuff.i2c_bus_call_count += 1;

	//	TakeDataLM73();

		TakeDataLM73(&lm73,*stuff.BusUsage ,stuff.BusUseMask );

		usleep(1000);
		// finnished with i2cbus, resets its bit
	//	*stuff.BusUsage = (~(stuff.BusUseMask) &  *stuff.BusUsage);
	}
		ThreadFinnished |= stuff.BusUseMask;
	return NULL;
}
#endif

#if using_HMC5883L
void *Manage_HMC(void* Something) {
	ThreadPass & stuff = *(ThreadPass*) Something;
	HMC& hmc = *(HMC*)stuff.SomeClass;
	int SampleRate = HMC_Sample_Period;
	int ReTryRate = Retry_Period;
	while (ContinueThreads) {
		WaitForBus(SampleRate, stuff.BusUsage, stuff.BusUseMask, ReTryRate);
		*stuff.i2c_bus_call_count += 1;
#if Debug
		cout << "HMC In action!" << endl;
#endif
//		TakeDataHMC5883L();

		TakeDataHMC5883L(&hmc,*stuff.BusUsage ,stuff.BusUseMask );

		usleep(1000);

		// finnished with i2cbus, resets its bit
	//	*stuff.BusUsage = (~(stuff.BusUseMask) &  *stuff.BusUsage);
	}
		ThreadFinnished |= stuff.BusUseMask;
	return NULL;
}
#endif