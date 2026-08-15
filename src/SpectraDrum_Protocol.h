/**
 * @file SpectraDrum_Protocol.h
 * @brief Shared binary protocol definitions for the SpectraDrum ecosystem.
 *        Used by Node Firmware, Master Firmware, and parsed by Web UI (TypeScript).
 */

#pragma once
#include <stdint.h>

// Ensure strict byte alignment across all architectures (RP2040, Web, etc.)
#pragma pack(push, 1)

// Packet Command Identifiers
enum SpectraCommand : uint8_t {
    CMD_NOTE_ON        = 0x10,  // Standard performance trigger event
    CMD_NOTE_OFF       = 0x11,  // Optional note off
    CMD_ENTER_CALIB    = 0x20,  // Command node to enter high-bandwidth debug mode
    CMD_EXIT_CALIB     = 0x21,  // Command node to return to standard performance mode
    CMD_CALIB_STREAM   = 0x22,  // High-bandwidth raw ADC spectral packet
    CMD_SET_CONFIG     = 0x30,  // Flash configuration update payload
    CMD_SET_PROFILE    = 0x31,  // Spectral profile configuration payload
};

// 1. Lightweight Control Packet (2 Bytes)
// Sent over USB/CAN to toggle modes (e.g., CMD_ENTER_CALIB / CMD_EXIT_CALIB)
struct ControlPacket {
    uint8_t cmd;             // SpectraCommand enum
    uint8_t target_node_id;  // Specific Node ID (0-15) or 0xFF for broadcast
};

// 2. Lightweight Performance Packet (5 Bytes)
struct HitPacket {
    uint8_t  cmd;        // CMD_NOTE_ON (0x10)
    uint8_t  node_id;    // Physical Pad / Node ID (0-15)
    uint8_t  zone_id;    // Classified Zone Identifier from Spectral Match (0-4)
    uint8_t  velocity;   // Translated MIDI intensity (1-127)
    uint8_t  flags;      // Bitmask for flags
};

// 3. Spectral Profile Definition Payload for Configuration (22 Bytes)
struct HitProfile {
    uint8_t profile_id;          // Profile index (0 to 4)
    uint8_t zone_out;            // Assigned output zone/MIDI ID (0=Center, 1=Edge, 2=Rimshot, 3=CrossStick, 4=Rim)
    uint8_t normalized_bands[8]; // Target spectral shape (0-255) for 8 filter bands
    uint8_t normalized_frame;    // Target shape component (0-255) for frame sensor
    uint8_t normalized_rim;      // Target shape component (0-255) for rim sensor
    uint8_t weights[10];         // Feature weights for 8 bands + frame + rim (1-10)
};

// 4. Calibration & Debug Data Packet (22 Bytes)
struct CalibStreamPacket {
    uint8_t  cmd;        // CMD_CALIB_STREAM (0x22)
    uint8_t  node_id;    // Physical Pad / Node ID (0-15)
    uint16_t bands[8];   // Raw 12-bit ADC peak envelope values across 8 filter bands
    uint16_t frame;      // Raw 12-bit ADC peak value for frame sensor
    uint16_t rim;        // Raw 12-bit ADC peak value for rim sensor
};

// 5. Node Configuration Payload (12 Bytes)
struct NodeConfig {
    uint8_t  node_id;
    uint16_t threshold_low;   // Noise floor cutoff
    uint16_t threshold_high;  // Retrigger mask ceiling
    uint8_t  midi_notes[5];   // Mapped MIDI notes for 5 zones
    uint8_t  velocity_curve;  // Curve type (0 = Linear, 1 = Log, 2 = Exp)
    uint8_t  crosstalk_factor;// Scaling factor for frame piezo rejection
};

#pragma pack(pop)