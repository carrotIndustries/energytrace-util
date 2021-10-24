#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>
#include <MSP430.h>
#include <MSP430_EnergyTrace.h>
#include <MSP430_Debug.h>

typedef struct __attribute__((packed))  {
	uint8_t id;
	uint64_t timestamp:56;
	uint32_t current;
	uint16_t voltage;
	uint32_t energy;
} event_t;

void push_cb(void* pContext, const uint8_t* pBuffer, uint32_t nBufferSize) {
	assert(sizeof(event_t)==18);
	assert(nBufferSize%sizeof(event_t)==0);
	uint32_t n=nBufferSize/sizeof(event_t);
	event_t *ev = (void*)pBuffer;
	uint32_t i = 0;
	while(i < n) {
		if(ev->id == 8) {
			printf("%e %e %e %e\n",ev->timestamp/1e6, ev->current/1e9, ev->voltage/1e3, ev->energy/1e7);
		}
		ev++;
		i++;
	}
}

void error_cb(void* pContext, const char* pszErrorText) {
	fprintf(stderr, "error %s\n", pszErrorText);
}

void usage(char *a0) {
	fprintf(stderr, "usage: %s <measurement duration in seconds>\n", a0);
}

int main(int argc, char *argv[]) {
	if(argc<2) {
		usage(argv[0]);
		return 1;
	}
	unsigned int duration = strtod(argv[1], 0);
	if(duration == 0) {
		usage(argv[0]);
		return 1;
	}
	
	
	STATUS_T status, secure = STATUS_OK;
	char* portNumber;
	int  version;
	long  vcc = 3300;
	union DEVICE_T device;
	portNumber = "TIUSB";
	
	fprintf(stderr, "Initializing the interface: ");
	status = MSP430_Initialize(portNumber, &version);
	fprintf(stderr, "MSP430_Initialize(portNumber=%s, version=%d) returns %d\n", portNumber, version, status);
	if(status != STATUS_OK) {
		return 1;
	}

	//status = MSP430_Configure(ET_CURRENTDRIVE_FINE, 1);
	//fprintf(stderr, "MSP430_Configure(ET_CURRENTDRIVE_FINE, 1) =%d\n", status);

	// 2. Set the device Vcc.
	fprintf(stderr, "Setting the device Vcc: ");
	status = MSP430_VCC(vcc);
	fprintf(stderr, "MSP430_VCC(%d) returns %d\n", vcc, status);


	// 3. Open the device.
	MSP430_LoadDeviceDb(NULL); //Required in more recent versions of tilib.
	fprintf(stderr, "Opening the device: ");
	status = MSP430_OpenDevice("DEVICE_UNKNOWN", "", 0, 0, DEVICE_UNKNOWN);
	fprintf(stderr, "MSP430_OpenDevice() returns %d\n", status);
	if(status != STATUS_OK) {
		return 1;
	}

	// 4. Get device information
	status = MSP430_GetFoundDevice((char*)&device, sizeof(device.buffer));
	fprintf(stderr, "MSP430_GetFoundDevice() returns %d\n", status);
	fprintf(stderr, " device.id: %d\n", device.id);
	fprintf(stderr, " device.string: %s\n", device.string);
	fprintf(stderr, " device.mainStart: 0x%04x\n", device.mainStart);
	fprintf(stderr, " device.infoStart: 0x%04x\n", device.infoStart);
	fprintf(stderr, " device.ramEnd: 0x%04x\n", device.ramEnd);
	fprintf(stderr, " device.nBreakpoints: %d\n", device.nBreakpoints);
	fprintf(stderr, " device.emulation: %d\n", device.emulation);
	fprintf(stderr, " device.clockControl: %d\n", device.clockControl);
	fprintf(stderr, " device.lcdStart: 0x%04x\n", device.lcdStart);
	fprintf(stderr, " device.lcdEnd: 0x%04x\n", device.lcdEnd);
	fprintf(stderr, " device.vccMinOp: %d\n", device.vccMinOp);
	fprintf(stderr, " device.vccMaxOp: %d\n", device.vccMaxOp);
	fprintf(stderr, " device.hasTestVpp: %d\n", device.hasTestVpp);
	
	
	EnergyTraceSetup ets = {  ET_PROFILING_ANALOG,                // Gives callbacks of with eventID 8
                      ET_PROFILING_1K,                   // N/A
                      ET_ALL,                             // N/A
                      ET_EVENT_WINDOW_100,                // N/A
                      ET_CALLBACKS_ONLY_DURING_RUN };           // Callbacks are continuously
	EnergyTraceHandle ha;
	EnergyTraceCallbacks cbs = {
		.pContext = 0,
		.pPushDataFn = push_cb,
		.pErrorOccurredFn = error_cb
	};
	MSP430_Run(FREE_RUN, 1);
	status = MSP430_EnableEnergyTrace(&ets, &cbs, &ha);
	fprintf(stderr, "MSP430_EnableEnergyTrace=%d\n", status);
	
	status = MSP430_ResetEnergyTrace(ha);
	fprintf(stderr, "MSP430_ResetEnergyTrace=%d\n", status);
	
	sleep(duration);
	
	status = MSP430_DisableEnergyTrace(ha);
	fprintf(stderr, "MSP430_DisableEnergyTrace=%d\n", status);
	
	fprintf(stderr, "Closing the interface: ");
	status = MSP430_Close(0);
	fprintf(stderr, "MSP430_Close(FALSE) returns %d\n", status);

	return 0;
}
