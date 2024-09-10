
/*****************************************************************************
 *                                                                           *
 *                     ██╗   ██╗██╗    ██╗███████╗                           *
 *                     ██║   ██║██║    ██║██╔════╝                           *
 *                     ██║   ██║██║ █╗ ██║█████╗                             *
 *                     ██║   ██║██║███╗██║██╔══╝                             *
 *                     ╚██████╔╝╚███╔███╔╝███████╗                           *
 *                      ╚═════╝  ╚══╝╚══╝ ╚══════╝                           *
 *                                                                           *
 *   Library Engineered by FAIZAN MIR                                        *
 *   Date: August 2024                                                       *
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
 *****************************************************************************/


#ifndef UWERMDX_H
#define UWERMDX_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <linux/if.h>
#include <time.h>

#define MAX_MOTORS_PER_INTERFACE 8  // 8 motors per CAN interface
#define MAX_CAN_INTERFACES 2        // Only 2 CAN interfaces are supported

#define POSITION_MULTIPLIER 903     // Multiplier for position - Encoder counts for each degree
#define SPEED_MULTIPLIER 10         // Multiplier for speed

// Define hardcoded angle limits (in degrees)
#define MIN_ANGLE_LIMIT -120         // Minimum allowable angle
#define MAX_ANGLE_LIMIT 120          // Maximum allowable angle

// Structure to map motor IDs to CAN interfaces
typedef struct {
    uint32_t motor_ids[MAX_MOTORS_PER_INTERFACE];
    char can_interface[10];
    int motor_count;
    int socket;                     // Socket for the CAN interface
} MotorBinding;

// Global array of motor bindings, one for each CAN interface
static MotorBinding motor_bindings[MAX_CAN_INTERFACES];
static int binding_count = 0;

// Initialize the CAN interface with the specified bitrate
void init_can(const char *can_interface, int bitrate);

// Bind motor IDs to a specific CAN interface
void bind_motors(const char *can_interface, uint32_t motor_ids[], int motor_count);

// Close the specified CAN interface
void close_can(const char *can_interface);

// Send a position and speed command to the motor, automatically using the correct CAN interface
void send_pos_speed(uint32_t motor_id, int32_t position, uint16_t speed);

// Internal helper function to create and bind a CAN socket
static int create_can_socket(const char *can_interface) {
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("socket PF_CAN failed");
        return -1;
    }

    strcpy(ifr.ifr_name, can_interface);
    if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl failed");
        close(s);
        return -1;
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind failed");
        close(s);
        return -1;
    }

    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);  // Disable filtering rules
    return s;
}

// Implementation of the functions
#ifdef UWERMDX_IMPLEMENTATION

void init_can(const char *can_interface, int bitrate) {
    char command[128];

    // Bring down the interface first to avoid "Device or resource busy" error
    snprintf(command, sizeof(command), "sudo ifconfig %s down", can_interface);
    system(command);  // Don't check the return value; it may fail if the interface is already down.

    // Set CAN bitrate
    snprintf(command, sizeof(command), "sudo ip link set %s type can bitrate %d", can_interface, bitrate);
    if (system(command) != 0) {
        perror("Failed to set CAN bitrate");
        return;
    }

    // Bring up the CAN interface
    snprintf(command, sizeof(command), "sudo ifconfig %s up", can_interface);
    if (system(command) != 0) {
        perror("Failed to bring up the interface");
    }

    // Create and store the CAN socket
    for (int i = 0; i < binding_count; i++) {
        if (strcmp(motor_bindings[i].can_interface, can_interface) == 0) {
            motor_bindings[i].socket = create_can_socket(can_interface);
            if (motor_bindings[i].socket < 0) {
                fprintf(stderr, "Failed to create socket for interface %s\n", can_interface);
            } else {
                printf("Socket for interface %s created successfully: %d\n", can_interface, motor_bindings[i].socket);
            }
            break;
        }
    }
}



void bind_motors(const char *can_interface, uint32_t motor_ids[], int motor_count) {
    printf("Binding motors to interface %s\n", can_interface);  // Debugging output
    if (binding_count >= MAX_CAN_INTERFACES) {
        fprintf(stderr, "Max CAN interface bindings exceeded\n");
        return;
    }
    
    if (motor_count > MAX_MOTORS_PER_INTERFACE) {
        fprintf(stderr, "Exceeded maximum motors per CAN interface\n");
        return;
    }

    MotorBinding *binding = &motor_bindings[binding_count++];
    strncpy(binding->can_interface, can_interface, sizeof(binding->can_interface));
    
    for (int i = 0; i < motor_count; i++) {
        binding->motor_ids[i] = motor_ids[i];
        printf("Bound motor ID 0x%x to interface %s\n", motor_ids[i], can_interface);  // Debugging output
    }
    binding->motor_count = motor_count;
    binding->socket = -1;  // Initialize socket as invalid
}


void close_can(const char *can_interface) {
    for (int i = 0; i < binding_count; i++) {
        if (strcmp(motor_bindings[i].can_interface, can_interface) == 0) {
            if (motor_bindings[i].socket >= 0) {
                close(motor_bindings[i].socket);  // Close the socket if it is valid
                motor_bindings[i].socket = -1;    // Mark socket as closed
            }
            break;
        }
    }

    char command[128];
    snprintf(command, sizeof(command), "sudo ifconfig %s down", can_interface);
    if (system(command) != 0) {
        perror("Failed to bring down the interface");
    }
}

void send_pos_speed(uint32_t motor_id, int32_t x, uint16_t y) {
    // Check if the position is within the hardcoded angle limits
    if (x < MIN_ANGLE_LIMIT || x > MAX_ANGLE_LIMIT) {
        fprintf(stderr, "Error: Position %d out of range (%d to %d)\n", x, MIN_ANGLE_LIMIT, MAX_ANGLE_LIMIT);
        return;  // Do not send the frame if out of range
    }

    // Calculate position and speed based on multipliers
    int32_t position = x * POSITION_MULTIPLIER;
    uint16_t speed = y * SPEED_MULTIPLIER;

    // Find the correct CAN interface for the given motor ID
    MotorBinding *binding = NULL;
    for (int i = 0; i < binding_count; i++) {
        for (int j = 0; j < motor_bindings[i].motor_count; j++) {
            if (motor_bindings[i].motor_ids[j] == motor_id) {
                binding = &motor_bindings[i];
                break;
            }
        }
        if (binding != NULL) {
            break;
        }
    }

    if (binding == NULL || binding->socket < 0) {
        fprintf(stderr, "Motor ID 0x%x not bound to any CAN interface or socket is invalid\n", motor_id);
        return;
    }

    printf("Sending command to motor ID 0x%x using socket %d\n", motor_id, binding->socket);

    struct can_frame frame;
    memset(&frame, 0, sizeof(struct can_frame));

    frame.can_id = motor_id;
    frame.can_dlc = 8;
    frame.data[0] = 0xA4;  // Command for position control
    frame.data[1] = 0x00;  // Reserved
    frame.data[2] = (uint8_t)(speed & 0xFF);
    frame.data[3] = (uint8_t)((speed >> 8) & 0xFF);
    frame.data[4] = (uint8_t)(position & 0xFF);
    frame.data[5] = (uint8_t)((position >> 8) & 0xFF);
    frame.data[6] = (uint8_t)((position >> 16) & 0xFF);
    frame.data[7] = (uint8_t)((position >> 24) & 0xFF);

    write(binding->socket, &frame, sizeof(frame));
}

#endif // UWERMDX_IMPLEMENTATION

#endif // UWERMDX_H
