#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#define CURR_KP_MAX 3.0
#define CURR_KI_MAX 0.1
#define SPD_KP_MAX  0.1
#define SPD_KI_MAX  0.01
#define POS_KP_MAX  0.1
#define POS_KI_MAX  0.01

typedef struct {
    float curr_kp;
    float curr_ki;
    float spd_kp;
    float spd_ki;
    float pos_kp;
    float pos_ki;
} PIDParams;

// Function to configure CAN interface
int configure_can_interface(const char *ifname) {
    char command[128];

    // Check if the interface is already up
    snprintf(command, sizeof(command), "ip link show %s | grep 'state UP'", ifname);
    if (system(command) == 0) {
        printf("CAN interface %s is already up.\n", ifname);
        return 0;  // Interface is already up, no need to reconfigure
    }

    // Bring the interface down
    snprintf(command, sizeof(command), "sudo ip link set %s down", ifname);
    if (system(command) != 0) {
        perror("Failed to bring down the CAN interface");
        return -1;
    }

    // Set the bitrate and type
    snprintf(command, sizeof(command), "sudo ip link set %s type can bitrate 1000000", ifname);
    if (system(command) != 0) {
        perror("Failed to set CAN interface bitrate");
        return -1;
    }

    // Bring the interface up
    snprintf(command, sizeof(command), "sudo ip link set %s up", ifname);
    if (system(command) != 0) {
        perror("Failed to bring up the CAN interface");
        return -1;
    }

    return 0;
}

int main() {
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame send_frame, recv_frame;
    const char *ifname = "can0";
    PIDParams params = {0};

    // Configure CAN interface if needed
    if (configure_can_interface(ifname) != 0) {
        return 1;  // Exit if CAN interface configuration fails
    }

    // 1. Create a socket
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // 2. Specify the CAN interface
    strcpy(ifr.ifr_name, ifname);
    if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl failed");
        close(s);
        return 1;
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    // 3. Bind the socket to the CAN interface
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        close(s);
        return 1;
    }

    // 4. Prepare and send the CAN frame to read PID parameters
    send_frame.can_id = 0x146;
    send_frame.can_dlc = 8;
    send_frame.data[0] = 0x30;  // Command byte for reading PID parameters
    memset(&send_frame.data[1], 0x00, 7);  // Fill the rest with 0x00

    if (write(s, &send_frame, sizeof(send_frame)) != sizeof(send_frame)) {
        perror("Write failed");
        close(s);
        return 1;
    }

    printf("Sent PID read command to motor 0x%X\n", send_frame.can_id);

    // 5. Extend the read timeout to 2 seconds
    struct timeval timeout = {2, 0};
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    // 6. Read the response
    int nbytes = read(s, &recv_frame, sizeof(recv_frame));
    if (nbytes > 0) {
        printf("Received %d bytes from CAN ID 0x%X\n", nbytes, recv_frame.can_id);
        printf("Data length: %d\n", recv_frame.can_dlc);
        for (int i = 0; i < recv_frame.can_dlc; i++) {
            printf("Data[%d]: 0x%02X\n", i, recv_frame.data[i]);
        }

        if (recv_frame.data[0] == 0x30) {  // Check if the command byte matches
            params.curr_kp = recv_frame.data[2] * (CURR_KP_MAX / 256.0);
            params.curr_ki = recv_frame.data[3] * (CURR_KI_MAX / 256.0);
            params.spd_kp = recv_frame.data[4] * (SPD_KP_MAX / 256.0);
            params.spd_ki = recv_frame.data[5] * (SPD_KI_MAX / 256.0);
            params.pos_kp = recv_frame.data[6] * (POS_KP_MAX / 256.0);
            params.pos_ki = recv_frame.data[7] * (POS_KI_MAX / 256.0);

            printf("Current loop KP: %.6f\n", params.curr_kp);
            printf("Current loop KI: %.6f\n", params.curr_ki);
            printf("Speed loop KP: %.6f\n", params.spd_kp);
            printf("Speed loop KI: %.6f\n", params.spd_ki);
            printf("Position loop KP: %.6f\n", params.pos_kp);
            printf("Position loop KI: %.6f\n", params.pos_ki);
        } else {
            fprintf(stderr, "Unexpected response: 0x%X\n", recv_frame.data[0]);
        }
    } else {
        perror("Failed to read PID parameters");
        printf("Error: No response or read timed out\n");
    }

    // 7. Close the socket
    close(s);

    return 0;
}
