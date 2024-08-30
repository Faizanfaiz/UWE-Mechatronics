#define UWERMDX_IMPLEMENTATION
#include "uwermdx.h"
#include <stdint.h>
#include <stdio.h>

int main() {

    uint32_t motors_can0[] = {0x146, 0x142, 0x143, 0x144};
    
    bind_motors("can0", motors_can0, 4); // Bind 4 motors to CAN0

    init_can("can0", 1000000);

    send_pos_speed(0x146, 30, 50); 
    sleep(1);
    send_pos_speed(0x146, 20, 50); 
    sleep(1);
    send_pos_speed(0x146, 30, 50); 
    sleep(1);
    send_pos_speed(0x146, 20, 50); 

        // Example: Read PID parameters from motor with ID 0x141
    PIDParams params;
    if (read_motor_pid_parameters(0x141, &params) == 0) {
        // Print out the PID parameters
        printf("Current loop KP: %.6f\n", params.curr_kp);
        printf("Current loop KI: %.6f\n", params.curr_ki);
        printf("Speed loop KP: %.6f\n", params.spd_kp);
        printf("Speed loop KI: %.6f\n", params.spd_ki);
        printf("Position loop KP: %.6f\n", params.pos_kp);
        printf("Position loop KI: %.6f\n", params.pos_ki);
    } else {
        printf("Failed to read PID parameters from motor 0x141\n");
    }

    // When done, close the CAN interface
    close_can("can0");
    
    close_can("can0");
    
    return 0;
}
