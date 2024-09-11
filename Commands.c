/*****************************************************************************
 *                                                                           *
 *                     ██╗   ██╗██╗    ██╗███████╗                           *
 *                     ██║   ██║██║    ██║██╔════╝                           *
 *                     ██║   ██║██║ █╗ ██║█████╗                             *
 *                     ██║   ██║██║███╗██║██╔══╝                             *
 *                     ╚██████╔╝╚███╔███╔╝███████╗                           *
 *                      ╚═════╝  ╚══╝╚══╝ ╚══════╝                           *
 *                                                                           *
 *   Kinematics for Robot Leg Engineered by FAIZAN MIR                       *
 *   Date: September 2024                                                    *
 *   For the UWE RMD X8 Motors     											 *
 * 																			 *
 *	 MCP2515 controller with SPI interface was used                          *
 *                                                                           *
 *   The Work was conducted for the UWE Engineering Mechatronics LAB         *
 *                                                                           *
 *   Project Partner: Matteo Ciua                                            *
 *   Project Supervisor: Appolinaire Etoundi                                 *
 *   Project Technician: Romain Derval                                       *
 *                                                                           *
 *                                                                           *
 *  To operate the Leg, input commands:                                      *
 *      - Send 0 for Sitting (x = 0, y = -10)                                *
 *      - Send 1 for Standing (x = 10, y = -70)                              *
 *      - Send 2 for Hopping (requires standing first)                       *
 *****************************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

#define PI 3.14159265358979323846
#define L1 55.00
#define L2 55.00

/// Motor library (ensure this header is available in your project)
#define UWERMDX_IMPLEMENTATION
#include "uwermdx.h"

bool is_standing = false; // Track if the dog is in standing position

// Function to calculate angles theta1 and theta2
void inversekinematics(double x, double y, double *theta1_deg, double *theta2_deg) {
    double d = sqrt(x * x + y * y);
    
    // Check if target is within reach
    if (d > (L1 + L2)) {
        printf("Target (%.2f, %.2f) is out of reach. Maximum reach is %.2f.\n", x, y, L1 + L2);
        return;
    }
    
    double cos_theta2 = (d * d - L1 * L1 - L2 * L2) / (2 * L1 * L2);
    double theta2 = acos(cos_theta2);  // Theta 2 in radians
    
    // Ensure correct quadrant for theta2
    if (y < 0) {
        theta2 = -theta2;
    }
    
    // Use atan2 to get correct angle for theta1 in all quadrants
    double theta1 = atan2(y, x) - atan2(L2 * sin(theta2), L1 + L2 * cos(theta2));
    
    // Convert radians to degrees
    *theta1_deg = theta1 * (180.0 / PI);
    *theta2_deg = 180 + theta2 * (180.0 / PI);
}

// Function to check if the point is in the 3rd or 4th quadrant
int is_valid_quadrant(double x, double y) {
    if ((x <= 0 && y <= -10) || (x >= 0 && y <= -10) || (x >= 0 && y >= -90) || (x <= 0 && y >= -90)) {
        return 1;  // Valid coordinate
    }
    printf("Coordinates (%.2f, %.2f) are in the 1st or 2nd quadrant, which are not allowed.\n", x, y);
    return 0;  // Invalid coordinate
}

// Function to send command-based positions
void execute_command(int command) {
    double x = 0, y = 0;
    double joint1, joint2;
    
    switch (command) {
        case 0:
            // Sitting position
            x = 0;
            y = -10;
            is_standing = false; // Update the state to indicate the dog is sitting
            break;
        case 1:
            // Standing position
            x = 10;
            y = -70;
            is_standing = true;  // Update the state to indicate the dog is standing
            break;
        case 2:
            // Hopping, only allowed if the dog is standing
            if (!is_standing) {
                printf("Dog must be standing before hopping.\n");
                return;
            }
            for (int i = 0; i < 35; ++i) { // Hop 5 times
                x = 10;
                y = (i % 2 == 0) ? -70 : -75;
                inversekinematics(x, y, &joint1, &joint2);
                printf("Hop to position: x=%.2f, y=%.2f\n", x, y);
                send_pos_speed(0x143, joint1, 100);  // Send theta1 to motor 1
                send_pos_speed(0x146, joint2, 100);  // Send theta2 to motor 2
                usleep(100000); // 0.5-second delay between hops
            }
            return;
        default:
            printf("Invalid command\n");
            return;
    }

    // Check if the coordinates are in a valid quadrant
    if (!is_valid_quadrant(x, y)) {
        return;  // If not valid, skip
    }

    // Calculate inverse kinematics
    inversekinematics(x, y, &joint1, &joint2);

    // Send the calculated angles to the motors
    send_pos_speed(0x143, joint1, 20);  // Send theta1 to motor 1
    send_pos_speed(0x146, joint2, 20);  // Send theta2 to motor 2
}

// Signal handler to ensure the dog sits before termination
void handle_termination(int signum) {
    printf("\nTerminating program. Dog will now sit...\n");
    execute_command(0);  // Set the dog to sit
    close_can("can0");    // Close CAN communication
    printf("CAN communication closed. Exiting program.\n");
    exit(0);
}

int main() {
    int command;

    // Motor Control
    uint32_t motors_can0[] = {0x143, 0x146};
    bind_motors("can0", motors_can0, 2); // Bind 2 motors to CAN0

    // Initialize CAN
    init_can("can0", 1000000);

    // Register signal handler to sit the dog on termination
    signal(SIGINT, handle_termination);  // Handle Ctrl+C (SIGINT)

    while (1) {
        // Input command from user
        printf("\nEnter command (0 = Sit, 1 = Stand, 2 = Hop): ");
        scanf("%d", &command);

        // Execute the corresponding command
        execute_command(command);
    }

    return 0;
}
