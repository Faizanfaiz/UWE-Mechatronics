
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
 *  To operate the Leg, input X and Y coordinates for the toe                *
 *                                                                           *   
 *  ***Important: Always keep these Coordinates in 3rd and 4th Quadrant      *
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define PI 3.14159265358979323846
#define L1 55.00
#define L2 55.00

/// Motor library (ensure this header is available in your project)
#define UWERMDX_IMPLEMENTATION
#include "uwermdx.h"

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
    *theta2_deg = 180+theta2 * (180.0 / PI);
}

// Function to check if the point is in the 3rd or 4th quadrant
int is_valid_quadrant(double x, double y) {
    // 3rd Quadrant: x < 0, y < 0
    // 4th Quadrant: x > 0, y < 0
    if ((x <= 0 && y <= -10) || (x >= 0 && y <= -10) || (x >= 0 && y>=-90) || (x <= 0 && y>= -90)) {
        return 1;  // Valid coordinate
    }
    printf("Coordinates (%.2f, %.2f) are in the 1st or 2nd quadrant, which are not allowed.\n", x, y);
    return 0;  // Invalid coordinate
}

int main() {
    double x, y, joint1, joint2;

    // Motor Control
    uint32_t motors_can0[] = {0x143, 0x146};
    bind_motors("can0", motors_can0, 2); // Bind 2 motors to CAN0

    // Initialize CAN
    init_can("can0", 1000000);

    while (1) {
        // Input x, y from user
        printf("\nEnter x: ");
        scanf("%lf", &x);
        printf("Enter y: ");
        scanf("%lf", &y);

        // Check if the coordinates are in a valid quadrant
        if (!is_valid_quadrant(x, y)) {
            continue;  // If not valid, skip to the next iteration
        }
        // Calculate inverse kinematics
        inversekinematics(x, y, &joint1, &joint2);

        // Print the results
        printf("Theta 1: %.2f degrees\n", joint1);
        printf("Theta 2: %.2f degrees\n", joint2);

        // Send the calculated angles to the motors
        send_pos_speed(0x143, joint1, 20);  // Send theta1 to motor 1
        send_pos_speed(0x146, joint2, 20);  // Send theta2 to motor 2
    }

    // Close the CAN communication (this won't be reached unless the program is terminated)
    close_can("can0");

    return 0;
}
