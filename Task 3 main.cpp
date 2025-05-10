#include "mbed.h"
#include "arm_book_lib.h"


DigitalIn gasDetector(D2);         
DigitalIn overTempDetector(D3);    
DigitalIn resetButton(BUTTON1);    

UnbufferedSerial pc(USBTX, USBRX, 115200);


bool alarmState = OFF;
bool tempTriggered = OFF;          //  for temperature
bool gasTriggered = OFF;           //  for gas
bool prevGasState = OFF;           // For edge detection
bool prevTempState = OFF;          // For edge detection


int main() {
    gasDetector.mode(PullDown);    
    overTempDetector.mode(PullDown);
    resetButton.mode(PullUp);      // Configure reset button

    while (1) {
      
        if (gasDetector) gasTriggered = ON;
        if (overTempDetector) tempTriggered = ON;
        alarmState = gasTriggered || tempTriggered;

        // Reset alarm and latches
        if (!resetButton) {        // Active-low button pressed
            alarmState = OFF;
            gasTriggered = OFF;
            tempTriggered = OFF;
        }

        // Check for threshold breaches and send warnings
        if (gasTriggered != prevGasState) {
            if (gasTriggered) pc.write("WARNING: Gas level exceeded!\r\n", 29);
            prevGasState = gasTriggered;
        }
        if (tempTriggered != prevTempState) {
            if (tempTriggered) pc.write("WARNING: Over-temperature!\r\n", 28);
            prevTempState = tempTriggered;
        }

        // Send continuous data
        char buffer[64];
        sprintf(buffer, "Alarm: %d | Temp: %d | Gas: %d\r\n", 
                alarmState, tempTriggered, gasTriggered);
        pc.write(buffer, strlen(buffer));

        ThisThread::sleep_for(500ms);
    }
}
