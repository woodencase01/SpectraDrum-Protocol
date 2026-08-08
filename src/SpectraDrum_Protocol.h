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
};

// 1. Lightweight Performance Packet (5 Bytes)
// Sent over CAN bus during normal playing mode for ultra-low latency.
struct HitPacket {
    uint8_t  cmd;        // CMD_NOTE_ON (0x10)
    uint8_t  node_id;    // Physical Pad / Node ID (0-15)
    uint8_t  zone_id;    // Zone Identifier (0 = Center/Head, 1 = Rim, etc.)
    uint8_t  velocity;   // Final translated MIDI intensity (0-127)
    uint8_t  flags;      // Bitmask for flags (e.g., frame crosstalk rejected)
};

// 2. High-Bandwidth Calibration Packet (18 Bytes)
// Sent continuously by a node only when in Calibration Mode for UI graphing.
struct CalibStreamPacket {
    uint8_t  cmd;        // CMD_CALIB_STREAM (0x22)
    uint8_t  node_id;    // Physical Pad / Node ID (0-15)
    uint16_t bands[8];   // Raw 12-bit ADC peak envelope values across your 8 filter bands
};

// 3. Node Configuration Payload (For EEPROM / Flash Storage)
struct NodeConfig {
    uint8_t  node_id;
    uint16_t threshold_low;   // Noise floor cutoff
    uint16_t threshold_high;  // Retrigger mask ceiling
    uint8_t  midi_notes[4];   // Mapped MIDI notes for zones
    uint8_t  velocity_curve;  // Curve type (0 = Linear, 1 = Log, 2 = Exp)
    uint8_t  crosstalk_factor;// Scaling factor for frame piezo rejection
};

#pragma pack(pop)